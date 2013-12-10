#include "sounds.h"
#include "s_sound.h"
#include "z_zone.h"
#include "m_fixed.h"
#include "p_local.h"
#include "tables.h"
#include "r_local.h"
#include "d_main.h"

// Adjustable by menu.
#define NORM_VOLUME     127
#define NORM_SEP        128

// when to clip out sounds
// Does not fit the large outdoor areas.
#define S_CLIPPING_DIST (1200<<FRACBITS)

// Distance to origin when sounds should be maxed out.
// This should relate to movement clipping resolution
// (see BLOCKMAP handling).

#define S_MAX_DIST (NORM_VOLUME * (S_CLIPPING_DIST >> FRACBITS))    // [d64]
#define S_CLOSE_DIST (200 << FRACBITS)
#define S_ATTENUATOR ((S_CLIPPING_DIST - S_CLOSE_DIST) >> FRACBITS)

#define S_PITCH_PERTURB         1
#define S_STEREO_SWING          (96*0x10000)

// percent attenuation from front to back
#define S_IFRACVOL              30

dboolean nosound = false;
dboolean nomusic = false;

static int lastmusic = 0;

typedef struct
{
    int handle;
    int starttic;
    fixed_t org_x;
    fixed_t org_y;
} sfxchannel_t;

static sfxchannel_t sfxchannels[NUMSFX];
static sfxchannel_t* sfxchan_ptr[16];

//
// Internals.
//
int S_AdjustSoundParams(fixed_t x, fixed_t y, int* vol, int* sep);

//
// S_Init
//
// Initializes sound stuff, including volume
// and the sequencer
//

void S_Init(void)
{
    if(nosound && nomusic)
        return;

    soundEnable();

    S_SetMusicVolume(80);
    S_SetSoundVolume(80);
}

//
// S_SetSoundVolume
//

void S_SetSoundVolume(float volume)
{
}

//
// S_SetMusicVolume
//

void S_SetMusicVolume(float volume)
{
}

//
// S_StartMusic
//

void S_StartMusic(int mnum)
{
    if(nomusic)
        return;

    if(mnum <= -1)
        return;

    lastmusic = mnum;
}

//
// S_StopMusic
//

void S_StopMusic(void)
{
    lastmusic = 0;
}

//
// S_ResetSound
//

void S_ResetSound(void)
{
    if(nosound && nomusic)
		return;
}

//
// S_PauseSound
//

void S_PauseSound(void)
{
    if(nosound && nomusic)
        return;
}

//
// S_ResumeSound
//

void S_ResumeSound(void)
{
    if(nosound && nomusic)
        return;
}

//
// S_StopSound
//

void S_StopSound(mobj_t* origin, int sfx_id)
{
    sfxchannel_t* sfxchan = &sfxchannels[sfx_id];
    // TODO
    // TEMP
    if(sfx_id == 0) return;
    sfxchan_ptr[sfxchan->handle] = NULL;
    soundKill(sfxchan->handle);
}

//
// S_GetActiveSounds
//

int S_GetActiveSounds(void)
{
    return 0;
}

//
// S_RemoveOrigin
//

void S_RemoveOrigin(mobj_t* origin)
{
}

//
// S_UpdateSounds
//

void S_UpdateSounds(void)
{
    // TODO - do this on ARM7
    /*int i;

    for(i = 0; i < 16; i++)
    {
        sfxchannel_t* sfxchan = sfxchan_ptr[i];

        if(!sfxchan)
            continue;

        if(sfxchan->data == NULL)
            continue;

        if(sfxchan->org_x | sfxchan->org_y)
        {
            int volume = NORM_VOLUME;
            int sep = NORM_SEP;

            if(!S_AdjustSoundParams(sfxchan->org_x, sfxchan->org_y, &volume, &sep))
            {
                sfxchan_ptr[sfxchan->handle] = NULL;
                soundKill(sfxchan->handle);
                return;
            }

            soundSetVolume(sfxchan->handle, volume);
            soundSetPan(sfxchan->handle, sep / 2);
        }
    }*/
}

//
// S_StartSound
//

void S_StartSound(mobj_t* origin, int sfx_id)
{
    int volume;
    int sep;
    //int reverb;

    if(nosound)
        return;

    if(sfx_id == sfx_None)
        return;

    if(origin && origin != players[consoleplayer].cameratarget)
    {
        if(!S_AdjustSoundParams(origin->x, origin->y, &volume, &sep))
            return;
    }
    else
    {
        sep = NORM_SEP;
        volume = NORM_VOLUME;
    }

    // TODO
    /*reverb = 0;
    
    if(origin)
    {
        subsector_t* subsector;
        
        subsector = R_PointInSubsector(origin->x, origin->y);

        if(subsector->sector->flags & MS_REVERB)
            reverb = 16;
        else if(subsector->sector->flags & MS_REVERBHEAVY)
            reverb = 32;
    }*/

    // Assigns the handle to one of the channels in the mix/output buffer.
    
    {
        char name[9];
        int lump;
        sfxchannel_t* sfxchan;

        sprintf(name, "SFX_%03d", sfx_id - 1);
        lump = W_GetNumForName(name);

        sfxchan = &sfxchannels[sfx_id];

        if(sfxchan->starttic == gametic)
            return;

        sfxchan->starttic = gametic;
        if(origin)
        {
            sfxchan->org_x = origin->x;
            sfxchan->org_y = origin->y;
        }
        else
        {
            sfxchan->org_x = 0;
            sfxchan->org_y = 0;
        }

        sfxchan->handle = soundPlaySample(
            W_CacheLumpNum(lump, PU_AUDIO),
            SoundFormat_16Bit,
            W_LumpLength(lump),
            sfx_id == sfx_itemup ? 22050 : 11025,
            sfx_id == sfx_electric ? 20 : volume,
            sep / 2,
            sfx_id == sfx_electric ? 1 : 0,
            0
            );

        sfxchan_ptr[sfxchan->handle] = &sfxchannels[sfx_id];
    }
}

//
// S_AdjustSoundParams
//
// Changes volume, stereo-separation, and pitch variables
// from the norm of a sound effect to be played.
// If the sound is not audible, returns a 0.
// Otherwise, modifies parameters and returns 1.
//

int S_AdjustSoundParams(fixed_t x, fixed_t y, int* vol, int* sep)
{
    fixed_t     approx_dist;
    angle_t     angle;
    mobj_t*		listener;
    player_t*	player;
    
    player = &players[consoleplayer];
    
    listener = player->cameratarget;
    
    // calculate the distance to sound origin
    //  and clip it if necessary
    
    // From _GG1_ p.428. Appox. eucledian distance fast.
    approx_dist = P_AproxDistance(listener->x - x, listener->y - y);
    
    if(approx_dist > S_CLIPPING_DIST)
        return 0;

    if(listener->x != x || listener->y != y)
    {
        // angle of source to listener
        angle = R_PointToAngle2(listener->x, listener->y, x, y);
    
        if(angle <= listener->angle)
            angle += 0xffffffff;
        angle -= listener->angle;
    
        // stereo separation
        *sep = (NORM_VOLUME + 1) - (FixedMul(S_STEREO_SWING, dsin(angle)) >> FRACBITS);
    }
    else
        *sep = NORM_SEP;
    
    // volume calculation
    if (approx_dist < S_CLOSE_DIST)
        *vol = NORM_VOLUME;
    else
    {
        // distance effect
        approx_dist >>= FRACBITS;
        *vol = (((-approx_dist << 7) + (approx_dist)) + S_MAX_DIST) / S_ATTENUATOR;
    }
    
    return (*vol > 0);
}




