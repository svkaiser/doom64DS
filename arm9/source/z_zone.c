#include <stdlib.h>
#include "ds.h"
#include "doomtype.h"
#include "d_main.h"
#include "z_zone.h"
#include "r_local.h"

#define ZONEID	0x1d4a11
#define ZONEID2 0x1d4a

//#define ZONEFILE

typedef struct memblock_s memblock_t;

struct memblock_s
{
    int id; // = ZONEID
    int tag;
    int size;
    void **user;
    memblock_t *prev;
    memblock_t *next;
};

#define VBLOCK_ALIGN    31

vramzone_t* vramzone;

#ifdef ZONEFILE

static FILE *zonelog;

static void Z_OpenLogFile(void)
{
   zonelog = fopen("zonelog.txt", "w");
}

static void Z_CloseLogFile(void)
{
   if(zonelog)
   {
      fputs("Closing zone log", zonelog);
      fclose(zonelog);
      zonelog = NULL;
   }
}

static void Z_LogPrintf(const char *msg, ...)
{
   if(zonelog)
   {
      va_list ap;

      fprintf(zonelog, "%i: ", gametic);
      va_start(ap, msg);
      vfprintf(zonelog, msg, ap);
      va_end(ap);

      // flush after every message
      fflush(zonelog);
   }
}

static void Z_LogPuts(const char *msg)
{
   if(zonelog)
      fputs(msg, zonelog);
}

#endif

// Linked list of allocated blocks for each tag type

static memblock_t *allocated_blocks[PU_MAX];

//
// Z_InsertBlock
// Add a block into the linked list for its type.
//

static void Z_InsertBlock(memblock_t *block)
{
    block->prev = NULL;
    block->next = allocated_blocks[block->tag];
    allocated_blocks[block->tag] = block;
    
    if(block->next != NULL)
        block->next->prev = block;
}

//
// Z_RemoveBlock
// Remove a block from its linked list.
//

static void Z_RemoveBlock(memblock_t *block)
{
    // Unlink from list
    if(block->prev == NULL)
        allocated_blocks[block->tag] = block->next; // Start of list
    else
        block->prev->next = block->next;
    
    if(block->next != NULL)
        block->next->prev = block->prev;
}

//
// Z_InitVram
//

static void Z_InitVram(byte* base_start, uint32 size)
{
    vramblock_t* vblock;

    vramzone = (vramzone_t*)Z_Calloc(sizeof(vramzone_t), PU_STATIC, NULL);
    vramzone->size = size;
    vramzone->free = 0;

    // set the entire zone to one free block
    vramzone->blocklist.block = base_start;
    vramzone->blocklist.next =
    vramzone->blocklist.prev =
    vblock = (vramblock_t*)Z_Calloc(sizeof(vramblock_t), PU_STATIC, NULL);

    vramzone->blocklist.gfx = (void*)vramzone;
    vramzone->blocklist.tag = PU_STATIC;
    vramzone->rover = vblock;
	
    vblock->prev = vblock->next = &vramzone->blocklist;

    // free block
    vblock->tag = PU_FREE;
    vblock->block = vramzone->blocklist.block;
    vblock->size = vramzone->size;
    vblock->prevtic = frametic;
}

//
// Z_Init
//

void Z_Init(void)
{
    uint32 memory = (u32)getHeapLimit() - (u32)getHeapEnd();

    if(memory <= 0x150000)
        I_Error("Z_Init: not enough memory to allocate: %ikb", memory >> 10);

    memset(allocated_blocks, 0, sizeof(allocated_blocks));

    Z_InitVram(gfx_tex_buffer, GFX_BUFFER_SIZE);

#ifdef ZONEFILE
    atexit(Z_CloseLogFile); // exit handler
    Z_OpenLogFile();

    Z_LogPrintf("* Z_Init\n");
#endif
}


//
// Z_Free
//

void (Z_Free)(void* ptr, const char *file, int line)
{
    memblock_t* block;
    
    block = (memblock_t *) ((byte *)ptr - sizeof(memblock_t));
    
    if(block->id != ZONEID)
        I_Error("Z_Free: freed a pointer without ZONEID (%s:%d)", file, line);
    
    // clear the user's mark
    if(block->user != NULL)
        *block->user = NULL;
    
    Z_RemoveBlock(block);
    
    // Free back to system
    free(block);

#ifdef ZONEFILE
    Z_LogPrintf("* Z_Free(ptr=%p, file=%s:%d)\n", ptr, file, line);
#endif
}

#if 0
//
// Z_ClearCache
//
// Empty data from the cache list to allocate enough data of the size
// required.
//
// Returns true if any blocks were freed.
//

static dboolean Z_ClearCache(int size)
{
    memblock_t *block;
    memblock_t *next_block;
    int remaining;

    block = allocated_blocks[PU_CACHE];

    if(block == NULL)
    {
        // Cache is already empty.
        return false;
    }

    //
    // Search to the end of the PU_CACHE list.  The blocks at the end
    // of the list are the ones that have been free for longer and
    // are more likely to be unneeded now.
    //
    while(block->next != NULL)
        block = block->next;

    //
    // Search backwards through the list freeing blocks until we have
    // freed the amount of memory required.
    //
    remaining = size;

    while(remaining > 0)
    {
        if (block == NULL)
        {
            // No blocks left to free; we've done our best.
            break;
        }

        next_block = block->prev;

        Z_RemoveBlock(block);

        remaining -= block->size;

        if(block->user)
            *block->user = NULL;

        free(block);

        block = next_block;
    }

    return true;
}
#endif

//
// Z_Malloc
// You can pass a NULL user if the tag is < PU_PURGELEVEL.
//

void *(Z_Malloc)(int size, int tag, void *user, const char *file, int line)
{
    memblock_t *newblock;
    unsigned char *data;
    void *result;
    
    if(tag < 0 || tag >= PU_MAX)
        I_Error("Z_Malloc: tag out of range: %i (%s:%d)", tag, file, line);
    
    if(user == NULL && tag >= PU_PURGELEVEL)
        I_Error("Z_Malloc: an owner is required for purgable blocks (%s:%d)", file, line);
    
    // Malloc a block of the required size
    
    newblock = NULL;
    
    if(!(newblock = (memblock_t*)malloc(sizeof(memblock_t) + size)))
    {
        Z_FreeTags(PU_AUDIO, PU_AUDIO);
        if(!(newblock = (memblock_t*)malloc(sizeof(memblock_t) + size)))
        {
            Z_FreeTags(PU_CACHE, PU_CACHE);
            newblock = (memblock_t*)malloc(sizeof(memblock_t) + size);
        }
    }

    if(!newblock)
        I_Error("Z_Malloc: failed on allocation of %u bytes (%s:%d)", size, file, line);
    
    // Hook into the linked list for this tag type
    
    newblock->tag = tag;
    newblock->id = ZONEID;
    newblock->user = user;
    newblock->size = size;
    
    Z_InsertBlock(newblock);
    
    data = (unsigned char*)newblock;
    result = data + sizeof(memblock_t);
    
    if(user != NULL)
        *newblock->user = result;

#ifdef ZONEFILE
    Z_LogPrintf("* %p = Z_Malloc(size=%lu, tag=%d, user=%p, source=%s:%d)\n", 
        result, size, tag, user, file, line);
#endif
    
    return result;
}

//
// Z_Realloc
//

void *(Z_Realloc)(void *ptr, int size, int tag, void *user, const char *file, int line)
{
    memblock_t *block;
    memblock_t *newblock;
    unsigned char *data;
    void *result;

    if(!ptr)
        return (Z_Malloc)(size, tag, user, file, line);

    if(size == 0)
    {
        (Z_Free)(ptr, file, line);
        return NULL;
    }

    if(tag < 0 || tag >= PU_MAX)
        I_Error("Z_Realloc: tag out of range: %i (%s:%d)", tag, file, line);
    
    if(user == NULL && tag >= PU_PURGELEVEL)
        I_Error("Z_Realloc: an owner is required for purgable blocks (%s:%d)", file, line);

    block = (memblock_t*)((byte *)ptr - sizeof(memblock_t));

    newblock = NULL;

    if(block->id != ZONEID)
        I_Error("Z_Realloc: Reallocated a pointer without ZONEID (%s:%d)", file, line);

    Z_RemoveBlock(block);

    block->next = NULL;
    block->prev = NULL;

    if(block->user)
        *block->user = NULL;

    if(!(newblock = (memblock_t*)realloc(block, sizeof(memblock_t) + size)))
    {
        Z_FreeTags(PU_AUDIO, PU_AUDIO);
        if(!(newblock = (memblock_t*)malloc(sizeof(memblock_t) + size)))
        {
            Z_FreeTags(PU_CACHE, PU_CACHE);
            newblock = (memblock_t*)malloc(sizeof(memblock_t) + size);
        }
    }

    if(!newblock)
        I_Error("Z_Realloc: failed on allocation of %u bytes (%s:%d)", size, file, line);

    newblock->tag = tag;
    newblock->id = ZONEID;
    newblock->user = user;
    newblock->size = size;
    
    Z_InsertBlock(newblock);

    data = (unsigned char*)newblock;
    result = data + sizeof(memblock_t);
    
    if(user != NULL)
        *newblock->user = result;

#ifdef ZONEFILE
    Z_LogPrintf("* %p = Z_Realloc(ptr=%p, n=%lu, tag=%d, user=%p, source=%s:%d)\n", 
        result, ptr, size, tag, user, file, line);
#endif
    
    return result;
}

//
// Z_FreeTags
//

void (Z_FreeTags)(int lowtag, int hightag, const char *file, int line)
{
    int i;
    
    for(i = lowtag; i <= hightag; ++i)
    {
        memblock_t *block;
        memblock_t *next;
        
        // Free all in this chain
        
        for(block = allocated_blocks[i]; block != NULL;)
        {
            next = block->next;

            if(block->id != ZONEID)
                I_Error("Z_FreeTags: Changed a tag without ZONEID (%s:%d)", file, line);
            
            // Free this block
            
            if(block->user != NULL)
                *block->user = NULL;
            
            free(block);
            
            // Jump to the next in the chain
            
            block = next;
        }
        
        // This chain is empty now
        allocated_blocks[i] = NULL;
    }

#ifdef ZONEFILE
    Z_LogPrintf("* Z_FreeTags(lowtag=%d, hightag=%d, file=%s:%d)\n",
        lowtag, hightag, file, line);
#endif
}

//
// Z_Calloc
//

void *(Z_Calloc)(int n1, int tag, void *user, const char *file, int line)
{
#ifdef ZONEFILE
    Z_LogPrintf("* Z_Calloc(file=%s:%d)\n", file, line);
#endif
	return memset((Z_Malloc)(n1, tag, user, file, line), 0, n1);
}

//
// Z_Strdup
//

char *(Z_Strdup)(const char *s, int tag, void *user, const char *file, int line)
{
#ifdef ZONEFILE
    Z_LogPrintf("* Z_Strdup(file=%s:%d)\n", file, line);
#endif
	return strcpy((Z_Malloc)(strlen(s)+1, tag, user, file, line), s);
}

//
// Z_Strdupa
//
// strdup that uses alloca, for convenience.
//

char *(Z_Strdupa)(const char *s, const char *file, int line)
{
#ifdef ZONEFILE
    Z_LogPrintf("* Z_Strdupa(file=%s:%d)\n", file, line);
#endif
	return strcpy((Z_Alloca)(strlen(s)+1, file, line), s);
}

//
// Z_Touch
//

void (Z_Touch)(void *ptr, const char *file, int line)
{
    memblock_t *block;

    block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));

    if(block->id != ZONEID)
        I_Error("Z_Touch: touched a pointer without ZONEID (%s:%d)", file, line);

#ifdef ZONEFILE
    Z_LogPrintf("* Z_Touch(ptr=%p, file=%s:%d)\n", ptr, file, line);
#endif
}

//
// Z_FreeAlloca
//

void (Z_FreeAlloca)(const char *file, int line)
{
#ifdef ZONEFILE
    Z_LogPrintf("* Z_FreeAlloca(file=%s:%d)\n", file, line);
#endif
    (Z_FreeTags)(PU_AUTO, PU_AUTO, file, line);
}

//
// Z_Alloca
//

void *(Z_Alloca)(int n, const char *file, int line)
{
#ifdef ZONEFILE
    Z_LogPrintf("* Z_Alloca(file=%s:%d)\n", file, line);
#endif
    return n == 0 ? NULL : (Z_Calloc)(n, PU_AUTO, NULL, file, line);
}

//
// Z_CheckHeap
//

void (Z_CheckHeap)(const char *file, int line)
{
    memblock_t *block;
    memblock_t *prev;
    int i;
    
    //
    // Check all chains
    //
    for(i = 0; i < PU_MAX; ++i)
    {
        prev = NULL;
        
        for(block = allocated_blocks[i]; block != NULL; block = block->next)
        {
            if(block->id != ZONEID)
                I_Error("Z_CheckHeap: Block without a ZONEID! (%s:%d)", file, line);
            
            if(block->prev != prev)
                I_Error("Z_CheckHeap: Doubly-linked list corrupted! (%s:%d)", file, line);
            
            prev = block;
        }
    }

#ifdef ZONEFILE
    Z_LogPrintf("* Z_CheckHeap(file=%s:%d)\n", file, line);
#endif
}

//
// Z_CheckTag
//

int (Z_CheckTag)(void *ptr, const char *file, int line)
{
    memblock_t*	block;
    
    block = (memblock_t*)((byte *)ptr - sizeof(memblock_t));

    if(block->id != ZONEID)
        I_Error("Z_CheckTag: block doesn't have ZONEID (%s:%d)", file, line);

    return block->tag;
}

//
// Z_ChangeTag
//

void (Z_ChangeTag)(void *ptr, int tag, const char *file, int line)
{
    memblock_t*	block;
    
    block = (memblock_t*)((byte *)ptr - sizeof(memblock_t));
    
    if(block->id != ZONEID)
        I_Error("Z_ChangeTag: block without a ZONEID! (%s:%d)",
        file, line);
    
    if(tag >= PU_PURGELEVEL && block->user == NULL)
        I_Error("Z_ChangeTag: an owner is required for purgable blocks (%s:%d)", file, line);
    
    //
    // Remove the block from its current list, and rehook it into
    // its new list.
    //
    Z_RemoveBlock(block);
    block->tag = tag;
    Z_InsertBlock(block);

#ifdef ZONEFILE
    Z_LogPrintf("* Z_ChangeTag(ptr=%p, tag=%d, file=%s:%d)\n",
        ptr, tag, file, line);
#endif
}

//
// Z_TagUsage
//

int Z_TagUsage(int tag)
{
    int bytes = 0;
    memblock_t *block;

    if(tag < 0 || tag >= PU_MAX)
        I_Error("Z_TagUsage: tag out of range: %i", tag);

    for(block = allocated_blocks[tag]; block != NULL; block = block->next)
        bytes += block->size;

    return bytes;
}

//
// Z_FreeMemory
//

int Z_FreeMemory(void)
{
    int bytes = 0;
    int i;
    memblock_t *block;

    for(i = 0; i < PU_MAX; i++)
    {
        for(block = allocated_blocks[i]; block != NULL; block = block->next)
            bytes += block->size;
    }

    return bytes;
}

//
// Z_VFree
//

void Z_VFree(vramzone_t* vram, vramblock_t* block)
{
    vramblock_t* other;

    if(block->id != ZONEID2)
        I_Error("Z_VFree: freed a block without ZONEID (%x)", vram->free);

    if(block->tag != PU_FREE && block->gfx != NULL)
        *block->gfx = 0;

    // mark as free
    block->tag = PU_FREE;
    block->gfx = NULL;
    block->prevtic = 0;
	
    other = block->prev;

    if(other->tag == PU_FREE)
    {
        // merge with previous free block
        other->size += block->size;
        other->next = block->next;
        other->next->prev = other;

        if(block == vram->rover)
            vram->rover = other;

        Z_Free(block);

        block = other;
    }
	
    other = block->next;

    if(other->tag == PU_FREE)
    {
        // merge the next free block onto the end
        block->size += other->size;
        block->next = other->next;
        block->next->prev = block;

        if(other == vram->rover)
            vram->rover = block;

        Z_Free(other);
    }
}

//
// Z_Valloc
//

#define MINFRAGMENT     64

vramblock_t* Z_VAlloc(vramzone_t* vram, int size, int tag, void* gfx)
{
    int         extra;
    void        *result;
    vramblock_t *start;
    vramblock_t *rover;
    vramblock_t *newblock;
    vramblock_t *base;

    if(size & 7)
        I_Error("Z_VAlloc: size %i must be aligned to 8 bytes", size);

    // if there is a free block behind the rover,
    //  back up over them
    base = vram->rover;
    
    if(base->prev->tag == PU_FREE)
        base = base->prev;
	
    rover = base;
    start = base->prev;
	
    do
    {
        if(rover == start)
        {
            // scanned all the way around the list
            return NULL;
        }
	
        if(rover->tag != PU_FREE)
        {
            if(frametic - rover->prevtic >= 2 &&
                rover->tag == PU_CACHE)
            {
                base = base->prev;

                Z_VFree(vram, rover);
                base = base->next;
                rover = base->next;
            }
            else
                base = rover = rover->next;
        }
        else
        {
            rover = rover->next;
        }

    } while(base->tag != PU_FREE || base->size < size);

    // found a block big enough
    extra = base->size - size;
    
    if(extra > MINFRAGMENT)
    {
        // there will be a free fragment after the allocated block
        newblock = (vramblock_t*)Z_Calloc(sizeof(vramblock_t), PU_STATIC, NULL);
        newblock->size = extra;
        newblock->tag = PU_FREE;
        newblock->gfx = NULL;
        newblock->block = base->block + size;
        newblock->prevtic = frametic;
        newblock->prev = base;
        newblock->next = base->next;
        newblock->next->prev = newblock;

        base->next = newblock;
        base->size = size;
    }

    base->gfx = gfx;
    base->tag = tag;
    base->id = ZONEID2;

    result = base;

    // next allocation will start looking here
    vram->rover = base->next;
    base->prevtic = frametic;
    
    return result;
}

//
// Z_VTouch
//

void Z_VTouch(vramzone_t* vram, vramblock_t *block)
{
    if(block->id != ZONEID2)
        I_Error("Z_VTouch: touched a block without ZONEID");

    block->prevtic = frametic;
}

//
// Z_SetVAllocList
//

void Z_SetVAllocList(vramzone_t* vram)
{
    vram->rover = vram->blocklist.next;
}

//
// Z_FreeVMemory
//

int Z_FreeVMemory(vramzone_t* vram)
{
    vramblock_t* block;
	
    vram->free = 0;
    
    for(block = vram->blocklist.next;
        block != &vram->blocklist;
        block = block->next)
    {
        if(block->tag == PU_CACHE)
            vram->free += block->size;
    }
    
    return vram->free;
}


