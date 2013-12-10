#ifndef __S_SOUND__
#define __S_SOUND__

#include "p_mobj.h"
#include "sounds.h"

extern dboolean nosound;
extern dboolean nomusic;

//
// Initializes sound stuff, including volume
// Sets channels, SFX and music volume,
//  allocates channel buffer
//
void S_Init(void);
void S_SetSoundVolume(float volume);
void S_SetMusicVolume(float volume);
void S_ResetSound(void);
void S_PauseSound(void);
void S_ResumeSound(void);

//
// Start sound for thing at <origin>
//  using <sound_id> from sounds.h
//
void S_StartSound(mobj_t* origin, int sound_id);
void S_UpdateSounds(void);
void S_RemoveOrigin(mobj_t* origin);
// Will start a sound at a given volume.
void S_StartSoundAtVolume(mobj_t* origin, int sound_id, int volume);
// Stop sound for thing at <origin>
void S_StopSound(mobj_t* origin, int sfx_id);
int S_GetActiveSounds(void);
// Start music using <music_id> from sounds.h
void S_StartMusic(int mnum);
void S_StopMusic(void);


#endif
