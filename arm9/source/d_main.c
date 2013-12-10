#include "ds.h"
#include "doomtype.h"
#include "d_main.h"
#include "z_zone.h"
#include "r_local.h"
#include "w_wad.h"
#include "p_local.h"
#include "g_game.h"
#include "s_sound.h"
#include "st_main.h"

gameaction_t    gameaction;
gamestate_t     gamestate;
skill_t         gameskill;
int             gamemap;
int             nextmap;
int             gametic         = 0;
int             validcount      = 1;
int             totalkills      = 0;
int             totalsecret     = 0;
int             totalitems      = 0;
int             consoleplayer   = 0;
int             leveltime       = 0;
int             compatflags     = 0;
dboolean        nomonsters      = false;
dboolean        lockmonsters    = false;
dboolean        netgame         = false;
dboolean        respawnmonsters = false;
dboolean        respawnspecials = false;
dboolean        fastparm        = false;    // checkparm of -fast
dboolean        respawnparm     = false;    // checkparm of -respawn
dboolean        respawnitem     = false;    // checkparm of -respawnitem
skill_t         startskill;
int             startmap;
dboolean        paused          = false;
dboolean        nolights        = false;
dboolean        devparm         = false;
int             dsvertices      = 0;
int             dspolygons      = 0;
int             gfxdmasize      = 0;

player_t players[MAXPLAYERS];
dboolean playeringame[MAXPLAYERS];
ticcmd_t netcmds[MAXPLAYERS][BACKUPTICS];

int maketic = 0;
int ticdup = 0;
int skiptics = 0;
fixed_t offsetms = 0;

static int      pagetic;
static int      splashstage;
static int      screenalpha;
static dboolean splashwait;

//
// D_Printf
//

void D_Printf(const char *s, ...)
{
    static char msg[100];
    va_list	va;
    
    va_start(va, s);
    vsprintf(msg, s, va);
    va_end(va);
    
    players[consoleplayer].message = msg;
}

//
// datoi
//

int datoi(const char *str)
{
    int val;
    int sign;
    int c;
    
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else
        sign = 1;
    
    val = 0;
    
    // check for hex
    
    if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        str += 2;
        while (1)
        {
            c = *str++;
            if(c >= '0' && c <= '9')
                val = (val<<4) + c - '0';
            else if(c >= 'a' && c <= 'f')
                val = (val<<4) + c - 'a' + 10;
            else if(c >= 'A' && c <= 'F')
                val = (val<<4) + c - 'A' + 10;
            else
                return val*sign;
        }
    }
    
    // check for character
    
    if(str[0] == '\'')
        return sign * str[1];
    
    // assume decimal
    
    while(1)
    {
        c = *str++;
        if(c <'0' || c > '9')
            return val*sign;
        
        val = val*10 + c - '0';
    }
    
    return 0;
}

//
// datof
//

float datof(char *str)
{
    double	val;
    int		sign;
    int		c;
    int		decimal, total;
    
    if(*str == '-')
    {
        sign = -1;
        str++;
    }
    else
        sign = 1;
    
    val = 0;
    
    // check for hex
    if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        str += 2;
        while(1)
        {
            c = *str++;
            if(c >= '0' && c <= '9')
                val = (val*16) + c - '0';
            else if(c >= 'a' && c <= 'f')
                val = (val*16) + c - 'a' + 10;
            else if(c >= 'A' && c <= 'F')
                val = (val*16) + c - 'A' + 10;
            else
                return (float)val*sign;
        }
    }
    
    // check for character
    if(str[0] == '\'')
        return (float)sign * str[1];
    
    // assume decimal
    decimal = -1;
    total = 0;
    while(1)
    {
        c = *str++;
        if(c == '.')
        {
            decimal = total;
            continue;
        }
        if(c <'0' || c > '9')
            break;
        val = val*10 + c - '0';
        total++;
    }
    
    if(decimal == -1)
        return (float)val*sign;
    
    while (total > decimal)
    {
        val /= 10;
        total--;
    }
    
    return (float)val*sign;
}

//
// dhtoi
//

int dhtoi(char* str)
{
	char *s;
	int num;

	num = 0;
	s = str;

	while(*s)
	{
		num <<= 4;
		if(*s >= '0' && *s <= '9')
			num += *s-'0';
		else if(*s >= 'a' && *s <= 'f')
			num += 10 + *s-'a';
		else if(*s >= 'A' && *s <= 'F')
			num += 10 + *s-'A';
        else
            return 0;
		s++;
	}

	return num;
}

//
// dfcmp
//

dboolean dfcmp(float f1, float f2)
{
    float precision = 0.00001f;
    if(((f1 - precision) < f2) && 
        ((f1 + precision) > f2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//
// M_ClearBox
//

void M_ClearBox (fixed_t *box)
{
    box[BOXTOP] = box[BOXRIGHT] = MININT;
    box[BOXBOTTOM] = box[BOXLEFT] = MAXINT;
}

//
// M_AddToBox
//

void M_AddToBox(fixed_t* box, fixed_t x, fixed_t y)
{
    if(x<box[BOXLEFT])
        box[BOXLEFT] = x;
    else if(x>box[BOXRIGHT])
        box[BOXRIGHT] = x;
    if(y<box[BOXBOTTOM])
        box[BOXBOTTOM] = y;
    else if(y>box[BOXTOP])
        box[BOXTOP] = y;
}

//
// M_Random
// Returns a 0-255 number
//

byte rndtable[256] = {
	0,   8, 109, 220, 222, 241, 149, 107,  75, 248, 254, 140,  16,  66 ,
	74,  21, 211,  47,  80, 242, 154,  27, 205, 128, 161,  89,  77,  36 ,
	95, 110,  85,  48, 212, 140, 211, 249,  22,  79, 200,  50,  28, 188 ,
	52, 140, 202, 120,  68, 145,  62,  70, 184, 190,  91, 197, 152, 224 ,
	149, 104,  25, 178, 252, 182, 202, 182, 141, 197,   4,  81, 181, 242 ,
	145,  42,  39, 227, 156, 198, 225, 193, 219,  93, 122, 175, 249,   0 ,
	175, 143,  70, 239,  46, 246, 163,  53, 163, 109, 168, 135,   2, 235 ,
	25,  92,  20, 145, 138,  77,  69, 166,  78, 176, 173, 212, 166, 113 ,
	94, 161,  41,  50, 239,  49, 111, 164,  70,  60,   2,  37, 171,  75 ,
	136, 156,  11,  56,  42, 146, 138, 229,  73, 146,  77,  61,  98, 196 ,
	135, 106,  63, 197, 195,  86,  96, 203, 113, 101, 170, 247, 181, 113 ,
	80, 250, 108,   7, 255, 237, 129, 226,  79, 107, 112, 166, 103, 241 ,
	24, 223, 239, 120, 198,  58,  60,  82, 128,   3, 184,  66, 143, 224 ,
	145, 224,  81, 206, 163,  45,  63,  90, 168, 114,  59,  33, 159,  95 ,
	28, 139, 123,  98, 125, 196,  15,  70, 194, 253,  54,  14, 109, 226 ,
	71,  17, 161,  93, 186,  87, 244, 138,  20,  52, 123, 251,  26,  36 ,
	17,  46,  52, 231, 232,  76,  31, 221,  84,  37, 216, 165, 212, 106 ,
	197, 242,  98,  43,  39, 175, 254, 145, 190,  84, 118, 222, 187, 136 ,
	120, 163, 236, 249 
};

int rndindex = 0;
static int prndindex = 0;

//
// P_Random
//

int P_Random(void)
{
    prndindex = (prndindex+1)&0xff;
    return rndtable[prndindex];
}

//
// M_Random
//

int M_Random(void)
{
    rndindex = (rndindex+1)&0xff;
    return rndtable[rndindex];
}

//
// M_ClearRandom
//

void M_ClearRandom(void)
{
    rndindex = prndindex = 0;
}

//
// P_RandomShift
//

int P_RandomShift(int shift)
{
    int rand = P_Random();
    return (rand - P_Random()) << shift;
}

//
// EVENT HANDLING
//
// Events are asynchronous inputs generally generated by the game user.
// Events can be discarded if no responder claims them
//
event_t events[MAXEVENTS];
int eventhead = 0;
int eventtail = 0;

//
// D_PostEvent
// Called by the I/O functions when input is detected
//

void D_PostEvent(event_t* ev)
{
    events[eventhead] = *ev;
    eventhead = (++eventhead) & (MAXEVENTS - 1);
}

//
// D_ProcessEvents
// Send all the events of the given timestamp down the responder chain
//
void D_ProcessEvents(void)
{
    event_t* ev;
    
    for(; eventtail != eventhead; eventtail = (++eventtail) & (MAXEVENTS - 1))
    {
        ev = &events[eventtail];

        if(M_Responder(ev))
            continue;               // menu ate the event

        G_Responder(ev);
    }
}

//
// D_PrintDevStats
//

int bsptic = 0;
int rendertic = 0;
int ptic = 0;

static void D_PrintDevStats(void)
{
    int verts = dsvertices;
    int tris = dspolygons;

    ST_DrawMessage(0, 0, ARGB16(31, 0, 31, 0), "Memory: %ikb\n", Z_FreeMemory() >> 10);
    ST_DrawMessage(0, 8, ARGB16(31, 0, 31, 0), "Static: %ikb\n", Z_TagUsage(PU_STATIC) >> 10);
    ST_DrawMessage(0, 16, ARGB16(31, 0, 31, 0), "Cache: %ikb\n", Z_TagUsage(PU_CACHE) >> 10);
    ST_DrawMessage(0, 24, ARGB16(31, 0, 31, 0), "Audio: %ikb\n", Z_TagUsage(PU_AUDIO) >> 10);
    ST_DrawMessage(0, 32, ARGB16(31, 0, 31, 0), "DMA: %ikb\n", gfxdmasize >> 10);
    ST_DrawMessage(0, 40, ARGB16(31, 0, 31, 0), "Vram: %ikb\n", Z_FreeVMemory(vramzone) >> 10);
    ST_DrawMessage(0, 48, ARGB16(31, 0, 31, 0), "Vertex: %i\n", verts);
    ST_DrawMessage(0, 56, ARGB16(31, 0, 31, 0), "Tris: %i\n", tris);
    ST_DrawMessage(0, 64, ARGB16(31, 0, 31, 0), "Bsp: %ims\n", bsptic);
    ST_DrawMessage(0, 72, ARGB16(31, 0, 31, 0), "Draw: %ims\n", rendertic);
    ST_DrawMessage(0, 80, ARGB16(31, 0, 31, 0), "Game: %ims\n", ptic);
}

//
// D_GetLowTic
//

static int D_GetLowTic(void)
{
    //int i;
    int lowtic;

    // TODO
	/*if(net_client_connected)
	{
		lowtic = INT_MAX;
    
		for(i = 0; i < MAXPLAYERS; ++i)
		{
			if(playeringame[i])
			{
				if(nettics[i] < lowtic)
					lowtic = nettics[i];
			}
		}
	}
	else*/
		lowtic = maketic;

	return lowtic;
}

//
// D_GetAdjustedTime
//

static int D_GetAdjustedTime(void)
{
    int time_ms;

    time_ms = I_GetTimeMS();
    time_ms += (offsetms / FRACUNIT);

    return (time_ms * TICRATE) / 1000;
}

//
// D_PlayersInGame
// Returns true if there are currently any players in the game.
//

static dboolean D_PlayersInGame(void)
{
	int i;

	for(i = 0; i < MAXPLAYERS; ++i)
	{
        if(playeringame[i])
			return true;
	}

	return false;
}

//
// D_UpdateTiccmd
// Builds ticcmds for console player,
// sends out a packet
//

static int gametime = 0;

void D_UpdateTiccmd(void)
{
    int nowtime;
    int newtics;
    int i;
    int gameticdiv;

    // check time
    nowtime = D_GetAdjustedTime()/ticdup;
    newtics = nowtime - gametime;
    gametime = nowtime;

    if(skiptics <= newtics)
    {
		newtics -= skiptics;
		skiptics = 0;
    }
    else
    {
		skiptics -= newtics;
		newtics = 0;
    }

    // build new ticcmds for console player(s)
    gameticdiv = gametic/ticdup;

    for(i = 0; i < newtics; i++)
    {
		ticcmd_t cmd;

		I_StartTic();
		D_ProcessEvents();

        M_Ticker();
	
		// If playing single player, do not allow tics to buffer
        // up very far

        if((!netgame || demoplayback) && maketic - gameticdiv > 2)
            break;

        // Never go more than ~200ms ahead
        if(maketic - gameticdiv > 8)
            break;

		G_BuildTiccmd(&cmd);

        netcmds[consoleplayer][maketic % BACKUPTICS] = cmd;
		++maketic;
    }
}

//
// D_MiniLoop
//

int D_MiniLoop(void(*start)(void), void(*stop)(void),
               void (*draw)(void), dboolean(*tick)(void))
{
    int action = gameaction = ga_nothing;

    if(start) start();

    while(!action)
    {
        int i = 0;
        int lowtic = 0;
        int entertic = 0;
        int availabletics = 0;
        int counts = 0;

        // process one or more tics

        // get real tics
        entertic = I_GetTime() / ticdup;

        // get available ticks

        D_UpdateTiccmd();
        lowtic = D_GetLowTic();

        availabletics = lowtic - gametic/ticdup;

        // decide how many tics to run

        counts = availabletics;

        if(counts < 1) counts = 1;

        // wait for new tics if needed

        while(!D_PlayersInGame() || lowtic < gametic/ticdup + counts)	
        {
            D_UpdateTiccmd();
            lowtic = D_GetLowTic();
	
            if(lowtic < gametic/ticdup)
                I_Error("D_MiniLoop: lowtic < gametic");

            // Don't stay in this loop forever.  The menu is still running,
            // so return to update the screen

            if(I_GetTime() / ticdup - entertic > 0)
                goto drawframe;

            I_Sleep(1);
        }

        // run the count * ticdup dics
        while(counts--)
        {
            for(i = 0; i < ticdup; i++)
            {
                // check that there are players in the game.  if not, we cannot
                // run a tic.
        
                if(!D_PlayersInGame())
                    break;
    
                if(gametic / ticdup > lowtic)
                    I_Error("gametic>lowtic");

                G_Ticker();

                if(tick)
                    action = tick();

                if(gameaction != ga_nothing)
                    action = gameaction;

                gametic++;
	    
                // modify command for duplicated tics
                if(i != ticdup-1)
                {
                   ticcmd_t *cmd;
                   int buf;
                   int j;
				
                   buf = (gametic / ticdup) % BACKUPTICS; 
                   for(j = 0; j < MAXPLAYERS; j++)
                   {
                       cmd = &netcmds[j][buf];
                       if(cmd->buttons & BT_SPECIAL)
                           cmd->buttons = 0;
                   }
                }
            }

            D_UpdateTiccmd();   // check for new console commands
        }

drawframe:

        S_UpdateSounds();
        
        // Update display, next frame, with current state.

        if(draw && !action)
        {
            draw();
            I_FinishFrame();
        }
        
        if(menuactive)
            M_Drawer();

        // send out any new accumulation
        D_UpdateTiccmd();

        // force garbage collection
        Z_FreeAlloca();

        if(devparm)
            D_PrintDevStats();
    }

    if(stop) stop();

    return action;
}

//
// Splash_Start
//

static void Splash_Start(void)
{
    screenalpha = 0x1F;
    splashstage = 0;
    splashwait = false;
    menuactive = false;
    gamestate = GS_SKIPPABLE;
    pagetic = gametic;
    gameaction = ga_nothing;
}

//
// Splash_Stop
//

static void Splash_Stop(void)
{
    gamestate = GS_NONE;
    gameaction = ga_nothing;
    menuactive = false;
}

//
// Splash_Drawer
//

static void Splash_Drawer(void)
{
    I_CheckGFX();

    GFX_ORTHO();

    if(screenalpha > 1)
    {
        GFX_POLY_FORMAT =
            POLY_ALPHA(screenalpha) |
            POLY_ID(63)             |
            POLY_CULL_NONE          |
            POLY_MODULATION;

        GFX_TEX_FORMAT  = 0;
        GFX_PAL_FORMAT  = 0;
        GFX_COLOR       = 0;
        GFX_SCREENRECT();
    }

    if(screenalpha < 0x1F)
    {
        switch(splashstage)
        {
        case 0:
            R_SlamBackground("IDLOGO", 70, 28, 0xFFFFF);
            break;
        case 1:
            R_SlamBackground("WMSCRED1", 0, 58, 0xFFFFF);
            break;
        case 2:
            R_SlamBackground("USLEGAL", 0, 50, 0xFFFFF);
            break;
        default:
            break;
        }
    }
}

//
// Splash_Ticker
//

static int Splash_Ticker(void)
{
    if(splashstage >= 3)
        return 1;

    if(!splashwait)
    {
        screenalpha -= 4;

        if(screenalpha <= 1)
        {
            screenalpha = 1;
            pagetic = gametic;
            splashwait = true;
        }
    }
    else
    {
        if(screenalpha > 1)
        {
            screenalpha += 4;

            if(screenalpha >= 0x1F)
            {
                splashstage++;
                screenalpha = 0x1F;
                splashwait = false;
            }
        }
        else if((gametic - pagetic) >= (TICRATE * 2))
            screenalpha = 2;
    }

    return 0;
}

//
// D_SplashScreen
//

static void D_SplashScreen(void)
{
    int skip = 0;

    if(gameaction || netgame)
        return;

    skip = D_MiniLoop(Splash_Start, Splash_Stop, Splash_Drawer, Splash_Ticker);

    if(skip != ga_title)
    {
        G_RunTitleMap();
        gameaction = ga_title;
    }
}

//
// Title_Drawer
//

static void Title_Drawer(void)
{
    I_CheckGFX();

    GFX_ORTHO();
    GFX_CLEAR_COLOR = 0x1F0000;
    R_SlamBackground("TITLE", 23, 40, 0xFFFFF);
}

//
// Title_Ticker
//

static int Title_Ticker(void)
{
    if((gametic - pagetic) >= (TICRATE * 30))
        return 1;

    return 0;
}

//
// Title_Start
//

static void Title_Start(void)
{
    gameaction = ga_nothing;
    pagetic = gametic;
    menuactive = true;
    paused = false;
    gamestate = GS_NONE;

    S_StartMusic(mus_title);
    M_SetMainMenu();
}

//
// Title_Stop
//

static void Title_Stop(void)
{
    menuactive = false;

    //WIPE_FadeScreen(8);
    S_StopMusic();
}

//
// D_DoomLoop
// Main game loop
//

static void D_DoomLoop(void)
{
    int exit;

    if(netgame)
        gameaction = ga_newgame;

    exit = gameaction;
    
    while(1)
    {
        exit = D_MiniLoop(Title_Start, Title_Stop, Title_Drawer, Title_Ticker);

        if(exit == ga_newgame || exit == ga_loadgame)
            G_RunGame();
        else
        {
            //D_MiniLoop(Credits_Start, NULL, Credits_Drawer, Credits_Ticker);

            if(gameaction == ga_title)
                continue;

            G_RunTitleMap();
            continue;
        }
    }
}

//
// D_DebugParams
//

static void D_DebugParams(void)
{
    int keys = 0;

    scanKeys();
    keys = keysHeld();

    if(!(keys & (KEY_Y | KEY_B)))
        return;

    I_Printf("Debug startup enabled\n\n");

    while(!(keys & KEY_START))
    {
        scanKeys();
        keys = keysDown();

        if(keys & KEY_UP)
        {
            keys &= ~KEY_UP;
            startmap++;
            if(startmap > 33)
                startmap = 33;

            I_Printf("startmap: %i\n", startmap);
        }
        if(keys & KEY_DOWN)
        {
            keys &= ~KEY_DOWN;
            startmap--;
            if(startmap < 1)
                startmap = 1;

            I_Printf("startmap: %i\n", startmap);
        }
        if(keys & KEY_A)
        {
            nomonsters ^= 1;
            I_Printf("nomonsters ");
            if(nomonsters)
                I_Printf("on\n");
            else
                I_Printf("off\n");
        }
        if(keys & KEY_B)
        {
            lockmonsters ^= 1;
            I_Printf("lockmonsters ");
            if(lockmonsters)
                I_Printf("on\n");
            else
                I_Printf("off\n");
        }
        if(keys & KEY_X)
        {
            devparm ^= 1;
            I_Printf("development mode ");
            if(devparm)
                I_Printf("on\n");
            else
                I_Printf("off\n");
        }
        if(keys & KEY_Y)
        {
            nosound ^= 1;
            I_Printf("sound ");
            if(nosound)
                I_Printf("disabled\n");
            else
                I_Printf("enabled\n");
        }

        swiWaitForVBlank();
    }

    I_Printf("Exiting...\n");
}

//
// D_DoomMain
//

void D_DoomMain(void)
{
    I_Printf("I_Init\n");
    I_Init();
    I_Printf("Z_Init\n");
    Z_Init();
    I_Printf("W_Init\n");
    W_Init();
    I_Printf("R_Init\n");
    R_Init();
    I_Printf("P_Init\n");
    P_Init();
    I_Printf("ST_Init\n");
    ST_Init();
    I_Printf("S_Init\n");
    S_Init();

    gameaction = ga_nothing;
    gamestate = GS_NONE;
    gametic = 0;
    startskill = sk_medium;
    gameskill = sk_medium;
    startmap = 1;
    ticdup = 1;
    offsetms = 0;
    playeringame[0] = true;

    D_DebugParams();
    D_SplashScreen();
    D_DoomLoop();
}

