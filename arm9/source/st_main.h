#ifndef __ST_MAIN_H__
#define __ST_MAIN_H__

#include "doomtype.h"

void ST_ClearMessage(void);
void ST_Ticker(void);
void ST_Drawer(void);
void ST_UpdateFlash(void);
void ST_Init(void);
int ST_DrawBigFont(int x, int y, rcolor color, const char* string);
int ST_DrawMessage(int x, int y, rcolor color, const char* string, ...);
void ST_DrawNumber(int x, int y, int num, int type, rcolor c);

#endif

