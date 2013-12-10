@d_base.s
@arm routines for doom64 DS

	.section .rodata

	.align	4
	.arm
	
@==========================================================================
@Functions
@==========================================================================
    .global _DivInt
    .global _SlopeDiv
	.global _R_PointToAngle
	.global _R_PointOnSide
	.global	FixedMul
	.global	FixedDiv
	.global	FixedDiv2

@=====================================
@_DivInt/_DivIntU
@regular int divide
@=====================================
.pool
_DivInt:
	@set r12 to address of DIV_NUMERATOR32
	ldr		r12, =67109520
	
	@set DIV_CR DIV_32_32 bit
	mov		r2, #0
	strh	r2, [r12, #-16]

	@move a to DIV_NUMERATOR32
	str		r0, [r12]
	@move b to DIV_DENOMINATOR32
	str		r1, [r12, #8]
	
	@hang around until busy bit is clear
.regWaitA:
	ldrh	r0, [r12, #-16]
	ands	r0, r0, #32768
	bne		.regWaitA
			
	@put result from DIV_RESULT32 into r0
	ldr		r0, [r12, #16]
		
	bx		lr
	
@=====================================
@_SlopeDiv
@=====================================
_SlopeDiv:
	push	{lr}
	cmp		r1, #512
	movls	r0, #2048
	bls		.sdEnd1
	
	sub		sp, sp, #4
	mov		r0, r0, lsl #3
	mov		r1, r1, lsr #8
	bl		_DivInt
	add		sp, sp, #4
	
	cmp		r0, #2048
	movhi	r0, #2048
	
.sdEnd1:
	pop		{pc}
	
@=====================================
@_R_PointToAngle
@=====================================
_R_PointToAngle:
	push	{lr}
	mov	r2, r1
	sub	sp, sp, #4
	mov	r3, r0
	orr	r2, r0
	beq	.L234
	
	cmp	r0, #0
	blt	.L226
	
	cmp	r1, #0
	blt	.L227
	
	cmp	r0, r1
	ble	.L228
	
	mov	r0, r1
	mov	r1, r3
	bl	_SlopeDiv
	ldr	r3, .L236
	lsl	r0, r0, #2
	ldr	r0, [r0, r3]
	add	sp, sp, #4
	pop	{pc}
	
.L234:
	mov	r0, #0
.L225:
	add	sp, sp, #4
	pop	{pc}
.L226:
	neg	r3, r0
	cmp	r1, #0
	
	blt	.L230
	cmp	r1, r3
	
	bge	.L231
	mov	r0, r1
	mov	r1, r3
	bl	_SlopeDiv
	ldr	r3, .L236
	lsl	r0, r0, #2
	ldr	r2, [r0, r3]
	ldr	r3, .L236+4
	sub	r0, r3, r2
	add	sp, sp, #4
	pop	{pc}
	
.L228:
	bl	_SlopeDiv
	ldr	r3, .L236
	lsl	r0, r0, #2
	ldr	r2, [r0, r3]
	ldr	r3, .L236+8
	sub	r0, r3, r2
	add	sp, sp, #4
	pop	{pc}
	
.L227:
	neg	r1, r1
	cmp	r0, r1
	ble	.L229
	
	mov	r0, r1
	mov	r1, r3
	bl	_SlopeDiv
	ldr	r3, .L236
	
	lsl	r0, r0, #2
	ldr	r3, [r0, r3]
	neg	r0, r3
	add	sp, sp, #4
	pop	{pc}
	
.L231:
	mov	r0, r3
	bl	_SlopeDiv
	ldr	r3, .L236
	
	lsl	r0, r0, #2
	ldr	r3, [r0, r3]
	mov	r2, #128
	lsl	r2, r2, #23
	add	r0, r3, r2
	add	sp, sp, #4
	pop	{pc}
	
.L229:
	bl	_SlopeDiv
	ldr	r3, .L236
	
	lsl	r0, r0, #2
	ldr	r3, [r0, r3]
	ldr	r2, .L236+12
	add	r0, r3, r2
	add	sp, sp, #4
	pop	{pc}
	
.L230:
	neg	r1, r1
	cmp	r3, r1
	bgt	.L235
	
	mov	r0, r3
	bl	_SlopeDiv
	ldr	r3, .L236
	
	lsl	r0, r0, #2
	ldr	r2, [r0, r3]
	ldr	r3, .L236+16
	sub	r0, r3, r2
	add	sp, sp, #4
	pop	{pc}
	
.L235:
	mov	r0, r1
	mov	r1, r3
	bl	_SlopeDiv
	ldr	r3, .L236
	
	lsl	r0, r0, #2
	ldr	r3, [r0, r3]
	ldr	r2, .L236+20
	add	r0, r3, r2
	add	sp, sp, #4
	pop	{pc}
	
.L237:
	.align	2
.L236:
	.word	tantoangle
	.word	2147483647
	.word	1073741823
	.word	-1073741824
	.word	-1073741825
	.word	-2147483648

@=====================================
@_R_PointOnSide
@=====================================

_R_PointOnSide:
	push	{r4, r5, r6, lr}
	mov	r4, r0
	ldr	r0, [r2, #8]

	mov	r6, r1
	mov	r5, r2
	cmp	r0, #0
	bne	.L182

	ldr	r3, [r2]
	cmp	r3, r4
	bge	.L189

	ldr	r3, [r2, #12]
	lsr	r0, r3, #31
.L184:
	pop	{r4, r5, r6, pc}

.L182:
	ldr	r1, [r2, #12]
	cmp	r1, #0
	bne	.L185

	ldr	r3, [r2, #4]
	cmp	r3, r6
	blt	.L186
	lsr	r0, r0, #31
	pop	{r4, r5, r6, pc}
	
.L185:
	ldr	r3, [r2]
	sub	r2, r4, r3
	
	ldr	r3, [r5, #4]
	sub	r6, r6, r3

	mov	r3, r1
	eor	r3, r3, r0
	eor	r3, r3, r2
	eor	r3, r3, r6
	cmp	r3, #0
	blt	.L190
	asr	r0, r1, #16
	mov	r1, r2
	bl	FixedMul

	ldr	r1, [r5, #8]
	mov	r4, r0

	asr	r1, r1, #16
	mov	r0, r6
	bl	FixedMul
	lsr	r2, r4, #31
	asr	r3, r0, #31
	
	cmp	r0, r4
	adc	r2, r2, r3
	mov	r0, r2

	pop	{r4, r5, r6, pc}

.L189:
	ldr	r2, [r2, #12]

	asr	r3, r2, #31
	sub	r3, r3, r2
	lsr	r0, r3, #31
	pop	{r4, r5, r6, pc}

.L186:
	asr	r3, r0, #31
	sub	r3, r3, r0
	lsr	r0, r3, #31
	pop	{r4, r5, r6, pc}

.L190:
	mov	r3, r2
	eor	r3, r3, r1
	lsr	r0, r3, #31
	pop	{r4, r5, r6, pc}
	
@=====================================
@FixedMul
@fast fixed multiply
@=====================================
FixedMul:
	smull	r2, r3, r0, r1
	
	@shift by FRACBITS
	mov		r1, r2, lsr #16
	mov		r2, r3, lsl #16
	orr		r0, r1, r2

	bx		lr

@=====================================
@FixedDiv
@bounds checking prefrace to FixedDiv2
@=====================================
.pool
FixedDiv:
	mov		r2, r0
	mov		r3, r1
	
	@abs a, b	
	cmp		r2, #0
	rsblt	r2, r2, #0
	cmp		r3, #0
	rsblt	r3, r3, #0

	mov		r2, r2, lsr #14

	@if abs(a)>>14 >= abs(b)
	cmp		r2, r3

	blt		FixedDiv2

	@then return (a^b)<0 ? MININT : MAXINT
	eors	r2, r0, r1
	movpl	r0, #2147483647
	movmi	r0, #2147483648
	
	bx		lr

@=====================================
@FixedDiv2
@fast fixed divide
@=====================================
FixedDiv2:
	@store low 32 bits in r2
	mov		r2, r0
	
	@extend to r3
	ands	r3, r0, #2147483648
	movmi	r3, #-1
	
	@shift over 2 registers	
	mov		r3, r3, lsl #16
	mov		r0, r2, lsr #16
	orr		r3, r3, r0
	mov		r2, r2, asl #16
	
	@set r12 to address of DIV_NUMERATOR64
	ldr		r12, =67109520
	
	@set DIV_CR DIV_64_32 bit
	mov		r0, #1
	strh	r0, [r12, #-16]

	@move expanded a into DIV_NUMERATOR64
	stmia	r12, {r2-r3}

	@move b into DIV_DENOMINATOR32
	str		r1, [r12, #8]

	@hang around until busy bit is clear
.regWaitB:
	ldrh	r0, [r12, #-16]
	ands	r0, r0, #32768
	bne		.regWaitB
	
	@put result from DIV_RESULT32 into r0
	ldr		r0, [r12, #16]

	bx		lr