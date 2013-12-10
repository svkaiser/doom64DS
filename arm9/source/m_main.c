#include "doomdef.h"
#include "d_main.h"
#include "p_local.h"
#include "sounds.h"
#include "s_sound.h"
#include "st_main.h"
#include "r_local.h"
#include "g_game.h"

typedef struct
{
    char* name;
    int x;
    int y;
} menuitem_t;

static void(*menudrawfunc)(void) = NULL;
static int m_menuitems = 0;
static int m_currentmenu = 0;
static int m_itemOn = 0;
static menuitem_t* menu;
static int menuskulldelay = 0;
static dboolean showfullitemvalue[2] = { false, false };
static int m_menualpha = 0x1F;
static int m_menuexec = -1;
static int m_menuchoice = -1;
static int m_levelwarp = 0;
static int m_soundtest_id = 0;

#define MENUFONTRED ARGB16(m_menualpha, 31, 0, 0)
#define MENUFONTWHITE ARGB16(m_menualpha, 31, 31, 31)
#define M_DRAWSKULL() ST_DrawBigFont(menu[m_itemOn].x - 34, menu[m_itemOn].y - 8, MENUFONTWHITE, "*")
#define M_DRAWARROW() ST_DrawBigFont(menu[m_itemOn].x - 12, menu[m_itemOn].y - 2, MENUFONTWHITE, "/l")
#define M_DRAWDYNAMICITEM1(item, offset, color, str)            \
    ST_DrawBigFont(menulist[item].x + offset, menulist[item].y, \
    color, str)
#define M_DRAWDYNAMICITEM2(item, offset, color, str)            \
    ST_DrawMessage(menulist[item].x + offset, menulist[item].y, \
    color, str)

dboolean menuactive = false;
int menuskullcounter = 0;

enum
{
    menu_newgame,
    menu_options1,
    menu_options2,
    menu_mainmenu,
    menu_restartlevel,
    menu_debug,
    menu_controls,
    menu_volume,
    menu_display,
    menu_returnpaused,
    menu_quitmain_yes,
    menu_quitmain_no,
    menu_restart_yes,
    menu_restart_no,
    menu_levelwarp,
    menu_soundtest,
    menu_nolights,
    menu_godmode,
    menu_noclip,
    menu_allkeys,
    menu_allweapons,
    menu_mapall,
    menu_lockmonsters,
    menu_devmode,
    menu_returnpaused2,
    menu_brightness,
    menu_swapscreens,
    menu_messages,
    menu_statusbar,
    menu_displaydefault,
    menu_returndisplay,
    menu_musicvolume,
    menu_soundvolume,
    menu_sounddefault,
    menu_returnsound,
    menu_skbaby,
    menu_skeasy,
    menu_skmedium,
    menu_skhard,
    NUM_MENU_ITEMS
};

#define MENU_MAINMENU       menu_newgame
#define MENU_GAMEMENU       menu_options2
#define MENU_GAMEOPTIONS    menu_controls
#define MENU_QUITTOMAIN     menu_quitmain_yes
#define MENU_RESTARTLEVEL   menu_restart_yes
#define MENU_DEBUGMENU      menu_levelwarp
#define MENU_DISPLAY        menu_brightness
#define MENU_VOLUME         menu_musicvolume
#define MENU_SKILL          menu_skbaby

static menuitem_t menulist[NUM_MENU_ITEMS] =
{
    { "New Game", 82, 144 },
    { "Options", 82, 162 },
    { "Options", 82, 64 },
    { "Main Menu", 82, 82 },
    { "Restart Level", 82, 100 },
    { "Debug", 82, 118 },
    { "Controls", 82, 64 },
    { "Volume", 82, 82 },
    { "Display", 82, 100 },
    { "/r Return", 82, 118 },
    { "Yes", 108, 80 },
    { "No", 108, 98 },
    { "Yes", 108, 80 },
    { "No", 108, 98 },
    { "Warp To Level:", 32, 40 },
    { "Sound Test:", 32, 52 },
    { "No Lights", 32, 64 },
    { "God Mode", 32, 76 },
    { "No Clip", 32, 88 },
    { "All Keys", 32, 100 },
    { "All Weapons", 32, 112 },
    { "Map Everything", 32, 124 },
    { "Lock Monsters", 32, 136 },
    { "Developer Mode", 32, 148 },
    { "Return", 32, 160 },
    { "Brightness", 40, 48 },
    { "Swap Screen:", 40, 84 },
    { "Messages:", 40, 102 },
    { "Status Bar:", 40, 120 },
    { "Default", 40, 138 },
    { "/r Return", 40, 156 },
    { "Music Volume", 64, 52 },
    { "Sound Volume", 64, 88 },
    { "Default", 64, 124 },
    { "/r Return", 64, 142 },
    { "Be Gentle!", 70, 64 },
    { "Bring It On!", 70, 82 },
    { "I Own Doom!", 70, 100 },
    { "Watch Me Die!", 70, 118 }
};

//
// M_SetMenu
//

void M_SetMenu(int item, int numitems, void(*drawer)(void))
{
    m_menuitems = numitems;
    menu = menulist + item;
    m_currentmenu = item;
    menudrawfunc = drawer;
    m_itemOn = 0;
}

//
// M_ClearMenu
//

static void M_ClearMenu(void)
{
    m_menualpha = 0x1F;
    m_menuitems = 0;
    menudrawfunc = NULL;
    m_itemOn = 0;
}

//
// M_FadeMenuOut
//

static int M_FadeMenuOut(void)
{
    switch(gamestate)
    {
    case GS_LEVEL:
        P_Ticker();
        break;
    case GS_WISTATS:
        WI_Ticker();
        break;
    default:
        break;
    }

    m_menualpha -= 4;

    if(m_menualpha <= 1)
    {
        m_menualpha = 1;
        return 1;
    }

    return 0;
}

//
// M_FadeMenuIn
//

static int M_FadeMenuIn(void)
{
    switch(gamestate)
    {
    case GS_LEVEL:
        P_Ticker();
        break;
    case GS_WISTATS:
        WI_Ticker();
    default:
        break;
    }

    m_menualpha += 4;

    if(m_menualpha >= 0x1F)
    {
        m_menualpha = 0x1F;
        return 1;
    }

    return 0;
}

//
// M_FadeMenuDrawer
//

static void M_FadeMenuDrawer(void)
{
    switch(gamestate)
    {
    case GS_LEVEL:
        P_Drawer();
        break;
    case GS_WISTATS:
        WI_Drawer();
        break;
    default:
        GFX_ORTHO();
        GFX_CLEAR_COLOR = 0x1F0000;
        R_SlamBackground("TITLE", 23, 40, 0xFFFFF);
        break;
    }
}

//
// M_GenericDrawer
//

static void M_GenericDrawer(void)
{
    int i;

    switch(m_currentmenu)
    {
    case MENU_GAMEMENU:
        ST_DrawBigFont(-1, 16, MENUFONTRED, "Paused");
        ST_DrawBigFont(-1, 160, MENUFONTWHITE, "Press Start To Exit");
        break;
    case MENU_SKILL:
        ST_DrawBigFont(-1, 16, MENUFONTRED, "Choose Your Skill...");
        break;
    case MENU_GAMEOPTIONS:
        ST_DrawBigFont(-1, 16, MENUFONTRED, "Options");
        break;
    case MENU_QUITTOMAIN:
    case MENU_RESTARTLEVEL:
        ST_DrawBigFont(-1, 16, MENUFONTRED, "Quit Game?");
        break;
    default:
        break;
    }

    for(i = 0; i < m_menuitems; i++)
        ST_DrawBigFont(menu[i].x, menu[i].y, MENUFONTRED, menu[i].name);

    M_DRAWSKULL();
}

//
// M_VolumeDrawer
//

static void M_VolumeDrawer(void)
{
    int i;

    ST_DrawBigFont(-1, 16, MENUFONTRED, "Volume");
    ST_DrawBigFont(menu[0].x, menu[0].y, MENUFONTRED, menu[0].name);
    ST_DrawBigFont(menu[0].x, menu[0].y + 18, MENUFONTWHITE, "/t");
    ST_DrawBigFont(menu[0].x, menu[0].y + 18, MENUFONTWHITE, "/s");

    ST_DrawBigFont(menu[1].x, menu[1].y, MENUFONTRED, menu[1].name);
    ST_DrawBigFont(menu[1].x, menu[1].y + 18, MENUFONTWHITE, "/t");
    ST_DrawBigFont(menu[1].x, menu[1].y + 18, MENUFONTWHITE, "/s");

    for(i = 2; i < m_menuitems; i++)
        ST_DrawBigFont(menu[i].x, menu[i].y, MENUFONTRED, menu[i].name);

    M_DRAWSKULL();
}

//
// M_DisplayDrawer
//

static void M_DisplayDrawer(void)
{
    int i;

    ST_DrawBigFont(-1, 16, MENUFONTRED, "Display");
    ST_DrawBigFont(menu[0].x, menu[0].y, MENUFONTRED, menu[0].name);
    ST_DrawBigFont(menu[0].x, menu[0].y + 18, MENUFONTWHITE, "/t");
    ST_DrawBigFont(menu[0].x, menu[0].y + 18, MENUFONTWHITE, "/s");

    for(i = 1; i < m_menuitems; i++)
        ST_DrawBigFont(menu[i].x, menu[i].y, MENUFONTRED, menu[i].name);

    M_DRAWDYNAMICITEM1(menu_swapscreens, 160, MENUFONTRED, "Off");
    M_DRAWDYNAMICITEM1(menu_messages, 160, MENUFONTRED, "On");
    M_DRAWDYNAMICITEM1(menu_statusbar, 160, MENUFONTRED, "On");
    M_DRAWSKULL();
}

//
// M_DebugDrawer
//

static void M_DebugDrawer(void)
{
    int i;

     ST_DrawBigFont(-1, 16, MENUFONTRED, "Debug");

    for(i = 0; i < m_menuitems; i++)
        ST_DrawMessage(menu[i].x, menu[i].y, MENUFONTWHITE, menu[i].name);

    ST_DrawMessage(menulist[menu_levelwarp].x + 160, menulist[menu_levelwarp].y,
        MENUFONTWHITE, ":%02d", m_levelwarp + 1);

    ST_DrawMessage(menulist[menu_soundtest].x + 160, menulist[menu_soundtest].y,
        MENUFONTWHITE, ":%03d", m_soundtest_id);

    M_DRAWDYNAMICITEM2(menu_nolights, 160, MENUFONTWHITE,
        nolights ? ":On" : ":Off");
    M_DRAWDYNAMICITEM2(menu_godmode, 160, MENUFONTWHITE,
        players[consoleplayer].cheats & CF_GODMODE ? ":On" : ":Off");
    M_DRAWDYNAMICITEM2(menu_noclip, 160, MENUFONTWHITE,
        players[consoleplayer].cheats & CF_NOCLIP ? ":On" : ":Off");
    M_DRAWDYNAMICITEM2(menu_allkeys, 160, MENUFONTWHITE,
        showfullitemvalue[0] ? ":100%%" : ":-");
    M_DRAWDYNAMICITEM2(menu_allweapons, 160, MENUFONTWHITE,
        showfullitemvalue[1] ? ":100%%" : ":-");
    M_DRAWDYNAMICITEM2(menu_mapall, 160, MENUFONTWHITE,
        amCheating ? ":On" : ":Off");
    M_DRAWDYNAMICITEM2(menu_lockmonsters, 160, MENUFONTWHITE,
        lockmonsters ? ":On" : ":Off");
    M_DRAWDYNAMICITEM2(menu_devmode, 160, MENUFONTWHITE,
        devparm ? ":On" : ":Off");

    M_DRAWARROW();
}

//
// M_AdvanceMenu
//

static void M_AdvanceMenu(int menu, int items, void(*drawer)(void))
{
    D_MiniLoop(NULL, NULL, M_FadeMenuDrawer, M_FadeMenuOut);
    M_SetMenu(menu, items, drawer);
    D_MiniLoop(NULL, NULL, M_FadeMenuDrawer, M_FadeMenuIn);
}

//
// M_SetMainMenu
//

void M_SetMainMenu(void)
{
    m_menualpha = 1;
    M_SetMenu(MENU_MAINMENU, 2, M_GenericDrawer);
    D_MiniLoop(NULL, NULL, M_FadeMenuDrawer, M_FadeMenuIn);
}

//
// M_Ticker
//

void M_Ticker(void)
{
    int i;
    int exec;
    player_t* p;

    p = &players[consoleplayer];

    if(!(menuskulldelay++ & 3))
        menuskullcounter = ((menuskullcounter + 1) & 7);

    if(m_menuexec <= -1)
        return;

    exec = m_menuexec;
    m_menuexec = -1;

    if(m_menuchoice == 0)
    {
        switch(exec)
        {
        case menu_levelwarp:
            m_levelwarp--;
            if(m_levelwarp < 0)
                m_levelwarp = 0;
            else
                S_StartSound(NULL, sfx_switch2);
            break;
        case menu_soundtest:
            m_soundtest_id--;
            if(m_soundtest_id < 0)
                m_soundtest_id = 0;
            else
                S_StartSound(NULL, sfx_switch2);
            break;
        default:
            break;
        }
    }
    else if(m_menuchoice == 1)
    {
        switch(exec)
        {
        case menu_levelwarp:
            m_levelwarp++;
            if(m_levelwarp > 31)
                m_levelwarp = 31;
            else
                S_StartSound(NULL, sfx_switch2);
            break;
        case menu_soundtest:
            m_soundtest_id++;
            if(m_soundtest_id > 123)
                m_soundtest_id = 123;
            else
                S_StartSound(NULL, sfx_switch2);
            break;
        default:
            break;
        }
    }
    else
    {
        switch(exec)
        {
        case menu_newgame:
            S_StartSound(NULL, sfx_pistol);
            M_AdvanceMenu(MENU_SKILL, 4, M_GenericDrawer);
            break;
        case menu_skbaby:
        case menu_skeasy:
        case menu_skmedium:
        case menu_skhard:
            S_StartSound(NULL, sfx_pistol);
            D_MiniLoop(NULL, NULL, M_FadeMenuDrawer, M_FadeMenuOut);
            G_DeferedInitNew(exec - menu_skbaby, startmap);
            M_ClearMenu();
            break;
        case menu_options1:
        case menu_options2:
            S_StartSound(NULL, sfx_pistol);
            M_AdvanceMenu(MENU_GAMEOPTIONS, 4, M_GenericDrawer);
            break;
        case menu_mainmenu:
            S_StartSound(NULL, sfx_pistol);
            M_AdvanceMenu(MENU_QUITTOMAIN, 2, M_GenericDrawer);
            break;
        case menu_restartlevel:
            S_StartSound(NULL, sfx_pistol);
            M_AdvanceMenu(MENU_RESTARTLEVEL, 2, M_GenericDrawer);
            break;
        case menu_debug:
            S_StartSound(NULL, sfx_pistol);
            M_AdvanceMenu(MENU_DEBUGMENU, 11, M_DebugDrawer);
            showfullitemvalue[0] = showfullitemvalue[1] = false;
            break;
        case menu_volume:
            S_StartSound(NULL, sfx_pistol);
            M_AdvanceMenu(MENU_VOLUME, 4, M_VolumeDrawer);
            break;
        case menu_display:
            S_StartSound(NULL, sfx_pistol);
            M_AdvanceMenu(MENU_DISPLAY, 6, M_DisplayDrawer);
            break;
        case menu_returnpaused:
        case menu_returnpaused2:
        case menu_quitmain_no:
        case menu_restart_no:
            S_StartSound(NULL, sfx_pistol);
            if(gamestate == GS_NONE)
                M_AdvanceMenu(MENU_MAINMENU, 2, M_GenericDrawer);
            else
                M_AdvanceMenu(MENU_GAMEMENU, 4, M_GenericDrawer);
            break;
        case menu_quitmain_yes:
            S_StartSound(NULL, sfx_pistol);
            M_ClearMenu();
            menuactive = false;
            gameaction = ga_title;
            break;
        case menu_restart_yes:
            S_StartSound(NULL, sfx_pistol);
            M_ClearMenu();
            menuactive = false;
            gameaction = ga_loadlevel;
            nextmap = gamemap;
            players[consoleplayer].playerstate = PST_REBORN;
        break;
        case menu_returndisplay:
        case menu_returnsound:
            S_StartSound(NULL, sfx_pistol);
            M_AdvanceMenu(MENU_GAMEOPTIONS, 4, M_GenericDrawer);
            break;
        case menu_levelwarp:
            gameaction = ga_warplevel;
            gamemap = nextmap = m_levelwarp + 1;
            M_ClearMenu();
            menuactive = false;
            break;
        case menu_soundtest:
            S_StartSound(NULL, m_soundtest_id + 1);
            break;
        case menu_nolights:
            S_StartSound(NULL, sfx_switch2);
            nolights ^= 1;
            break;
        case menu_godmode:
            S_StartSound(NULL, sfx_switch2);
            if(p->cheats & CF_GODMODE)
                p->cheats &= ~CF_GODMODE;
            else
                p->cheats |= CF_GODMODE;
            break;
        case menu_noclip:
            S_StartSound(NULL, sfx_switch2);
            if(p->cheats & CF_NOCLIP)
                p->cheats &= ~CF_NOCLIP;
            else
                p->cheats |= CF_NOCLIP;
            break;
        case menu_allkeys:
            for(i = 0; i < NUMCARDS; i++)
                p->cards[i] = true;

            showfullitemvalue[0] = true;
            break;
        case menu_allweapons:
            S_StartSound(NULL, sfx_switch2);
            for(i = 0; i < NUMWEAPONS; i++)
                p->weaponowned[i] = true;

            if(!p->backpack)
            {
                p->backpack = true;

                for (i = 0; i < NUMAMMO; i++)
                    p->maxammo[i] *= 2;
            }

            for (i = 0; i < NUMAMMO; i++)
                p->ammo[i] = p->maxammo[i];

            showfullitemvalue[1] = true;
            break;
        case menu_mapall:
            S_StartSound(NULL, sfx_switch2);
            amCheating ^= 2;
            break;
        case menu_lockmonsters:
            S_StartSound(NULL, sfx_switch2);
            lockmonsters ^= 1;
            break;
        case menu_devmode:
            S_StartSound(NULL, sfx_switch2);
            devparm ^= 1;
            break;
        default:
            break;
        }
    }

    m_menuchoice = -1;
}

//
// M_Drawer
//

void M_Drawer(void)
{
    int alpha;

    I_CheckGFX();

    if(m_currentmenu != MENU_MAINMENU)
        alpha = gamestate == GS_LEVEL ? 16 : 24;
    else
        alpha = 20;

    GFX_ORTHO();
    GFX_POLY_FORMAT =
        POLY_ALPHA(alpha)   |
        POLY_ID(63)         |
        POLY_CULL_NONE      |
        POLY_MODULATION;

        GFX_TEX_FORMAT  = 0;
        GFX_PAL_FORMAT  = 0;
        GFX_COLOR       = 0;
        GFX_SCREENRECT();

    if(menudrawfunc)
        menudrawfunc();
}

//
// M_Responder
//

dboolean M_Responder(event_t* ev)
{
    int rc = false;

    if(m_menualpha < 0x1F)
        return false;

    if(ev->type == ev_btndown)
    {
        if(gamestate != GS_NONE)
        {
            if(ev->data & KEY_START && gamestate != GS_SKIPPABLE)
            {
                menuactive ^= 1;

                if(menuactive && gamestate == GS_LEVEL)
                    M_SetMenu(MENU_GAMEMENU, 4, M_GenericDrawer);

                rc = true;
            }
        }

        if(menuactive && m_menuitems > 0)
        {
            if(ev->data & KEY_DOWN)
            {
                rc = true;

                S_StartSound(NULL, sfx_switch1);
                m_itemOn++;
                if(m_itemOn >= m_menuitems)
                    m_itemOn = 0;
            }
            if(ev->data & KEY_UP)
            {
                rc = true;

                S_StartSound(NULL, sfx_switch1);
                m_itemOn--;
                if(m_itemOn < 0)
                    m_itemOn = (m_menuitems - 1);
            }
            if(ev->data & KEY_A)
            {
                rc = true;
                m_menuexec = m_currentmenu + m_itemOn;
            }
            else if(ev->data & KEY_LEFT)
            {
                rc = true;
                m_menuchoice = 0;
                m_menuexec = m_currentmenu + m_itemOn;
            }
            else if(ev->data & KEY_RIGHT)
            {
                rc = true;
                m_menuchoice = 1;
                m_menuexec = m_currentmenu + m_itemOn;
            }
        }
    }
    else if(ev->type == ev_btnup)
    {
    }

    return rc;
}

