#ifndef __M_FIXED__
#define __M_FIXED__

//
// Fixed point, 32bit as 16.16.
//
#define FRACBITS		16
#define FRACUNIT		(1<<FRACBITS)
#define FRACROUNDOFF(x) ((x & 0xFFFE0000) | FRACUNIT)
#define INT2F(x)		((x)<<FRACBITS)
#define F2INT(x)		((x)>>FRACBITS)
#define F2D3D(x) 		(((float)(x))/FRACUNIT)
#define F2DSFIXED(x)    ((x)>>14)
#define INT2DSFIXED(x)  ((x)<<2)

typedef int fixed_t;

fixed_t FixedMul    (fixed_t a, fixed_t b);
fixed_t FixedDiv    (fixed_t a, fixed_t b);
fixed_t FixedDiv2   (fixed_t a, fixed_t b);

static inline fixed_t FixedDot(fixed_t a1, fixed_t b1,
                 fixed_t c1, fixed_t a2,
                 fixed_t b2, fixed_t c2)
{
    return 
        FixedMul(a1, a2) +
        FixedMul(b1, b2) +
        FixedMul(c1, c2);
}

#endif