#ifndef __D_PLAYER__
#define __D_PLAYER__

#include "p_pspr.h"
#include "p_mobj.h"
#include "doomdef.h"

// The data sampled per tick (single player)
// and transmitted to other peers (multiplayer).
// Mainly movements/button commands per game tick,
// plus a checksum for internal state consistency.
typedef struct
{
    short   forwardmove;    // *2048 for move
    short   sidemove;       // *2048 for move
    short   angleturn;      // <<16 for angle delta
	short   pitch;
    byte    consistancy;    // checks for net game
    byte    buttons;
} ticcmd_t;

//
// Player states.
//
typedef enum
{
    PST_LIVE,   // Playing or camping.
    PST_DEAD,   // Dead on the ground, view follows killer.
    PST_REBORN  // Ready to restart/respawn???
        
} playerstate_t;


//
// Player internal flags, for cheats and debug.
//
typedef enum
{
    CF_NOCLIP       = 1,    // No clipping, walk through barriers.
    CF_GODMODE      = 2,    // No damage, no health loss.
    CF_UNDYING      = 4,    // Take damage but not die
    CF_SPECTATOR    = 8,    // Specator mode
    CF_CHASECAM     = 16,   // Chase cam mode
    CF_FLOATCAM     = 32    // Floating camera
        
} cheat_t;

typedef enum
{
    ART_FAST    = 0,
    ART_DOUBLE,
    ART_TRIPLE
        
} artifacts_t;


//
// Extended player object info: player_t
//
typedef struct player_s
{
    mobj_t*         mo;
    playerstate_t   playerstate;
    ticcmd_t        cmd;
    
    // Determine POV,
    //  including viewpoint bobbing during movement.
    // Focal origin above r.z
    fixed_t         viewz;
    // Base height above floor for viewz.
    fixed_t         viewheight;
    // Bob/squat speed.
    fixed_t         deltaviewheight;
    // bounded/scaled total momentum.
    fixed_t         bob;

    // [d64] extra pitch for recoil/knockback
    angle_t         recoilpitch;
    
    // This is only used between levels,
    // mo->health is used during levels.
    int             health;
    int             armorpoints;
    // Armor type is 0-2.
    int             armortype;
    
    // Power ups. invinc and invis are tic counters.
    int             powers[NUMPOWERS];
    dboolean        cards[NUMCARDS];

    // [kex] for hud when trying to open a locked door
    dboolean        tryopen[NUMCARDS];

    // [d64] laser artifact flags
    int             artifacts;

    dboolean        backpack;

    weapontype_t    readyweapon;
    
    // Is wp_nochange if not changing.
    weapontype_t    pendingweapon;
    
    dboolean        weaponowned[NUMWEAPONS];
    int             ammo[NUMAMMO];
    int             maxammo[NUMAMMO];
    
    // True if button down last tic.
    int             attackdown;
    int             usedown;

    // [kex] true if jump button down last tic
    int             jumpdown;
    
    // Bit flags, for cheats and debug.
    // See cheat_t, above.
    int             cheats;
    
    // Refired shots are less accurate.
    int             refire;
    
    // For intermission stats.
    int             killcount;
    int             itemcount;
    int             secretcount;
    
    // Hint messages.
    char*           message;

    // [d64] tic for how long message should stay on hud...not used in d64ex
    // int          messagetic;
    
    // For screen flashing (red or bright).
    int             damagecount;
    int             bonuscount;
    int             bfgcount;
    
    // Who did damage (NULL for floors/ceilings).
    mobj_t*         attacker;
    
    // [kex] which mobj to use as the view camera (defaults to player->mo)
    mobj_t*         cameratarget;
    
    // Overlay view sprites (gun, etc).
    pspdef_t        psprites[NUMPSPRITES];
    
    // Translation palettes for multiplayer
    int             palette;
    
    // [d64] Track if player is on the ground or not
    dboolean        onground;

    // [kex] allow autoaim?
    dboolean        autoaim;

    // [kex] display pic as message instead of text
    int             messagepic;
    
} player_t;

extern player_t* player;

#endif

