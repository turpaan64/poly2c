#ifndef _TEXTMAGIC_H_INCLUDED_
#define _TEXTMAGIC_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "types.h"

void print_usage(int c, char *ref, configure *conf, int exit)
{
    if (!conf->printUsageFlag){
        conf->printUsageFlag = 1;
        fprintf(stderr,
                "--- POLY2C --- \n"
                "\e[0;94m[*] "
                "\e[mWritten by: rankaisija <github.com/turpaan64>\n"
                "\e[0;94m[*] "
                "\e[mAdditional contributions by: CrookedPoe <nickjs.site>\n-- -- -- -- --\n");
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
    }

    if (exit)
        fprintf(stderr, "\e[0;94m[*] "
                        "\e[mExiting...\n");
}

int check_extension(char *in, char *ext)
{
    int idx = 0; /* Index of `.` character. */
    char *check; /* String to test against. */

    /* Search backwards for the most recent occurence of `.` */
    for (int i = strlen(in); i > 0; i--)
    {
        if (in[i] == '.')
        {
            idx = i;
            break;
        }
    }

    if (idx) /* If idx != 0, check extension. */
    {
        check = calloc(strlen(in) + 1, 1);
        memcpy(check, (in + idx), idx);

        if (!strcmp(ext, check)) /* If ext == check, return index of position. (true == non-zero) */
            return (strlen(in) - idx);
        else
            return 0;
    }
    else
        return 0;
}

char *get_filename(char *in)
{
    int head = 0;
    int tail = 0;
    char *new;

    //fprintf(stderr, "in = %s\n", in);
    /* Search backwards for the most recent occurence of `.` */
    for (int i = strlen(in); i > 0; i--)
    {
        if (in[i] == '.')
        {
            tail = i;
            break;
        }
    }
    //fprintf(stderr, "tail = %d\n", tail);

    /* Search backwards for the most recent occurence of `/` or `\` */
    for (int i = strlen(in); i > 0; i--)
    {
        if (in[i] == '/' || !strcmp(in + i, "\\"))
        {
            head = i + 1;
            break;
        }
    }
    //fprintf(stderr, "head = %d\n", head);

    /* Allocate space for new string, and grab characters between head and tail. */
    new = malloc(sizeof(char) * strlen(in));
    for (int i = head; i < (strlen(in) - (strlen(in) - tail)); i++)
    {
        if (in[i] == '.')
            new[i - head] = '_';
        else
            new[i - head] = in[i];
        //fprintf(stderr, "new[%d] = %c\n", (i - head), in[i]);
    }

    return new;
}

void writeNull(configure* conf)
{
    if (!conf->confSaveFile) {
        fprintf(stdout, "\t\e[31mNULL\e[m,\n");
    } else {
        fprintf(stdout, "\tNULL,\n");
    }
}

#endif