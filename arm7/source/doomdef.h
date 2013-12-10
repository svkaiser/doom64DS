#ifndef __DOOMDEF__
#define __DOOMDEF__

#include <stdio.h>
#include "doomtype.h"
#include "tables.h"

void *dmemcpy(void *s1, const void *s2, unsigned int n);
void *dmemset(void *s, unsigned int c, unsigned int n);

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

#endif          // __DOOMDEF__
