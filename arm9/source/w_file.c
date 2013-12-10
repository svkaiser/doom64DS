#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "ds.h"
#include "doomtype.h"
#include "z_zone.h"
#include "w_file.h"

typedef struct
{
    wad_file_t wad;
    FILE *fstream;
} stdc_wad_file_t;

wad_file_class_t stdc_wad_file;

static wad_file_t *W_StdC_OpenFile(char *path)
{
    stdc_wad_file_t *result;
    FILE *fstream;

    fstream = fopen(I_FilePath(path), "rb");

    if(fstream == NULL)
        return NULL;

    // Create a new stdc_wad_file_t to hold the file handle.

    result = Z_Malloc(sizeof(stdc_wad_file_t), PU_STATIC, 0);
    result->wad.file_class = &stdc_wad_file;
    result->wad.mapped = NULL;
    result->wad.length = I_FileLength(fstream);
    result->fstream = fstream;

    return &result->wad;
}

static void W_StdC_CloseFile(wad_file_t *wad)
{
    stdc_wad_file_t *stdc_wad;

    stdc_wad = (stdc_wad_file_t *) wad;

    fclose(stdc_wad->fstream);
    Z_Free(stdc_wad);
}

// Read data from the specified position in the file into the 
// provided buffer.  Returns the number of bytes read.

size_t W_StdC_Read(wad_file_t *wad, unsigned int offset,
                   void *buffer, size_t buffer_len)
{
    stdc_wad_file_t *stdc_wad;
    size_t result;

    stdc_wad = (stdc_wad_file_t *) wad;

    // Jump to the specified position in the file.

    fseek(stdc_wad->fstream, offset, SEEK_SET);

    // Read into the buffer.

    result = fread(buffer, 1, buffer_len, stdc_wad->fstream);

    return result;
}


wad_file_class_t stdc_wad_file = 
{
    W_StdC_OpenFile,
    W_StdC_CloseFile,
    W_StdC_Read,
};

wad_file_t *W_OpenFile(char *path)
{
    return stdc_wad_file.OpenFile(path);
}

void W_CloseFile(wad_file_t *wad)
{
    wad->file_class->CloseFile(wad);
}

size_t W_Read(wad_file_t *wad, unsigned int offset,
              void *buffer, size_t buffer_len)
{
    return wad->file_class->Read(wad, offset, buffer, buffer_len);
}

// Array of locations to search for IWAD files
//
// "128 IWAD search directories should be enough for anybody".

#define MAX_IWAD_DIRS 128

static dboolean iwad_dirs_built = false;
static char *iwad_dirs[MAX_IWAD_DIRS];
static int num_iwad_dirs = 0;

static void AddIWADDir(char *dir)
{
    if(num_iwad_dirs < MAX_IWAD_DIRS)
    {
        iwad_dirs[num_iwad_dirs] = dir;
        ++num_iwad_dirs;
    }
}

//
// BuildIWADDirList
// Build a list of IWAD files
//

static void BuildIWADDirList(void)
{
    if(iwad_dirs_built)
        return;

    // Look in the current directory.  Doom always does this.

    AddIWADDir(".");

#ifndef _WIN32

    // Standard places where IWAD files are installed under Unix.

    AddIWADDir("/usr/share/games/doom64");
    AddIWADDir("/usr/local/share/games/doom64");

#endif

    // Don't run this function again.

    iwad_dirs_built = true;
}

//
// SearchDirectoryForIWAD
// Search a directory to try to find an IWAD
// Returns the location of the IWAD if found, otherwise NULL.

static char *SearchDirectoryForIWAD(char *dir)
{
    char *filename; 
    char *iwadname;
    
    iwadname = "DOOM64.WAD";
    filename = malloc(strlen(dir) + strlen(iwadname) + 3);

    if(!strcmp(dir, "."))
        strcpy(filename, iwadname);
    else
        sprintf(filename, "%s%c%s", dir, '/', iwadname);

    if(I_FileExists(filename))
        return filename;

    free(filename);

    return NULL;
}

//
// W_FindWADByName
// Searches WAD search paths for an WAD with a specific filename.
// 

char *W_FindWADByName(char *name)
{
    char *buf;
    int i;
    dboolean exists;
    
    // Absolute path?
    if(I_FileExists(name))
        return name;

    BuildIWADDirList();
    
    // Search through all IWAD paths for a file with the given name.

    for(i = 0; i < num_iwad_dirs; ++i)
    {
        // Construct a string for the full path

        buf = malloc(strlen(iwad_dirs[i]) + strlen(name) + 5);
        sprintf(buf, "%s%c%s", iwad_dirs[i], '/', name);

        exists = I_FileExists(buf);

        if(exists)
            return buf;

        free(buf);
    }

    // File not found

    return NULL;
}

//
// W_TryFindWADByName
//
// Searches for a WAD by its filename, or passes through the filename
// if not found.
//

char *W_TryFindWADByName(char *filename)
{
    char *result;

    result = W_FindWADByName(filename);

    if(result != NULL)
        return result;
    else
     return filename;
}

//
// W_FindIWAD
// Checks availability of IWAD files by name,
//

char *W_FindIWAD(void)
{
    char *result;
    int i;

    result = NULL;
    BuildIWADDirList();
    
    for(i = 0; result == NULL && i < num_iwad_dirs; ++i)
        result = SearchDirectoryForIWAD(iwad_dirs[i]);

    return result;
}

