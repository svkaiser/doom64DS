#ifndef __W_WAD__
#define __W_WAD__

#include "d_main.h"
#include "w_file.h"
#include "w_merge.h"

//
// WADFILE I/O related stuff.
//
typedef struct
{
    char        name[8];
    wad_file_t* wadfile;
    int         position;
    int         size;
    int         next;
    int         index;
    void*       cache;
} lumpinfo_t;

extern	lumpinfo_t*	lumpinfo;
extern	int			numlumps;

void            W_Init(void);
wad_file_t*     W_AddFile(char *filename);
unsigned int    W_HashLumpName(const char* str);
int             W_CheckNumForName(const char* name);
int             W_FindNumForName(const char* match);
int             W_GetNumForName(const char* name);
int             W_LumpLength(int lump);
void            W_ReadLump(int lump, void *dest);
void*           W_GetMapLump(int lump);
void            W_CacheMapLump(int map);
void            W_FreeMapLump(void);
int             W_MapLumpLength(int lump);
void*           W_CacheLumpNum(int lump, int tag);
void*           W_CacheLumpName(const char* name, int tag);


#endif