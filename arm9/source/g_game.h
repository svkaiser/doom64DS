#ifndef __G_GAME__
#define __G_GAME__

#include "doomdef.h"
#include "d_main.h"

extern dboolean sendpause;
extern int buttons;

//
// GAME
//

void G_InitNew(skill_t skill, int map);
void G_DeferedInitNew(skill_t skill, int map);
void G_PlayDemo(const char* name);
void G_CompleteLevel(void);
void G_ExitLevel(void);
void G_SecretExitLevel(int map);
void G_Ticker(void);
void G_ScreenShot(void);
void G_RunTitleMap(void);
void G_RunGame(void);
void G_BuildTiccmd(ticcmd_t* cmd);

dboolean G_CheckDemoStatus(void);
dboolean G_Responder(event_t* ev);

#endif
