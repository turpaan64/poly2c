#ifndef _FILE_H_INCLUDED_
#define _FILE_H_INCLUDED_

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#define READ "rb"
#define WRITE "wb"
#define APPEND "ab"
#define READ_WRITE "rb+"
#define READ_WRITE_EMPTY "wb+"
#define READ_APPEND "ab+"

#define ENDIAN_BIG    0
#define ENDIAN_LITTLE 1

static inline int getFileSize(FILE* f)
{
    int fileSize = 0;
    fseek(f, 0, SEEK_END);
    fileSize = (int)ftell(f);
    rewind(f);
    return fileSize;
}

static inline FILE* openFile(const char* in, const char* mode, int* fileSizeDest)
{
    FILE* f;
    if (!(f = fopen(in, mode)))
    {
        f = NULL;
        *fileSizeDest = 0;
        fprintf(stderr, "fopen('%s', '%s') failed\n", in, mode);
        //debug_printf("There was a problem accessing the file.\n", CSTR_ERROR);
        return 0;
    }
    *fileSizeDest = getFileSize(f);
    return f;
}

#if 0
/* Kind of broken. Do not use. */
static inline char** readAllLines(const char* path, int lineSize, int* fileSizeDest, int* totalLines)
{
    int i, l;
    int fileSize = 0;
    char lineBuffer[lineSize];
    char** result = 0;
    FILE* f = openFile(path, READ, &fileSize);

    /* First Pass, Count Lines */
    while(fgets(lineBuffer, lineSize, f) != NULL) { l++; }
    //if (l <= 0) { printf("Lines not counted.\nExiting...\n"); return 1; }

    /* Second Pass, Reset File, Allocate Lines*/
    char* lines[l];
    fseek(f, 0, SEEK_SET);
    while(fgets(lineBuffer, lineSize, f) != NULL)
    {
        //printf("lines[%d] = %s", i, lineBuffer);
        lines[i] = strdup(lineBuffer);
        i++;
    }
    *totalLines = l;
    result = (char**)malloc(sizeof(char*) * l);
    fclose(f);

    for (i = 0; i < l; i++)
    {
        *(result + i) = strdup(lines[i]);
    }

    return result;
}
#endif

/* Return `input` as an array of bytes and store its file size to an integer. */
static inline unsigned char* makeFileBuffer(const char* path, int fileEndian, int* fileSizeDest)
{
    unsigned char *buf = 0; /* Initialize to 0 */
    int fileSize = 0;
    FILE* f = openFile(path, READ, &fileSize);
    if (!f)
        goto L_oof;

    if ((buf = (unsigned char*)malloc(fileSize)))
    {
        if (buf && fread(buf, 1, fileSize, f))
        {
            *fileSizeDest = fileSize;

            if (fileEndian == ENDIAN_BIG)
            {
                for (int i = 0; i < fileSize; i += 4)
                {
                    
                        AVAL(buf, unsigned int, i) = SWAP_V64_BE(AVAL(buf, unsigned int, i));
                    
                    
                }
            }

            fclose(f);
            return buf;
        }
        else
        {
L_oof:
            fprintf(stderr, "Failed to read file contents...\n");
            //debug_printf("Failed to read file contents...\n", CSTR_ERROR);
            free(buf);
            *fileSizeDest = 0;
            if (f)
                fclose(f);
            return NULL;
        }
    }
    else
    {
        fprintf(stderr, "Buffer is not allocated!\n");
        //debug_printf("Buffer is not allocated!\n", CSTR_ERROR);
        *fileSizeDest = 0;
        fclose(f);
        return NULL;
    }
}

#endif
