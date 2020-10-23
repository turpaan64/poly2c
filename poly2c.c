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

enum {
    false,
    true
} state;

enum {
    Cont = 0,
    Exit = 1,
} exitFlag;

enum {
    notEnoughArguments,
    notZobj,
    pad,
    notWithinFilesize,
    notGoodOffset,
    notFailed,
} usageWarnings;

int main(int argc, char **argv)
{
    configure *thisCnf = malloc(sizeof(configure));
    thisCnf->confSaveFile = true;
    // TODO: copy the results to clipboard
    // TODO: name the xxxxVertexTable[] = {... based on input file
    //       by removing the word collision, if used + make the casing small
    // TODO: Start offset to get the collision for example from vanilla files
    static char *filename = 0;
    static char *extension = ".zobj";
    static char *output_ext = ".h";
    FILE* out;

    // ARGUMENTS
    int offset = 0;
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--first"))
        {
            thisCnf->confFirst = 1;
        }
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            fprintf(stderr,
                    "--- POLY2C --- \n"
                    "\e[0;94m[*] "
                    "\e[mWritten by: rankaisija <github.com/turpaan64>\n"
                    "\e[0;94m[*] "
                    "\e[mAdditional contributions by: CrookedPoe <nickjs.site>\n");
            fprintf(stderr,
                    "--- ARGMNT --- \n"
                    "\e[0;94m[*] "
                    "\e[m-f\t\tFirst\n"
                    "\e[0;94m[*] "
                    "\e[m-h\t\tHelp\n"
                    "\e[0;94m[*] "
                    "\e[m-p\t\tPrint\n"
                    "\e[0;94m[*] "
                    "\e[mXXXX\tOffset\n-- -- -- -- --\n");
            return 1;
        }
        else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--print")) 
        {
            thisCnf->confSaveFile = false;
        }
        else if (i > 1)
        {
            offset = strtol(argv[i], NULL, 16); // Load 2nd argument as a hex
        }
    }

    if (argc < 2)
    {
        print_usage(notEnoughArguments, argv[0], thisCnf, Exit);
        return 1;
    }

    if (check_extension(argv[1], ".zobj"))
    {
        filename = calloc(strlen(argv[1]), 1);
        filename = get_filename(argv[1]);
        //fprintf(stderr, "%s\n", filename);
    }
    else {
        print_usage(notZobj, argv[1], thisCnf, Exit);
        return 1;
    }

    int zobjFileSize = 0;
    unsigned char *zobj = makeFileBuffer(argv[1], 0, &zobjFileSize);
    if (offset > zobjFileSize - sizeof(z64_bgcheck_data_info_t))
    {
        print_usage(notWithinFilesize, argv[1], thisCnf, Exit);
        return 1;
    }

    if ((offset&0x3)) { // Fix alignment if user inputs 0xZZZZZZZY, where Y is either 1, 2 or 3
        int oldOfs = offset;
        for (size_t i = 0; i < 4; i++)
        {
            offset++;
            if (!(offset & 0x3)) {

                break;
            }
        }
    }

    z64_bgcheck_data_info_t bgData[1];
    memcpy(bgData, zobj + offset, sizeof(z64_bgcheck_data_info_t));

    uint32_t testLegit[] = {
        (SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)) & 0xF0F00000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)) & 0xF0F00000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)) & 0xF0F00000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)) & 0xF0F00000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)) & 0xF0F00000)
    };
    
    if (testLegit[0] == 0 && testLegit[1] == 0 && testLegit[2] == 0 && testLegit[3] == 0 && testLegit[4] == 0) {
    } else {
        int stored = offset;
        int res = 0;
        int segmentTest;

        print_usage(notGoodOffset, argv[1], thisCnf, Cont);

        for (int i = 0; (i * 4) < zobjFileSize - stored; i++)
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

                if (segmentTest && bgData->pad == 0 && bgData->pad2 == 0 && testLegit2[0] == 0 && testLegit2[1] == 0 && testLegit2[2] == 0 && testLegit2[3] == 0 && testLegit2[4] == 0)
                {
                    if (!thisCnf->confFirst)
                    {
                        fprintf(stderr, "\e[0;94m[*] "
                                        "\e[mFound at \e[0;91m0x%08X\e[m\n",
                                stored + (i * 4));
                        res += 1;
                        offset += sizeof(z64_bgcheck_polygon_info_t);
                    } else {
                        offset = stored + (i * 4);
                        break;
                    }
                }
            }
        }

        if (!thisCnf->confFirst)
        {
            if (res == 0) {
                fprintf(stderr, "\e[0;94m[*] "
                                "\e[mUnfortunately couldn't find any collisions in the file.\n");
                fprintf(stderr, "\e[0;94m[*] "
                                "\e[mExiting...\n");
                return 1;
            } else {
                
                if (res == 1) {
                    fprintf(stderr, "\e[0;94m[*] "
                                "\e[mFound %d collision.\n", res);
                } else {
                    fprintf(stderr, "\e[0;94m[*] "
                                    "\e[mFound %d collisions.\n",
                            res);
                }
                
                fprintf(stderr, "\e[0;94m[*] "
                                "\e[mExiting...\n");
                return 1;
            }
        } else {
            fprintf(stderr, "\e[0;94m[*] "
                            "\e[mFound collision.\n",
                    res);
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

    
    if (thisCnf->confSaveFile) {
        char** saveFile = strcat(filename, output_ext);
        stdout = fopen(saveFile, "w");

        // return name
        if (check_extension(filename, ".h"))
        {
            filename = calloc(strlen(argv[1]), 1);
            filename = get_filename(argv[1]);
            //fprintf(stderr, "%s\n", filename);
        }
    }

    /* ////////////////////////////////////////////////////////////////////////////*
    *                                                                              *
    *  Vertex Table                                                                *
    *                                                                              *
    * //////////////////////////////////////////////////////////////////////////// */

    int arraySize = bgData->vtx_num;
    z64_bgcheck_vertex_t vertexTable[arraySize];
    memcpy(vertexTable, zobj + vertexTableOffset, vertexTableSize);
    fprintf(stdout, "\rconst z64_bgcheck_vertex_t %sVertexTable[] = {\n", filename);
    thisCnf->flagVtxTable = arraySize;

    if (arraySize > 0)
    {
        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(stdout, "\t{ 0x%04X, 0x%04X, 0x%04X },\n", vertexTable[i].x & 0xffff, vertexTable[i].y & 0xffff, vertexTable[i].z & 0xffff);
        }
    }
    else
    {

        fprintf(stdout, "\t/* EMPTY */\n");
    }
    fprintf(stdout, "};\n\n");

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
        fprintf(stdout, "const z64_bgcheck_polygon_t %sPolyTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(stdout, "\t{\n\t\t0x%04X,\n", polyTable[i].info & 0xffff);
            fprintf(stdout, "\t\t{ 0x%04X, 0x%04X, 0x%04X },\n", polyTable[i].v[0] & 0xffff, polyTable[i].v[1] & 0xffff, polyTable[i].v[2] & 0xffff);
            fprintf(stdout, "\t\t0x%04X, 0x%04X, 0x%04X, 0x%04X,\n", polyTable[i].a & 0xffff, polyTable[i].b & 0xffff, polyTable[i].c & 0xffff, polyTable[i].d & 0xffff);
            fprintf(stdout, "\t},\n");

            // Find the largest value
            if (polyInfoTableSize < (polyTable[i].info & 0xffff) + 1)
                polyInfoTableSize = (polyTable[i].info & 0xffff) + 1;
        }
        fprintf(stdout, "};\n\n");
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
        fprintf(stdout, "const z64_bgcheck_polygon_info_t %sPolyInfoTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(stdout, "\t{ 0x%08X, 0x%08X },\n", SWAP_LE_BE(SWAP_V64_BE(polyInfoTable[i].info[0])) & 0xFFFFFFFF, SWAP_LE_BE(SWAP_V64_BE(polyInfoTable[i].info[1]))) & 0xFFFFFFFF;
        }
        fprintf(stdout, "};\n\n");
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
        fprintf(stdout, "const z64_bgcheck_camera_data_t %sCameraDataTable[] = {\n", filename);

        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(stdout, "\t/* TODO */,\n");
        }
        fprintf(stdout, "};\n\n");
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
        fprintf(stdout, "const z64_bgcheck_water_info_t %sWaterInfoTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(stdout, "\t{ 0x%04X, 0x%04X, 0x%04X },\n", waterInfoTable[i].min_pos.x & 0xFFFF, waterInfoTable[i].min_pos.y & 0xFFFF, waterInfoTable[i].min_pos.z & 0xFFFF);
            fprintf(stdout, "\t0x%04X,\n", waterInfoTable[i].size_x & 0xFFFF);
            fprintf(stdout, "\t0x%04X,\n", waterInfoTable[i].size_y & 0xFFFF);
            fprintf(stdout, "\t0x%04X,\n", SWAP_LE_BE(SWAP_V64_BE(waterInfoTable[i].info)) & 0xFFFFFFFF);
        }
        fprintf(stdout, "};\n\n");
    }

    /* ////////////// */

    fprintf(stdout, "const z64_bgcheck_data_info_t %sPolyIdData = {\n", filename);
    fprintf(stdout, "\t{ 0x%04X, 0x%04X, 0x%04X },\n", bgData->vtx_min[0] & 0xFFFF, bgData->vtx_min[1] & 0xFFFF, bgData->vtx_min[2] & 0xFFFF);
    fprintf(stdout, "\t{ 0x%04X, 0x%04X, 0x%04X },\n", bgData->vtx_max[0] & 0xFFFF, bgData->vtx_max[1] & 0xFFFF, bgData->vtx_max[2] & 0xFFFF);
    fprintf(stdout, "\t0x%04X,\n", bgData->vtx_num & 0xFFFF);

    if (thisCnf->flagVtxTable)
    {
        fprintf(stdout, "\t&%sVertexTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf);
    }

    fprintf(stdout, "\t0x%04X,\n", bgData->poly_num & 0xFFFF);

    if (thisCnf->flagPolyTable)
    {
        fprintf(stdout, "\t&%sPolyTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf);
    }

    if (thisCnf->flagPolyInfo)
    {
        fprintf(stdout, "\t&%sPolyInfoTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf);
    }

    if (thisCnf->flagCameraData)
    {
        fprintf(stdout, "\t&%sCameraDataTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf);
    }

    fprintf(stdout, "\t0x%04X,\n", bgData->water_info_num & 0xFFFF);

    if (thisCnf->flagWaterInfo)
    {
        fprintf(stdout, "\t&%sWaterInfoTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf);
    }

    fprintf(stdout, "};\n");
    putchar('\n');

    if (!thisCnf->confSaveFile){
        fprintf(stdout, "\e[0;91m/**\n");
        fprintf(stdout, " * TYPE:\t\t\tOFFSET:\n");
    } else {
        fprintf(stdout, "/**\n");
        fprintf(stdout, " * TYPE:\t\t\t\tOFFSET:\n");
    }
    fprintf(stdout, " * This Collision\t\t0x%08X\n", offset);
    fprintf(stdout, " * Vertex table\t\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)));
    fprintf(stdout, " * Poly table\t\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)));
    fprintf(stdout, " * PolyInf table\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)));
    fprintf(stdout, " * CamData table\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)));
    fprintf(stdout, " * WaterInf table\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)));
    fprintf(stdout, " * File size\t\t\t0x0000%04X\n", zobjFileSize & 0xFFFF);
    fprintf(stdout, " */\n");

    if (thisCnf->confSaveFile) {
        fclose(stdout);
        print_usage(notFailed, argv[1], thisCnf, Exit);
    }

    return 0;
}