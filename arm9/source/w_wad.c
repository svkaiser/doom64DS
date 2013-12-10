#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ds.h"
#include "doomdef.h"
#include "z_zone.h"
#include "w_wad.h"

//
// TYPES
//
typedef struct
{
    // Should be "IWAD" or "PWAD".
    char		identification[4];
    int			numlumps;
    int			infotableofs;
} PACKEDATTR wadinfo_t;


typedef struct
{
    int			filepos;
    int			size;
    char		name[8];
} PACKEDATTR filelump_t;

lumpinfo_t*	lumpinfo;
int			numlumps;

void ExtractFileBase(char* path, char* dest)
{
    char*	src;
    int		length;
    
    src = path + strlen(path) - 1;
    
    // back up until a \ or the start
    while (src != path
        && *(src-1) != '\\'
        && *(src-1) != '/')
    {
        src--;
    }
    
    // copy up to eight characters
    memset(dest,0,8);
    length = 0;
    
    while (*src && *src != '.')
    {
        if (++length == 9)
            I_Error ("Filename base of %s >8 chars",path);
        
        *dest++ = toupper((int)*src++);
    }
}

//
// W_HashLumpName
//

unsigned int W_HashLumpName(const char* str)
{
    unsigned int hash = 1315423911;
    unsigned int i    = 0;
    
    for(i = 0; i < 8 && *str != '\0'; str++, i++)
        hash ^= ((hash << 5) + toupper((int)*str) + (hash >> 2));
    
    return hash;
}

//
// W_HashLumps
//

static void W_HashLumps(void)
{
    int i;
    unsigned int hash;
    
    for(i = 0; i < numlumps; i++)
    {
        lumpinfo[i].index = -1;
        lumpinfo[i].next = -1;
    }
    
    for(i = 0; i < numlumps; i++)
    {
        hash = (W_HashLumpName(lumpinfo[i].name) % numlumps);
        lumpinfo[i].next = lumpinfo[hash].index;
        lumpinfo[hash].index = i;
    }
}

//
// W_Init
//

void W_Init(void)
{
    char*           iwad;
    wadinfo_t       header;
    lumpinfo_t*     lump_p;
    int             i;
    wad_file_t*     wadfile;
    int             length;
    int             startlump;
    filelump_t*     fileinfo;
    filelump_t*     filerover;
    //int             p;
    
    // open the file and add to directory
    iwad = W_FindIWAD();

    if(iwad == NULL)
        I_Error("W_Init: IWAD not found");

    wadfile = W_OpenFile(iwad);
    
    W_Read(wadfile, 0, &header, sizeof(header));
    
    if(strnicmp(header.identification,"IWAD",4))
        I_Error("W_Init: Invalid main IWAD id");
    
    numlumps = 0;
    lumpinfo = malloc(1); // Will be realloced as lumps are added
    
    startlump = numlumps;
    
    header.numlumps = header.numlumps;
    header.infotableofs = header.infotableofs;
    length = header.numlumps*sizeof(filelump_t);
    fileinfo = Z_Malloc(length, PU_STATIC, 0);
    
    W_Read(wadfile, header.infotableofs, fileinfo, length);
    numlumps += header.numlumps;
    
    // Fill in lumpinfo
    lumpinfo = realloc(lumpinfo, numlumps*sizeof(lumpinfo_t));
    memset(lumpinfo, 0, numlumps * sizeof(lumpinfo_t));
    
    if(!lumpinfo)
        I_Error("W_Init: Couldn't realloc lumpinfo");
    
    lump_p = &lumpinfo[startlump];
    filerover = fileinfo;
    
    for(i = startlump; i < numlumps; i++, lump_p++, filerover++)
    {
        lump_p->wadfile = wadfile;
        lump_p->position = filerover->filepos;
        lump_p->size = filerover->size;
        lump_p->cache = NULL;
        memcpy(lump_p->name, filerover->name, 8);
    }
    
    if(!numlumps)
        I_Error("W_Init: no files found");
    
    lumpinfo = realloc(lumpinfo, numlumps*sizeof(lumpinfo_t));

    Z_Free(fileinfo);

    /*p = M_CheckParm("-file");
    if(p)
    {
        // the parms after p are wadfile/lump names,
        // until end of parms or another - preceded parm
        while(++p != myargc && myargv[p][0] != '-')
        {
            char *filename;
            filename = W_TryFindWADByName(myargv[p]);
            W_MergeFile(filename);
        }
    }*/

    W_HashLumps();
}

//
// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.

wad_file_t *W_AddFile(char *filename)
{
    wadinfo_t   header;
    lumpinfo_t* lump_p;
    int         i;
    wad_file_t* wadfile;
    int         length;
    int         startlump;
    filelump_t* fileinfo;
    filelump_t* filerover;
    
    // open the file and add to directory

    wadfile = W_OpenFile(filename);
		
    if(wadfile == NULL)
    {
	    I_Printf("W_AddFile: couldn't open %s\n", filename);
	    return NULL;
    }

    I_Printf("W_AddFile: Adding %s\n", filename);

    startlump = numlumps;
	
    if(strcasecmp(filename+strlen(filename)-3 , "wad"))
    {
	    // single lump file

        // fraggle: Swap the filepos and size here.  The WAD directory
        // parsing code expects a little-endian directory, so will swap
        // them back.  Effectively we're constructing a "fake WAD directory"
        // here, as it would appear on disk.

        fileinfo = Z_Malloc(sizeof(filelump_t), PU_STATIC, 0);
        fileinfo->filepos = 0;
        fileinfo->size = wadfile->length;

        // Name the lump after the base of the filename (without the
        // extension).

        ExtractFileBase(filename, fileinfo->name);
        numlumps++;
    }
    else 
    {
        // WAD file
        W_Read(wadfile, 0, &header, sizeof(header));

        if(strncmp(header.identification,"PWAD",4) &&
            strncmp(header.identification,"IWAD",4))
            I_Error("W_AddFile: Wad file %s doesn't have valid IWAD or PWAD id\n", filename);

        header.numlumps = header.numlumps;
        header.infotableofs = header.infotableofs;
        length = header.numlumps*sizeof(filelump_t);
        fileinfo = Z_Malloc(length, PU_STATIC, 0);
        
        W_Read(wadfile, header.infotableofs, fileinfo, length);
        numlumps += header.numlumps;
    }

    // Fill in lumpinfo
    lumpinfo = realloc(lumpinfo, numlumps * sizeof(lumpinfo_t));

    if(lumpinfo == NULL)
        I_Error("W_AddFile: Couldn't realloc lumpinfo");

    lump_p = &lumpinfo[startlump];
	
    filerover = fileinfo;

    for(i = startlump; i < numlumps; ++i)
    {
        lump_p->wadfile = wadfile;
        lump_p->position = filerover->filepos;
        lump_p->size = filerover->size;
        lump_p->cache = NULL;
        memcpy(lump_p->name, filerover->name, 8);

        ++lump_p;
        ++filerover;
    }
	
    Z_Free(fileinfo);

    W_HashLumps();

    return wadfile;
}

//
// W_CheckNumForName
// Returns -1 if name not found.
//

int W_CheckNumForName(const char* name)
{
    register int i = -1;
    
    if(numlumps)
        i = lumpinfo[W_HashLumpName(name) % numlumps].index;
    
    while(i >= 0 && strncmp(lumpinfo[i].name, name, 8))
        i = lumpinfo[i].next;
    
    return i;
}

//
// W_FindNumForName
//

int W_FindNumForName(const char* match)
{
    register int i = -1;
    int size;

    size = strlen(match) - 1;
    
    for(i = 0; i < numlumps; i++)
    {
        if(!strncmp(lumpinfo[i].name, match, size))
            break;
    }

    return i;
}

//
// W_GetNumForName
// Calls W_CheckNumForName, but bombs out if not found.
//

int W_GetNumForName(const char* name)
{
    int	i;
    
    i = W_CheckNumForName(name);
    
    if(i == -1)
        I_Error("W_GetNumForName: %s not found!", name);
    
    return i;
}

//
// W_LumpLength
// Returns the buffer size needed to load the given lump.
//

int W_LumpLength(int lump)
{
    if(lump >= numlumps)
        I_Error("W_LumpLength: %i >= numlumps",lump);
    
    return lumpinfo[lump].size;
}

//
// W_ReadLump
// Loads the lump into the given buffer,
//  which must be >= W_LumpLength().
//

void W_ReadLump(int lump, void *dest)
{
    int c;
    lumpinfo_t *l;
	
    if(lump >= numlumps)
        I_Error("W_ReadLump: %i >= numlumps", lump);

    l = lumpinfo+lump;
    c = W_Read(l->wadfile, l->position, dest, l->size);

    if(c < l->size)
        I_Error("W_ReadLump: only read %i of %i on lump %i", c, l->size, lump);	
}

//
// W_CacheLumpNum
//

void* W_CacheLumpNum(int lump, int tag)
{
    lumpinfo_t *l;

    if(lump < 0 || lump >= numlumps)
        I_Error("W_CacheLumpNum: lump %i out of range", lump);
    
    l = &lumpinfo[lump];

    if(!l->cache)      // read the lump in
    {
        Z_Malloc(W_LumpLength(lump), tag, &l->cache);
        W_ReadLump(lump, l->cache);
    }
    else
    {
        // [d64] 'touch' static caches
        if(tag == PU_STATIC)
            Z_Touch(l->cache);

        // avoid changing PU_STATIC data into PU_CACHE
        if(tag < Z_CheckTag(l->cache))
            Z_ChangeTag(l->cache, tag);
    }
    
    return l->cache;
}

//
// W_CacheLumpName
//

void* W_CacheLumpName(const char* name, int tag)
{
    return W_CacheLumpNum(W_GetNumForName(name), tag);
}

static dboolean nonmaplump = false;

filelump_t *mapLump;
int numMapLumps;
byte *mapLumpData = NULL;

//
// W_CacheMapLump
//

void W_CacheMapLump(int map)
{
    char name8[9];
    int lump;
    
    sprintf(name8, "MAP%02d", map);
    name8[8] = 0;
        
    lump = W_GetNumForName(name8);

    // check if non-lump map, aka standard doom map storage
    if(!((lump+1) >= numlumps) && !strncmp(lumpinfo[lump+1].name, "THINGS", 8))
    {
        nonmaplump = true;
        return;
    }
    else
        mapLumpData = (byte*)W_CacheLumpNum(lump, PU_STATIC);
    
    numMapLumps = ((wadinfo_t*)mapLumpData)->numlumps;
    mapLump = (filelump_t*)(mapLumpData + ((wadinfo_t*)mapLumpData)->infotableofs);
}

//
// W_FreeMapLump
//

void W_FreeMapLump(void)
{
    if(nonmaplump)
        Z_FreeTags(PU_MAPLUMP, PU_MAPLUMP);

    mapLump = NULL;
    numMapLumps = 0;
    nonmaplump = false;

    if(mapLumpData)
        Z_Free(mapLumpData);

    mapLumpData = NULL;
}

//
// W_MapLumpLength
//

int W_MapLumpLength(int lump)
{
    if(nonmaplump)
    {
        char name8[9];
        int l;

        sprintf(name8, "MAP%02d", gamemap);
        name8[8] = 0;
        
        l = W_GetNumForName(name8);

        return lumpinfo[l + lump].size;
    }

    if(lump >= numMapLumps)
        I_Error("W_MapLumpLength: %i out of range", lump);
    
    return mapLump[lump].size;
}

//
// W_GetMapLump
//

void* W_GetMapLump(int lump)
{
    if(nonmaplump)
    {
        char name8[9];
        int l;

        sprintf(name8, "MAP%02d", gamemap);
        name8[8] = 0;
        
        l = W_GetNumForName(name8);

        return W_CacheLumpNum(l + lump, PU_MAPLUMP);
    }

    if(lump >= numMapLumps)
        I_Error("W_GetMapLump: lump %d out of range", lump);
    
    return (mapLumpData + mapLump[lump].filepos);
}

