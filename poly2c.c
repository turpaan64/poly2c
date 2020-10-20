/* ////////////////////////////////////////////// *
 *                                                *
 *                 - POLY2C -                     *
 *                                                *
 * ////////////////////////////////////////////// */

// Build in terminal for windows
// x86_64-w64-mingw32-gcc -o poly2c-64.exe poly2c.c
// i686-w64-mingw32-gcc -o poly2c-32.exe poly2c.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "includes/data.h"
#include "includes/file.h"
#include "includes/types.h"
#include "includes/textmagic.h"

int main(int argc, char **argv)
{
    // TODO: copy the results to clipboard
    // TODO: name the xxxxVertexTable[] = {... based on input file
    //       by removing the word collision, if used + make the casing small
    // TODO: Start offset to get the collision for example from vanilla files
    if (argc < 2)
    {
        fprintf(stderr,
                "Please provide at least one input file.\n"
                "Exiting...\n");
        // getchar();
        return 1;
    }

    int found;
    char *p = strrchr(argv[1], '.');
    if (p)
    {
        found = strcmp(p, ".zobj") == 0;
    }
    else
    {
        fprintf(stderr, "This is not a zobj file.\n"
                        "Exiting...\n");
        return 1;
    }

    // Handle the Name
    // Delete word does not work on Windows. Results are pretty random and adds \r
    char inputName[sizeof(argv[1])];
    char defaultArray[] = "s";
    int index;
    strcpy(inputName, extract_filename(argv[1]));
    strcpy(inputName, extract_filenameWindows(inputName));
    printf("/* %s */\n", argv[1]);
    char wordColl[] = "collisio"; // For some reason having "collision" deletes the next character after this word
    char wordZobj[] = ".zobj";

    index = search(inputName, wordColl);
    if (index != -1) {
        delete_word(inputName, wordColl, index);
    }
    index = search(inputName, wordZobj);
    if (index != -1) {
        delete_word(inputName, wordZobj, index);
    }
    char** arrayName = concat(defaultArray, inputName);


    int zobjFileSize = 0;
    unsigned char* zobj   = makeFileBuffer(argv[1], 0, &zobjFileSize);
    z64_bgcheck_data_info_t bgData[1];
    memcpy(bgData, zobj, sizeof(z64_bgcheck_data_info_t));



    int vertexTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)) ^ 0x05000000;
    int polyTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)) ^ 0x05000000;
    int polyInfoTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)) ^ 0x05000000;
    int cameraDataTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)) ^ 0x05000000;
    int waterInfoTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)) ^ 0x05000000;

    uint32_t vertexTableSize = 0x6 * bgData->vtx_num;
    uint32_t polyTableSize = 0x10 * bgData->poly_num;
    uint32_t polyInfoTableSize = cameraDataTableOffset - polyInfoTableOffset;
    uint32_t cameraDataTableSize = 0;
    uint32_t waterInfoTableSize = 0;

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Vertex Table                                                                *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    int arraySize = bgData->vtx_num;
    z64_bgcheck_vertex_t vertexTable[arraySize];
    memcpy(vertexTable, zobj + vertexTableOffset, vertexTableSize);
    printf("const z64_bgcheck_vertex_t %sVertexTable[] = {\n", arrayName);
    if (arraySize > 0) {
        for (int32_t i = 0; i < arraySize; i++) {
            printf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", vertexTable[i].x & 0xffff, vertexTable[i].y & 0xffff, vertexTable[i].z & 0xffff);
        }
    } else {
        printf("\t/* EMPTY */\n");
    }
    printf("};\n\n");

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Poly Table                                                                  *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    arraySize = bgData->poly_num;
    z64_bgcheck_polygon_t polyTable[arraySize];
    memcpy(polyTable, zobj + polyTableOffset, polyTableSize);
    printf("const z64_bgcheck_polygon_t %sPolyTable[] = {\n", arrayName);
    if (arraySize > 0)
    {
        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t{\n\t\t0x%04X,\n", polyTable[i].info & 0xffff);
            printf("\t\t{ 0x%04X, 0x%04X, 0x%04X },\n", polyTable[i].v[0] & 0xffff, polyTable[i].v[1] & 0xffff, polyTable[i].v[2] & 0xffff);
            printf("\t\t0x%04X, 0x%04X, 0x%04X, 0x%04X,\n", polyTable[i].a & 0xffff, polyTable[i].b & 0xffff, polyTable[i].c & 0xffff, polyTable[i].d & 0xffff);
            printf("\t},\n");
        }
    }
    else
    {
        printf("\t/* EMPTY */\n");
    }
    printf("};\n\n");

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Poly Info Table                                                             *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    arraySize = polyInfoTableSize / 8;
    z64_bgcheck_polygon_info_t polyInfoTable[arraySize];
    memcpy(polyInfoTable, zobj + polyInfoTableOffset, polyInfoTableSize);
    printf("const z64_bgcheck_polygon_info_t %sPolyInfoTable[] = {\n", arrayName);
    if (arraySize > 0)
    {
        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t{ 0x%08X, 0x%08X},\n", SWAP_LE_BE(SWAP_V64_BE(polyInfoTable[i].info[0])) & 0xFFFFFFFF, SWAP_LE_BE(SWAP_V64_BE(polyInfoTable[i].info[1]))) & 0xFFFFFFFF;
        }
    }
    else
    {
        printf("\t/* EMPTY */\n");
    }
    printf("};\n\n");

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Camera Data Table                                                           *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    arraySize = cameraDataTableSize / 8;
    z64_bgcheck_camera_data_t cameraDataTable[arraySize];
    memcpy(cameraDataTable, zobj + cameraDataTableOffset, cameraDataTableSize);
    printf("const z64_bgcheck_camera_data_t %sCameraDataTable[] = {\n", arrayName);
    if (arraySize > 0)
    {
        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t/* TODO */,\n");
        }
    }
    else
    {
        printf("\t/* EMPTY */\n");
    }
    printf("};\n\n");

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Water Info Table                                                            *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    arraySize = waterInfoTableSize / 14;
    z64_bgcheck_water_info_t waterInfoTable[arraySize];
    memcpy(waterInfoTable, zobj + waterInfoTableOffset, waterInfoTableSize);
    printf("const z64_bgcheck_water_info_t %sWaterInfoTable[] = {\n", arrayName);
    if (arraySize > 0)
    {
        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t/* TODO */\n");
        }
    }
    else
    {
        printf("\t/* EMPTY */\n");
    }
    printf("};\n\n");

    /* ////////////// */

    printf("const z64_bgcheck_data_info_t %sPolyIdData = {\n", arrayName);
    printf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", bgData->vtx_min[0] & 0xFFFF, bgData->vtx_min[1] & 0xFFFF, bgData->vtx_min[2] & 0xFFFF);
    printf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", bgData->vtx_max[0] & 0xFFFF, bgData->vtx_max[1] & 0xFFFF, bgData->vtx_max[2] & 0xFFFF);
    printf("\t0x%04X,\n", bgData->vtx_num & 0xFFFF);
    printf("\t&%sVertexTable,\n", arrayName);
    printf("\t0x%04X,\n", bgData->poly_num & 0xFFFF);
    printf("\t&%sPolyTable,\n", arrayName);
    printf("\t&%sPolyInfoTable,\n", arrayName);
    printf("\t&%sCameraDataTable,\n", arrayName);
    printf("\t0x%04X,\n", bgData->water_info_num & 0xFFFF);
    printf("\t&%sWaterInfoTable,\n", arrayName);
    printf("};\n");
    putchar('\n');

    printf("/* ////////////////////////////////////////////// *\n");
    printf(" * ADDRESS\t\t ZOBJ:\t\t\t  *\n");
    printf(" * Vertex table\t\t 0x%08X\t\t  *\n", SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)), vertexTableOffset);
    printf(" * Poly table\t\t 0x%08X\t\t  *\n", SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)), polyTableOffset);
    printf(" * PolyInf table\t 0x%08X\t\t  *\n", SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)), polyInfoTableOffset);
    printf(" * CamData table\t 0x%08X\t\t  *\n", SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)), cameraDataTableOffset);
    printf(" * WaterInf table\t 0x%08X\t\t  *\n", SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)), waterInfoTableOffset);
    printf(" * File size\t\t 0x0500%04X\t\t  *\n", zobjFileSize & 0xFFFF);
    printf(" * ////////////////////////////////////////////// */\n");

    getchar();

    return 0;
}