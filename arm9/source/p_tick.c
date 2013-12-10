#include "doomdef.h"
#include "z_zone.h"
#include "p_local.h"
#include "d_main.h"
#include "s_sound.h"
#include "p_spec.h"
#include "r_local.h"
#include "st_main.h"

void G_PlayerFinishLevel(int player);
void G_DoReborn(int playernum);

//
// THINKERS
//
// All thinkers should be allocated by Z_Malloc
// so they can be operated on uniformly.
// The actual structures will vary in size,
// but the first element must be thinker_t.
//
// Mobjs are now kept seperate for more optimal
// list processing.
//

thinker_t   thinkercap;		// Both the head and tail of the thinker list.
mobj_t      mobjhead;		// Both the head and tail of the mobj list.
mobj_t      *currentmobj;
thinker_t   *currentthinker;


//
// P_InitThinkers
//

void P_InitThinkers(void)
{
    thinkercap.prev = thinkercap.next  = &thinkercap;
    mobjhead.next = mobjhead.prev = &mobjhead;
}

//
// P_AddThinker
// Adds a new thinker at the end of the list.
//

void P_AddThinker(thinker_t* thinker)
{
    thinkercap.prev->next = thinker;
    thinker->next = &thinkercap;
    thinker->prev = thinkercap.prev;
    thinkercap.prev = thinker;
}

//
// P_UnlinkThinker
//

static void P_UnlinkThinker(thinker_t* thinker)
{
    thinker_t* next = currentthinker->next;
    (next->prev = currentthinker = thinker->prev)->next = next;

    Z_Free(thinker);
}

//
// P_RemoveThinker
// Deallocation is lazy -- it will not actually be freed
// until its thinking turn comes up.
//

void P_RemoveThinker(thinker_t* thinker)
{
    thinker->function.acp1 = (actionf_p1)P_UnlinkThinker;
    P_MacroDetachThinker(thinker);
}

//
// P_LinkMobj
//

void P_LinkMobj(mobj_t* mobj)
{
    mobjhead.prev->next = mobj;
    mobj->next = &mobjhead;
    mobj->prev = mobjhead.prev;
    mobjhead.prev = mobj;
}

//
// P_UnlinkMobj
//

void P_UnlinkMobj(mobj_t* mobj)
{
    /* Remove from main mobj list */
    mobj_t* next = currentmobj->next;

    /* Note that currentmobj is guaranteed to point to us,
    * and since we're freeing our memory, we had better change that. So
    * point it to mobj->prev, so the iterator will correctly move on to
    * mobj->prev->next = mobj->next */
    (next->prev = currentmobj = mobj->prev)->next = next;
}

//
// P_RunMobjs
//

void P_RunMobjs(void)
{
    for(currentmobj = mobjhead.next; currentmobj != &mobjhead; currentmobj = currentmobj->next)
    {
        // Special case only
        if(currentmobj->flags & MF_NOSECTOR)
            continue;

        if(!currentmobj->player)
        {
            // [kex] don't bother if about to be removed
            if(currentmobj->mobjfunc != P_SafeRemoveMobj)
            {
                // [kex] don't clear callback if mobj is going to be respawning
                if(currentmobj->mobjfunc != P_RespawnSpecials)
                    currentmobj->mobjfunc = NULL;

                if(lockmonsters && currentmobj->flags & MF_COUNTKILL)
                    continue;

                P_MobjThinker(currentmobj);
            }

            if(currentmobj->mobjfunc)
                currentmobj->mobjfunc(currentmobj);
        }
    }
}

//
// P_RunThinkers
//

void P_RunThinkers(void)
{
    for(currentthinker = thinkercap.next;
        currentthinker != &thinkercap;
        currentthinker = currentthinker->next)
    {
        if(currentthinker->function.acp1)
            currentthinker->function.acp1(currentthinker);
    }
}

//
// P_Start
//

void P_Start(void)
{
    int i;
    mapdef_t* map;

    map = P_GetMapInfo(gamemap);

    for(i = 0; i < MAXPLAYERS; i++)
    {
        // players can't be hurt on title map
        if(map->forcegodmode)
            players[i].cheats |= CF_GODMODE;
        // turn off godmode on hectic map
        else if(map->clearchts)
            players[i].cheats &= ~CF_GODMODE;
        else
            break;
    }

    // turn off/clear some things
    AM_Stop();
    AM_Start();
    M_ClearRandom();

    // do a nice little fade in effect
    //P_FadeInBrightness(); // TODO

    // autoactivate line specials
    P_ActivateLineByTag(999, players[0].mo);

    // enable menu and set gamestate
    //allowmenu = true; // TODO
    gamestate = GS_LEVEL;
    
    S_StartMusic(map->music);
}

//
// P_Stop
//

void P_Stop(void)
{
    int i = 0;
    int action = gameaction;

    //
    // [d64] stop plasma buzz
    //
    S_StopSound(NULL, sfx_electric);

    for(i = 0; i < MAXPLAYERS; i++)
    {
        // take away cards and stuff
        if(playeringame[i])
            G_PlayerFinishLevel(i);
    }

    // TODO
    /*
    // [kex] reset damage indicators
    if(p_damageindicator.value)
        ST_ClearDamageMarkers();*/

    // free level tags
    Z_FreeTags(PU_AUDIO, PU_PURGELEVEL);

    // TODO
    if(automapactive)
        AM_Stop();

    // music continues on exit if defined
    if(!P_GetMapInfo(gamemap)->contmusexit)
        S_StopMusic();

    if(demoplayback)
        demoplayback = false;

    // TODO
    /*
    // do wipe/melt effect
    if(gameaction != ga_loadgame)
    {
        if(gameaction != ga_warpquick)
            WIPE_MeltScreen();
        else
        {
            S_StopMusic();
            WIPE_FadeScreen(8);
        }
    }*/

    S_ResetSound();

    gamestate = GS_NONE;
    gameaction = action;
}

//
// P_Drawer
//

void P_Drawer(void)
{
    // [kex] don't draw on first tic
    if(!leveltime)
        return;

    if(!I_DmaBGBusy() && automapactive)
        AM_Drawer();

    R_DrawFrame();
    ST_Drawer();

    if(automapactive)
        I_RefreshBG();
}

//
// P_Ticker
//

extern int ptic;

int P_Ticker(void)
{
    int i;
    
    if(paused)
        return 0;
    
    // pause if in menu and at least one tic has been run
    if(!netgame && menuactive &&
        !demoplayback && players[consoleplayer].viewz != 1)
        return 0;
    
    for(i = 0; i < MAXPLAYERS; i++)
    {
        if(playeringame[i])
        {
            // do player reborns if needed
            if(players[i].playerstate == PST_REBORN)
                G_DoReborn(i);

            P_PlayerThink(&players[i]);
        }
    }

    if(devparm)
        cpuStartTiming(2);
    
    P_RunThinkers();
    P_ScanSights();
    P_RunMobjs();
    P_UpdateSpecials();
    P_RunMacros();
    
    ST_Ticker();
    AM_Ticker();

    if(devparm)
        ptic = timerTicks2msec(cpuEndTiming());
    
    // for par times
    leveltime++;

    return gameaction;
}

