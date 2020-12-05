#ifndef _TEXTMAGIC_H_INCLUDED_
#define _TEXTMAGIC_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "types.h"

void print_usage(int c, char *ref, configure *conf, int exit)
{
    if (!conf)
        return;
    if (!conf->printUsageFlag){
        conf->printUsageFlag = 1;
        fprintf(stderr,
                "--- POLY2C --- \n"
                "\e[0;94m[*] "
                "\e[mWritten by: rankaisija <github.com/turpaan64>\n"
                "\e[0;94m[*] "
                "\e[mAdditional contributions by: CrookedPoe <nickjs.site> and z64me <z64.me>\n-- -- -- -- --\n");
    }
    switch (c)
    {
    case 0: /* There is no input file provided. */
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mPlease provide an input file.\n");
        break;
    case 1: /* The provided file is not a valid .zobj. */
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[0;91m%s \e[mis not a valid .zobj file.\n",
                ref);
        break;
    case 2: /* Bad Offset */
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mBad offset.\n");
        break;
    case 3:
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mThe offset goes beyond the filesize.\n");
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mLoad file again with smaller offset value.\n");
        break;
    case 4:
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mDidn't find valid collision at given offset.\n");
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mFinding next collision(s) in the file:\n");
        break;
    case 5:
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mFile has been written succesfully!\n");
        break;
    case 6:
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mUnfortunately couldn't find any collisions in the file.\n");
        break;
    case 7:
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mFound collision.\n");
        break;
    case 8:
        fprintf(stderr,
                "\e[0;94m[*] "
                "\e[m-f\t\tFirst\n"
                "\e[0;94m[*] "
                "\e[m-h\t\tHelp\n"
                "\e[0;94m[*] "
                "\e[m-p\t\tPrint\n"
                "\e[0;94m[*] "
                "\e[mXXXX\tOffset\n-- -- -- -- --\n");
        break;
    }

    if (exit)
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mExiting...\n");
}

int extension_matches(char *in, char *ext)
{
    const char *ss;
    
    /* find last period in string (extension) */
    if (!(ss = strrchr(in, '.')))
        return 0;
    
    /* return non-zero if extension as desired */
    return !strcmp(ss, ext);
}

char *get_filename(char *in)
{
    char *ss;
    char *result;
    
    /* find and skip last '/' or '\' if present */
    if (!(ss = strrchr(in, '/')))
        ss = strrchr(in, '\\');
    if (ss)
        ++ss;
    else
        ss = in;
    
    /* duplicate remainder of string */
    result = strdup(ss);
    
    /* zero-terminate last period (extension) if present */
    if ((ss = strrchr(result, '.')))
        *ss = '\0';

    return result;
}

void writeNull(configure* conf, FILE* writeTo)
{
    if (!conf->confSaveFile) {
        fprintf(writeTo, "\t\e[31mNULL\e[m,\n");
    } else {
        fprintf(writeTo, "\tNULL,\n");
    }
}

#endif
