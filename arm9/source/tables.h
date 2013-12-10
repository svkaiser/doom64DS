#ifndef __TABLES__
#define __TABLES__

#include "m_fixed.h"

#define M_PI            3.14159265358979323846

#define FINEANGLES		8192
#define FINEMASK		(FINEANGLES-1)


// 0x100000000 to 0x2000
#define ANGLETOFINESHIFT	19

// Binary Angle Measument, BAM.
#define ANG45			0x20000000
#define ANG90			0x40000000
#define ANG180			0x80000000
#define ANG270			0xc0000000
#define ANG1			(ANG45/45)
#define ANG5			(ANG90/18)
#define ANGLE_MAX		(0xffffffff)


#define SLOPERANGE		2048
#define SLOPEBITS		11
#define DBITS			(FRACBITS-SLOPEBITS)

#define TRUEANGLES(x) (((x) >> ANGLETOFINESHIFT) * 360.0f / FINEANGLES)

typedef unsigned angle_t;

// Effective size is 10240.
extern  fixed_t		finesine[5*FINEANGLES/4];

// Re-use data, is just PI/2 pahse shift.
extern  fixed_t*	finecosine;

// Effective size is 2049;
// The +1 size is to handle the case when x==y
//  without additional checking.
extern angle_t		tantoangle[SLOPERANGE+1];


#endif

