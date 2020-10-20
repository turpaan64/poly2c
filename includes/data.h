#ifndef _DATA_H_INCLUDED_
#define _DATA_H_INCLUDED_

#define CSTR_DEBUG 0
#define CSTR_ERROR 1
#define CSTR_NOTE 2

/* Convert 32-bit word to big or little endian. */
#define SWAP_LE_BE(x) ((((x) & 0x000000FF) << 24) | (((x) & 0x0000FF00) << 8) | (((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24))

#define SWAP_V64_BE(x) ((((x) & 0xFF000000) >> 8) | (((x) & 0x00FF0000) << 8) | (((x) & 0x0000FF00) >> 8) | (((x) & 0x000000FF) << 8))

/* Access an arbitrary value in memory. */
#define AVAL(base, type, offset)  (*(type*)((unsigned char*)(base)+(offset)))

static inline unsigned long long toBits(unsigned long long n)
{
    unsigned long long r = 0;
    unsigned long long x = 1;

    while (n)
    {
        r += (n & 1) * x;
        x *= 10;
        n >>= 1;
    }

    return r;
}

/* Customized strings. */
#if defined(_CONSOLE_COLORS_H_INCLUDED_)
#define debug_printf(str, mode) (debug_printf)((str), (mode), __FILE__, __LINE__)
static inline void (debug_printf)(const char* str, int mode, char* file, int line)
{
    switch(mode)
    {
        case CSTR_DEBUG:
            setTextColor(CC_FOREGROUND, CC_GREEN, CC_BRIGHT);
            fprintf(stderr, "[DEBUG]");
            break;
        case CSTR_ERROR:
            setTextColor(CC_FOREGROUND, CC_RED, CC_BRIGHT);
            fprintf(stderr, "[ERROR]");
            break;
        case CSTR_NOTE:
            setTextColor(CC_FOREGROUND, CC_YELLOW, CC_BRIGHT);
            fprintf(stderr, "[NOTE]");
            break;
    }
    resetText();
    fprintf(stderr, ": (");
    setTextColor(CC_FOREGROUND, CC_CYAN, CC_BRIGHT);
    fprintf(stderr, "File %s at line %d", file, line);
    resetText();
    fprintf(stderr, ") | %s", str);
}
#endif



#endif