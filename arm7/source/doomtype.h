#ifndef __DOOMTYPE__
#define __DOOMTYPE__

#include <stdint.h>

#undef true

#define false 0
#define true (!false)

typedef int             dboolean;
typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned long   dword;
typedef unsigned int    rcolor;
typedef int             dtexture;
typedef int8_t          int8;
typedef uint8_t         uint8;
typedef int16_t         int16;
typedef uint16_t        uint16;
typedef int32_t         int32;
typedef uint32_t        uint32;

#include <limits.h>
#define MININT INT_MIN
#define MAXINT INT_MAX
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef BETWEEN
#define BETWEEN(l,u,x) ((l)>(x)?(l):(x)>(u)?(u):(x))
#endif

