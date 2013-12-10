#ifndef __Z_ZONE__
#define __Z_ZONE__

#undef strdup

// Include system definitions so that prototypes become
// active before macro replacements below are in effect.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "doomtype.h"

// ZONE MEMORY

// PU - purge tags.
enum 
{
   PU_STATIC,  // block is static (remains until explicitly freed)
   PU_MAPLUMP, // block is allocated for data stored in map wads
   PU_AUTO,
   PU_AUDIO,   // allocation of midi data
   PU_LEVEL,   // allocation belongs to level (freed at next level load)
   PU_LEVSPEC, // used for thinker_t's (same as PU_LEVEL basically)
   PU_CACHE,   // block is cached (may be implicitly freed at any time!)
   PU_MAX,     // Must always be last -- killough
   PU_NEWBLOCK
};

#define PU_PURGELEVEL PU_CACHE        /* First purgable tag's level */
#define PU_FREE -1

void*   (Z_Malloc)(int size, int tag, void *user, const char *, int);
void    (Z_Free)(void *ptr, const char *, int);
void    (Z_FreeTags)(int lowtag, int hightag, const char *, int);
void    (Z_ChangeTag)(void *ptr, int tag, const char *, int);
void    (Z_Init)(void);
void*   (Z_Calloc)(int n, int tag, void *user, const char *, int);
void*   (Z_Realloc)(void *ptr, int size, int tag, void *user, const char *, int);
char*   (Z_Strdup)(const char *s, int tag, void *user, const char *, int);
char*   (Z_Strdupa)(const char *s, const char *file, int line);
void*   (Z_Alloca)(int n, const char *file, int line);
void    (Z_FreeAlloca)(const char *file, int line);
void    (Z_CheckHeap)(const char *,int);   // killough 3/22/98: add file/line info
int     (Z_CheckTag)(void *,const char *,int);
void    (Z_Touch)(void *ptr, const char *, int);

#define Z_Free(a)           (Z_Free)        (a,      __FILE__,__LINE__)
#define Z_FreeTags(a,b)     (Z_FreeTags)    (a,b,    __FILE__,__LINE__)
#define Z_ChangeTag(a,b)    (Z_ChangeTag)   (a,b,    __FILE__,__LINE__)
#define Z_Malloc(a,b,c)     (Z_Malloc)      (a,b,c,  __FILE__,__LINE__)
#define Z_Strdup(a,b,c)     (Z_Strdup)      (a,b,c,  __FILE__,__LINE__)
#define Z_Strdupa(a)        (Z_Strdupa)     (a,      __FILE__,__LINE__)
#define Z_Calloc(a,b,c)     (Z_Calloc)      (a,b,c,  __FILE__,__LINE__)
#define Z_Realloc(a,b,c,d)  (Z_Realloc)     (a,b,c,d,__FILE__,__LINE__)
#define Z_Alloca(a)         (Z_Alloca)      (a,      __FILE__,__LINE__)
#define Z_CheckHeap()       (Z_CheckHeap)   (        __FILE__,__LINE__)
#define Z_CheckTag(a)       (Z_CheckTag)    (a,      __FILE__,__LINE__)
#define Z_Touch(a)          (Z_Touch)       (a,      __FILE__,__LINE__)
#define Z_FreeAlloca()      (Z_FreeAlloca)  (        __FILE__,__LINE__)

#define strdup(s)           (Z_Strdup) (s, PU_STATIC,0,__FILE__,__LINE__)

int Z_TagUsage(int tag);
int Z_FreeMemory(void);

typedef struct vramblock_s vramblock_t;

struct vramblock_s
{
    int size;
    short id;
    short tag;
    int prevtic;
    void **gfx;
    byte *block;
    vramblock_t *prev;
    vramblock_t *next;
};

typedef struct
{
    int size;               // total bytes malloced, including header
    int free;
    vramblock_t blocklist;  // start / end cap for linked list
    vramblock_t* rover;
} vramzone_t;

extern vramzone_t* vramzone;

void Z_VFree(vramzone_t* vram, vramblock_t* block);
vramblock_t* Z_VAlloc(vramzone_t* vram, int size, int tag, void* gfx);
void Z_SetVAllocList(vramzone_t* vram);
void Z_VTouch(vramzone_t* vram, vramblock_t *block);
int Z_FreeVMemory(vramzone_t* vram);

#endif


