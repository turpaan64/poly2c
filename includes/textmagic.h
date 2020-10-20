#ifndef _TEXTMAGIC_H_INCLUDED_
#define _TEXTMAGIC_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *extract_filename(char *str)
{
    int ch = '//';
    size_t len;
    char *pdest;
    char *inpfile = NULL;

    // Search backwards for last backslash in filepath
    pdest = strrchr(str, ch);

    // if backslash not found in filepath
    if (pdest == NULL)
    {
        pdest = str; // The whole name is a file in current path?
    }
    else
    {
        pdest++; // Skip the backslash itself.
    }

    // extract filename from file path
    len = strlen(pdest);
    inpfile = malloc(len + 1);        // Make space for the zero.
    strncpy(inpfile, pdest, len + 1); // Copy including zero.
    return inpfile;
}

char *extract_filenameWindows(char *str)
{
    int ch = '\\';
    size_t len;
    char *pdest;
    char *inpfile = NULL;

    // Search backwards for last backslash in filepath
    pdest = strrchr(str, ch);

    // if backslash not found in filepath
    if (pdest == NULL)
    {
        pdest = str; // The whole name is a file in current path?
    }
    else
    {
        pdest++; // Skip the backslash itself.
    }

    // extract filename from file path
    len = strlen(pdest);
    inpfile = malloc(len + 1);        // Make space for the zero.
    strncpy(inpfile, pdest, len + 1); // Copy including zero.
    return inpfile;
}
/* Function returns the index of str where word is found */
int search(char str[], char word[])
{
    int l, i, j;

    /* finding length of word */
    for (l = 0; word[l] != '\0'; l++)
        ;

    for (i = 0, j = 0; str[i] != '\0' && word[j] != '\0'; i++)
    {
        if (str[i] == word[j])
        {
            j++;
        }
        else
        {
            j = 0;
        }
    }

    if (j == l)
    {
        /* substring found */
        return (i - j);
    }
    else
    {
        return -1;
    }
}

int delete_word(char str[], char word[], int index)
{
    int i, l;

    /* finding length of word */
    for (l = 0; word[l] != '\0'; l++)
        ;

    for (i = index; str[i] != '\0'; i++)
    {
        str[i] = str[i + l + 1];
    }
}

#endif