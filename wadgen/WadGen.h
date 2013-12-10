#ifndef _WADGEN_H_
#define _WADGEN_H_

#ifdef _WIN32
#include "SDL_config.h"
#else
#include <stdint.h>
#endif

//#define PLATFORM_PC
#define PLATFORM_DS

#ifdef _WIN32
#include <windows.h>
#include <commctrl.h>
#include <resource.h>
#include <rpcdce.h>
#include <io.h>
#else
#include <limits.h>
#define MAX_PATH PATH_MAX
#define ZeroMemory(a,l) memset(a, 0, l)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "Mem.h"

//#define USE_SOUNDFONTS

#pragma warning(disable:4996)

typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned int    uint;
typedef int32_t         int32;
typedef uint32_t        uint32;
typedef int             bool;
typedef byte*			cache;
typedef char			path[MAX_PATH];

#define false	0
#define true	1

typedef struct
{
    byte r;
    byte g;
    byte b;
    byte a;
} dPalette_t;

#define RGBDS(r,g,b)  (((r)>>3)|(((g)>>3)<<5)|(((b)>>3)<<10))

#ifdef _WIN32
extern HWND	hwnd;
extern HWND	hwndWait;
#endif
extern int myargc;
extern char** myargv;

int WGen_Swap16(int x);
uint WGen_Swap32(unsigned int x);

#define	_SWAP16(x)	WGen_Swap16(x)
#define _SWAP32(x)	WGen_Swap32(x)
#define _PAD4(x)	x += (4 - ((uint) x & 3)) & 3
#define _PAD8(x)	x += (8 - ((uint) x & 7)) & 7
#define _PAD16(x)	x += (16 - ((uint) x & 15)) & 15

void WGen_Printf(char* s, ...);
void WGen_Complain(char *fmt, ...);
void WGen_UpdateProgress(char *fmt, ...);
void WGen_ConvertN64Pal(dPalette_t* palette, word* data, int indexes);
void WGen_AddDigest(char* name, int lump, int size);
int WGen_GetSizeDS(int size);

#define TOTALSTEPS	3500

#ifndef _WIN32
static inline char* strupr(char* in)
{
    char* ptr = in;
    while(*ptr != '\0')
    {
        *ptr = toupper(*ptr);
        ptr++;
    }
    return in;
}
#endif

#include "MD5.h"

#endif
