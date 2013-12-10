#include "stdlib.h"
#include "ds.h"
#include "doomtype.h"
#include "doomdef.h"
#include "m_fixed.h"
#include "d_main.h"

// Fixme. __USE_C_FIXED__ or something.

#if 0

fixed_t
FixedMul
( fixed_t	a,
  fixed_t	b )
{
#ifdef USE_ASM
    fixed_t	c;
    _asm
    {
	mov eax, [a]
	mov ecx, [b]
	imul ecx
	shr eax, 16
	shl edx, 16
	or eax, edx
	mov [c], eax
    }
    return(c);
#else
    return (fixed_t)(((int64) a * (int64) b) >> FRACBITS);
#endif
}



//
// FixedDiv, C version.
//

fixed_t
FixedDiv
( fixed_t	a,
  fixed_t	b )
{
    if ( (D_abs(a)>>14) >= D_abs(b))
	return (a^b)<0 ? MININT : MAXINT;
    return FixedDiv2 (a,b);

}



fixed_t
FixedDiv2
( fixed_t	a,
  fixed_t	b )
{
    return (fixed_t)((((int64)a)<<FRACBITS)/b);
}

#endif

