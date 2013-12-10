#include "doomdef.h"
#include "d_main.h"
#include "p_local.h"
#include "sounds.h"
#include "s_sound.h"
#include "st_main.h"
#include "r_local.h"

#define WIALPHARED      ARGB16(24, 31, 0, 0)

static int itempercent[MAXPLAYERS];
static int itemvalue[MAXPLAYERS];

static int killpercent[MAXPLAYERS];
static int killvalue[MAXPLAYERS];

static int secretpercent[MAXPLAYERS];
static int secretvalue[MAXPLAYERS];

static char timevalue[16];

static int wi_stage = 0;
static int wi_counter = 0;
static int wi_advance = 0;

//
// WI_Start
//

void WI_Start(void)
{
    int i;
    int minutes = 0;
    int seconds = 0;

    // initialize player stats
    for(i = 0; i < MAXPLAYERS; i++)
    {
        itemvalue[i] = killvalue[i] = secretvalue[i] = -1;

        if(!totalkills)
            killpercent[i] = 100;
        else
            killpercent[i] = (players[i].killcount * 100) / totalkills;

        if(!totalitems)
            itempercent[i] = 100;
        else
            itempercent[i] = (players[i].itemcount * 100) / totalitems;

        if(!totalsecret)
            secretpercent[i] = 100;
        else
            secretpercent[i] = (players[i].secretcount * 100) / totalsecret;
    }

    // setup level time
    if(leveltime)
    {
        minutes = (leveltime / TICRATE) / 60;
        seconds = (leveltime / TICRATE) % 60;
    }

    snprintf(timevalue, 16, "%2.2d:%2.2d", minutes, seconds);

    // clear variables
    wi_counter = 0;
    wi_stage = 0;
    wi_advance = 0;

    // start music
    S_StartMusic(mus_complete);
    gamestate = GS_WISTATS;

    //allowmenu = true; // TODO
}

//
// WI_Stop
//

void WI_Stop(void)
{
    wi_counter = 0;
    wi_stage = 0;
    wi_advance = 0;

    //allowmenu = false;    // TODO

    S_StopMusic();
    gamestate = GS_NONE;
    //WIPE_FadeScreen(6);   // TODO
}

//
// WI_Ticker
//

int WI_Ticker(void)
{
    dboolean    state = false;
    player_t*   player;
    int         i;
    dboolean    next = false;

    if(wi_advance <= 3)
    {
        // check for button presses to skip delays
        for(i = 0, player = players; i < MAXPLAYERS; i++, player++)
        {
            if(playeringame[i])
            {
                if(player->cmd.buttons & BT_ATTACK)
                {
                    if(!player->attackdown)
                    {
                        S_StartSound(NULL, sfx_explode);
                        wi_advance++;
                    }
                    player->attackdown = true;
                }
                else
                    player->attackdown = false;

                if(player->cmd.buttons & BT_USE)
                {
                    if(!player->usedown)
                    {
                        S_StartSound(NULL, sfx_explode);
                        wi_advance++;
                    }
                    player->usedown = true;
                }
                else
                    player->usedown = false;
            }
        }
    }

    // accelerate counters
    if(wi_advance == 1)
    {
        wi_stage = 5;
        wi_counter = 0;
        wi_advance = 2;

        for(i = 0; i < MAXPLAYERS; i++)
        {
            killvalue[i] = killpercent[i];
            itemvalue[i] = itempercent[i];
            secretvalue[i] = secretpercent[i];
        }
    }

    if(wi_advance == 2)
        return 0;

    if(wi_advance == 3)
    {
        //S_StartSound(NULL, sfx_explode);
        wi_advance = 4;
    }

    // fade out, complete world
    if(wi_advance >= 4)
    {
        clusterdef_t* cluster;
        clusterdef_t* nextcluster;

        cluster = P_GetCluster(gamemap);
        nextcluster = P_GetCluster(nextmap);

        if((nextcluster && cluster != nextcluster && nextcluster->enteronly) ||
            (cluster && cluster != nextcluster && !cluster->enteronly))
            return ga_victory;

        return 1;
    }

    if(wi_counter)
    {
        if((gametic - wi_counter) <= 60)
            return 0;
    }

    next = true;

    // counter ticks
    switch(wi_stage)
    {
    case 0:
        S_StartSound(NULL, sfx_explode);
        wi_stage = 1;
        state = false;
        break;

    case 1:     // kills
        for(i = 0; i < MAXPLAYERS; i++)
        {
            killvalue[i] += 4;
            if(killvalue[i] > killpercent[i])
                killvalue[i] = killpercent[i];
            else
                next = false;
        }

        if(next)
        {
            S_StartSound(NULL, sfx_explode);

            wi_counter = gametic;
            wi_stage = 2;
        }

        state = true;
        break;

    case 2:     // item
        for(i = 0; i < MAXPLAYERS; i++)
        {
            itemvalue[i] += 4;
            if(itemvalue[i] > itempercent[i])
                itemvalue[i] = itempercent[i];
            else
                next = false;
        }

        if(next)
        {
            S_StartSound(NULL, sfx_explode);

            wi_counter = gametic;
            wi_stage = 3;
        }

        state = true;
        break;

    case 3:     // secret
        for(i = 0; i < MAXPLAYERS; i++)
        {
            secretvalue[i] += 4;
            if(secretvalue[i] > secretpercent[i])
                secretvalue[i] = secretpercent[i];
            else
                next = false;
        }

        if(next)
        {
            S_StartSound(NULL, sfx_explode);

            wi_counter = gametic;
            wi_stage = 4;
        }

        state = true;
        break;

    case 4:
        if(gamemap > 33 && nextmap > 33)
            S_StartSound(NULL, sfx_explode);

        wi_counter = gametic;
        wi_stage = 5;
        state = false;
        break;
    }

    if(!wi_advance && !state)
    {
        if(wi_stage == 5)
            wi_advance = 1;
    }

    // play sound as counter increases
    if(state && !(gametic & 3))
        S_StartSound(NULL, sfx_pistol);

    return 0;
}

//
// WI_Drawer
//

void WI_Drawer(void)
{
    int currentmap = gamemap;

    GFX_CLEAR_COLOR = 0x1F0000;

    if(currentmap < 0)
        currentmap = 0;

    if(currentmap > 33)
        currentmap = 33;

    if(wi_advance >= 4)
        return;

    GFX_ORTHO();

    R_SlamBackground("EVIL", 31, 1, 0xFFFFF);

    if(menuactive)
        return;

    // draw 'mapname' Finished text
    ST_DrawBigFont(-1, 20, 0xFFFFF, P_GetMapInfo(currentmap)->mapname);
	ST_DrawBigFont(-1, 36, 0xFFFFF, "Finished");

    // draw kills
    ST_DrawBigFont(45, 60, WIALPHARED, "Kills");
    ST_DrawBigFont(204, 60, WIALPHARED, "%");
    if(wi_stage > 0 && killvalue[0] > -1)
        ST_DrawNumber(168, 60, killvalue[0], 1, WIALPHARED);

    // draw items
    ST_DrawBigFont(45, 78, WIALPHARED, "Items");
    ST_DrawBigFont(204, 78, WIALPHARED, "%");
    if(wi_stage > 1 && itemvalue[0] > -1)
        ST_DrawNumber(168, 78, itemvalue[0], 1, WIALPHARED);

    // draw secrets
    ST_DrawBigFont(45, 99, WIALPHARED, "Secrets");
    ST_DrawBigFont(204, 99, WIALPHARED, "%");
    if(wi_stage > 2 && secretvalue[0] > -1)
        ST_DrawNumber(168, 99, secretvalue[0], 1, WIALPHARED);

    // draw time
    if(wi_stage > 3)
    {
        ST_DrawBigFont(45, 120, WIALPHARED, "Time");
        ST_DrawBigFont(168, 120, WIALPHARED, timevalue);
    }

    // draw password and name of next map
    if(wi_stage > 4 && (P_GetMapInfo(nextmap) != NULL))
    {
        ST_DrawBigFont(-1, 145, 0xFFFFF, "Entering");
        ST_DrawBigFont(-1, 145 + 16, 0xFFFFF, P_GetMapInfo(nextmap)->mapname);
    }
}

