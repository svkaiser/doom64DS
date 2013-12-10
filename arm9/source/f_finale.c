#include "z_zone.h"
#include "w_wad.h"
#include "s_sound.h"
#include "d_englsh.h"
#include "sounds.h"
#include "info.h"
#include "r_local.h"
#include "st_main.h"
#include "g_game.h"

static int              castrotation = 0;
static int              castnum;
static int              casttics;
static state_t*         caststate;
static dboolean         castdeath;
static dboolean         castdying;
static int              castframes;
static int              castonmelee;
static dboolean         castattacking;

static int              f_alpha = 0;
static int              fInterFadeOut = false;
static char             fInterString[16][32];
static dboolean         fInterDone = false;
static int              fInterAlpha = 0;
static int              fInterSlot = 0;
static int              fTextOffset = 0;
static clusterdef_t*    fcluster = NULL;
static dboolean         fstopmusic = false;

typedef struct
{
    char *name;
    mobjtype_t type;
} castinfo_t;

castinfo_t castorder[] = 
{
    {   CC_ZOMBIE,  MT_POSSESSED1   },
    {   CC_SHOTGUN, MT_POSSESSED2   },
    {   CC_IMP,     MT_IMP1         },
    {   CC_IMP2,    MT_IMP2         },
    {   CC_DEMON,   MT_DEMON1       },
    {   CC_DEMON2,  MT_DEMON2       },
    {   CC_LOST,    MT_SKULL        },
    {   CC_CACO,    MT_CACODEMON    },
    {   CC_HELL,    MT_BRUISER2     },
    {   CC_BARON,   MT_BRUISER1     },
    {   CC_ARACH,   MT_BABY         },
    {   CC_PAIN,    MT_PAIN         },
    {   CC_MANCU,   MT_MANCUBUS     },
    {   CC_CYBER,   MT_CYBORG       },
    {   CC_HERO,    MT_PLAYER       },
    {   NULL,       0               }
};

//
// IN_Start
//

void IN_Start(void)
{
    int i = 0;
    int j = 0;
    int k = 0;

    // initialize variables
    automapactive   = false;
    fInterDone      = false;
    fstopmusic      = true;
    fInterAlpha     = 0;
    fInterSlot      = 0;
    fInterFadeOut   = false;
    fTextOffset     = 0;
    f_alpha         = 0;
    fcluster        = NULL;

    memset(fInterString, 0, 16*32);

    fcluster = P_GetCluster(nextmap);

    if(!fcluster)
        fcluster = P_GetCluster(gamemap);

    // try to bail out if no cluster is found at all
    if(!fcluster)
    {
        gameaction = ga_loadlevel;
        return;
    }

    i = 0;

    // setup intermission text
    while(k < strlen(fcluster->text))
    {
        char c = fcluster->text[k++];

        if(c == '\n')
        {
            j = 0;
            if(fInterString[i][0] == '\0')
                fInterString[i][0] = ' ';

            i++;
            continue;
        }
        else
            fInterString[i][j++] = c;
    }

    gameaction  = ga_nothing;
    
    S_StartMusic(fcluster->music);
}

//
// IN_Stop
//

void IN_Stop(void)
{
    if(fstopmusic)
        S_StopMusic();

    // TODO
    //WIPE_FadeScreen(6);
}

//
// IN_Drawer
//

void IN_Drawer(void)
{
    int i = 0;
    byte alpha = 0;
    int y = 0;
    rcolor color;

    GFX_CLEAR_COLOR = 0x1F0000;
    GFX_ORTHO();

    if(fcluster->scrolltextend)
        color = ARGB16((f_alpha >> 3), 31, 31, 31);
    else
        color = 0xFFFFF;
    
    // Draw background
    R_SlamBackground(fcluster->pic, fcluster->pic_x, fcluster->pic_y, color);

    if(!fInterFadeOut)
    {
        // don't draw anything else until background is fully opaque
        if(f_alpha < 0xff)
            return;
    }

    i = 0;
    while(fInterString[i][0] != '\0')
        i++;

    y = (SCREEN_HEIGHT / 2) - ((i * 14) / 2);

    // draw strings
    for(i = 0;; i++)
    {
        if(i == fInterSlot)
            alpha = (byte)fInterAlpha;
        else
            alpha = 0xff;
        
        ST_DrawBigFont(-1, y - fTextOffset, ARGB16((alpha >> 3), 31, 31, 31), fInterString[i]);
        y += 14;
        
        if(i == fInterSlot || !fInterString[i][0])
            return;
    }
}

//
// IN_Finish
//

static void IN_Finish(void)
{
    if(fInterFadeOut)
        return;
    
    if(fcluster->scrolltextend)
        fInterFadeOut = true;
    else
        gameaction = ga_loadlevel;

    fInterDone = true;
}

//
// IN_Ticker
//

int IN_Ticker(void)
{
    int   i;
    player_t  *player;

    // Fade out for finale after all of text has scrolled up towards the screen
    if(fInterFadeOut)
    {
        // text hasn't scrolled off screen yet
        if(fTextOffset++ < SCREENHEIGHT)
            return 0;

        if(!fcluster->enteronly)
        {
            fstopmusic = false;
            return ga_finale;
        }

        return 1;
    }
    else
    {
        // fade in for finale
        f_alpha = MIN(f_alpha + 8, 0xff);

        // wait until fully opaque
        if(f_alpha < 0xff)
            return 0;
    }
    
    if(fInterDone || devparm)
    {
        // check for button presses to skip delays
        for(i = 0, player = players; i < MAXPLAYERS; i++, player++)
        {
            if(playeringame[i])
            {
                if(player->cmd.buttons & BT_ATTACK)
                {
                    if(!player->attackdown)
                        IN_Finish();
                    player->attackdown = true;
                }
                else
                    player->attackdown = false;
            
                if(player->cmd.buttons & BT_USE)
                {
                    if(!player->usedown)
                        IN_Finish();
                    player->usedown = true;
                }
                else
                    player->usedown = false;
            }
        }
    }
    
    if(fInterDone)
        return 0;
    
    // fade in each line of text
    fInterAlpha += 6;
    
    if(fInterAlpha >= 0xff)
    {
        fInterAlpha = 0xff;
        if(!fInterString[++fInterSlot][0])
        {
            fInterSlot = fInterSlot - 1;
            fInterDone = true;
            return 0;
        }

        fInterAlpha = 0;
    }

    return 0;
}

//
// F_Start
//

void F_Start(void)
{
    gameaction = ga_nothing;
    automapactive = false;
    
    castnum = 0;
    caststate = &states[mobjinfo[castorder[castnum].type].seestate];
    casttics = caststate->tics;
    castdeath = false;
    castframes = 0;
    castonmelee = 0;
    castattacking = false;
    castdying = false;
    
    // hack - force-play seesound from first cast
    S_StartSound(NULL, mobjinfo[castorder[castnum].type].seesound);
}

//
// F_Stop
//

void F_Stop(void)
{
    S_StopMusic();
    //gameaction = ga_nothing;
    // TODO
    //WIPE_FadeScreen(6);
}

//
// F_Ticker
//

int F_Ticker(void)
{
    int st;

    if(!castdeath)
    {
        if(buttons & KEY_LEFT)
        {
            castrotation = (castrotation+1) & 7;
            buttons &= ~KEY_LEFT;
        }
        else if(buttons & KEY_RIGHT)
        {
            castrotation = (castrotation-1) & 7;
            buttons &= ~KEY_RIGHT;
        }
        else if(players[consoleplayer].cmd.buttons)
            castdying = true;
    }
    
    // TODO
    //finalePal.r = MIN(finalePal.r += 2, 250);
    //finalePal.g = MIN(finalePal.g += 2, 250);
    //finalePal.b = MIN(finalePal.b += 2, 250);
    
    if(!castdeath && castdying)
    {
        S_StartSound(NULL, sfx_shotgun);
        S_StartSound(NULL, mobjinfo[castorder[castnum].type].deathsound);
        caststate = &states[mobjinfo[castorder[castnum].type].deathstate];
        casttics = caststate->tics;
        castframes = 0;
        castattacking = false;
        castdying = false;
        castdeath = true;
    }
    
    // advance state
    if(--casttics > 0)
        return 0; // not time to change state yet
    
    if(caststate->tics == -1 || caststate->nextstate == S_000)
    { 
        // switch from deathstate to next monster
        
        castnum++;
        castdeath = false;
        castrotation = 0;
        if(castorder[castnum].name == NULL)
            castnum = 0;
        
        // TODO
        //finalePal.a = 255;
        //finalePal.r = 0;
        //finalePal.g = 0;
        //finalePal.b = 0;
        
        if(mobjinfo[castorder[castnum].type].seesound)
            S_StartSound(NULL, mobjinfo[castorder[castnum].type].seesound);
        
        caststate = &states[mobjinfo[castorder[castnum].type].seestate];
        castframes = 0;
    }
    else
    {
        // just advance to next state in animation
        
        if(caststate == &states[S_007]) // gross hack..
            goto stopattack;
        
        st = caststate->nextstate;
        caststate = &states[st];
        castframes++;
        
        // sound hacks
        {
            int sound = 0;
            
            switch(st)
            {
            case S_007: sound = sfx_sht2fire; break;     // player
            case S_055: sound = sfx_sargatk; break;     // demon
            case S_084:                             // mancubus
            case S_086:                             // mancubus
            case S_170:                             // imp
            case S_201:                             // cacodemon
            case S_245:                             // hell knight
            case S_224:                             // bruiser
            case S_088: sound = sfx_bdmissile; break;     // mancubus
            case S_168: sound = sfx_scratch; break;     // imp scratch
            case S_109: sound = sfx_pistol; break;     // possessed
            case S_138: sound = sfx_shotgun; break;     // shotgun guy
            case S_331:                             // pain
            case S_261: sound = sfx_skullatk; break;     // skull
            case S_288: sound = sfx_plasma; break;     // baby
            case S_308:                             // cyborg
            case S_310:                             // cyborg
            case S_312: sound = sfx_missile; break;     // cyborg
            default: sound = 0; break;
            }
            
            if(sound)
                S_StartSound(NULL, sound);
        }
    }
    
    if(castframes == 12)
    {
        // go into attack frame
        castattacking = true;
        if(castonmelee)
            caststate = &states[mobjinfo[castorder[castnum].type].meleestate];
        else
            caststate = &states[mobjinfo[castorder[castnum].type].missilestate];
        castonmelee ^= 1;
        
        if(caststate == &states[S_000])
        {
            if(castonmelee)
                caststate = &states[mobjinfo[castorder[castnum].type].meleestate];
            else
                caststate = &states[mobjinfo[castorder[castnum].type].missilestate];
        }
    }
    
    if(castattacking)
    {
        if(castframes == 24 ||
            caststate == &states[mobjinfo[castorder[castnum].type].seestate])
        {
stopattack:
        castattacking = false;
        castframes = 0;
        caststate = &states[mobjinfo[castorder[castnum].type].seestate];
        }
    }
    
    casttics = caststate->tics;
    if(casttics == -1)
        casttics = TICRATE;

    return 0;
}


//
// F_Drawer
//

void F_Drawer(void)
{
    GFX_CLEAR_COLOR = 0x1F0000;
    GFX_ORTHO();

    R_SlamBackground("EVIL", 31, 1, 0xFFFFF);
    ST_DrawBigFont(-1, 260, ARGB16(31, 31, 0, 0), castorder[castnum].name);
    /*R_GLClearFrame(0xFF000000);
    R_DrawGfx(64, 30, "EVIL", D_RGBA(255, 255, 255, 0xff), false);
    M_DrawSmbText(-1, 240-32, D_RGBA(255, 0, 0, 0xff), castorder[castnum].name);
    R_DrawHudSprite(
        caststate->sprite,
        castrotation,
        (caststate->frame & FF_FRAMEMASK),
        160,
        180,
        1.0f,
        mobjinfo[castorder[castnum].type].palette,
        D_RGBA(finalePal.r, finalePal.g, finalePal.b, finalePal.a)
        );*/
}


