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

typedef struct
{
    int32_t flagVtxTable;
    int32_t flagPolyTable;
    int32_t flagPolyInfo;
    int32_t flagCameraData;
    int32_t flagWaterInfo;
} configure;

int main(int argc, char **argv)
{
    configure *thisCnf = malloc(sizeof(configure));
    // TODO: copy the results to clipboard
    // TODO: name the xxxxVertexTable[] = {... based on input file
    //       by removing the word collision, if used + make the casing small
    // TODO: Start offset to get the collision for example from vanilla files
    static char *filename = 0;
    static char *extension = ".zobj";
    static char *output_ext = ".h";

    if (argc < 2)
    {
        print_usage(0, argv[0]);
        return 1;
    }

    if (check_extension(argv[1], ".zobj"))
    {
        filename = calloc(strlen(argv[1]), 1);
        filename = get_filename(argv[1]);
        //fprintf(stderr, "%s\n", filename);
    }
    else {
        print_usage(1, argv[1]);
        return 1;
    }
        
    int offset = 0;
    if (argv[2] != NULL)
    {
        offset = strtol(argv[2], NULL, 16); // Load 2nd argument as a hex
    }

    int zobjFileSize = 0;
    unsigned char *zobj = makeFileBuffer(argv[1], 0, &zobjFileSize);
    if (offset > zobjFileSize - sizeof(z64_bgcheck_data_info_t))
    {
        print_usage(3, argv[1]);
        return 1;
    }
    z64_bgcheck_data_info_t bgData[1];
    memcpy(bgData, zobj + offset, sizeof(z64_bgcheck_data_info_t));

    uint32_t testLegit[] = {
        (SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)) & 0xF0FF0000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)) & 0xF0FF0000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)) & 0xF0FF0000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)) & 0xF0FF0000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)) & 0xF0FF0000)
    };
    
    if (testLegit[0] == 0 && testLegit[1] == 0 && testLegit[2] == 0 && testLegit[3] == 0 && testLegit[4] == 0) {
    } else {
        // print_usage(2, argv[1]);
        // return 1;

        int stored = offset;
        int res;
        int segmentTest;

        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mFinding offset for you, you lazy bastard.\n");
        printf("0x%08X", 0);

        for (int i = 0; (i * 4) < zobjFileSize; i++)
        {
            if (i == i)
            {
                memcpy(bgData, zobj + stored + (i * 4), sizeof(z64_bgcheck_data_info_t));

                uint32_t testLegit2[] = {
                    (SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)) & 0xFFF00000),
                    (SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)) & 0xFFF00000),
                    (SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)) & 0xFFF00000),
                    (SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)) & 0xF0F00000),
                    (SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)) & 0xF0F00000)
                };

                segmentTest = 0;
                if (testLegit2[0] != 0)
                {
                    if (testLegit2[0] == testLegit2[1] && testLegit2[1] == testLegit2[2])
                    {
                        testLegit2[0] = testLegit2[1] = testLegit2[2] = 0;
                        segmentTest = 1;
                    }
                }

                res = 0;

                if (segmentTest && bgData->pad == 0 && bgData->pad2 == 0 && testLegit2[0] == 0 && testLegit2[1] == 0 && testLegit2[2] == 0 && testLegit2[3] == 0 && testLegit2[4] == 0)
                {
                    offset = stored + (i * 4);
                    fprintf(stderr, "\e[0;94m[*] "
                                    "\e[mIt's your lucky day! Offset found at 0x%08X\n\n\n",
                            offset);
                    res = 1;
                    break;
                }
                
            }
        }

        if (!res) {
            print_usage(4, argv[1]);
            return 1;
        } else {
            memcpy(bgData, zobj + offset, sizeof(z64_bgcheck_data_info_t));
        }
    }

    int vertexTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)) & 0x00FFFFFF;
    int polyTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)) & 0x00FFFFFF;
    int polyInfoTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)) & 0x00FFFFFF;
    int cameraDataTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)) & 0x00FFFFFF;
    int waterInfoTableOffset = SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)) & 0x00FFFFFF;

    uint32_t vertexTableSize = 0x6 * bgData->vtx_num;
    uint32_t polyTableSize = 0x10 * bgData->poly_num;
    int16_t polyInfoTableSize = 0; //Biggest value in poly_table info
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
    printf("\rconst z64_bgcheck_vertex_t %sVertexTable[] = {\n", filename);
    thisCnf->flagVtxTable = arraySize;

    if (arraySize > 0)
    {
        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", vertexTable[i].x & 0xffff, vertexTable[i].y & 0xffff, vertexTable[i].z & 0xffff);
        }
    }
    else
    {

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

    thisCnf->flagPolyTable = arraySize;

    if (arraySize > 0)
    {
        printf("const \e[mz64_bgcheck_polygon_t %sPolyTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t{\n\t\t0x%04X,\n", polyTable[i].info & 0xffff);
            printf("\t\t{ 0x%04X, 0x%04X, 0x%04X },\n", polyTable[i].v[0] & 0xffff, polyTable[i].v[1] & 0xffff, polyTable[i].v[2] & 0xffff);
            printf("\t\t0x%04X, 0x%04X, 0x%04X, 0x%04X,\n", polyTable[i].a & 0xffff, polyTable[i].b & 0xffff, polyTable[i].c & 0xffff, polyTable[i].d & 0xffff);
            printf("\t},\n");

            // Find the largest value
            if (polyInfoTableSize < (polyTable[i].info & 0xffff) + 1)
                polyInfoTableSize = (polyTable[i].info & 0xffff) + 1;
        }
        printf("};\n\n");
    }

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Poly Info Table                                                             *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    arraySize = polyInfoTableSize;
    z64_bgcheck_polygon_info_t polyInfoTable[arraySize];
    memcpy(polyInfoTable, zobj + polyInfoTableOffset, (polyInfoTableSize * 8));
    thisCnf->flagPolyInfo = arraySize;

    if (arraySize > 0)
    {
        printf("const z64_bgcheck_polygon_info_t %sPolyInfoTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t{ 0x%08X, 0x%08X },\n", SWAP_LE_BE(SWAP_V64_BE(polyInfoTable[i].info[0])) & 0xFFFFFFFF, SWAP_LE_BE(SWAP_V64_BE(polyInfoTable[i].info[1]))) & 0xFFFFFFFF;
        }
        printf("};\n\n");
    }

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Camera Data Table                                                           *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    arraySize = cameraDataTableSize / 8;
    z64_bgcheck_camera_data_t cameraDataTable[arraySize];
    memcpy(cameraDataTable, zobj + cameraDataTableOffset, cameraDataTableSize);
    thisCnf->flagCameraData = arraySize;

    if (arraySize > 0)
    {
        printf("const z64_bgcheck_camera_data_t %sCameraDataTable[] = {\n", filename);

        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t/* TODO */,\n");
        }
        printf("};\n\n");
    }

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Water Info Table                                                            *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    arraySize = bgData->water_info_num;
    z64_bgcheck_water_info_t waterInfoTable[arraySize];
    memcpy(waterInfoTable, zobj + waterInfoTableOffset, waterInfoTableSize);
    thisCnf->flagWaterInfo = arraySize;

    if (arraySize > 0)
    {
        printf("const z64_bgcheck_water_info_t %sWaterInfoTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            printf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", waterInfoTable[i].min_pos.x & 0xFFFF, waterInfoTable[i].min_pos.y & 0xFFFF, waterInfoTable[i].min_pos.z & 0xFFFF);
            printf("\t0x%04X,\n", waterInfoTable[i].size_x & 0xFFFF);
            printf("\t0x%04X,\n", waterInfoTable[i].size_y & 0xFFFF);
            printf("\t0x%04X,\n", SWAP_LE_BE(SWAP_V64_BE(waterInfoTable[i].info)) & 0xFFFFFFFF);
        }
        printf("};\n\n");
    }

    /* ////////////// */

    printf("const z64_bgcheck_data_info_t %sPolyIdData = {\n", filename);
    printf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", bgData->vtx_min[0] & 0xFFFF, bgData->vtx_min[1] & 0xFFFF, bgData->vtx_min[2] & 0xFFFF);
    printf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", bgData->vtx_max[0] & 0xFFFF, bgData->vtx_max[1] & 0xFFFF, bgData->vtx_max[2] & 0xFFFF);
    printf("\t0x%04X,\n", bgData->vtx_num & 0xFFFF);

    if (thisCnf->flagVtxTable)
    {
        printf("\t&%sVertexTable,\n", filename);
    }
    else
    {
        printf("\t\e[31mNULL\e[m,\n");
    }

    printf("\t0x%04X,\n", bgData->poly_num & 0xFFFF);

    if (thisCnf->flagPolyTable)
    {
        printf("\t&%sPolyTable,\n", filename);
    }
    else
    {
        printf("\t\e[31mNULL\e[m,\n");
    }

    if (thisCnf->flagPolyInfo)
    {
        printf("\t&%sPolyInfoTable,\n", filename);
    }
    else
    {
        printf("\t\e[31mNULL\e[m,\n");
    }

    if (thisCnf->flagCameraData)
    {
        printf("\t&%sCameraDataTable,\n", filename);
    }
    else
    {
        printf("\t\e[31mNULL\e[m,\n");
    }

    printf("\t0x%04X,\n", bgData->water_info_num & 0xFFFF);

    if (thisCnf->flagWaterInfo)
    {
        printf("\t&%sWaterInfoTable,\n", filename);
    }
    else
    {
        printf("\t\e[31mNULL\e[m,\n");
    }

    printf("};\n");
    putchar('\n');

    printf("\e[0;91m/**\n");
    printf(" * TYPE:\t\t OFFSET:\n");
    printf(" * Vertex table\t\t 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)), vertexTableOffset);
    printf(" * Poly table\t\t 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)), polyTableOffset);
    printf(" * PolyInf table\t 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)), polyInfoTableOffset);
    printf(" * CamData table\t 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)), cameraDataTableOffset);
    printf(" * WaterInf table\t 0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)), waterInfoTableOffset);
    printf(" * File size\t\t 0x0000%04X\n", zobjFileSize & 0xFFFF);
    printf(" */\n");

    return 0;
}