/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/*******************************************************
 *                                                     *
 *  -------------------------------------------------  *
 *  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  *
 *  -------------------------------------------------  *
 *  |  0  |  4  |  8  |  12 |  16 |  20 |  24 |  28 |  *
 *  -------------------------------------------------  *
 *  |    F14    |    F15    |    F16    |    F17    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  8  |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  *
 *  -------------------------------------------------  *
 *  |  32 |  36 |  40 |  44 |  48 |  52 |  56 |  60 |  *
 *  -------------------------------------------------  *
 *  |    F18    |    F19    |    F20    |    F21    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  16 |  17 |  18 |  19 |  20 |  21 |  22 |  23 |  *
 *  -------------------------------------------------  *
 *  |  64 |  68 |  72 |  76 |  80 |  84 |  88 |  92 |  *
 *  -------------------------------------------------  *
 *  |    F22    |    F23    |    F24    |    F25    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  24 |  25 |  26 |  27 |  28 |  29 |  30 |  31 |  *
 *  -------------------------------------------------  *
 *  |  96 | 100 | 104 | 108 | 112 | 116 | 120 | 124 |  *
 *  -------------------------------------------------  *
 *  |    F26    |    F27    |    F28    |    F29    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  32 |  33 |  34 |  35 |  36 |  37 |  38 |  39 |  *
 *  -------------------------------------------------  *
 *  | 128 | 132 | 136 | 140 | 144 | 148 | 152 | 156 |  *
 *  -------------------------------------------------  *
 *  |    F30    |    F31    |   fpscr   |    TOC    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  40 |  41 |  42 |  43 |  44 |  45 |  46 |  47 |  *
 *  -------------------------------------------------  *
 *  | 160 | 164 | 168 | 172 | 176 | 180 | 184 | 188 |  *
 *  -------------------------------------------------  *
 *  |    R14    |    R15    |     R16   |    R17    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  48 |  49 |  50 |  51 |  52 |  53 |  54 |  55 |  *
 *  -------------------------------------------------  *
 *  | 192 | 196 | 200 | 204 | 208 | 212 | 216 | 220 |  *
 *  -------------------------------------------------  *
 *  |    R18    |    R19    |    R20    |    R21    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  56 |  57 |  58 |  59 |  60 |  61 |  62 |  63 |  *
 *  -------------------------------------------------  *
 *  | 224 | 228 | 232 | 236 | 240 | 244 | 248 | 252 |  *
 *  -------------------------------------------------  *
 *  |    R22    |    R23    |    R24    |    R25    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  64 |  65 |  66 |  67 |  68 |  69 |  70 |  71 |  *
 *  -------------------------------------------------  *
 *  | 256 | 260 | 264 | 268 | 272 | 276 | 280 | 284 |  *
 *  -------------------------------------------------  *
 *  |    R26    |    R27    |    R28    |    R29    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  72 |  73 |  74 |  75 |  76 |  77 |  78 |  79 |  *
 *  -------------------------------------------------  *
 *  | 288 | 292 | 296 | 300 | 304 | 308 | 312 | 316 |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |    R30    |    R31    |     CR    |     LR    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  80 |  81 |                                   |  *
 *  -------------------------------------------------  *
 *  | 320 | 324 |                                   |  *
 *  -------------------------------------------------  *
 *  |    PC     |                                   |  *
 *  -------------------------------------------------  *
 *                                                     *
 *******************************************************/

.globl jump_fcontext_v1
#if _CALL_ELF == 2
	.text
	.align 2
jump_fcontext_v1:
        addis   %r2, %r12, .TOC.-jump_fcontext_v1@ha
        addi    %r2, %r2, .TOC.-jump_fcontext_v1@l
        .localentry jump_fcontext_v1, . - jump_fcontext_v1
#else
	.section ".opd","aw"
	.align 3
jump_fcontext_v1:
# ifdef _CALL_LINUX
        .quad   .L.jump_fcontext_v1,.TOC.@tocbase,0
        .type   jump_fcontext_v1,@function
        .text
        .align 2
.L.jump_fcontext_v1:
# else
        .hidden .jump_fcontext_v1
        .globl  .jump_fcontext_v1
        .quad   .jump_fcontext_v1,.TOC.@tocbase,0
        .size   jump_fcontext_v1,24
        .type   .jump_fcontext_v1,@function
        .text
        .align 2
.jump_fcontext_v1:
# endif
#endif
    # reserve space on stack
    subi  %r1, %r1, 328

#if _CALL_ELF != 2
    std  %r2,  152(%r1)  # save TOC
#endif
    std  %r14, 160(%r1)  # save R14
    std  %r15, 168(%r1)  # save R15
    std  %r16, 176(%r1)  # save R16
    std  %r17, 184(%r1)  # save R17
    std  %r18, 192(%r1)  # save R18
    std  %r19, 200(%r1)  # save R19
    std  %r20, 208(%r1)  # save R20
    std  %r21, 216(%r1)  # save R21
    std  %r22, 224(%r1)  # save R22
    std  %r23, 232(%r1)  # save R23
    std  %r24, 240(%r1)  # save R24
    std  %r25, 248(%r1)  # save R25
    std  %r26, 256(%r1)  # save R26
    std  %r27, 264(%r1)  # save R27
    std  %r28, 272(%r1)  # save R28
    std  %r29, 280(%r1)  # save R29
    std  %r30, 288(%r1)  # save R30
    std  %r31, 296(%r1)  # save R31

    # save CR
    mfcr  %r0
    std  %r0, 304(%r1)
    # save LR
    mflr  %r0
    std  %r0, 312(%r1)
    # save LR as PC
    std  %r0, 320(%r1)

    # test if fpu env should be preserved
    cmpwi  cr7, %r6, 0
    beq  cr7, 1f

    stfd  %f14, 0(%r1)  # save F14
    stfd  %f15, 8(%r1)  # save F15
    stfd  %f16, 16(%r1)  # save F16
    stfd  %f17, 24(%r1)  # save F17
    stfd  %f18, 32(%r1)  # save F18
    stfd  %f19, 40(%r1)  # save F19
    stfd  %f20, 48(%r1)  # save F20
    stfd  %f21, 56(%r1)  # save F21
    stfd  %f22, 64(%r1)  # save F22
    stfd  %f23, 72(%r1)  # save F23
    stfd  %f24, 80(%r1)  # save F24
    stfd  %f25, 88(%r1)  # save F25
    stfd  %f26, 96(%r1)  # save F26
    stfd  %f27, 104(%r1)  # save F27
    stfd  %f28, 112(%r1)  # save F28
    stfd  %f29, 120(%r1)  # save F29
    stfd  %f30, 128(%r1)  # save F30
    stfd  %f31, 136(%r1)  # save F31
    mffs  %f0  # load FPSCR
    stfd  %f0, 144(%r1)  # save FPSCR

1:
    # store RSP (pointing to context-data) in R3
    std  %r1, 0(%r3)

    # restore RSP (pointing to context-data) from R4
    mr  %r1, %r4

    # test if fpu env should be preserved
    cmpwi  cr7, %r6, 0
    beq  cr7, 2f

    lfd  %f14, 0(%r1)  # restore F14
    lfd  %f15, 8(%r1)  # restore F15
    lfd  %f16, 16(%r1)  # restore F16
    lfd  %f17, 24(%r1)  # restore F17
    lfd  %f18, 32(%r1)  # restore F18
    lfd  %f19, 40(%r1)  # restore F19
    lfd  %f20, 48(%r1)  # restore F20
    lfd  %f21, 56(%r1)  # restore F21
    lfd  %f22, 64(%r1)  # restore F22
    lfd  %f23, 72(%r1)  # restore F23
    lfd  %f24, 80(%r1)  # restore F24
    lfd  %f25, 88(%r1)  # restore F25
    lfd  %f26, 96(%r1)  # restore F26
    lfd  %f27, 104(%r1)  # restore F27
    lfd  %f28, 112(%r1)  # restore F28
    lfd  %f29, 120(%r1)  # restore F29
    lfd  %f30, 128(%r1)  # restore F30
    lfd  %f31, 136(%r1)  # restore F31
    lfd  %f0,  144(%r1)  # load FPSCR
    mtfsf  0xff, %f0  # restore FPSCR

2:
#if _CALL_ELF != 2
    ld  %r2,  152(%r1)  # restore TOC
#endif
    ld  %r14, 160(%r1)  # restore R14
    ld  %r15, 168(%r1)  # restore R15
    ld  %r16, 176(%r1)  # restore R16
    ld  %r17, 184(%r1)  # restore R17
    ld  %r18, 192(%r1)  # restore R18
    ld  %r19, 200(%r1)  # restore R19
    ld  %r20, 208(%r1)  # restore R20
    ld  %r21, 216(%r1)  # restore R21
    ld  %r22, 224(%r1)  # restore R22
    ld  %r23, 232(%r1)  # restore R23
    ld  %r24, 240(%r1)  # restore R24
    ld  %r25, 248(%r1)  # restore R25
    ld  %r26, 256(%r1)  # restore R26
    ld  %r27, 264(%r1)  # restore R27
    ld  %r28, 272(%r1)  # restore R28
    ld  %r29, 280(%r1)  # restore R29
    ld  %r30, 288(%r1)  # restore R30
    ld  %r31, 296(%r1)  # restore R31

    # restore CR
    ld  %r0, 304(%r1)
    mtcr  %r0
    # restore LR
    ld  %r0, 312(%r1)
    mtlr  %r0

    # load PC
    ld  %r12, 320(%r1)
    # restore CTR
    mtctr  %r12

    # adjust stack
    addi  %r1, %r1, 328

    # use third arg as return value after jump
    # use third arg as first arg in context function
    mr  %r3, %r5

    # jump to context
    bctr
#if _CALL_ELF == 2
	.size jump_fcontext_v1, .-jump_fcontext_v1
#else
# ifdef _CALL_LINUX
	.size .jump_fcontext_v1, .-.L.jump_fcontext_v1
# else
	.size .jump_fcontext_v1, .-.jump_fcontext_v1
# endif
#endif


#ifndef __NetBSD__
/* Mark that we don't need executable stack.  */
.section .note.GNU-stack,"",%progbits
#endif
