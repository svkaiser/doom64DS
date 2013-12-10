#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "ds.h"
#include "d_main.h"
#include "z_zone.h"
#include "r_local.h"

static char strbuffer[256];
static int bg_id = 0;
byte bg_buffer[BGMAIN_WIDTH * BGMAIN_HEIGHT];

//
// I_Error
//

void I_Error(const char *s, ...)
{
    va_list v;

    va_start(v, s);
    vsprintf(strbuffer, s, v);
    va_end(v);

    consoleDemoInit();
    iprintf(strbuffer);

    while (1) { swiWaitForVBlank(); }
}

//
// I_Printf
//

void I_Printf(const char *s, ...)
{
    va_list v;

    va_start(v, s);
    vsprintf(strbuffer, s, v);
    va_end(v);

    iprintf(strbuffer);
}

//
// I_PrintWait
//

void I_PrintWait(const char *s, ...)
{
    va_list v;
    int keys = 0;

    va_start(v, s);
    vsprintf(strbuffer, s, v);
    va_end(v);

    iprintf(strbuffer);

    while(!(keys & KEY_START))
    {
        scanKeys();
        keys = keysDown();
        swiWaitForVBlank();
    }
}

//
// I_FilePath
//

const char* I_FilePath(const char* file)
{
    static char fname[256];

    sprintf(fname, "fat0:/%s", file);
    return fname;
}

//
// I_ReadFile
//

int I_ReadFile(char const* name, byte** buffer)
{
    FILE *fp;

    errno = 0;
    
    if((fp = fopen(I_FilePath(name), "rb")))
    {
        size_t length;

        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *buffer = Z_Malloc(length, PU_STATIC, 0);
      
        if(fread(*buffer, 1, length, fp) == length)
        {
            fclose(fp);
            return length;
        }
        
        fclose(fp);
   }
   
   return -1;
}

//
// I_FileLength
//

long I_FileLength(FILE *handle)
{ 
    long savedpos;
    long length;

    // save the current position in the file
    savedpos = ftell(handle);
    
    // jump to the end and find the length
    fseek(handle, 0, SEEK_END);
    length = ftell(handle);

    // go back to the old location
    fseek(handle, savedpos, SEEK_SET);

    return length;
}

//
// I_FileExists
//

int I_FileExists(char *filename)
{
    FILE *fstream;

    fstream = fopen(I_FilePath(filename), "r");

    if (fstream != NULL)
    {
        fclose(fstream);
        return 1;
    }
    else
    {
        // If we can't open because the file is a directory, the 
        // "file" exists at least!

        if(errno == 21)
            return 2;
    }

    return 0;
}

//
// I_Init
//

void I_Init(void)
{
    // init file system
    fatInitDefault();

    // init register stuff
    REG_POWERCNT    = POWER_3D_CORE | POWER_MATRIX | POWER_LCD | POWER_2D_A | POWER_2D_B | POWER_SWAP_LCDS;
    REG_DISPCNT     = MODE_5_3D;
    REG_DISPCNT_SUB = MODE_5_2D | DISPLAY_BG3_ACTIVE;

    // init timers
    TIMER0_CR       = TIMER_ENABLE | TIMER_DIV_1024;
    TIMER1_CR       = TIMER_ENABLE | TIMER_CASCADE;

    // init vram banks
    // a - d: textures
    // e: palettes
    // f - g: screen1 background
    // h - i: screen2 background
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);
    vramSetBankC(VRAM_C_LCD);
    vramSetBankD(VRAM_D_LCD);
    vramSetBankE(VRAM_E_LCD);
    vramSetBankF(VRAM_F_MAIN_BG_0x06000000);
    vramSetBankG(VRAM_G_MAIN_BG_0x06004000);
    vramSetBankH(VRAM_H_SUB_BG);
    vramSetBankI(VRAM_I_SUB_BG_0x06208000);

    while(GFX_BUSY);

    // make sure there are no push/pops that haven't executed yet
    // clear push/pop errors or push/pop busy bit never clears
    while(GFX_STATUS & GFX_MTX_BUSY)
        GFX_STATUS |= GFX_MTX_STACK_RESET;

    // pop the projection stack to the top; poping 0 off an
    // empty stack causes an error... weird?
    if((GFX_STATUS & GFX_MTX_PROJ_STACK) != 0)
    {
        MATRIX_CONTROL  = GL_PROJECTION;
        MATRIX_POP      = 1;
    }

    // 31 deep modelview matrix; 32nd entry works but sets error flag
    MATRIX_CONTROL  = GL_MODELVIEW;
    MATRIX_POP      = GFX_MTX_STACK_LEVEL;

    // init background
    bg_id = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    bgSetPriority(0, 1);

    // console/debug stuff
    //consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 11, 8, false, true);
    //consoleClear();

    // init gfx registers
    GFX_CONTROL         = GL_FOG | GL_BLEND | GL_TEXTURE_2D | GL_ALPHA_TEST;
    GFX_ALPHA_TEST      = 0;
    GFX_CUTOFF_DEPTH    = GL_MAX_DEPTH;
    GFX_CLEAR_DEPTH     = GL_MAX_DEPTH;
    GFX_VIEWPORT        = 0xBFFF0000;
    GFX_TEX_FORMAT      = 0;
    GFX_PAL_FORMAT      = 0;
    GFX_POLY_FORMAT     = 0;
}

//
// I_GetBackground
//

byte* I_GetBackground(void)
{
    swiWaitForVBlank();

    REG_BG3CNT  = BG_BMP8_256x256;
    REG_BG3PA   = 256;
    REG_BG3PB   = 0;
    REG_BG3PC   = 0;
    REG_BG3PD   = 256;
    REG_BG3X    = 0;
    REG_BG3Y    = 0;

    return (byte*)bgGetGfxPtr(bg_id);
}


//
// I_RefreshBG
//

void I_RefreshBG(void)
{
    const int size = 0xC000;

    if(I_DmaBGBusy())
        return;

    DC_FlushRange(bg_buffer, size);
    DC_FlushRange(BG_GFX_SUB, size);
    dmaCopyWordsAsynch(3, bg_buffer, BG_GFX_SUB, size);
    DC_InvalidateRange(BG_GFX_SUB, size);
    memset(bg_buffer, 0, size);
}

//
// I_DmaBGBusy
//

dboolean I_DmaBGBusy(void)
{
    return dmaBusy(3);
}

//
// I_CheckGFX
//

dboolean (I_CheckGFX)(char* file, int line)
{
    dsvertices = GFX_VERTEX_RAM_USAGE;
    dspolygons = GFX_POLYGON_RAM_USAGE;

    if(GFX_VERTEX_RAM_USAGE >= 4096)
    {
#ifdef CHECKGFX_ABORT
        I_Error("I_CheckGFX: vertex overflowed by %d\n(%s : %i)",
        GFX_VERTEX_RAM_USAGE - 4096, file, line);
#else
        return false;
#endif
    }

    if(GFX_POLYGON_RAM_USAGE >= 2048)
    {
#ifdef CHECKGFX_ABORT
        I_Error("I_CheckGFX: polygon overflowed by %d\n(%s : %i)",
        GFX_POLYGON_RAM_USAGE - 2048, file, line);
#else
        return false;
#endif
    }

    return true;
}

//
// I_AllocVBlock
//

dboolean I_AllocVBlock(gfx_t* gfx, byte* data, int size,
                        int flags, int texel_w, int texel_h, int type)
{
    if(gfx->params == 0)
    {
        int copyflag;

        if(!(gfx->vram = Z_VAlloc(vramzone, size, PU_NEWBLOCK, &gfx->params)))
            return false;

        if(((int)data | (int)gfx->vram->block | size) & 3)
            copyflag = (size >> 1) | COPY_MODE_HWORD;
        else
            copyflag = (size >> 2) | COPY_MODE_WORD;

        swiCopy(data, gfx->vram->block, copyflag);
    }
    else
        Z_VTouch(vramzone, gfx->vram);

    gfx->params = GFX_TEXTURE(flags, texel_w, texel_h, type,
        ((uint32*)gfx_base + ((gfx->vram->block - gfx_tex_buffer) >> 2)));

    return true;
}

//
// I_SetPalette
//

uint32 I_SetPalette(uint16* data, int offset, int size)
{
    if(offset >= 0x10000)
        I_Error("I_SetPalette: palette cache overflowed by %d", offset - 0x10000);

    vramSetBankE(VRAM_E_LCD);
    memcpy16(VRAM_E + (offset >> 1), data, size >> 1);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    return GFX_VRAM_OFFSET((VRAM_E + (offset >> 2)));
}

//
// I_FinishFrame
//

void I_FinishFrame(void)
{
    vramblock_t* block;
    
    I_CheckGFX();
    
    // wait for vblank before flushing cache
    while(REG_VCOUNT > 200);
    while(REG_VCOUNT < 150);

    // lock banks
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);
    vramSetBankC(VRAM_C_LCD);
    vramSetBankD(VRAM_D_LCD);

    gfxdmasize = 0;

    // scan through list and check for any new block that needs to be dma'ed
    for(block = vramzone->blocklist.next; block != &vramzone->blocklist;
        block = block->next)
    {
        if(block->gfx == NULL)
            continue;

        // new block
        if(block->tag == PU_NEWBLOCK)
        {
            uint32 src;
            uint32 dst;
            uint32* vram;
            uint32 copyflag;

            vram = (uint32*)gfx_base + (((*(uint32*)block->gfx & 0xffff) << 3) >> 2);
            src = (uint32)block->block;
            dst = (uint32)vram;

            // data is dma'ed asynchronously so don't copy
            // new data until it's done
            while(DMA0_CR & DMA_BUSY);

            // flush source and destination just to be sure
            DC_FlushRange(block->block, block->size);
            DC_FlushRange(vram, block->size);

            if((src | dst | block->size) & 3)
                copyflag = DMA_COPY_HALFWORDS | (block->size >> 1);
            else
                copyflag = DMA_COPY_WORDS | (block->size >> 2);

            DMA0_SRC = src;
            DMA0_DEST = dst;
            DMA0_CR = copyflag;

            DC_InvalidateRange(vram, block->size);

            // mark this block as cache
            block->tag = PU_CACHE;
            gfxdmasize += block->size;
        }
    }

    // unlock banks
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_TEXTURE);
    vramSetBankD(VRAM_D_TEXTURE);

    frametic++;

    GFX_FLUSH = 1;
}

//
// I_StartTic
//

void I_StartTic(void)
{
    u16 keys;
    event_t ev;
    
    scanKeys();

    if((keys = keysDown()))
    {
        ev.type = ev_btndown;
        ev.data = keys;
        D_PostEvent(&ev);
    }

    if((keys = keysUp()))
    {
        ev.type = ev_btnup;
        ev.data = keys;
        D_PostEvent(&ev);
    }

    if((keys = keysHeld()))
    {
        ev.type = ev_btnheld;
        ev.data = keys;
        D_PostEvent(&ev);
    }
}

//
// I_GetTimeTicks
//

int I_GetTimeTicks(void)
{
    return (TIMER0_DATA >> 5) + (TIMER1_DATA << 11);
}

//
// I_GetTimeMS
//

static int basetime = 0;

int I_GetTime(void)
{
    uint32 ticks;

    ticks = I_GetTimeTicks();

    if(basetime == 0)
        basetime = ticks;

    ticks -= basetime;

    return (ticks * TICRATE) / 1000;
}

//
// I_GetTimeMS
//
// Same as I_GetTime, but returns time in milliseconds
//

int I_GetTimeMS(void)
{
    uint32 ticks;
    
    ticks = I_GetTimeTicks();
    
    if (basetime == 0)
        basetime = ticks;
    
    return ticks - basetime;
}

//
// I_Sleep
//

void I_Sleep(uint32 ms)
{
    uint32 now;

    now = I_GetTimeTicks();
    while(I_GetTimeTicks() < now + ms);
}

//
// ARM9 MAIN
//

int ARM9_MAIN(void)
{
    defaultExceptionHandler();
    D_DoomMain();
    return 0;
}

