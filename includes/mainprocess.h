#ifndef _MAINPROC_H_INCLUDED_
#define _MAINPROC_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "data.h"
#include "file.h"
#include "types.h"
#include "textmagic.h"

void mainProcess(configure *thisCnf, z64_bgcheck_data_info_t *bgData, FILE *writeTo, char *zobj, char *filename, int *offset, int *zobjFileSize)
{
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
    fprintf(writeTo, "\rconst z64_bgcheck_vertex_t %sVertexTable[] = {\n", filename);
    thisCnf->flagVtxTable = arraySize;

    if (arraySize > 0)
    {
        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(writeTo, "\t{ 0x%04X, 0x%04X, 0x%04X },\n", vertexTable[i].x & 0xffff, vertexTable[i].y & 0xffff, vertexTable[i].z & 0xffff);
        }
    }
    else
    {

        fprintf(writeTo, "\t/* EMPTY */\n");
    }
    fprintf(writeTo, "};\n\n");

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
        fprintf(writeTo, "const z64_bgcheck_polygon_t %sPolyTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(writeTo, "\t{\n\t\t0x%04X,\n", polyTable[i].info & 0xffff);
            fprintf(writeTo, "\t\t{ 0x%04X, 0x%04X, 0x%04X },\n", polyTable[i].v[0] & 0xffff, polyTable[i].v[1] & 0xffff, polyTable[i].v[2] & 0xffff);
            fprintf(writeTo, "\t\t0x%04X, 0x%04X, 0x%04X, 0x%04X,\n", polyTable[i].a & 0xffff, polyTable[i].b & 0xffff, polyTable[i].c & 0xffff, polyTable[i].d & 0xffff);
            fprintf(writeTo, "\t},\n");

            // Find the largest value
            if (polyInfoTableSize < (polyTable[i].info & 0xffff) + 1)
                polyInfoTableSize = (polyTable[i].info & 0xffff) + 1;
        }
        fprintf(writeTo, "};\n\n");
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
        fprintf(writeTo, "const z64_bgcheck_polygon_info_t %sPolyInfoTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(writeTo, "\t{ 0x%08X, 0x%08X },\n", SWAP_LE_BE(SWAP_V64_BE(polyInfoTable[i].info[0])) & 0xFFFFFFFF, SWAP_LE_BE(SWAP_V64_BE(polyInfoTable[i].info[1]))) & 0xFFFFFFFF;
        }
        fprintf(writeTo, "};\n\n");
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
        fprintf(writeTo, "const z64_bgcheck_camera_data_t %sCameraDataTable[] = {\n", filename);

        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(writeTo, "\t/* TODO */,\n");
        }
        fprintf(writeTo, "};\n\n");
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
        fprintf(writeTo, "const z64_bgcheck_water_info_t %sWaterInfoTable[] = {\n", filename);
        for (int32_t i = 0; i < arraySize; i++)
        {
            fprintf(writeTo, "\t{ 0x%04X, 0x%04X, 0x%04X },\n", waterInfoTable[i].min_pos.x & 0xFFFF, waterInfoTable[i].min_pos.y & 0xFFFF, waterInfoTable[i].min_pos.z & 0xFFFF);
            fprintf(writeTo, "\t0x%04X,\n", waterInfoTable[i].size_x & 0xFFFF);
            fprintf(writeTo, "\t0x%04X,\n", waterInfoTable[i].size_y & 0xFFFF);
            fprintf(writeTo, "\t0x%04X,\n", SWAP_LE_BE(SWAP_V64_BE(waterInfoTable[i].info)) & 0xFFFFFFFF);
        }
        fprintf(writeTo, "};\n\n");
    }

    /* ////////////// */

    fprintf(writeTo, "const z64_bgcheck_data_info_t %sPolyIdData = {\n", filename);
    fprintf(writeTo, "\t{ 0x%04X, 0x%04X, 0x%04X },\n", bgData->vtx_min[0] & 0xFFFF, bgData->vtx_min[1] & 0xFFFF, bgData->vtx_min[2] & 0xFFFF);
    fprintf(writeTo, "\t{ 0x%04X, 0x%04X, 0x%04X },\n", bgData->vtx_max[0] & 0xFFFF, bgData->vtx_max[1] & 0xFFFF, bgData->vtx_max[2] & 0xFFFF);
    fprintf(writeTo, "\t0x%04X,\n", bgData->vtx_num & 0xFFFF);

    if (thisCnf->flagVtxTable)
    {
        fprintf(writeTo, "\t&%sVertexTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf, writeTo);
    }

    fprintf(writeTo, "\t0x%04X,\n", bgData->poly_num & 0xFFFF);

    if (thisCnf->flagPolyTable)
    {
        fprintf(writeTo, "\t&%sPolyTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf, writeTo);
    }

    if (thisCnf->flagPolyInfo)
    {
        fprintf(writeTo, "\t&%sPolyInfoTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf, writeTo);
    }

    if (thisCnf->flagCameraData)
    {
        fprintf(writeTo, "\t&%sCameraDataTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf, writeTo);
    }

    fprintf(writeTo, "\t0x%04X,\n", bgData->water_info_num & 0xFFFF);

    if (thisCnf->flagWaterInfo)
    {
        fprintf(writeTo, "\t&%sWaterInfoTable,\n", filename);
    }
    else
    {
        writeNull(thisCnf, writeTo);
    }

    fprintf(writeTo, "};\n");
    putchar('\n');

    if (!thisCnf->confSaveFile)
    {
        fprintf(writeTo, "\e[0;91m/**\n");
        fprintf(writeTo, " * TYPE:\t\t\tOFFSET:\n");
    }
    else
    {
        fprintf(writeTo, "/**\n");
        fprintf(writeTo, " * TYPE:\t\t\t\tOFFSET:\n");
    }
    fprintf(writeTo, " * This Collision\t\t0x%08X\n", *offset);
    fprintf(writeTo, " * Vertex table\t\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->vtx_table)));
    fprintf(writeTo, " * Poly table\t\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->poly_table)));
    fprintf(writeTo, " * PolyInf table\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->poly_info_table)));
    fprintf(writeTo, " * CamData table\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->camera_data_table)));
    fprintf(writeTo, " * WaterInf table\t\t0x%08X\n", SWAP_LE_BE(SWAP_V64_BE(bgData->water_info_table)));
    fprintf(writeTo, " * File size\t\t\t0x0000%04X\n", *zobjFileSize & 0xFFFF);
    fprintf(writeTo, " */\n");
}

#endif