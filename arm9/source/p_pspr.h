#ifndef __P_PSPR__
#define __P_PSPR__

#include "doomdef.h"
#include "tables.h"
#include "info.h"
#include "m_fixed.h"

//
// Frame flags:
// handles maximum brightness (torches, muzzle flare, light sources)
//

#define FF_FULLBRIGHT	0x8000	// flag in thing->frame
#define FF_FRAMEMASK	0x7fff



//
// Overlay psprites are scaled shapes
// drawn directly on the view screen,
// coordinates are given for a 320*200 view screen.
//
typedef enum
{
    ps_weapon,
    ps_flash,
    NUMPSPRITES

} psprnum_t;

typedef struct
{
    state_t*    state;	// a NULL state means not active
    int         tics;
    fixed_t     sx;
    fixed_t     sy;
    int         alpha;  // [d64] for rendering
} pspdef_t;

typedef struct
{
    ammotype_t	ammo;
    int		upstate;
    int		downstate;
    int		readystate;
    int		atkstate;
    int		flashstate;
} weaponinfo_t;

// Weapon info: sprite frames, ammunition use.
extern  weaponinfo_t    weaponinfo[NUMWEAPONS];

#endif

