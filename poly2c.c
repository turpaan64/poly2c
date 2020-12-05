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
#include "includes/mainprocess.h"

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
    configure thisCnf = {0};
    thisCnf.confSaveFile = true;
    thisCnf.confFirst = false;
    // TODO: copy the results to clipboard
    // TODO: name the xxxxVertexTable[] = {... based on input file
    //       by removing the word collision, if used + make the casing small
    // TODO: Start offset to get the collision for example from vanilla files
    static char *filename = 0;
    static char *extension = ".zobj";
    static char *output_ext = ".h";
    FILE* out = 0;

    // ARGUMENTS
    int offset = 0;
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--first"))
        {
            thisCnf.confFirst = 1;
        }
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            fprintf(stderr,
                    "--- POLY2C --- \n"
                    "\e[0;94m[*] "
                    "\e[mWritten by: rankaisija <github.com/turpaan64>\n"
                    "\e[0;94m[*] "
                    "\e[mAdditional contributions by: CrookedPoe <nickjs.site> and z64me <z64.me>\n");
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
            thisCnf.confSaveFile = false;
        }
        else if (i > 1)
        {
            offset = strtol(argv[i], NULL, 16); // Load 2nd argument as a hex
        }
    }

    if (argc < 2)
    {
        print_usage(notEnoughArguments, argv[0], &thisCnf, Exit);
        return 1;
    }

    if (extension_matches(argv[1], ".zobj"))
    {
        filename = get_filename(argv[1]);
        //fprintf(stderr, "%s\n", filename);
    }
    else {
        print_usage(notZobj, argv[1], &thisCnf, Exit);
        return 1;
    }

    int zobjFileSize = 0;
    unsigned char *zobj = makeFileBuffer(argv[1], 0, &zobjFileSize);
    if (!zobj)
        return 1;
    if (offset >= zobjFileSize - sizeof(z64_bgcheck_data_info_t))
    {
        print_usage(notWithinFilesize, argv[1], &thisCnf, Exit);
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

    z64_bgcheck_data_info_t bgData;
    memcpy(&bgData, zobj + offset, sizeof(z64_bgcheck_data_info_t));

    uint32_t testLegit[] = {
        (SWAP_LE_BE(SWAP_V64_BE(bgData.vtx_table)) & 0xF0F00000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData.poly_table)) & 0xF0F00000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData.poly_info_table)) & 0xF0F00000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData.camera_data_table)) & 0xF0F00000),
        (SWAP_LE_BE(SWAP_V64_BE(bgData.water_info_table)) & 0xF0F00000)
    };
    
    if (testLegit[0] == 0 && testLegit[1] == 0 && testLegit[2] == 0 && testLegit[3] == 0 && testLegit[4] == 0) {
    } else {
        int stored = offset;
        int res = 0;
        int segmentTest;

        print_usage(notGoodOffset, argv[1], &thisCnf, Cont);

        for (int i = 0; (i * 4) < zobjFileSize - stored; i++)
        {
            memcpy(&bgData, zobj + stored + (i * 4), sizeof(z64_bgcheck_data_info_t));

            uint32_t testLegit2[] = {
                (SWAP_LE_BE(SWAP_V64_BE(bgData.vtx_table)) & 0xFFF00000),
                (SWAP_LE_BE(SWAP_V64_BE(bgData.poly_table)) & 0xFFF00000),
                (SWAP_LE_BE(SWAP_V64_BE(bgData.poly_info_table)) & 0xFFF00000),
                (SWAP_LE_BE(SWAP_V64_BE(bgData.camera_data_table)) & 0xF0F00000),
                (SWAP_LE_BE(SWAP_V64_BE(bgData.water_info_table)) & 0xF0F00000)
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

            if (segmentTest && bgData.pad == 0 && bgData.pad2 == 0 && testLegit2[0] == 0 && testLegit2[1] == 0 && testLegit2[2] == 0 && testLegit2[3] == 0 && testLegit2[4] == 0)
            {
                if (!thisCnf.confFirst)
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

        if (!thisCnf.confFirst)
        {
            if (res == 0) {
                print_usage(6, argv[1], &thisCnf, Exit);
                return 1;
            } else {

                fprintf(stderr, "\e[0;94m[*] "
                                    "\e[mFound %d collisions.\n",
                            res);
                print_usage(200, argv[1], &thisCnf, Exit); //Just Exit
                return 1;
            }
        } else {
            print_usage(7, argv[1], &thisCnf, Cont);
            memcpy(&bgData, zobj + offset, sizeof(z64_bgcheck_data_info_t));
        }
    }

 
    if (thisCnf.confSaveFile)
    {
        /* resize filename so extension can be appended */
        filename = realloc(filename, strlen(filename) + strlen(output_ext) + 1);
        char* saveFile = strcat(filename, output_ext);
        out = fopen(saveFile, "w");

        // return name
        if (extension_matches(filename, ".h"))
        {
            free(filename);
            filename = get_filename(argv[1]);
            //fprintf(stderr, "%s\n", filename);
        }
        // Write to a file
        mainProcess(&thisCnf, &bgData, out, zobj, filename, &offset, &zobjFileSize);
    } else {
        // Print out
        mainProcess(&thisCnf, &bgData, stdout, zobj, filename, &offset, &zobjFileSize);
    }
    
    /* cleanup */
    if (filename)
        free(filename);
    if (zobj)
        free(zobj);


    

    if (thisCnf.confSaveFile) {
        if (out)
            fclose(out);
        print_usage(notFailed, argv[1], &thisCnf, Exit);
    }

    return 0;
}
