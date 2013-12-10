#ifndef __DOOMDEF__
#define __DOOMDEF__

#include <stdio.h>
#include "ds.h"
#include "doomtype.h"
#include "tables.h"

#define dcos(angle) finecosine[(angle) >> ANGLETOFINESHIFT]
#define dsin(angle) finesine[(angle) >> ANGLETOFINESHIFT]

//
// The packed attribute forces structures to be packed into the minimum 
// space necessary.  If this is not done, the compiler may align structure
// fields differently to optimise memory access, inflating the overall
// structure size.  It is important to use the packed attribute on certain
// structures where alignment is important, particularly data read/written
// to disk.
//

#define PACKEDATTR  __attribute__((packed))
#define PUREFUNC    __attribute__((pure))
#define NOINLINE    __attribute__((noinline))

//
// Global parameters/defines.
//

#define SCREENWIDTH		320
#define SCREENHEIGHT	240

#define MAX_MESSAGE_SIZE 1024

// The maximum number of players, multiplayer/networking.
// remember to add settings for extra skins if increase:)
#define MAXPLAYERS      4

// State updates, number of tics / second.
#define TICRATE         30

// Networking and tick handling related.
#define BACKUPTICS		128

// The current state of the game: whether we are
// playing, gazing at the intermission screen,
// the game final animation, or a demo.
typedef enum
{
    GS_NONE,
    GS_LEVEL,
    GS_WISTATS,
    GS_FINALE,
    GS_SKIPPABLE
} gamestate_t;

//
// Difficulty/skill settings/filters.
//

// Skill flags.
#define	MTF_EASY            1
#define	MTF_NORMAL          2
#define	MTF_HARD            4
#define	MTF_AMBUSH          8		// Deaf monsters/do not react to sound.
#define MTF_MULTI           16		// Multiplayer specific
#define MTF_SPAWN           32		// Don't spawn until triggered in level
#define MTF_ONTOUCH         64		// Trigger something when picked up
#define MTF_ONDEATH         128		// Trigger something when killed
#define MTF_SECRET          256		// Count as secret for intermission when picked up
#define MTF_NOINFIGHTING    512		// Ignore other attackers
#define MTF_NODEATHMATCH    1024	// Don't spawn in deathmatch games
#define MTF_NONETGAME       2048	// Don't spawn in standard netgame mode

//
// MAPINFO
//

typedef struct
{
    char        mapname[64];
    int         mapid;
    int         music;
    int         type;
    int         cluster;
    int         exitdelay;
    dboolean    nointermission;
    dboolean    clearchts;
    dboolean    forcegodmode;
    dboolean    contmusexit;
    int         oldcollision;
    int         allowjump;
} mapdef_t;

typedef struct
{
    int         id;
    int         music;
    dboolean    enteronly;
    short       pic_x;
    short       pic_y;
    dboolean    nointermission;
    dboolean    scrolltextend;
    char        text[512];
    char        pic[9];
} clusterdef_t;

//
// [kex] mapinfo
//
mapdef_t* P_GetMapInfo(int map);
clusterdef_t* P_GetCluster(int map);

//
// [kex] sky definitions
//
typedef enum
{
    SKF_CLOUD       = 0x1,
    SKF_THUNDER     = 0x2,
    SKF_FIRE        = 0x4,
    SKF_BACKGROUND  = 0x8,
    SKF_FADEBACK    = 0x10,
    SKF_VOID        = 0x20
} skyflags_e;

typedef struct
{
    char        flat[9];
    int         flags;
    char        pic[9];
    char        backdrop[9];
    rcolor      fogcolor;
    rcolor      skycolor[3];
    int         fognear;
} skydef_t;

typedef enum
{
    sk_baby,
    sk_easy,
    sk_medium,
    sk_hard,
    sk_nightmare
} skill_t;

//
// Key cards.
//
typedef enum
{
    it_bluecard,
    it_yellowcard,
    it_redcard,
    it_blueskull,
    it_yellowskull,
    it_redskull,
        
    NUMCARDS
        
} card_t;

// The defined weapons,
//  including a marker indicating
//  user has not changed weapon.
typedef enum
{
    wp_chainsaw,
    wp_fist,
    wp_pistol,
    wp_shotgun,
    wp_supershotgun,
    wp_chaingun,
    wp_missile,
    wp_plasma,
    wp_bfg,
    wp_laser,
    NUMWEAPONS,
    
    // No pending weapon change.
    wp_nochange
    
} weapontype_t;

// Ammunition types defined.
typedef enum
{
    am_clip,	// Pistol / chaingun ammo.
    am_shell,	// Shotgun / double barreled shotgun.
    am_cell,	// Plasma rifle, BFG.
    am_misl,	// Missile launcher.
    NUMAMMO,
    am_noammo	// Unlimited for chainsaw / fist.
    
} ammotype_t;

// Power up artifacts.
typedef enum
{
    pw_invulnerability,
    pw_strength,
    pw_invisibility,
    pw_ironfeet,
    pw_allmap,
    pw_infrared,
    NUMPOWERS
    
} powertype_t;

#define BONUSADD    4

//
// Power up durations,
//  how many seconds till expiration,
//
typedef enum
{
    INVULNTICS	= (30*TICRATE),
    INVISTICS	= (60*TICRATE),
    INFRATICS	= (120*TICRATE),
    IRONTICS	= (60*TICRATE),
    STRTICS     = (3*TICRATE)
    
} powerduration_t;

//
// MISC
//

// bbox coordinates
enum
{
    BOXTOP,
    BOXBOTTOM,
    BOXLEFT,
    BOXRIGHT
};

// Bounding box functions.
void M_ClearBox (fixed_t* box);
void M_AddToBox(fixed_t* box, fixed_t x, fixed_t y);

// Randomizer functions
int P_Random(void); // tic dependent
int M_Random(void); // not tic dependent
void M_ClearRandom(void);
int P_RandomShift(int shift);

extern int compatflags;

// 20120209 villsa - compatibility flags
enum
{
    COMPATF_COLLISION   = (1 << 0),     // don't use maxradius for mobj position checks
    COMPATF_MOBJPASS    = (1 << 1),     // allow mobjs to stand on top one another
    COMPATF_LIMITPAIN   = (1 << 2),     // pain elemental limited to 17 lost souls?
};

#endif          // __DOOMDEF__
