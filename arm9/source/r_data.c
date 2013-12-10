#include "m_fixed.h"
#include "r_local.h"
#include "z_zone.h"
#include "w_wad.h"
#include "p_local.h"
#include "st_main.h"

uint32* gfx_base = (uint32*)VRAM_A;
byte    gfx_tex_buffer[GFX_BUFFER_SIZE];
gfx_t*  gfx_images;
uint32* gfx_imgpal_params;

static int              gfx_texpal_stride = 0;
static uint32**         gfx_texpal_params;
static uint32*          gfx_sprpal_params;
static uint32           gfx_extpal_params[NUMSPRITES][8];
static byte             gfx_padbuffer[512 * 256];
static gfx_t*           gfx_textures;
static gfx_t*           gfx_sprites;
static short*           texturetranslation;
static short*           palettetranslation;

//
// R_CachePalette
// Returns the palette parameter for generic texture lumps
//

uint32 R_CachePalette(const char* name)
{
    uint16* lump;
    uint16* pal;
    int stride;
    int width;
    int height;

    stride = gfx_texpal_stride;
    lump = (uint16*)W_CacheLumpName(name, PU_AUTO);
    width = lump[0];
    height = lump[1];
    pal = lump + 4 + ((width * height) >> 1);

    gfx_texpal_stride += 512;

    return I_SetPalette(pal, stride, 512);
}

//
// R_CopyPic
//

byte* R_CopyPic(byte* pic, int x, int y, int rows, int colsize,
               int copysize, int mainwidth)
{
    int i;

    memset(gfx_padbuffer, 0, R_PadTextureDims(rows) * colsize);

    for(i = 0; i < rows; i++)
    {
        byte* src;
        byte* dst;

        src = &pic[((i + y) * mainwidth) + x];
        dst = &gfx_padbuffer[i * colsize];

        memcpy(dst, src, copysize);
    }

    return gfx_padbuffer;
}

//
// R_CacheTexture
//

static void R_CacheTexturePalette(int index)
{
    byte* gfx;
    int size;
    uint16* paldata;
    int i;

    gfx = (byte*)W_CacheLumpNum(t_start + index, PU_STATIC);
    size = (((8 << gfx[0]) * (8 << gfx[1])) >> 1);

    gfx_texpal_params[index] = (uint32*)Z_Calloc(sizeof(uint32) * gfx[2], PU_STATIC, NULL);

    for(i = 0; i < gfx[2]; i++)
    {
        paldata = (uint16*)((gfx + 4 + size + (32 * i)));
        gfx_texpal_params[index][i] = I_SetPalette(paldata, gfx_texpal_stride, 32);
        gfx_texpal_stride += 32;
    }

    Z_Free(gfx);
}

//
// R_CacheImgPalette
//

static void R_CacheImgPalette(int index)
{
    uint16* lump;
    uint16* pal;
    int width;
    int height;

    lump = (uint16*)W_CacheLumpNum(g_start + index, PU_STATIC);
    width = lump[0];
    height = lump[1];
    pal = lump + 4 + ((width * height) >> 1);

    gfx_imgpal_params[index] = I_SetPalette(pal, gfx_texpal_stride, 512);
    gfx_texpal_stride += 512;

    Z_Free(lump);
}

//
// R_SetupSpriteData
//

static void R_SetupSpriteData(int spritenum)
{
    short* gfx;
    byte* data;
    byte* pal;
    uint16* paldata;

    gfx = (short*)W_CacheLumpNum(s_start + spritenum, PU_STATIC);

    spritewidth[spritenum]      = gfx[0];
    spriteheight[spritenum]     = gfx[1];
    spriteoffset[spritenum]     = gfx[2];
    spritetopoffset[spritenum]  = gfx[3];

    if(!gfx[4])
    {
        data = (byte*)(gfx + 5);

        pal = data + ((gfx[0] * gfx[1]) >> 1);
        paldata = (uint16*)pal;

        gfx_sprpal_params[spritenum] = I_SetPalette(paldata, gfx_texpal_stride, 32);
        gfx_texpal_stride += 32;
    }
    else
        spritetiles[spritenum] = gfx[5];

    Z_Free(gfx);
}

//
// R_CacheExternalPalette
//

static void R_CacheExternalPalette(int sprite)
{
    char palname[9];
    spritedef_t *sprdef;
    spriteframe_t *sprframe;
    int spritenum;
    short* gfx;
    byte* data;
    int i;

    sprdef = &spriteinfo[sprite];
    sprframe = &sprdef->spriteframes[0];
    spritenum = sprframe->lump[0];

    if(gfx_sprpal_params[spritenum] == 0)
    {
        gfx = (short*)W_CacheLumpNum(s_start + spritenum, PU_STATIC);

        if(gfx[4] == 2)
        {
            byte* pal;
            uint16* paldata;

            data = (byte*)(gfx + 6 + ((sizeof(short) * gfx[5]) >> 1));

            pal = data + (gfx[0] * gfx[1]);
            paldata = (uint16*)pal;

            gfx_extpal_params[sprite][0] = I_SetPalette(paldata, gfx_texpal_stride, 512);
            gfx_texpal_stride += 512;
        }
        else if(gfx[4] == 1)
        {
            for(i = 0; i < 8; i++)
            {
                int lump;
                uint16* paldata;

                sprintf(palname, "PAL%s%i", sprnames[sprite], i);
                lump = W_CheckNumForName(palname);

                if(lump == -1)
                    continue;

                paldata = (uint16*)W_CacheLumpNum(lump, PU_STATIC);
                gfx_extpal_params[sprite][i] = I_SetPalette(paldata, gfx_texpal_stride, 512);
                gfx_texpal_stride += 512;
            }
        }

        Z_Free(gfx);
    }
}

//
// R_InitPalettes
//

static void R_InitPalettes(void)
{
    int i;

    for(i = 0; i < numtextures; i++)
        R_CacheTexturePalette(i);

    for(i = 0; i < numgfximgs; i++)
        R_CacheImgPalette(i);

    for(i = 0; i < numgfxsprites; i++)
    {
        if(gfx_sprpal_params[i] == 0)
            R_SetupSpriteData(i);
    }

    for(i = 1; i < NUMSPRITES; i++)
        R_CacheExternalPalette(i);
}

//
// R_InitTextures
//

static void R_InitTextures(void)
{
    int i;

    t_start             = W_GetNumForName("T_START") + 1;
    t_end               = W_GetNumForName("T_END") - 1;
    swx_start           = W_FindNumForName("SWX") + 1;
    numtextures         = (t_end - t_start) + 1;
    texturetranslation  = (short*)Z_Calloc(sizeof(short) * numtextures, PU_STATIC, NULL);
    palettetranslation  = (short*)Z_Calloc(sizeof(short) * numtextures, PU_STATIC, NULL);
    gfx_textures        = (gfx_t*)Z_Calloc(sizeof(gfx_t) * numtextures, PU_STATIC, NULL);
    gfx_texpal_params   = (uint32**)Z_Calloc(sizeof(*gfx_texpal_params) * numtextures, PU_STATIC, NULL);

    for(i = 0; i < numtextures; i++)
    {
        texturetranslation[i] = i;
        palettetranslation[i] = 0;
    }
}

//
// R_InstallSpriteLump
// Local function for R_InitSprites.
//

static int              maxframe;
static spriteframe_t    sprtemp[29];
static char*            spritename;

static void R_InstallSpriteLump(int lump, unsigned frame, unsigned rotation, dboolean flipped)
{
    int	r;
    
    if(frame >= 29 || rotation > 8)
        I_Error("R_InstallSpriteLump: Bad frame characters in lump %i", lump);
    
    if((int)frame > maxframe)
        maxframe = frame;
    
    if(rotation == 0)
    {
        // the lump should be used for all rotations
        if((sprtemp[frame].rotate == false))
            I_Error("R_InitSprites: Sprite %s frame %c has multiple rot=0 lump", spritename, 'A'+frame);
        
        if(sprtemp[frame].rotate == true)
            I_Error("R_InitSprites: Sprite %s frame %c has rotations and a rot=0 lump", spritename, 'A'+frame);
        
        sprtemp[frame].rotate = false;
        for(r = 0; r < 8; r++)
        {
            sprtemp[frame].lump[r] = lump - s_start;
            sprtemp[frame].flip[r] = (byte)flipped;
        }
        return;
    }
    
    // the lump is only used for one rotation
    if(sprtemp[frame].rotate == false)
        I_Error("R_InitSprites: Sprite %s frame %c has rotations and a rot=0 lump", spritename, 'A'+frame);
    
    sprtemp[frame].rotate = true;
    
    // make 0 based
    rotation--;
    if((sprtemp[frame].lump[rotation] != -1))
        I_Error ("R_InitSprites: Sprite %s : %c : %c has two lumps mapped to it",
        spritename, 'A'+frame, '1'+rotation);
    
    sprtemp[frame].lump[rotation] = lump - s_start;
    sprtemp[frame].flip[rotation] = (byte)flipped;
}

//
// R_InitSprites
//

static void R_InitSprites(void)
{
    char**  check;
    int     i;
    int     l;
    int     frame;
    int     rotation;
    int     start;
    int     end;
    int     patched;
    int     sprnum;

    s_start             = W_GetNumForName("S_START") + 1;
    s_end               = W_GetNumForName("S_END") - 1;
    numgfxsprites       = (s_end - s_start) + 1;
    spritewidth         = (short*)Z_Calloc(sizeof(short) * numgfxsprites, PU_STATIC, NULL);
    spriteheight        = (short*)Z_Calloc(sizeof(short) * numgfxsprites, PU_STATIC, NULL);
    spriteoffset        = (short*)Z_Calloc(sizeof(short) * numgfxsprites, PU_STATIC, NULL);
    spritetopoffset     = (short*)Z_Calloc(sizeof(short) * numgfxsprites, PU_STATIC, NULL);
    gfx_sprites         = (gfx_t*)Z_Calloc(sizeof(gfx_t) * numgfxsprites, PU_STATIC, NULL);
    gfx_sprpal_params   = (uint32*)Z_Calloc(sizeof(uint32) * numgfxsprites, PU_STATIC, NULL);
    spritetiles         = (byte*)Z_Calloc(numgfxsprites, PU_STATIC, NULL);
    spritetilelist      = (short**)Z_Calloc(sizeof(*spritetilelist) * numgfxsprites, PU_STATIC, NULL);

    // count the number of sprite names
    check = sprnames;
    while(*check != NULL)
        check++;
    
    sprnum = check-sprnames;
    
    if(!sprnum)
        return;
    
    spriteinfo = Z_Malloc(sprnum * sizeof(*spriteinfo), PU_STATIC, NULL);
    
    start = s_start - 1;
    end = s_end + 1;
    
    // scan all the lump names for each of the names,
    //  noting the highest frame letter.
    // Just compare 4 characters as ints
    
    for(i = 0; i < sprnum; i++)
    {
        spritename = sprnames[i];
        memset(sprtemp,-1, sizeof(sprtemp));
        
        maxframe = -1;
        
        // scan the lumps,
        //  filling in the frames for whatever is found
        
        for(l = start + 1; l < end; l++)
        {
            // 20120422 villsa - gcc is such a crybaby sometimes...
            if(!strncmp(lumpinfo[l].name, sprnames[i], 4))
            {
                frame = lumpinfo[l].name[4] - 'A';
                rotation = lumpinfo[l].name[5] - '0';
                
                patched = l;
                
                R_InstallSpriteLump(patched, frame, rotation, false);
                
                if(lumpinfo[l].name[6])
                {
                    frame = lumpinfo[l].name[6] - 'A';
                    rotation = lumpinfo[l].name[7] - '0';
                    R_InstallSpriteLump (l, frame, rotation, true);
                }
            }
        }
        
        // check the frames that were found for completeness
        if (maxframe == -1)
        {
            spriteinfo[i].numframes = 0;
            continue;
        }
        
        maxframe++;
        
        for(frame = 0; frame < maxframe; frame++)
        {
            switch((int)sprtemp[frame].rotate)
            {
            case -1:
                // no rotations were found for that frame at all
                I_Error ("R_InitSprites: No patches found for %s frame %c", sprnames[i], frame+'A');
                break;
                
            case 0:
                // only the first rotation is needed
                break;
                
            case 1:
                // must have all 8 frames
                for(rotation = 0; rotation < 8; rotation++)
                    if (sprtemp[frame].lump[rotation] == -1)
                        I_Error ("R_InitSprites: Sprite %s frame %c is missing rotations",
                        sprnames[i], frame+'A');
                    break;
            }
        }
        
        // allocate space for the frames present and copy sprtemp to it
        spriteinfo[i].numframes = maxframe;
        spriteinfo[i].spriteframes =
            Z_Malloc(maxframe * sizeof(spriteframe_t), PU_STATIC, NULL);
        memcpy(spriteinfo[i].spriteframes, sprtemp, maxframe*sizeof(spriteframe_t));
    }
}

//
// R_InitImgs
//

void R_InitImgs(void)
{
    g_start             = W_GetNumForName("G_START") + 1;
    g_end               = W_GetNumForName("G_END") - 1;
    numgfximgs          = (g_end - g_start) + 1;
    gfx_images          = (gfx_t*)Z_Calloc(sizeof(gfx_t) * numgfximgs, PU_STATIC, NULL);
    gfx_imgpal_params   = (uint32*)Z_Calloc(sizeof(uint32) * numgfximgs, PU_STATIC, NULL);
}

//
// R_InitData
//

void R_InitData(void)
{
    P_LoadingPlaque();
    R_InitTextures();
    R_InitSprites();
    R_InitImgs();
    R_InitPalettes();
    P_ClearLoadingPlaque();
}

//
// R_GetTextureSize
//

int R_GetTextureSize(int size)
{
    if(size == 8)
        return TEXTURE_SIZE_8;
    if(size == 16)
        return TEXTURE_SIZE_16;
    if(size == 32)
        return TEXTURE_SIZE_32;
    if(size == 64)
        return TEXTURE_SIZE_64;
    if(size == 128)
        return TEXTURE_SIZE_128;
    if(size == 256)
        return TEXTURE_SIZE_256;
    if(size == 512)
        return TEXTURE_SIZE_512;

	return 0;
}

//
// R_GetDSTextureSize
//

int R_GetDSTextureSize(int size)
{
    if(size == TEXTURE_SIZE_8)
        return 8;
    if(size == TEXTURE_SIZE_16)
        return 16;
    if(size == TEXTURE_SIZE_32)
        return 32;
    if(size == TEXTURE_SIZE_64)
        return 64;
    if(size == TEXTURE_SIZE_128)
        return 128;
    if(size == TEXTURE_SIZE_256)
        return 256;
    if(size == TEXTURE_SIZE_512)
        return 512;

	return 0;
}

//
// R_PadTextureDims
//

#define MAXTEXSIZE	512
#define MINTEXSIZE	8

int R_PadTextureDims(int n)
{
    int mask = MINTEXSIZE;
    
    while(mask <= MAXTEXSIZE)
    {
        if(n == mask || (n & (mask-1)) == n)
            return mask;
        
        mask <<= 1;
    }
    return n;
}

//
// R_LoadTexture
//

void R_LoadTexture(dtexture texture, dboolean flip_s, dboolean flip_t, dboolean masked)
{
    byte* gfx;
    int w, dw;
    int h, dh;
    byte* data;
    short* pal;
    uint32 flags;
    int size;
    lumpinfo_t* lump;

    texture = texturetranslation[texture];

    lump = &lumpinfo[t_start + texture];

    if(lump->cache == NULL)
        W_CacheLumpNum(t_start + texture, PU_CACHE);

    gfx = (byte*)lump->cache;
        
    dw = gfx[0];
    dh = gfx[1];
    w = (8 << dw);
    h = (8 << dh);
    data = gfx + 4;
    size = ((w * h) >> 1);
    pal = (short*)(gfx + 4 + size);

    flags = TEXGEN_OFF | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T;
        
    if(pal[0] == 0 && masked)
        flags |= GL_TEXTURE_COLOR0_TRANSPARENT;

    if(flip_s)
        flags |= GL_TEXTURE_FLIP_S;

    if(flip_t)
        flags |= GL_TEXTURE_FLIP_T;

    if(!I_AllocVBlock(&gfx_textures[texture], data, size, flags, dw, dh, GL_RGB16))
    {
        GFX_TEX_FORMAT = 0;
        GFX_PAL_FORMAT = 0;
        return;
    }

    GFX_TEX_FORMAT = gfx_textures[texture].params;
    GFX_PAL_FORMAT = gfx_texpal_params[texture][palettetranslation[texture]];
}

//
// R_SetTextureFrame
//

void R_SetTextureFrame(int texture, int frame, dboolean palette)
{
    if(palette)
        palettetranslation[texture] = frame;
    else
        texturetranslation[texture] = texture + frame;
}

//
// R_LoadSprite
//

dboolean R_LoadSprite(int sprite, int frame, int rotation, int palindex,
                  int *x, int *y, int *w, int *h)
{
    spritedef_t *sprdef;
    spriteframe_t *sprframe;
    int spritenum;
    short* gfx;
    int width;
    int height;
    dboolean ext;
    byte* data;
    int size;
    lumpinfo_t* lump;

    if(sprite == SPR_SPOT)
        return false;

    sprdef      = &spriteinfo[sprite];
    sprframe    = &sprdef->spriteframes[frame];
    spritenum   = sprframe->lump[rotation];

    lump = &lumpinfo[s_start + spritenum];

    if(lump->cache == NULL)
        W_CacheLumpNum(s_start + spritenum, PU_CACHE);

    gfx     = (short*)lump->cache;
    width   = gfx[0];
    height  = gfx[1];
    ext     = gfx[4];
    size    = (width * height);

    if(ext)
        data = (byte*)(gfx + 6 + ((sizeof(short) * spritetiles[spritenum]) >> 1));
    else
        data = (byte*)(gfx + 5);

    if(!ext)
        size >>= 1;

    if(!I_AllocVBlock(
        &gfx_sprites[spritenum],
        data,
        size,
        TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT,
        R_GetTextureSize(width),
        R_GetTextureSize(R_PadTextureDims(height)),
        ext ? GL_RGB256 : GL_RGB16))
    {
        GFX_TEX_FORMAT = 0;
        GFX_PAL_FORMAT = 0;
        return false;
    }

    GFX_TEX_FORMAT = gfx_sprites[spritenum].params;

    if(palindex >= 8 || palindex < 0)
        palindex = 0;

    if(ext)
        GFX_PAL_FORMAT = gfx_extpal_params[sprite][palindex];
    else
        GFX_PAL_FORMAT = gfx_sprpal_params[spritenum];

    if(x) *x = spriteoffset[spritenum];
    if(y) *y = spritetopoffset[spritenum];
    if(w) *w = spritewidth[spritenum];
    if(h) *h = spriteheight[spritenum];

    return true;
}

//
// R_PrecacheLevel
// Loads and binds all world textures before level startup
//

void R_PrecacheLevel(void)
{
    int	i;

    for(i = 0; i < numsides; i++)
    {
        W_CacheLumpNum(t_start + sides[i].toptexture, PU_CACHE);
        W_CacheLumpNum(t_start + sides[i].bottomtexture, PU_CACHE);
        W_CacheLumpNum(t_start + sides[i].midtexture, PU_CACHE);
    }

    for(i = 0; i < numsectors; i++)
    {
        W_CacheLumpNum(t_start + sectors[i].ceilingpic, PU_CACHE);
        W_CacheLumpNum(t_start + sectors[i].floorpic, PU_CACHE);
    }
}

