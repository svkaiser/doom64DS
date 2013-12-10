#include <ctype.h>

#include "doomdef.h"
#include "d_englsh.h"
#include "d_main.h"
#include "p_local.h"
#include "r_local.h"
#include "sounds.h"
#include "s_sound.h"
#include "w_wad.h"
#include "st_main.h"

extern int menuskullcounter;    // from m_main.c

//
// STATUS BAR DATA
//

#define NUMSYMBOLS      97
#define NUMSTATUSITEMS  8

typedef struct
{
    dboolean    active;
    dboolean    doDraw;
    int         delay;
    int         times;
} keyflash_t;

static keyflash_t flashCards[NUMCARDS];	/* INFO FOR FLASHING CARDS & SKULLS */

#define ST_HUDCOLOR     ARGB16(20, 31, 0, 0)
#define	FLASHDELAY      8       /* # of tics delay (1/30 sec) */
#define FLASHTIMES      6       /* # of times to flash new frag amount (EVEN!) */
#define ST_HEALTHTEXTX  20
#define ST_HEALTHTEXTY  164
#define ST_ARMORTEXTX   196
#define ST_ARMORTEXTY   164
#define ST_KEYX         64
#define ST_KEYY         172
#define ST_JMESSAGES    45
#define ST_MSGTIMEOUT   (5*TICRATE)
#define ST_MSGFADESTART (ST_MSGTIMEOUT - (1*TICRATE))
#define ST_MSGFADETIME  5
#define ST_FONTWHSIZE   8
#define ST_FONTNUMSET   32	//# of fonts per row in font pic
#define ST_FONTSTART    '!'	// the first font characters
#define ST_FONTEND      '_'	// the last font characters
#define ST_FONTSIZE     (ST_FONTEND - ST_FONTSTART + 1) // Calculate # of glyphs in font.


static rcolor       st_flashcolor;
static byte         st_flashalpha;
static int          st_msgtic = 0;
static int          st_msgalpha = 0xf0;
static char*        st_msg = NULL;
static int          lump_bfontnum;
static lumpinfo_t*  lump_bfont;
static short*       lump_sfont;
static short*       lump_status;
static gfx_t        st_gfxsfont[64];
static gfx_t        st_gfxbfont[NUMSYMBOLS];
static gfx_t        st_gfxstatus[NUMSTATUSITEMS];
static byte*        st_fontbuffer;
static uint32       st_sfontpalparam;
static uint32       st_bfontpalparam;

#define ST_FONT1		16
#define ST_FONT2		42
#define ST_MISCFONT		10
#define ST_NUMBERS		0
#define ST_SKULLS		70
#define ST_THERMO		68
#define ST_MICONS		78

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} fontmap_t;

//
// ST_ClearMessage
//

void ST_ClearMessage(void)
{
    st_msgtic = 0;
    st_msgalpha = 0xf0;
    st_msg = NULL;
}

//
// ST_Ticker
//

void ST_Ticker(void)
{
    int ind = 0;
    player_t* plyr;
    
    plyr = &players[consoleplayer];
    
    //
    // keycard stuff
    //
    
    /* */
    /* Tried to open a CARD or SKULL door? */
    /* */
    for(ind = 0; ind < NUMCARDS; ind++)
    {
        /* CHECK FOR INITIALIZATION */
        if(plyr->tryopen[ind])
        {
            plyr->tryopen[ind] = false;
            flashCards[ind].active = true;
            flashCards[ind].delay = FLASHDELAY;
            flashCards[ind].times = FLASHTIMES+1;
            flashCards[ind].doDraw = false;
        }
        
        /* MIGHT AS WELL DO TICKING IN THE SAME LOOP! */
        if(flashCards[ind].active && !--flashCards[ind].delay)
        {
            flashCards[ind].delay = FLASHDELAY;
            flashCards[ind].doDraw ^= 1;
            
            if(!--flashCards[ind].times)
                flashCards[ind].active = false;
            
            if(flashCards[ind].doDraw && flashCards[ind].active)
                S_StartSound(NULL, sfx_itemup);
        }
    }
    
    //
    // messages
    //
    if(plyr->message)
    {
        ST_ClearMessage();
        st_msg = plyr->message;
        plyr->message = NULL;
    }
    
    if(st_msg || plyr->messagepic >= 0)
    {
        st_msgtic++;
        
        if(st_msgtic >= ST_MSGFADESTART)
            st_msgalpha = MAX((st_msgalpha -= ST_MSGFADETIME), 0);
        
        if(st_msgtic >= ST_MSGTIMEOUT)
        {
            ST_ClearMessage();
            plyr->messagepic = -1;
        }
    }

    //
    // flashes
    //
    if(plyr->cameratarget == plyr->mo)
        ST_UpdateFlash();
}

//
// ST_UpdateFlash
//

#define ST_MAXDMGCOUNT  160
#define ST_MAXSTRCOUNT  32
#define ST_MAXBONCOUNT  100

void ST_UpdateFlash(void)
{
    player_t* p = &players[consoleplayer];

    st_flashcolor = 0;

    // invulnerability flash (white)
    if(p->powers[pw_invulnerability] > 61 || (p->powers[pw_invulnerability] & 8))
    {
        st_flashcolor = RGB15(16, 16, 16);
        st_flashalpha = 16;
    }
    // bfg flash (green)
    else if(p->bfgcount)
    {
        st_flashcolor = RGB15(0, 31, 0);
        st_flashalpha = ((p->bfgcount << 1) >> 3);
    }
    // damage and strength flash (red)
    else if(p->damagecount || (p->powers[pw_strength] > 1))
    {
        int r1 = p->damagecount;
        int r2 = p->powers[pw_strength];

        if(r1)
        {
            if(r1 > ST_MAXDMGCOUNT)
                r1 = ST_MAXDMGCOUNT;
        }

        if(r2 == 1)
            r2 = 0;
        else if(r2 > ST_MAXSTRCOUNT)
            r2 = ST_MAXSTRCOUNT;

        // take priority based on value
        if(r1 > r2)
        {
            st_flashcolor = RGB15(31, 0, 0);
            st_flashalpha = ((r1 << 1) >> 3);
        }
        else
        {
            st_flashcolor = RGB15(31, 0, 0);
            st_flashalpha = ((r2 << 1) >> 3);
        }
    }
    // suit flash (green/yellow)
    else if(p->powers[pw_ironfeet] > 61 || (p->powers[pw_ironfeet] & 8))
    {
        st_flashcolor = RGB15(0, 31, 4);
        st_flashalpha = 12;
    }
    // bonus flash (yellow)
    else if(p->bonuscount)
    {
        int c1 = p->bonuscount + 8;

        if(c1 > ST_MAXBONCOUNT)
            c1 = ST_MAXBONCOUNT;

        st_flashcolor = RGB15(31, 31, 15);
        st_flashalpha = ((c1 + (c1 >> 1)) >> 3);
    }
}

//
// ST_DrawStatusItem
//

static const fontmap_t statusmap[NUMSTATUSITEMS] =
{
    { 0, 0, 40, 6 },
    { 40, 0, 36, 6 },
    { 0, 6, 9, 10 },
    { 9, 6, 9, 10 },
    { 18, 6, 9, 10 },
    { 27, 6, 9, 10 },
    { 36, 6, 9, 10 },
    { 45, 6, 9, 10 }
};

static void ST_DrawStatusItem(int index, int x, int y)
{
    byte* data;
    fontmap_t* fontmap;
    int pw;
    int ph;

    I_CheckGFX();

    data = (byte*)(lump_status + 4);
    fontmap = (fontmap_t*)&statusmap[index];
    pw = R_PadTextureDims(fontmap->w);
    ph = R_PadTextureDims(fontmap->h);

    st_fontbuffer = R_CopyPic(data, fontmap->x, fontmap->y,
        fontmap->h, pw, fontmap->w, lump_status[0]);

    if(!I_AllocVBlock(
        &st_gfxstatus[index],
        st_fontbuffer,
        pw * ph,
        TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT,
        R_GetTextureSize(pw),
        R_GetTextureSize(ph),
        GL_RGB256))
        return;

    GFX_TEX_FORMAT = st_gfxstatus[index].params;
    GFX_PAL_FORMAT = st_bfontpalparam;

    GFX_POLY_FORMAT =
        POLY_ALPHA(18)      |
        POLY_ID(0)          |
        POLY_CULL_NONE      |
        POLY_MODULATION;

    GFX_COLOR       = RGB15(31, 31, 31);
    GFX_BEGIN       = GL_QUADS;
    GFX_TEX_COORD   = COORD_PACK(0, 0);
    GFX_VERTEX16    = VERTEX_PACK(x, y);
    GFX_VERTEX16    = VERTEX_PACK(-2, 0);
    GFX_TEX_COORD   = COORD_PACK(pw, 0);
    GFX_VERTEX16    = VERTEX_PACK(pw + x, y);
    GFX_VERTEX16    = VERTEX_PACK(-2, 0);
    GFX_TEX_COORD   = COORD_PACK(pw, ph);
    GFX_VERTEX16    = VERTEX_PACK(pw + x, ph + y);
    GFX_VERTEX16    = VERTEX_PACK(-2, 0);
    GFX_TEX_COORD   = COORD_PACK(0, ph);
    GFX_VERTEX16    = VERTEX_PACK(x, ph + y);
    GFX_VERTEX16    = VERTEX_PACK(-2, 0);
}

//
// ST_DrawKey
//

static void ST_DrawKey(player_t* player, int key, int offset)
{
    if(player->cards[key] || (flashCards[key].doDraw && flashCards[key].active))
        ST_DrawStatusItem(2 + key, ST_KEYX + offset, ST_KEYY);
}

//
// ST_Drawer
//

void ST_Drawer(void)
{
    player_t* plyr = &players[consoleplayer];

    if(demoplayback)
        return;

    if(plyr->cheats & CF_NOCLIP)
        return;

    GFX_ORTHO();

    // messages
    if(st_msg)
    {
        int alpha = (st_msgalpha >> 3);

        if(alpha > 0)
        {
            ST_DrawMessage(10, 10,
                ARGB16(alpha, 31, 31, 31), st_msg);
        }
    }
    else if(automapactive)
    {
        ST_DrawMessage(10, 10, ARGB16(31, 31, 31, 31),
            "Level %i: %s", gamemap, P_GetMapInfo(gamemap)->mapname);
        ST_DrawMessage(10, 20, ARGB16(31, 31, 31, 31),
            "Monsters:  %i / %i", plyr->killcount, totalkills);
        ST_DrawMessage(10, 30, ARGB16(31, 31, 31, 31),
            "Items:     %i / %i", plyr->itemcount, totalitems);
        ST_DrawMessage(10, 40, ARGB16(31, 31, 31, 31),
            "Secrets:   %i / %i", plyr->secretcount, totalsecret);
        ST_DrawMessage(10, 50, ARGB16(31, 31, 31, 31),
            "Time:      %2.2d:%2.2d", (leveltime / TICRATE) / 60, (leveltime / TICRATE) % 60);
    }

    if(!automapactive && !menuactive)
    {
        // health text
        ST_DrawStatusItem(0, ST_HEALTHTEXTX, ST_HEALTHTEXTY);
        // armor text
        ST_DrawStatusItem(1, ST_ARMORTEXTX, ST_ARMORTEXTY);
        // blue card
        ST_DrawKey(plyr, it_bluecard, 0);
        // yellow card
        ST_DrawKey(plyr, it_yellowcard, 10);
        // red card
        ST_DrawKey(plyr, it_redcard, 20);
        // blue skull
        ST_DrawKey(plyr, it_blueskull, 0);
        // yellow skull
        ST_DrawKey(plyr, it_yellowskull, 10);
        // red skull
        ST_DrawKey(plyr, it_redskull, 20);
        // health value
        ST_DrawNumber(40, 172, plyr->health, 0, ST_HUDCOLOR);
        // armor value
        ST_DrawNumber(216, 172, plyr->armorpoints, 0, ST_HUDCOLOR);
        // ammo counter
        if(weaponinfo[plyr->readyweapon].ammo != am_noammo)
            ST_DrawNumber(128, 172, plyr->ammo[weaponinfo[plyr->readyweapon].ammo], 0, ST_HUDCOLOR);
    }

    // flash overlay
    if(st_flashcolor && st_flashalpha)
    {
        I_CheckGFX();

        GFX_POLY_FORMAT =
            POLY_ALPHA(st_flashalpha)   |
            POLY_ID(63)                 |
            POLY_CULL_NONE              |
            POLY_MODULATION;

        GFX_TEX_FORMAT  = 0;
        GFX_PAL_FORMAT  = 0;
        GFX_COLOR       = st_flashcolor;
        GFX_SCREENRECT();
    }
}

static const fontmap_t bfontmap[NUMSYMBOLS + 1] =
{
    { 120, 14, 13, 13 },
    { 134, 14, 9, 13 },
    { 144, 14, 14, 13 },
    { 159, 14, 14, 13 },
    { 174, 14, 16, 13 },
    { 191, 14, 13, 13 },
    { 205, 14, 13, 13 },
    { 219, 14, 14, 13 },
    { 234, 14, 14, 13 },
    { 0, 29, 13, 13 },
    { 67, 28, 14, 13 },	// -
    { 36, 28, 15, 14 },	// %
    { 28, 28, 7, 14 },	// !
    { 14, 29, 6, 13 },	// .
    { 52, 28, 13, 13 },	// ?
    { 21, 29, 6, 13 },	// :
    { 0, 0, 13, 13 },
    { 14, 0, 13, 13 },
    { 28, 0, 13, 13 },
    { 42, 0, 14, 13 },
    { 57, 0, 14, 13 },
    { 72, 0, 10, 13 },
    { 87, 0, 15, 13 },
    { 103, 0, 15, 13 },
    { 119, 0, 6, 13 },
    { 126, 0, 13, 13 },
    { 140, 0, 14, 13 },
    { 155, 0, 11, 13 },
    { 167, 0, 15, 13 },
    { 183, 0, 16, 13 },
    { 200, 0, 15, 13 },
    { 216, 0, 13, 13 },
    { 230, 0, 15, 13 },
    { 246, 0, 13, 13 },
    { 0, 14, 14, 13 },
    { 15, 14, 14, 13 },
    { 30, 14, 13, 13 },
    { 44, 14, 15, 13 },
    { 60, 14, 15, 13 },
    { 76, 14, 15, 13 },
    { 92, 14, 13, 13 },
    { 106, 14, 13, 13 },
    { 83, 31, 10, 11 },
    { 93, 31, 10, 11 },
    { 103, 31, 11, 11 },
    { 114, 31, 11, 11 },
    { 125, 31, 11, 11 },
    { 136, 31, 11, 11 },
    { 147, 31, 12, 11 },
    { 159, 31, 12, 11 },
    { 171, 31, 4, 11 },
    { 175, 31, 10, 11 },
    { 185, 31, 11, 11 },
    { 196, 31, 9, 11 },
    { 205, 31, 12, 11 },
    { 217, 31, 13, 11 },
    { 230, 31, 12, 11 },
    { 242, 31, 11, 11 },
    { 0, 43, 12, 11 },
    { 12, 43, 11, 11 },
    { 23, 43, 11, 11 },
    { 34, 43, 10, 11 },
    { 44, 43, 11, 11 },
    { 55, 43, 12, 11 },
    { 67, 43, 13, 11 },
    { 80, 43, 13, 11 },
    { 93, 43, 10, 11 },
    { 103, 43, 11, 11 },
    { 0, 95, 108, 11 },
    { 108, 95, 6, 11 },
    { 0, 54, 32, 26 },
    { 32, 54, 32, 26 },
    { 64, 54, 32, 26 },
    { 96, 54, 32, 26 },
    { 128, 54, 32, 26 },
    { 160, 54, 32, 26 },
    { 192, 54, 32, 26 },
    { 224, 54, 32, 26 },
    { 134, 97, 7, 11 },
    { 114, 95, 20, 18 },
    { 105, 80, 15, 15 },
    { 120, 80, 15, 15 },
    { 135, 80, 15, 15 },
    { 150, 80, 15, 15 },
    { 45, 80, 15, 15 },
    { 60, 80, 15, 15 },
    { 75, 80, 15, 15 },
    { 90, 80, 15, 15 },
    { 165, 80, 15, 15 },
    { 180, 80, 15, 15 },
    { 0, 80, 15, 15 },
    { 15, 80, 15, 15 },
    { 195, 80, 15, 15 },
    { 30, 80, 15, 15 },
    { 156, 96, 13, 13 },
    { 143, 96, 13, 13 },
    { 169, 96, 7, 13 },
    { -1, -1, -1, -1 }
};

//
// ST_CenterString
//

int ST_CenterString(const char* string)
{
    int width = 0;
    char t = 0;
    int id = 0;
    int len = 0;
    int i = 0;
    
    len = strlen(string);
    
    for(i = 0; i < len; i++)
    {
        t = string[i];
        
        switch(t)
        {
        case 0x20: width += 6;
            break;
        case '-': width += bfontmap[ST_MISCFONT].w;
            break;
        case '%': width += bfontmap[ST_MISCFONT + 1].w;
            break;
        case '!': width += bfontmap[ST_MISCFONT + 2].w;
            break;
        case '.': width += bfontmap[ST_MISCFONT + 3].w;
            break;
        case '?': width += bfontmap[ST_MISCFONT + 4].w;
            break;
        case ':': width += bfontmap[ST_MISCFONT + 5].w;
            break;
        default:
            if(t >= 'A' && t <= 'Z')
            {
                id = t - 'A';
                width += bfontmap[ST_FONT1 + id].w;
            }
            if(t >= 'a' && t <= 'z')
            {
                id = t - 'a';
                width += bfontmap[ST_FONT2 + id].w;
            }
            if(t >= '0' && t <= '9')
            {
                id = t - '0';
                width += bfontmap[ST_NUMBERS + id].w;
            }
            break;
        }
    }
    
    return (124 - (width / 2));
}

//
// ST_DrawBigFont
//

int ST_DrawBigFont(int x, int y, rcolor color, const char* string)
{
    byte* data;
    short* lumpdata;
    int width;
    int i;
    int c;
    uint32 flags;

    I_CheckGFX();

    if(x <= -1)
        x = ST_CenterString(string);

    if(lump_bfont->cache == NULL)
        W_CacheLumpNum(lump_bfontnum, PU_CACHE);

    lumpdata = (short*)lump_bfont->cache;

    width   = lumpdata[0];
    data    = (byte*)(lumpdata + 4);

    flags = POLY_ALPHA(color >> 15) |
            POLY_ID(0)              |
            POLY_CULL_NONE          |
            POLY_MODULATION;

    for(i = 0; i < strlen(string); i++)
    {
        int pw;
        int ph;
        fontmap_t* fontmap;
        int index = 0;
        int dy;

        c = string[i];
        dy = 0;

        if(c == '\n' || c == '\t')
            continue;
        else if(c == 0x20)
        {
            x += 6;
            continue;
        }
        else
        {
            if(c >= '0' && c <= '9')    { index = (c - '0') + ST_NUMBERS;       }
            if(c >= 'A' && c <= 'Z')    { index = (c - 'A') + ST_FONT1;         }
            if(c >= 'a' && c <= 'z')    { index = (c - 'a') + ST_FONT2; dy = 2; }
            if(c == '-')                { index = ST_MISCFONT;                  }
            if(c == '%')                { index = ST_MISCFONT + 1;              }
            if(c == '!')                { index = ST_MISCFONT + 2;              }
            if(c == '.')                { index = ST_MISCFONT + 3;              }
            if(c == '?')                { index = ST_MISCFONT + 4;              }
            if(c == ':')                { index = ST_MISCFONT + 5;              }
            if(c == '*')                { index = ST_SKULLS + menuskullcounter; }

            if(c == '/')
            {
                c = string[++i];

                switch(c)
                {
                    // up arrow
                case 'u':
                    index = ST_MICONS + 17;
                    break;
                    // down arrow
                case 'd':
                    index = ST_MICONS + 16;
                    break;
                    // right arrow
                case 'r':
                    index = ST_MICONS + 18;
                    break;
                    // left arrow
                case 'l':
                    index = ST_MICONS;
                    break;
                    // cursor box
                case 'b':
                    index = ST_MICONS + 1;
                    break;
                    // thermbar
                case 't':
                    index = ST_THERMO;
                    break;
                    // thermcursor
                case 's':
                    index = ST_THERMO + 1;
                    flags |= POLY_DEPTHTEST_EQUAL;
                    break;
                default:
                    return 0;
                }
            }
        }

        fontmap = (fontmap_t*)&bfontmap[index];
        pw = R_PadTextureDims(fontmap->w);
        ph = R_PadTextureDims(fontmap->h);

        st_fontbuffer = R_CopyPic(data, fontmap->x, fontmap->y,
            fontmap->h, pw, fontmap->w, width);

        if(!I_AllocVBlock(
            &st_gfxbfont[index],
            st_fontbuffer,
            pw * ph,
            TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT,
            R_GetTextureSize(pw),
            R_GetTextureSize(ph),
            GL_RGB256))
            continue;

        GFX_TEX_FORMAT = st_gfxbfont[index].params;
        GFX_PAL_FORMAT = st_bfontpalparam;

        GFX_POLY_FORMAT = flags;
        GFX_COLOR       = (color & 0x7FFF);
        GFX_BEGIN       = GL_QUADS;
        GFX_TEX_COORD   = COORD_PACK(0, 0);
        GFX_VERTEX16    = VERTEX_PACK(x, y + dy);
        GFX_VERTEX16    = VERTEX_PACK(-2, 0);
        GFX_TEX_COORD   = COORD_PACK(pw, 0);
        GFX_VERTEX16    = VERTEX_PACK(pw + x, y + dy);
        GFX_VERTEX16    = VERTEX_PACK(-2, 0);
        GFX_TEX_COORD   = COORD_PACK(pw, ph);
        GFX_VERTEX16    = VERTEX_PACK(pw + x, ph + y + dy);
        GFX_VERTEX16    = VERTEX_PACK(-2, 0);
        GFX_TEX_COORD   = COORD_PACK(0, ph);
        GFX_VERTEX16    = VERTEX_PACK(x, ph + y + dy);
        GFX_VERTEX16    = VERTEX_PACK(-2, 0);

        x += fontmap->w + 1;
    }

    return x;
}

//
// ST_DrawMessage
//

int ST_DrawMessage(int x, int y, rcolor color, const char* string, ...)
{
    int i;
    va_list	va;
    byte* data;
    char msg[128];
    const int ix = x;

    I_CheckGFX();

    va_start(va, string);
    vsprintf(msg, string, va);
    va_end(va);

    data = (byte*)(lump_sfont + 4);

    for(i = 0; i < strlen(msg); i++)
    {
        int c;
        int start = 0;
        int	col;
        int row;

        c = toupper((int)msg[i]);

        if(c == '\t')
        {
            while(x % 64) x++;
            continue;
        }

        if(c == '\n')
        {
            y += ST_FONTWHSIZE;
            x = ix;
            continue;
        }

        if(c == 0x20)
        {
            if(x > 192)
            {
                y += ST_FONTWHSIZE;
                x = ix;
                continue;
            }
            else
                x += ST_FONTWHSIZE;
        }
        else
        {
            start = (c - ST_FONTSTART);
            col = (start & (ST_FONTNUMSET - 1)) * ST_FONTWHSIZE;
            row = (start >= ST_FONTNUMSET) ? 8 : 0;

            st_fontbuffer = R_CopyPic(data, col, row,
                ST_FONTWHSIZE, ST_FONTWHSIZE, ST_FONTWHSIZE, lump_sfont[0]);

            if(!I_AllocVBlock(
                &st_gfxsfont[start],
                st_fontbuffer,
                64,
                TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT,
                TEXTURE_SIZE_8,
                TEXTURE_SIZE_8,
                GL_RGB256))
                continue;

            GFX_TEX_FORMAT = st_gfxsfont[start].params;
            GFX_PAL_FORMAT = st_sfontpalparam;

            GFX_POLY_FORMAT =
                POLY_ALPHA(color >> 15) |
                POLY_ID(0)              |
                POLY_CULL_NONE          |
                POLY_MODULATION;

            GFX_COLOR       = (color & 0x7FFF);
            GFX_BEGIN       = GL_QUADS;
            GFX_TEX_COORD   = COORD_PACK(0, 0);
            GFX_VERTEX16    = VERTEX_PACK(x, y);
            GFX_VERTEX16    = VERTEX_PACK(-2, 0);
            GFX_TEX_COORD   = COORD_PACK(ST_FONTWHSIZE, 0);
            GFX_VERTEX16    = VERTEX_PACK(ST_FONTWHSIZE + x, y);
            GFX_VERTEX16    = VERTEX_PACK(-2, 0);
            GFX_TEX_COORD   = COORD_PACK(ST_FONTWHSIZE, ST_FONTWHSIZE);
            GFX_VERTEX16    = VERTEX_PACK(ST_FONTWHSIZE + x, ST_FONTWHSIZE + y);
            GFX_VERTEX16    = VERTEX_PACK(-2, 0);
            GFX_TEX_COORD   = COORD_PACK(0, ST_FONTWHSIZE);
            GFX_VERTEX16    = VERTEX_PACK(x, ST_FONTWHSIZE + y);
            GFX_VERTEX16    = VERTEX_PACK(-2, 0);

            x += ST_FONTWHSIZE;
        }
    }

    return x;
}

//
// ST_DrawNumber
//
//

void ST_DrawNumber(int x, int y, int num, int type, rcolor c)
{
    int digits[16];
    int nx = 0;
    int count;
    int j;
    char str[2];

    for(count = 0, j = 0; count < 16; count++, j++)
    {
        digits[j] = num % 10;
        nx += bfontmap[ST_NUMBERS + digits[j]].w;

        num /= 10;

        if(!num)
            break;
    }

    if(type == 0)
        x -= (nx >> 1);

    if(type == 0 || type == 1)
    {
        if(count < 0)
            return;

        while(count >= 0)
        {
            sprintf(str, "%i", digits[j]);
            ST_DrawBigFont(x, y, c, str);

            x += bfontmap[ST_NUMBERS + digits[j]].w;

            count--;
            j--;
        }
    }
    else
    {
        if(count < 0)
            return;

        j = 0;

        while(count >= 0)
        {
            x -= bfontmap[ST_NUMBERS + digits[j]].w;

            sprintf(str, "%i", digits[j]);
            ST_DrawBigFont(x, y, c, str);

            count--;
            j++;
        }
    }
}

//
// ST_Init
//

void ST_Init(void)
{
    int i = 0;

    lump_sfont  = (short*)W_CacheLumpName("SFONT", PU_STATIC);
    lump_status = (short*)W_CacheLumpName("STATUS", PU_STATIC);

    lump_bfontnum = W_GetNumForName("SYMBOLS");
    lump_bfont = &lumpinfo[lump_bfontnum];

    st_bfontpalparam = R_CachePalette("SYMBOLS");
    st_sfontpalparam = R_CachePalette("SFONT");
    
    // setup keycards
    
    for (i = 0; i < NUMCARDS; i++)
    {
        flashCards[i].active = false;
        players[consoleplayer].tryopen[i] = false;
    }
    
    // setup hud messages
    
    ST_ClearMessage();
}

