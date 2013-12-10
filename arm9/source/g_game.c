#include <stdlib.h>
#include <stdarg.h>

#include "doomdef.h"
#include "z_zone.h"
#include "d_main.h"
#include "w_wad.h"
#include "p_local.h"
#include "d_englsh.h"
#include "sounds.h"
#include "tables.h"
#include "info.h"
#include "r_local.h"
#include "s_sound.h"
#include "p_spec.h"

#define DEMOMARKER      0x80
#define MAXPLMOVE       (forwardmove[1])
#define TURBOTHRESHOLD  0x32
#define SLOWTURNTICS    10
#define BODYQUESIZE     32

dboolean    G_CheckDemoStatus(void);
void        G_ReadDemoTiccmd(ticcmd_t* cmd);
void        G_PlayerReborn(int player);
void        G_InitNew(skill_t skill, int map);
void        G_DoReborn(int playernum);
void        G_SetFastParms(int fast_pending);

dboolean        sendpause       = false;    // send a pause event next tic
dboolean        usergame        = false;    // ok to save / end game
int             starttime       = 0;        // for comparative timing purposes
dboolean        demoplayback    = false;
byte*           demobuffer;
byte*           demo_p;
byte*           demoend;
dboolean        precache        = true;     // if true, load all graphics at start

byte            consistancy[MAXPLAYERS][BACKUPTICS];

int         buttons = 0;
fixed_t     forwardmove[2]  =   { 0x1c, 0x2c };
fixed_t     sidemove[2]     =   { 0x1c, 0x2c };
fixed_t     angleturn[20]   =   { 0x32, 0x32, 0x53, 0x53, 0x64, 0x74, 0x85, 0x96, 0x96, 0xA6,
                                  0x85, 0x85, 0x96, 0xA6, 0xA6, 0xC8, 0xC8, 0xD8, 0xD8, 0xE9
                                };

int         turnheld;                       // for accelerative turning

mobj_t*     bodyque[BODYQUESIZE];
int         bodyqueslot;

byte forcecollision = 0;
byte forcejump = 0;


//
// G_BuildTiccmd
// Builds a ticcmd from all of the available inputs
// or reads it from the demo buffer.
// If recording a demo, write it out
//

void G_BuildTiccmd(ticcmd_t* cmd)
{
    //int i;
    int speed;
    int forward;
    int side;
    
    memset(cmd, 0, sizeof(ticcmd_t));
    cmd->consistancy = consistancy[consoleplayer][maketic % BACKUPTICS];
    
    speed = (buttons & KEY_A);
    
    forward = side = 0;
    
    // use two stage accelerative turning
    // on the keyboard and joystick
    if(buttons & (KEY_LEFT | KEY_RIGHT))
        turnheld += ticdup;
    else
        turnheld = 0;
    
    if(turnheld >= SLOWTURNTICS)
        turnheld = SLOWTURNTICS-1;
    
    if(buttons & KEY_RIGHT)
        cmd->angleturn -= angleturn[turnheld + (speed ? SLOWTURNTICS : 0)] << 2;

    if(buttons & KEY_LEFT)
        cmd->angleturn += angleturn[turnheld + (speed ? SLOWTURNTICS : 0)] << 2;
    
    if(buttons & KEY_X || buttons & KEY_Y)
    {
        cmd->buttons |= BT_CHANGE;

        if(buttons & KEY_X)
        {
            cmd->buttons |= BT_NEXTWEAP;
            buttons &= ~KEY_X;
        }
        else if(buttons & KEY_Y)
        {
            cmd->buttons |= BT_PREVWEAP;
            buttons &= ~KEY_Y;
        }
    }

    if(buttons & KEY_R)
        side -= sidemove[speed];
    
    if(buttons & KEY_L)
        side += sidemove[speed];

    if(buttons & KEY_UP)
        forward -= forwardmove[speed];
    
    if(buttons & KEY_DOWN)
        forward += forwardmove[speed];
    
    if(buttons & KEY_B)
        cmd->buttons |= BT_ATTACK;
    
    if(buttons & KEY_A)
        cmd->buttons |= BT_USE;
    
    
    if(forward > MAXPLMOVE)
        forward = MAXPLMOVE;
    else if(forward < -MAXPLMOVE)
        forward = -MAXPLMOVE;

    if(side > MAXPLMOVE)
        side = MAXPLMOVE;
    else if(side < -MAXPLMOVE)
        side = -MAXPLMOVE;
    
    cmd->forwardmove -= forward;
    cmd->sidemove -= side;
}

//
// G_DoLoadLevel
//

void G_DoLoadLevel (void)
{
    int i;
    mapdef_t* map;
    
    P_LoadingPlaque();

    for(i = 0; i < MAXPLAYERS; i++)
    {
        if(playeringame[i] && players[i].playerstate == PST_DEAD)
            players[i].playerstate = PST_REBORN;
    }

    map = P_GetMapInfo(gamemap);

    forcecollision  = map->oldcollision;
    forcejump       = map->allowjump;

    // This was quite messy with SPECIAL and commented parts.
    // Supposedly hacks to make the latest edition work.
    // It might not work properly.
    
    G_SetFastParms(fastparm || gameskill == sk_nightmare);  // killough 4/10/98

    if(gameskill == sk_nightmare || respawnparm )
        respawnmonsters = true;
    else
        respawnmonsters = false;
    
    if(respawnitem)
        respawnspecials = true;
    else
        respawnspecials = false;
    
    P_SetupLevel(gamemap, 0, gameskill);
    starttime = I_GetTime();
    gameaction = ga_nothing;
    buttons = 0;
    sendpause = paused = false;

    P_ClearLoadingPlaque();
}


//
// G_Responder
// Get info needed to make ticcmd_ts for the players.
//

dboolean G_Responder(event_t* ev)
{
    switch(ev->type)
    {
    case ev_btndown:
        if(ev->data & KEY_START && ev->data & KEY_SELECT)
        {
            sendpause = true;
            return true;
        }

        buttons |= ev->data;
        break;

    case ev_btnup:
        buttons &= ~ev->data;
        break;

    case ev_btnheld:
        break;
    }

    // Handle level specific ticcmds
    if(gamestate == GS_LEVEL)
    {
        // TODO TEMP
        if(ev->type == ev_btndown)
        {
            if(ev->data & KEY_SELECT)
                players[0].cheats |= CF_NOCLIP;
        }
        else if(ev->type == ev_btnup)
        {
            if(ev->data & KEY_SELECT)
                players[0].cheats &= ~CF_NOCLIP;
        }

        if(demoplayback && gameaction == ga_nothing)
        {
            if(ev->type == ev_btndown)
            {
                G_CheckDemoStatus();
                gameaction = ga_warpquick;
                return true;
            }
            else
                return false;
        }

        if(AM_Responder(ev))
            return true;    // automap ate it
    }

    // Handle screen specific ticcmds
    if(gamestate == GS_SKIPPABLE)
    {
        if(gameaction == ga_nothing)
        {
            if(ev->type == ev_btndown)
            {
                gameaction = ga_title;
                return true;
            }
            return false;
        }
    }
    
    return false;
}

//
// G_Ticker
// Make ticcmd_ts for the players.
//

void G_Ticker(void)
{
    int         i;
    int         buf;
    ticcmd_t*   cmd;
        
    // get commands, check consistancy,
    // and build new consistancy check
    buf = (gametic / ticdup) % BACKUPTICS;
        
    for(i = 0; i < MAXPLAYERS; i++)
    {
        if(playeringame[i])
        {
            cmd = &players[i].cmd;
                
            memcpy(cmd, &netcmds[i][buf], sizeof(ticcmd_t));

            //
            // 20120404 villsa - make sure gameaction isn't set to anything before
            // reading a demo lump
            //
            if(demoplayback && gameaction == ga_nothing)
                G_ReadDemoTiccmd(cmd);
                
            if(netgame && !(gametic % ticdup))
            {
                if(gametic > BACKUPTICS
                    && consistancy[i][buf] != cmd->consistancy)
                {
                    I_Error("consistency failure (%i should be %i)",
                        cmd->consistancy, consistancy[i][buf], consoleplayer);
                }
                if(players[i].mo)
                    consistancy[i][buf] = players[i].mo->x;
                else
                    consistancy[i][buf] = rndindex;
            }
        }
    }
        
    // check for special buttons
    for(i = 0; i < MAXPLAYERS; i++)
    {
        if(playeringame[i])
        {
            if(players[i].cmd.buttons & BT_SPECIAL)
            {
            /*villsa - fixed crash when player restarts level after dying
                Changed switch statments to if statments*/
                if((players[i].cmd.buttons & BT_SPECIALMASK) == BTS_PAUSE)
                {
                    paused ^= 1;

                    if(paused)
                        S_PauseSound();
                    else
                        S_ResumeSound();
                }
            }
        }
    }
}


//
// PLAYER STRUCTURE FUNCTIONS
// also see P_SpawnPlayer in P_Mobj
//

//
// G_PlayerFinishLevel
// Can when a player completes a level.
//

void G_PlayerFinishLevel(int player)
{
    player_t*   p;
    
    p = &players[player];
    
    memset(p->powers, 0, sizeof(p->powers));
    memset(p->cards, 0, sizeof(p->cards));
    p->mo->flags &= ~MF_SHADOW; 	// cancel invisibility
    p->damagecount = 0; 		// no palette changes
    p->bonuscount = 0;
    p->bfgcount = 0;
}

//
// G_PlayerReborn
// Called after a player dies
// almost everything is cleared and initialized
//

void G_PlayerReborn(int player)
{
    player_t    *p;
    int         i;
    int         killcount;
    int         itemcount;
    int         secretcount;
    dboolean    cards[NUMCARDS];
    dboolean    wpns[NUMWEAPONS];
    int         pammo[NUMAMMO];
    int         pmaxammo[NUMAMMO];
    int         artifacts;
    dboolean    backpack;
    
    memcpy(cards, players[player].cards, sizeof(dboolean)*NUMCARDS);
    memcpy(wpns, players[player].weaponowned, sizeof(dboolean)*NUMWEAPONS);
    memcpy(pammo, players[player].ammo, sizeof(int)*NUMAMMO);
    memcpy(pmaxammo, players[player].maxammo, sizeof(int)*NUMAMMO);

    backpack = players[player].backpack;
    artifacts = players[player].artifacts;
    killcount = players[player].killcount;
    itemcount = players[player].itemcount;
    secretcount = players[player].secretcount;
    
    quakeviewx = 0;
    quakeviewy = 0;
    infraredFactor = 0;
    R_RefreshBrightness();
    
    p = &players[player];
    memset(p, 0, sizeof(*p));
    
    players[player].killcount = killcount;
    players[player].itemcount = itemcount;
    players[player].secretcount = secretcount;
    
    p->usedown = p->attackdown = p->jumpdown = true;  // don't do anything immediately
    p->playerstate = PST_LIVE;
    p->health = MAXHEALTH;
    p->readyweapon = p->pendingweapon = wp_pistol;
    p->weaponowned[wp_fist] = true;
    p->weaponowned[wp_pistol] = true;
    p->ammo[am_clip] = 50;
    p->recoilpitch = 0;
    
    for(i = 0; i < NUMAMMO; i++)
        p->maxammo[i] = maxammo[i];

    p->artifacts = artifacts;
    p->backpack = backpack;

    if(netgame)
    {
        for(i = 0; i < NUMCARDS; i++)
            players[player].cards[i] = cards[i];
    }
}

//
// G_CheckSpot
// Returns false if the player cannot be respawned
// at the given mapthing_t spot
// because something is occupying it
//

dboolean G_CheckSpot(int playernum, mapthing_t* mthing)
{
    fixed_t         x;
    fixed_t         y;
    subsector_t*    ss;
    angle_t         an;
    mobj_t*         mo;
    int             i;
    
    if(!players[playernum].mo)
    {
        // first spawn of level, before corpses
        for(i = 0; i < playernum; i++)
        {
            if((players[i].mo->x == INT2F(mthing->x)) && (players[i].mo->y == INT2F(mthing->y)))
                return false;
        }
        return true;
    }
    
    x = INT2F(mthing->x);
    y = INT2F(mthing->y);
    
    if(!P_CheckPosition(players[playernum].mo, x, y) )
        return false;
    
    // flush an old corpse if needed
    if(bodyqueslot >= BODYQUESIZE)
        P_RemoveMobj(bodyque[bodyqueslot % BODYQUESIZE]);

    bodyque[bodyqueslot%BODYQUESIZE] = players[playernum].mo;
    bodyqueslot++;
    
    // spawn a teleport fog
    ss = R_PointInSubsector(x, y);

    // 20120402 villsa - force angle_t typecast to avoid issues on 64-bit machines
    an = ANG45 * (angle_t)(mthing->angle / 45);
    
    mo = P_SpawnMobj(
        x + 20*dcos(an),
        y + 20*dsin(an),
        ss->sector->floorheight,
        MT_TELEPORTFOG
        );
    
    if(players[playernum].viewz != 1)
        S_StartSound(mo, sfx_telept);   // don't start sound on first frame
    
    return true;
}

//
// G_DoReborn
//

void G_DoReborn(int playernum)
{
    if(!netgame)
        gameaction = ga_loadlevel;      // reload the level from scratch
    else	 // respawn at the start
    {
        int i;
        
        // first dissasociate the corpse
        if(players[playernum].mo == NULL)
            I_Error("G_DoReborn: Player start #%i not found!", playernum+1);
        
        players[playernum].mo->player = NULL;
        
        if(G_CheckSpot (playernum, &playerstarts[playernum]) )
        {
            P_SpawnPlayer (&playerstarts[playernum]);
            return;
        }
        
        // try to spawn at one of the other players spots
        for(i = 0; i < MAXPLAYERS; i++)
        {
            if(G_CheckSpot(playernum, &playerstarts[i]) ) 
            { 
                playerstarts[i].type = playernum+1;	// fake as other player 
                P_SpawnPlayer(&playerstarts[i]); 
                playerstarts[i].type = i+1;			// restore 
                return;
            }	    
            // he's going to be inside something.  Too bad.
        }
        P_SpawnPlayer (&playerstarts[playernum]);
    }
}

//
// G_ScreenShot
//

void G_ScreenShot(void)
{
    gameaction = ga_screenshot;
}

//
// G_CompleteLevel
//

void G_CompleteLevel(void)
{
    mapdef_t* map;
    clusterdef_t* cluster;

    map = P_GetMapInfo(gamemap);

    if(!map->nointermission)
        gameaction = ga_completed;
    else
        gameaction = ga_victory;

    cluster = P_GetCluster(nextmap);

    if(cluster && cluster->nointermission)
        gameaction = ga_victory;
}

//
// G_ExitLevel
//

void G_ExitLevel(void)
{
    line_t junk;
    mapdef_t* map;
    
    map = P_GetMapInfo(gamemap);

    memset(&junk, 0, sizeof(line_t));
    junk.tag = map->exitdelay;
    
    P_SpawnDelayTimer(&junk, G_CompleteLevel);
    
    nextmap = gamemap + 1;
}

//
// G_SecretExitLevel
//

void G_SecretExitLevel(int map)
{
    line_t junk;
    mapdef_t* mapdef;
    
    mapdef = P_GetMapInfo(gamemap);

    memset(&junk, 0, sizeof(line_t));
    junk.tag = mapdef->exitdelay;
    
    P_SpawnDelayTimer(&junk, G_CompleteLevel);
    
    nextmap = map;
}

//
// G_RunTitleMap
//

void G_RunTitleMap(void)
{
    demobuffer = Z_Calloc(0x16000, PU_STATIC, NULL);
    demo_p = demobuffer;
    demobuffer[0x16000-1] = DEMOMARKER;

    G_InitNew(sk_medium, 33);

    precache = true;
    usergame = false;
    demoplayback = true;

    G_DoLoadLevel();

    D_MiniLoop(P_Start, P_Stop, P_Drawer, P_Ticker);
}

//
// G_RunGame
// The game should already have been initialized or loaded
//

void G_RunGame(void)
{
    int next = 0;

    G_InitNew(startskill, startmap);

    while(gameaction != ga_title)
    {
        G_DoLoadLevel();

        next = D_MiniLoop(P_Start, P_Stop, P_Drawer, P_Ticker);

        if(next == ga_loadlevel)
            continue;   // restart level from scratch

        if(next == ga_warplevel || next == ga_warpquick)
            continue;   // skip intermission

        if(next == ga_title)
            return;     // exit game and return to title screen

        if(next == ga_completed)
            next = D_MiniLoop(WI_Start, WI_Stop, WI_Drawer, WI_Ticker);

        if(next == ga_victory)
        {
            next = D_MiniLoop(IN_Start, IN_Stop, IN_Drawer, IN_Ticker);

            if(next == ga_finale)
                D_MiniLoop(F_Start, F_Stop, F_Drawer, F_Ticker);
        }

        gamemap = nextmap;
    }
}


//
// G_DeferedInitNew
// Can be called by the startup code or the menu task,
// consoleplayer, displayplayer, playeringame[] should be set.
//

void G_DeferedInitNew(skill_t skill, int map)
{
    startskill = skill;
    startmap = map;
    nextmap = map;
    gameaction = ga_newgame;
}

//
// G_SetFastParms
// killough 4/10/98: New function to fix bug which caused Doom
// lockups when idclev was used in conjunction with -fast.

void G_SetFastParms(int fast_pending)
{
    static int fast = 0;            // remembers fast state
    int i;
    if(fast != fast_pending) 
    {     /* only change if necessary */
        if((fast = fast_pending))
        {
            for(i = S_044; i <= S_058; i++)
            {
                if(states[i].tics != 1) // killough 4/10/98
                    states[i].tics >>= 1;  // don't change 1->0 since it causes cycles
            }
            mobjinfo[MT_PROJ_BRUISER1].speed = 20*FRACUNIT;
            mobjinfo[MT_PROJ_HEAD].speed = 30*FRACUNIT;
            mobjinfo[MT_PROJ_IMP2].speed = 35*FRACUNIT;
            mobjinfo[MT_PROJ_IMP1].speed = 20*FRACUNIT;
        }
        else
        {
            for(i = S_044; i <= S_058; i++)
                states[i].tics <<= 1;
            
            mobjinfo[MT_PROJ_BRUISER1].speed = 15*FRACUNIT;
            mobjinfo[MT_PROJ_HEAD].speed = 20*FRACUNIT;
            mobjinfo[MT_PROJ_IMP2].speed = 20*FRACUNIT;
            mobjinfo[MT_PROJ_IMP1].speed = 10*FRACUNIT;
        }
    }
}

//
// G_InitNew
//

void G_InitNew(skill_t skill, int map)
{
    int i;

    if(!netgame)
    {
        netgame = false;
        playeringame[1] = playeringame[2] = playeringame[3] = 0;
        playeringame[0]=true;
        consoleplayer = 0;
    }
    
    if(paused)
        paused = false;

    if(skill > sk_nightmare)
        skill = sk_nightmare;
    
    // force players to be initialized upon first level load
    for(i = 0; i < MAXPLAYERS; i++)
        players[i].playerstate = PST_REBORN;
    
    usergame		= true;				// will be set false if a demo
    paused			= false;
    demoplayback	= false;
    automapactive	= false;
    gamemap			= map;
    gameskill		= skill;

    // [d64] For some reason this is added here
    M_ClearRandom();
}


//
// DEMO RECORDING
//

//
// G_ReadDemoTiccmd
//

void G_ReadDemoTiccmd(ticcmd_t* cmd)
{
    if(*demo_p == DEMOMARKER)
    {
        // end of demo data stream
        G_CheckDemoStatus();
        return;
    }
    
    cmd->forwardmove    = ((signed char)*demo_p++);
    cmd->sidemove       = ((signed char)*demo_p++);
    cmd->angleturn      = ((unsigned char)*demo_p++)<<8;
    cmd->pitch          = ((unsigned char)*demo_p++)<<8;
    cmd->buttons        = (unsigned char)*demo_p++;
}

//
// G_PlayDemo
//

void G_PlayDemo(const char* name)
{
    int i;

    gameaction = ga_nothing;

    if(W_CheckNumForName(name) == -1)
    {
        gameaction = ga_exitdemo;
        return;
    }
        
    demobuffer = demo_p = W_CacheLumpName(name, PU_STATIC);

    demo_p++;
    
    startskill      = *demo_p++;
    startmap        = *demo_p++;
    respawnparm     = *demo_p++;
    respawnitem     = *demo_p++;
    fastparm        = *demo_p++;
    nomonsters      = *demo_p++;
    consoleplayer   = *demo_p++;
    
    for(i = 0; i < MAXPLAYERS; i++)
        playeringame[i] = *demo_p++;

    G_InitNew(startskill, startmap);

    precache = true;
    usergame = false;
    demoplayback = true;

    G_DoLoadLevel();

    // TODO
    //D_MiniLoop(P_Start, P_Stop, P_Drawer, P_Ticker);
}

//
// G_CheckDemoStatus
// Called after a death or level completion to allow demos to be cleaned up
// Returns true if a new demo loop action will take place
//

dboolean G_CheckDemoStatus(void)
{
    if(demoplayback)
    {
        Z_Free(demobuffer);
        
        netgame         = false;
        playeringame[1]	= playeringame[2] = playeringame[3] = 0;
        respawnparm     = false;
        respawnitem     = false;
        fastparm        = false;
        nomonsters      = false;
        consoleplayer   = 0;
        gameaction      = ga_exitdemo;
        
        return true;
    }

    return false;
}


