
;@***********************************************
;@ NOTE: 64-bit MUL & DIV have *NOT* been tested
;@***********************************************

;@ Support functions for ARM CPU

	.text
	.align 4
	
	.global READ_LONG         ;@ Read 32-bit data from Amiga memory (used, assembler version is faster)
   .global WRITE_LONG        ;@ Write 32-bit data to Amiga memory (not used, assembler version is slower)
	.global MULL_ASM_FAME     ;@ 64-bit Multiplication
	.global DIVL_ASM_FAME     ;@ 64-bit Division
   .global PFIELD_DOLINE_N4  ;@ drawing.cpp, assembler version
   .global PFIELD_DOLINE_N5  ;@ drawing.cpp, assembler version
   .global PFIELD_DOLINE_N6  ;@ drawing.cpp, assembler version
   .global PFIELD_DOLINE_N7  ;@ drawing.cpp, assembler version
   .global PFIELD_DOLINE_N8  ;@ drawing.cpp, assembler version

	
;@ ----------------------- Support Functions ----------------------

READ_LONG:
         ;@ (
         ;@    r0=u32 addr,
         ;@    r1=u32 *datarw,
         ;@ )
      ;@ NOTE: 24-bit Amiga addressing!
      stmdb sp!,{lr}
      lsl   r0,r0,#8          ;@ addr &= M68K_ADR_MASK (0x00FFFFFF)
      lsr   r0,r0,#8
      mov   r2,r0,lsr #16     ;@ i = (addr & M68K_ADR_MASK) >> M68K_DATASHIFT (16)
      add   r1,r1,r2,lsl #3   ;@ &DataRW[i]
      ldr   r2,[r1]           ;@ DataRW[i].mem_handler
      
      tst   r2,r2             ;@ DataRW[i].mem_handler != 0 ?
      bne   1f
      
      ;@ DataRW[i].mem_handler == 0
      tst   r0,#1             ;@ (addr & 1) != 0 ?
      ldr   r2,[r1,#4]        ;@ DataRW[i].data
      add   r2,r2,r0          ;@ DataRW[i].data + addr
      
      ;@ Even address - unswab data
      ldreq r0,[r2]
      reveq r0,r0             ;@ Reverse whole word
      rev16eq r0,r0           ;@ Reverse both halfwords
      ldmeqia sp!,{pc}        ;@ return r0
      
      ;@ Odd address - unswab data
      ;@ Example (little-endian, as appears in memory):
      ;@    address 0x00000000 : 11EE3322x0 BB449988x0
      ;@    read from 0x00000001 => 0x11223344
      sub   r2,r2,#1          ;@ pdata--
      ldr   r0,[r2]
      ldrb  r1,[r2,#5]
      rev16 r0,r0
      lsr   r0,r0,#8
      orr   r0,r0,r1,lsl #24
      rev   r0,r0
      ldmia sp!,{pc}          ;@ return r0
      
   1:
      ;@ DataRW[i].mem_handler != 0
      ;@ When reading data through handlers, 32-bit data is read as
      ;@ 16 bits from (addr) (high) + 16 bits from (addr + 2) (low).
      stmdb sp!,{r4,r5}
      mov   r4,r2             ;@ Save DataRW[i].mem_handler
      mov   r5,r0             ;@ Save addr
      blx   r4                ;@ r0=DataRW[i].mem_handler(addr)
      mov   r1,r5             ;@ addr + 2
      lsl   r5,r0,#16         ;@ High 16 bits of the result
      add   r0,r1,#2
      blx   r4                ;@ r0=DataRW[i].mem_handler(addr + 2)
      orr   r0,r5,r0          ;@ Complete 32 bits of the result
      ldmia sp!,{r4,r5,pc}    ;@ return r0
   
      
WRITE_LONG:
         ;@ (
         ;@    r0=u32 addr,
         ;@    r1=u32 data
         ;@    r2=u32 *dataww,
         ;@ )
      ;@ NOTE: 24-bit Amiga addressing!
      stmdb sp!,{lr}
      lsl   r0,r0,#8          ;@ addr &= M68K_ADR_MASK (0x00FFFFFF)
      lsr   r0,r0,#8
      mov   r3,r0,lsr #16     ;@ i = (addr & M68K_ADR_MASK) >> M68K_DATASHIFT (16)
      add   r2,r2,r3,lsl #3   ;@ &DataWW[i]
      ldr   r3,[r2]           ;@ DataWW[i].mem_handler
      
      tst   r3,r3             ;@ DataWW[i].mem_handler != 0 ?
      bne   1f
      
      ;@ DataRW[i].mem_handler == 0
      tst   r0,#1             ;@ (addr & 1) != 0 ?
      ldr   r2,[r2,#4]        ;@ DataWW[i].data
      add   r2,r2,r0          ;@ DataWW[i].data + addr
      
      ;@ Even address - swab data
      reveq r1,r1             ;@ Reverse whole word
      rev16eq r1,r1           ;@ Reverse both halfwords
      streq r1,[r2]           ;@ Write result
      ldmeqia sp!,{pc}        ;@ return
      
      ;@ Odd address - swab data
      ;@ Example (little-endian, as appears in memory):
      ;@    address 0x00000000 : FFEEDDCCx0 BBAA9988x0
      ;@    0x11223344 write to address 0x00000001 => 11EE3322x0 BB449988x0
      sub   r2,r2,#1          ;@ pdata--
      ldrb  r0,[r2,#1]
      lsr   r3,r1,#8
      orr   r3,r3,r0,lsl #24
      rev16 r3,r3
      rev   r3,r3
      str   r3,[r2]           ;@ Write swabbed word without lowest byte
      strb  r1,[r2,#5]        ;@ Write lowest byte
      ldmia sp!,{pc}          ;@ return
      
   1:
      ;@ DataWW[i].mem_handler != 0
      ;@ When write data through handlers, 32-bit data is written as
      ;@ 16 bits from (addr) (high) + 16 bits from (addr + 2) (low).
      stmdb sp!,{r4,r5,r6}
      mov   r4,r3             ;@ Save DataWW[i].mem_handler
      mov   r5,r0             ;@ Save addr
      mov   r6,r1,lsl #16     ;@ Save data low 16 bits
      lsr   r1,r1,#16         ;@ data high 16 bits
      blx   r4                ;@ DataWW[i].mem_handler(addr,data>>16)
      add   r0,r5,#2          ;@ addr + 2
      mov   r1,r6,lsr #16
      blx   r4                ;@ r0=DataWW[i].mem_handler(addr + 2,data)
      ldmia sp!,{r4,r5,r6,pc} ;@ return
   

MULL_ASM_FAME:
         ;@ (
         ;@    r0=u32 src_1st_val,
         ;@    r1=u32 extra_word,
         ;@    r2=u32 *fame_dregs,
         ;@    r3=u32 *fame_flags,
         ;@ )
      stmdb sp!,{r4,r5,lr}

      ;@ Signed/Unsigned MUL?
      tst   r1,#0x0800
      
      mov   r5,r1                ;@ Extra word
      mov   r5,r5,lsr #12        ;@ r5=&Dl
      ldr   r5,[r2,r5,lsl #2]    ;@ r5=2nd source operand value (Dl)
      
      smullne r4,r5,r0,r5        ;@ Signed 64-bit MUL (r4=Dl,r5=Dh)
      umulleq r4,r5,r0,r5        ;@ Unsigned 64-bit MUL (r4=Dl,r5=Dh)
      
      ;@ 64-bit MUL (32x32->64) ?
      tst   r1,#0x0400
      
      ;@ Store low 32 bits of result in FAME dreg
      mov   r0,r1,lsr #12
      str   r4,[r2,r0,lsl #2]    ;@ Dl: DREG(res >> 12) = (u32)x64result
      mov   r0,#0                ;@ Clear flags
      
      bne   MULL64
      
   MULL32:
      ;@ Update CZVN flags
      ;@ Overflow if high 32 bits are not sign extension of low 32 bits
      asr   r1,r4,#31            ;@ Extend sign of low 32 bits to the whole register
      str   r0,[r3]              ;@ Clear flag_C
      cmp   r1,r5                ;@ Compare to high 32 bits
      movne r0,#0x0080           ;@ Overflow if not equal
      str   r1,[r3,#12]          ;@ Update flag_N
      str   r5,[r3,#8]           ;@ Update flag_NotZ
      str   r0,[r3,#4]           ;@ Update flag_V
      
      ldmia sp!,{r4,r5,pc}
      
   MULL64:
      ;@ Update CZV flags
      str   r0,[r3]              ;@ Clear flag_C
      orr   r4,r4,r5             ;@ flag_NotZ = any bit set
      str   r0,[r3,#4]           ;@ Clear flag_V
      str   r4,[r3,#8]
      
      ;@ Update N flag
      mov   r0,r5,lsr #24        ;@ MSB of the high 32 bits of result
      str   r0,[r3,#12]
      
      ;@ Store high result in FAME dreg
      and   r1,r1,#7             ;@ *interleaved* above
      str   r5,[r2,r1,lsl #2]    ;@ Dh: DREG(res & 7) = (u32)(x64result >> 32)
      
      ldmia sp!,{r4,r5,pc}
   
      
;@ 32-bit and 64-bit code separated, signed and unsigned code separated,
;@ resulting in less branches (smaller chance of branch misprediction).
DIVL_ASM_FAME:
         ;@ (
         ;@    r0=u32 src_val (divisor),
         ;@    r1=u32 extra_word,
         ;@    r2=u32 *fame_dregs,
         ;@    r3=u32 *fame_flags,
         ;@ )
      stmdb sp!,{r4-r8,r10,r11,lr}

      ;@ 64-bit division (64/32->32) ?
      ;@ *Much* higher probability of 32-bit division than of 64-bit one
      tst   r1,#0x0400
      bne   DIVL64
      
   DIVL32:
      ;@ 32/32->32
      movs  r10,r1,lsr #12       ;@ 1. Keep &Dl for later storing of result
                                 ;@ 2. Signed/Unsigned DIVL ? (tst 0x0800 and prepare for flags below)
      ldr   r5,[r2,r10,lsl #2]   ;@ r5=Dl
      mov   r4,#0
      mov   r7,r0
      bcs   DIVSL32
   
   DIVUL32:
      ;@ Unsigned DIVL, divide r5 by r0
      mov   r11,r1               ;@ Keep &Dh for later storing of result
      and   r11,r11,#7           ;@ r11=&Dh (&Dr)

   1:
      ;@ Shift up divisor till it's just less than numerator
      cmp   r7,r5,lsr #1
      movls r7,r7,lsl #1
      bcc   1b
      
   2:
      cmp   r5,r7
      subhs r5,r5,r7
      adc   r4,r4,r4             ;@ Double r4 and add 1 if carry set
      teq   r7,r0
      movne r7,r7,lsr #1
      bne   2b
      ;@r4==quotient,r5==remainder
   
      ;@ Store result in FAME dregs
      str   r5,[r2,r11,lsl #2]   ;@ Remainder (Dr)
      str   r4,[r2,r10,lsl #2]   ;@ Quotient (Dq)
      
      ;@ Update FAME flags
      str   r10,[r3]             ;@ Clear flag_C, always (r1 bit #8 is certainly cleared)
      str   r10,[r3,#4]          ;@ Clear flag_V, overflow not possible
      str   r4,[r3,#8]           ;@ Update flag_NotZ
      str   r4,[r3,#12]          ;@ Update flag_N
      
      ldmia sp!,{r4-r8,r10,r11,pc}

   DIVSL32:
      ;@ Signed DIVL
      
      ;@ Remember dividend and divisor (for result sign checks below),
      ;@ and make dividend and divisor positive
      movs  r8,r0                ;@ Sign of divisor
      mov   r11,r1               ;@ Keep &Dh for later storing of result
      rsbmi r0,r0,#0             ;@ Make divisor positive
      
      movs  r6,r5
      and   r11,r11,#7           ;@ r11=&Dh (&Dr)
      rsbmi r5,r5,#0             ;@ Make dividend positive

      ;@ Divide positive r5 by positive r0
   1:
      ;@ Shift up divisor till it's just less than numerator
      cmp   r7,r5,lsr #1
      movls r7,r7,lsl #1
      bls   1b
      
   2:
      cmp   r5,r7
      subhs r5,r5,r7
      adc   r4,r4,r4             ;@ Double r4 and add 1 if carry set
      teq   r7,r0
      movne r7,r7,lsr #1
      bne   2b
      ;@r4==quotient,r5==remainder
   
   DIVSL32_SIGN:
      ;@ Update FAME flags
      tst   r4,r4                ;@ Signed overflow (qutient is negative) ?
      str   r1,[r3]              ;@ Clear flag_C, always (r1 bit #8 is certainly cleared)
      orrmi r1,#0x0080
;@      str   r1,[r3,#4]           ;@ *interleaved* below, Update flag_V
      strpl r4,[r3,#8]           ;@ Update flag_NotZ if no overflow
      strpl r4,[r3,#12]          ;@ Update flag_N if no overflow
      
      ;@ Correct signs of quotient and remainder
      ;@ Store result in FAME dregs
      tst   r6,r6                ;@ Negative remainder if dividend was negative
      str   r1,[r3,#4]           ;@ *interleaved* from above, Update flag_V
      rsbmi r5,r5,#0             ;@ Negative remainder if dividend was negative
      eors  r6,r6,r8             ;@ Different signs od dividend and divisor ?
      str   r5,[r2,r11,lsl #2]   ;@ Remainder (Dr)
      rsbmi r4,r4,#0             ;@ Negative quotient
      str   r4,[r2,r10,lsl #2]   ;@ Quotient (Dq)
      
      ldmia sp!,{r4-r8,r10,r11,pc}
      
   DIVL64:
      ;@ 64/32->32
      tst   r1,#0x0800           ;@ Signed/Unsigned DIVL?
      mov   r10,r1,lsr #12       ;@ Keep &Dl for later storing of result
      ldr   r5,[r2,r10,lsl #2]   ;@ r5=Dividend low 32 bits
      mov   r7,r0
      mov   r8,#0
      ldr   r6,[r2,r11,lsl #2]   ;@ r6=Dh, dividend high 32 bits
      bne   DIVSL64
   
   DIVUL64:
      ;@ Unsigned DIVL, divide r6-r5 by r0.
      ;@ r8-r7 hold 64 bits used in calculation.
   1:
      ;@ Shift up divisor till it's just less than numerator
      cmp   r8,r6,lsr #1         ;@ Compare high 32 bits
      cmpls r7,r5,lsr #1         ;@ If lower or same, compare low 32 bits
      bhi   2f
      mov   r8,r8,lsl #1         ;@ Shift up high 32 bits
      movs  r7,r7,lsl #1         ;@ Shift up low 32 bits, highest bit goes to carry 
      adc   r8,r8,#0             ;@ Shift carry into r8
      b     1b
      
   2:
      cmp   r6,r8
      cmphi r5,r7
      blo   3f
      subs  r5,r5,r7             ;@ Substract low 32 bits
      sbc   r6,r6,r8             ;@ Substract with carry high 32 bits
      lsls  r4,r4,#1             ;@ Double r4...
      addccs r4,r4,#1            ;@ ... and add 1 if no carry after doubling
                                 ;@ ( previous carry was set by 'cmp r6,r8')
      bcs   4f                   ;@ Unsigned overflow (quotient is larger than 32 bits) ?
                                 ;@ Real 68020 probably finishes computation,
                                 ;@ but overflow makes any result invalid
    3:
      tst   r8,#0                ;@ r7 is compared to r0 only if r8==0
      teqeq r7,r0
      beq   4f
      movs  r8,r8,lsr #1         ;@ Shift down r8 (high 32 bits)
      mov   r7,r7,rrx            ;@ Shift down r7, with carry from r8
      b     2b
      ;@r4==quotient,r5==remainder

   4:
      ;@ If unsigned overflow occured, loop 2 was aborted, and code jumped here.
      ;@ Also a regular exit from loop 2, with carry clear.
      movcs r8,#0x0080           ;@ flag_V
      movcc r8,#0
      
      ;@ Update FAME flags
      strcc r4,[r3,#8]           ;@ Update flag_NotZ if no overflow
      strcc r4,[r3,#12]          ;@ Update flag_N if no overflow
      str   r8,[r3]              ;@ Clear flag_C (r8 bit #8 is certainly cleared)
      str   r8,[r3,#4]           ;@ Update flag_V
      
      ;@ Store result in FAME dregs
      str   r5,[r2,r11,lsl #2]   ;@ Remainder (Dr)
      str   r4,[r2,r10,lsl #2]   ;@ Quotient (Dq)
      
      ldmia sp!,{r4-r8,r10,r11,pc}

   DIVSL64:
      ;@ Signed DIVL
      
      ;@ Remember signs of dividend and divisor,
      ;@ and make dividend and divisor positive
      tst   r0,r0
         ;@ r1 bits:
         ;@    #0 overflow,
         ;@    #1 dividend sign,
         ;@    #2 divisor sign
      mov   r1,r0,lsr #31        ;@ Sign of divisor
      lsl   r1,r1,#1
      rsbmi r0,r0,#0             ;@ Make divisor positive

      tst   r6,r6
      orr   r1,r6,lsr #31        ;@ Sign of dividend
      lsl   r1,r1,#1
      rsbmi r5,r5,#0             ;@ Make dividend positive, low 32 bits
      sbcmi r6,r6,#0             ;@ Make dividend positive, high 32 bits
      
      ;@ Divide positive r6-r5 by positive r0.
      ;@ r8-r7 hold 64 bits used in calculation.
   1:
      ;@ Shift up divisor till it's just less than numerator
      cmp   r8,r6,lsr #1         ;@ Compare high 32 bits
      cmpls r7,r5,lsr #1         ;@ If lower or same, compare low 32 bits
      bhi   2f
      mov   r8,r8,lsl #1         ;@ Shift up high 32 bits
      movs  r7,r7,lsl #1         ;@ Shift up low 32 bits, highest bit goes to carry 
      adc   r8,r8,#0             ;@ Shift carry into r8
      b     1b
      
   2:
      cmp   r6,r8
      cmphi r5,r7
      blo   3f
      subs  r5,r5,r7             ;@ Substract low 32 bits
      sbcs  r6,r6,r8             ;@ Substract with carry high 32 bits
      adcs  r4,r4,r4             ;@ Double r4 and add 1 if carry set
      bcs   4f                   ;@ Unsigned overflow (quotient is larger than 32 bits) ?
    3:
      tst   r8,#0                ;@ r7 is compared to r0 only if r8==0
      teqeq r7,r0
      beq   4f
      movs  r8,r8,lsr #1         ;@ Shift down r8 (high 32 bits)
      mov   r7,r7,rrx            ;@ Shift down r7, with carry from r8
      b     2b
      ;@r4==quotient,r5==remainder

   4:
      ;@ If unsigned overflow occured, loop 2 was aborted, and code jumped here.
      ;@ Also a regular exit from loop 2, with carry clear.
      movcs r8,#0x0080           ;@ flag_V
      movcc r8,#0
   
   DIVSL64_SIGN:
      ;@ Update FAME flags
      tst   r4,r4                ;@ Signed overflow (quotient is negative) ?
      str   r8,[r3]              ;@ Clear flag_C, always (r8 bit #8 is certainly cleared)
      movmi r8,#0x0080           ;@ Signed overflow (quotient is negative) ?
      cmp   r8,#0                ;@ No overflow ?
      str   r8,[r3,#4]           ;@ Update flag_V
      streq r4,[r3,#8]           ;@ Update flag_NotZ if no overflow
      streq r4,[r3,#12]          ;@ Update flag_N if no overflow
      
      ;@ Correct signs of quotient and remainder
      mov   r7,r1,lsr #1         ;@ *interleaved* above, Different signs od dividend and divisor ?
      and   r7,r7,#1             ;@ *interleaved* above
      eors  r7,r7,r1,lsr #2
      rsbne r4,r4,#0             ;@ Negative quotient
      tst   r1,#2
      str   r4,[r2,r10,lsl #2]   ;@ Quotient (Dq)
      rsbne r5,r5,#0             ;@ Negative remainder if dividend was negative
      
      ;@ Store result in FAME dregs
      str   r5,[r2,r11,lsl #2]   ;@ Remainder (Dr)
      
      ldmia sp!,{r4-r8,r10,r11,pc}


;@ NOTE: See PFIELD_DOLINE_N6 for explanations
PFIELD_DOLINE_N4:
         ;@ (
         ;@    r0=uae_u32 *pixels,
         ;@    r1=int wordcount,
         ;@    r2=uae_u8 *line_data[lineno],
         ;@    r3=uae_u32 max_words
         ;@ )
      stmdb sp!,{r4-r8,r10-r12,lr}
      
      add   r10,r2,r3            ;@ b4 <=> DATA_POINTER (3)
      add   r10,r10,r3,lsl #1
      add   r11,r2,r3,lsl #1     ;@ b5 <=> DATA_POINTER (2)
      add   r12,r2,r3            ;@ b6 <=> DATA_POINTER (1)
      mov   lr,r2                ;@ b7 <=> DATA_POINTER (0)
      
      ;@ Pointer to the buffer
      ldr   r2,=DoLine_Data
      
      ;@ Store &pixels, wordcount, and original SP
      add   r3,r2,#8*4
      stmia r3,{r0,r1,sp}
      
      ;@ Adjust initial pointers by -4, so that loop can use
      ;@ pre-indexed mode with register update.
      sub   r10,r10,#4
      sub   r11,r11,#4
      sub   r12,r12,#4
      sub   lr,lr,#4
      
      ;@ Use SP as pointer to bitplane pointers in the buffer
      mov   sp,r2
      
   LINE_N4_LOOP:                 ;@ do { // initial wordcount should always be > 0
      ;@ Get bx and advance bitmap pointers to the next words.
      
      ldr   r1,[r10,#4]!
      ldr   r3,[r11,#4]!
      ldr   r5,[r12,#4]!
      ldr   r7,[lr,#4]!
      
      ;@ Store new bitplane pointers over the old values
      stmia sp,{r10-r12,lr}
      
      ;@ 1st bitmask (0x55555555)
      ldr   r0,[sp,#(11+0)*4]

      ;@ MERGE (b4(r1), b5(r3), 0x55555555, 1);
      eor   r8,r1,r3,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r0,r8             ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x55555555, 1);
      eor   r8,r5,r7,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r0,r8             ;@ tmp = mask & tmp
      ;@
      ;@ 2nd bitmask (0x33333333)
      ldr   r10,[sp,#(11+1)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #1      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b4(r1), b6(r5), 0x33333333, 2);
      eor   r8,r1,r5,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE (b5(r3), b7(r7), 0x33333333, 2);
      eor   r8,r3,r7,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 3rd bitmask (0x0f0f0f0f)
      ldr   r10,[sp,#(11+2)*4]
      ;@
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #2      ;@ b ^= (tmp << shift)
      
      ;@ MERGE_0(b0(r0), b4(r1), 0x0f0f0f0f, 1);
      and   r0,r10,r1,lsr #4     ;@ a = tmp = mask & (b>>shift);
      eor   r1,r1,r0,lsl #4      ;@ b ^= (tmp << shift)      
      ;@ MERGE_0(b1(r2), b5(r3), 0x0f0f0f0f, 1);
      and   r2,r10,r3,lsr #4     ;@ a = tmp = mask & (b>>shift);
      eor   r3,r3,r2,lsl #4      ;@ b ^= (tmp << shift)      
      ;@ MERGE_0(b2(r4), b6(r5), 0x0f0f0f0f, 1);
      and   r4,r10,r5,lsr #4     ;@ a = tmp = mask & (b>>shift);
      eor   r5,r5,r4,lsl #4      ;@ b ^= (tmp << shift)      
      ;@ MERGE_0(b3(r6), b7(r7), 0x0f0f0f0f, 1);
      and   r6,r10,r7,lsr #4     ;@ a = tmp = mask & (b>>shift);
      eor   r7,r7,r6,lsl #4      ;@ b ^= (tmp << shift)      
      
      ;@ 4th bitmask (0x00ff00ff)
      ldr   r10,[sp,#(11+3)*4]
      
      ;@ MERGE (b0(r0), b1(r2), 0x00ff00ff, 8);
      eor   r8,r0,r2,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r2,r2,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b3(r6), 0x00ff00ff, 8);
      eor   r8,r4,r6,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b5(r3), 0x00ff00ff, 8);
      eor   r8,r1,r3,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x00ff00ff, 8);
      eor   r8,r5,r7,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 5th bitmask (0x0000ffff)
      ldr   r10,[sp,#(11+4)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #8      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b2(r4), 0x0000ffff, 16);
      eor   r8,r0,r4,lsr #16     ;@ tmp = (a ^ (b >> shift))
      ;@
      ;@ Get &pixels from the buffer
      ldr   r11,[sp,#8*4]
      ;@
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r4,r4,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ Get wordcount from the buffer
      ldr   r12,[sp,#9*4]
      
      ;@ MERGE (b1(r2), b3(r6), 0x0000ffff, 16);
      eor   r8,r2,r6,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ wordcount--
      subs  r12,r12,#1
      
      ;@ MERGE (b4(r1), b6(r5), 0x0000ffff, 16);
      eor   r8,r1,r5,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ pixels += 8
      ;@ Keep old &pixels for storing result below
      addgt lr,r11,#8*4         
      strgt lr,[sp,#8*4]         ;@ Store &pixels
      
      ;@ MERGE (b5(r3), b7(r7), 0x0000ffff, 16);
      eor   r8,r3,r7,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      strgt r12,[sp,#9*4]        ;@ Store wordcount
      
      rev   r0,r0                ;@ Reverse byte order
      rev   r1,r1
      rev   r2,r2
      rev   r3,r3
      rev   r4,r4
      rev   r5,r5
      rev   r6,r6
      rev   r7,r7
      stmia r11,{r0-r7}          ;@ Store bx
      
      ;@ Get bitplane pointers for next iteration.
      ldmgtia sp,{r10-r12,lr}
      
      bgt   LINE_N4_LOOP         ;@ } while (wordcount > 0)
      
      ;@ Restore original SP
      ldr   sp,[sp,#10*4]
      
      ldmia sp!,{r4-r8,r10-r12,pc}
      

;@ NOTE: See PFIELD_DOLINE_N6 for explanations
PFIELD_DOLINE_N5:
         ;@ (
         ;@    r0=uae_u32 *pixels,
         ;@    r1=int wordcount,
         ;@    r2=uae_u8 *line_data[lineno],
         ;@    r3=uae_u32 max_words
         ;@ )
      stmdb sp!,{r4-r8,r10-r12,lr}
      
      add   r8,r2,r3,lsl #2      ;@ b3 <=> DATA_POINTER (4)
      add   r10,r2,r3            ;@ b4 <=> DATA_POINTER (3)
      add   r10,r10,r3,lsl #1
      add   r11,r2,r3,lsl #1     ;@ b5 <=> DATA_POINTER (2)
      add   r12,r2,r3            ;@ b6 <=> DATA_POINTER (1)
      mov   lr,r2                ;@ b7 <=> DATA_POINTER (0)
      
      ;@ Pointer to the buffer
      ldr   r2,=DoLine_Data
      
      ;@ Store &pixels, wordcount, and original SP
      add   r3,r2,#8*4
      stmia r3,{r0,r1,sp}
      
      ;@ Adjust initial pointers by -4, so that loop can use
      ;@ pre-indexed mode with register update.
      sub   r8,r8,#4
      sub   r10,r10,#4
      sub   r11,r11,#4
      sub   r12,r12,#4
      sub   lr,lr,#4
      
      ;@ Use SP as pointer to bitplane pointers in the buffer
      mov   sp,r2
      
   LINE_N5_LOOP:                 ;@ do { // initial wordcount should always be > 0
      ;@ Get bx and advance bitmap pointers to the next words.
      
      ldr   r6,[r8,#4]!
      ldr   r1,[r10,#4]!
      ldr   r3,[r11,#4]!
      ldr   r5,[r12,#4]!
      ldr   r7,[lr,#4]!
      
      ;@ Store new bitplane pointers over the old values
      stmia sp,{r8,r10-r12,lr}
      
      ;@ 1st bitmask (0x55555555)
      ldr   r0,[sp,#(11+0)*4]

      ;@ MERGE (b4(r1), b5(r3), 0x55555555, 1);
      eor   r8,r1,r3,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r0,r8             ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE_0(b2(r4), b3(r6), 0x55555555, 1);
      and   r4,r0,r6,lsr #1      ;@ a = tmp = mask & (b>>shift);
      eor   r6,r6,r4,lsl #1      ;@ b ^= (tmp << shift)      
      ;@ MERGE (b6(r5), b7(r7), 0x55555555, 1);
      eor   r8,r5,r7,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r0,r8             ;@ tmp = mask & tmp
      ;@
      ;@ 2nd bitmask (0x33333333)
      ldr   r10,[sp,#(11+1)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #1      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b4(r1), b6(r5), 0x33333333, 2);
      eor   r8,r1,r5,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE_0(b0(r0), b2(r4), 0x33333333, 2);
      and   r0,r10,r4,lsr #2     ;@ a = tmp = mask & (b>>shift);
      eor   r4,r4,r0,lsl #2      ;@ b ^= (tmp << shift)      
      ;@ MERGE_0(b1(r2), b3(r6), 0x33333333, 2);
      and   r2,r10,r6,lsr #2     ;@ a = tmp = mask & (b>>shift);
      eor   r6,r6,r2,lsl #2      ;@ b ^= (tmp << shift)       
      ;@ MERGE (b5(r3), b7(r7), 0x33333333, 2);
      eor   r8,r3,r7,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 3rd bitmask (0x0f0f0f0f)
      ldr   r10,[sp,#(11+2)*4]
      ;@
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #2      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b4(r1), 0x0f0f0f0f, 4);
      eor   r8,r0,r1,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r1,r1,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b1(r2), b5(r3), 0x0f0f0f0f, 4);
      eor   r8,r2,r3,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b6(r5), 0x0f0f0f0f, 4);
      eor   r8,r4,r5,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b3(r6), b7(r7), 0x0f0f0f0f, 4);
      eor   r8,r6,r7,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 4th bitmask (0x00ff00ff)
      ldr   r10,[sp,#(11+3)*4]
      ;@
      eor   r6,r6,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #4      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b1(r2), 0x00ff00ff, 8);
      eor   r8,r0,r2,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r2,r2,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b3(r6), 0x00ff00ff, 8);
      eor   r8,r4,r6,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b5(r3), 0x00ff00ff, 8);
      eor   r8,r1,r3,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x00ff00ff, 8);
      eor   r8,r5,r7,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 5th bitmask (0x0000ffff)
      ldr   r10,[sp,#(11+4)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #8      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b2(r4), 0x0000ffff, 16);
      eor   r8,r0,r4,lsr #16     ;@ tmp = (a ^ (b >> shift))
      ;@
      ;@ Get &pixels from the buffer
      ldr   r11,[sp,#8*4]
      ;@
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r4,r4,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ Get wordcount from the buffer
      ldr   r12,[sp,#9*4]
      
      ;@ MERGE (b1(r2), b3(r6), 0x0000ffff, 16);
      eor   r8,r2,r6,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ wordcount--
      subs  r12,r12,#1
      
      ;@ MERGE (b4(r1), b6(r5), 0x0000ffff, 16);
      eor   r8,r1,r5,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ pixels += 8
      ;@ Keep old &pixels for storing result below
      addgt lr,r11,#8*4         
      strgt lr,[sp,#8*4]         ;@ Store &pixels
      
      ;@ MERGE (b5(r3), b7(r7), 0x0000ffff, 16);
      eor   r8,r3,r7,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      strgt r12,[sp,#9*4]        ;@ Store wordcount
      
      rev   r0,r0                ;@ Reverse byte order
      rev   r1,r1
      rev   r2,r2
      rev   r3,r3
      rev   r4,r4
      rev   r5,r5
      rev   r6,r6
      rev   r7,r7
      stmia r11,{r0-r7}          ;@ Store bx
      
      ;@ Get bitplane pointers for next iteration.
      ldmgtia sp,{r8,r10-r12,lr}
      
      bgt   LINE_N5_LOOP         ;@ } while (wordcount > 0)
      
      ;@ Restore original SP
      ldr   sp,[sp,#10*4]
      
      ldmia sp!,{r4-r8,r10-r12,pc}
      

PFIELD_DOLINE_N6:
         ;@ (
         ;@    r0=uae_u32 *pixels,
         ;@    r1=int wordcount,
         ;@    r2=uae_u8 *line_data[lineno],
         ;@    r3=uae_u32 max_words
         ;@ )
      ;@ NOTE: ry have been selected for bx so that data doesn't have to be
      ;@       reshuffled before the "stmia rn,{rx-ry}" instruction at the
      ;@       end of the loop.
      ;@
      ;@ bx <=> ry:
      ;@    b0 <=> r0
      ;@    b1 <=> r2
      ;@    b2 <=> r4
      ;@    b3 <=> r6
      ;@    b4 <=> r1
      ;@    b5 <=> r3
      ;@    b6 <=> r5
      ;@    b7 <=> r7
      stmdb sp!,{r4-r8,r10-r12,lr}
      
      ;@ Bitplane pointers are kept in the buffer. They are PEEK-ed
      ;@ at the end of each iteration.
      
      ;@ Get bitplane pointers in temporary registers
      ;@ (registers unused before the computation of bx),
      ;@ so that bx can be LDR-ed directly into appropriate ry
      ;@ registers inside the loop.
      ;@
      mov   r4,r0                ;@ Save r0 before using it
      add   r0,r2,r3             ;@ b2 <=> DATA_POINTER (5)
      add   r0,r0,r3,lsl #2
      add   r8,r2,r3,lsl #2      ;@ b3 <=> DATA_POINTER (4)
      add   r10,r2,r3            ;@ b4 <=> DATA_POINTER (3)
      add   r10,r10,r3,lsl #1
      add   r11,r2,r3,lsl #1     ;@ b5 <=> DATA_POINTER (2)
      add   r12,r2,r3            ;@ b6 <=> DATA_POINTER (1)
      mov   lr,r2                ;@ b7 <=> DATA_POINTER (0)
      
      ;@ Pointer to the buffer
      ldr   r2,=DoLine_Data
      
      ;@ Store wordcount, &pixels, and original SP
      add   r3,r2,#8*4
      stmia r3,{r1,r4,sp}        ;@ NOTE: different order from other PFIELD_DOLINE_N functions !
      
      ;@ Adjust initial pointers by -4, so that loop can use
      ;@ pre-indexed mode with register update.
      sub   r0,r0,#4
      sub   r8,r8,#4
      sub   r10,r10,#4
      sub   r11,r11,#4
      sub   r12,r12,#4
      sub   lr,lr,#4
      
      ;@ Use SP as pointer to bitplane pointers in the buffer
      mov   sp,r2
      
   LINE_N6_LOOP:                 ;@ do { // initial wordcount should always be > 0
      ;@ Get bx and advance bitmap pointers to the next words.
      
      ;@ b2(r4) = GETLONG ((uae_u32 *)real_bplpt[5]); real_bplpt[5] += 4;
      ;@ b3(r6) = GETLONG ((uae_u32 *)real_bplpt[4]); real_bplpt[4] += 4;
      ;@ b4(r1) = GETLONG ((uae_u32 *)real_bplpt[3]); real_bplpt[3] += 4;
      ;@ b5(r3) = GETLONG ((uae_u32 *)real_bplpt[2]); real_bplpt[2] += 4;
      ;@ b6(r5) = GETLONG ((uae_u32 *)real_bplpt[1]); real_bplpt[1] += 4;
      ;@ b7(r7) = GETLONG ((uae_u32 *)real_bplpt[0]); real_bplpt[0] += 4;
      ldr   r4,[r0,#4]!
      ldr   r6,[r8,#4]!
      ldr   r1,[r10,#4]!
      ldr   r3,[r11,#4]!
      ldr   r5,[r12,#4]!
      ldr   r7,[lr,#4]!
      
      ;@ Store new bitplane pointers over the old values
      stmia sp,{r0,r8,r10-r12,lr}
      
      ;@ 1st bitmask (0x55555555)
      ldr   r10,[sp,#(11+0)*4]

      ;@ MERGE (b2(r4), b3(r6), 0x55555555, 1);  // MERGE(a,b,mask,shift)
      eor   r8,r4,r6,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b5(r3), 0x55555555, 1);
      eor   r8,r1,r3,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x55555555, 1);
      eor   r8,r5,r7,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 2nd bitmask (0x33333333)
      ldr   r10,[sp,#(11+1)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #1      ;@ b ^= (tmp << shift)
      
      ;@ MERGE_0(b0(r0), b2(r4), 0x33333333, 2);
      and   r0,r10,r4,lsr #2     ;@ a = tmp = mask & (b>>shift);
      eor   r4,r4,r0,lsl #2      ;@ b ^= (tmp << shift)      
      ;@ MERGE_0(b1(r2), b3(r6), 0x33333333, 2);
      and   r2,r10,r6,lsr #2     ;@ a = tmp = mask & (b>>shift);
      eor   r6,r6,r2,lsl #2      ;@ b ^= (tmp << shift)       
      ;@ MERGE (b4(r1), b6(r5), 0x33333333, 2);
      eor   r8,r1,r5,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE (b5(r3), b7(r7), 0x33333333, 2);
      eor   r8,r3,r7,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 3rd bitmask (0x0f0f0f0f)
      ldr   r10,[sp,#(11+2)*4]
      ;@
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #2      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b4(r1), 0x0f0f0f0f, 4);
      eor   r8,r0,r1,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r1,r1,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b1(r2), b5(r3), 0x0f0f0f0f, 4);
      eor   r8,r2,r3,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b6(r5), 0x0f0f0f0f, 4);
      eor   r8,r4,r5,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b3(r6), b7(r7), 0x0f0f0f0f, 4);
      eor   r8,r6,r7,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 4th bitmask (0x00ff00ff)
      ldr   r10,[sp,#(11+3)*4]
      ;@
      eor   r6,r6,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #4      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b1(r2), 0x00ff00ff, 8);
      eor   r8,r0,r2,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r2,r2,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b3(r6), 0x00ff00ff, 8);
      eor   r8,r4,r6,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b5(r3), 0x00ff00ff, 8);
      eor   r8,r1,r3,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x00ff00ff, 8);
      eor   r8,r5,r7,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 5th bitmask (0x0000ffff)
      ldr   r10,[sp,#(11+4)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #8      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b2(r4), 0x0000ffff, 16);
      eor   r8,r0,r4,lsr #16     ;@ tmp = (a ^ (b >> shift))
      ;@
      ;@ Get &pixels from the buffer
      ldr   r11,[sp,#9*4]
      ;@
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r4,r4,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ Get wordcount from the buffer
      ldr   r12,[sp,#8*4]
      
      ;@ MERGE (b1(r2), b3(r6), 0x0000ffff, 16);
      eor   r8,r2,r6,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ wordcount--
      subs  r12,r12,#1
      
      ;@ MERGE (b4(r1), b6(r5), 0x0000ffff, 16);
      eor   r8,r1,r5,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ pixels += 8
      ;@ Keep old &pixels for storing result below
      addgt lr,r11,#8*4         
      strgt lr,[sp,#9*4]         ;@ Store &pixels
      
      ;@ MERGE (b5(r3), b7(r7), 0x0000ffff, 16);
      eor   r8,r3,r7,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      strgt r12,[sp,#8*4]        ;@ Store wordcount
      
      ;@ DO_SWLONG(pixels, b0);
      ;@ DO_SWLONG(pixels + 1, b4);
      ;@ DO_SWLONG(pixels + 2, b1);
      ;@ DO_SWLONG(pixels + 3, b5);
      ;@ DO_SWLONG(pixels + 4, b2);
      ;@ DO_SWLONG(pixels + 5, b6);
      ;@ DO_SWLONG(pixels + 6, b3);
      ;@ DO_SWLONG(pixels + 7, b7);
      rev   r0,r0                ;@ Reverse byte order
      rev   r1,r1
      rev   r2,r2
      rev   r3,r3
      rev   r4,r4
      rev   r5,r5
      rev   r6,r6
      rev   r7,r7
      stmia r11,{r0-r7}          ;@ Store bx
      
      ;@ Get bitplane pointers for next iteration.
      ldmgtia sp,{r0,r8,r10-r12,lr}
      
      bgt   LINE_N6_LOOP         ;@ } while (wordcount > 0)
      
      ;@ Restore original SP
      ldr   sp,[sp,#10*4]
      
      ldmia sp!,{r4-r8,r10-r12,pc}

     
;@ NOTE: See PFIELD_DOLINE_N6 for explanations
PFIELD_DOLINE_N7:
         ;@ (
         ;@    r0=uae_u32 *pixels,
         ;@    r1=int wordcount,
         ;@    r2=uae_u8 *line_data[lineno],
         ;@    r3=uae_u32 max_words
         ;@ )
      stmdb sp!,{r4-r8,r10-r12,lr}
      
      mov   r4,r0                ;@ Save r0 before using it
      mov   r5,r1                ;@ Save r1 before using it
      
      add   r1,r2,r3,lsl #2      ;@ b1 <=> DATA_POINTER (6)
      add   r1,r1,r3,lsl #1
      
      add   r0,r2,r3             ;@ b2 <=> DATA_POINTER (5)
      add   r0,r0,r3,lsl #2
      add   r8,r2,r3,lsl #2      ;@ b3 <=> DATA_POINTER (4)
      add   r10,r2,r3            ;@ b4 <=> DATA_POINTER (3)
      add   r10,r10,r3,lsl #1
      add   r11,r2,r3,lsl #1     ;@ b5 <=> DATA_POINTER (2)
      add   r12,r2,r3            ;@ b6 <=> DATA_POINTER (1)
      mov   lr,r2                ;@ b7 <=> DATA_POINTER (0)
      
      ;@ Pointer to the buffer
      ldr   r2,=DoLine_Data
      
      ;@ Store &pixels, wordcount, and original SP
      add   r3,r2,#8*4
      stmia r3,{r4,r5,sp}
      
      ;@ Adjust initial pointers by -4, so that loop can use
      ;@ pre-indexed mode with register update.
      sub   r1,r1,#4
      sub   r0,r0,#4
      sub   r8,r8,#4
      sub   r10,r10,#4
      sub   r11,r11,#4
      sub   r12,r12,#4
      sub   lr,lr,#4
      
      ;@ Use SP as pointer to bitplane pointers in the buffer
      mov   sp,r2
      
   LINE_N7_LOOP:                 ;@ do { // initial wordcount should always be > 0
      ;@ Get bx and advance bitmap pointers to the next words.
      ldr   r2,[r1,#4]!
      ldr   r4,[r0,#4]!
      ldr   r6,[r8,#4]!
      ldr   r3,[r11,#4]!
      ldr   r5,[r12,#4]!
      ldr   r7,[lr,#4]!
      
      ;@ Store new bitplane pointers over the old values
      stmia sp,{r0,r1,r8,r11,r12,lr}
      
      ;@ Load bx/store new pointers
      ldr   r1,[r10,#4]!
      str   r10,[sp,#6*4]

      ;@ 1st bitmask (0x55555555)
      ldr   r10,[sp,#(11+0)*4]

      ;@ MERGE (b2(r4), b3(r6), 0x55555555, 1);  // MERGE(a,b,mask,shift)
      eor   r8,r4,r6,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE_0(b0(r0), b1(r2), 0x33333333, 2);
      and   r0,r10,r2,lsr #2     ;@ a = tmp = mask & (b>>shift);
      eor   r2,r2,r0,lsl #2      ;@ b ^= (tmp << shift)      
      ;@ MERGE (b4(r1), b5(r3), 0x55555555, 1);
      eor   r8,r1,r3,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x55555555, 1);
      eor   r8,r5,r7,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 2nd bitmask (0x33333333)
      ldr   r10,[sp,#(11+1)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #1      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b2(r4), 0x33333333, 2);
      eor   r8,r0,r4,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r4,r4,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE (b1(r2), b3(r6), 0x33333333, 2);
      eor   r8,r2,r6,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b6(r5), 0x33333333, 2);
      eor   r8,r1,r5,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE (b5(r3), b7(r7), 0x33333333, 2);
      eor   r8,r3,r7,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 3rd bitmask (0x0f0f0f0f)
      ldr   r10,[sp,#(11+2)*4]
      ;@
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #2      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b4(r1), 0x0f0f0f0f, 4);
      eor   r8,r0,r1,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r1,r1,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b1(r2), b5(r3), 0x0f0f0f0f, 4);
      eor   r8,r2,r3,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b6(r5), 0x0f0f0f0f, 4);
      eor   r8,r4,r5,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b3(r6), b7(r7), 0x0f0f0f0f, 4);
      eor   r8,r6,r7,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 4th bitmask (0x00ff00ff)
      ldr   r10,[sp,#(11+3)*4]
      ;@
      eor   r6,r6,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #4      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b1(r2), 0x00ff00ff, 8);
      eor   r8,r0,r2,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r2,r2,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b3(r6), 0x00ff00ff, 8);
      eor   r8,r4,r6,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b5(r3), 0x00ff00ff, 8);
      eor   r8,r1,r3,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x00ff00ff, 8);
      eor   r8,r5,r7,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 5th bitmask (0x0000ffff)
      ldr   r10,[sp,#(11+4)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #8      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b2(r4), 0x0000ffff, 16);
      eor   r8,r0,r4,lsr #16     ;@ tmp = (a ^ (b >> shift))
      ;@
      ;@ Get &pixels from the buffer
      ldr   r11,[sp,#8*4]
      ;@
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r4,r4,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ Get wordcount from the buffer
      ldr   r12,[sp,#9*4]
      
      ;@ MERGE (b1(r2), b3(r6), 0x0000ffff, 16);
      eor   r8,r2,r6,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ wordcount--
      subs  r12,r12,#1
      
      ;@ MERGE (b4(r1), b6(r5), 0x0000ffff, 16);
      eor   r8,r1,r5,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ pixels += 8
      ;@ Keep old &pixels for storing result below
      addgt lr,r11,#8*4         
      strgt lr,[sp,#8*4]         ;@ Store &pixels
      
      ;@ MERGE (b5(r3), b7(r7), 0x0000ffff, 16);
      eor   r8,r3,r7,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      strgt r12,[sp,#9*4]        ;@ Store wordcount
      
      rev   r0,r0                ;@ Reverse byte order
      rev   r1,r1
      rev   r2,r2
      rev   r3,r3
      rev   r4,r4
      rev   r5,r5
      rev   r6,r6
      rev   r7,r7
      stmia r11,{r0-r7}          ;@ Store bx
      
      ;@ Get bitplane pointers for next iteration.
      ldmgtia sp,{r0,r1,r8,r11,r12,lr}
      ldrgt r10,[sp,#6*4]
      
      bgt   LINE_N7_LOOP         ;@ } while (wordcount > 0)
      
      ;@ Restore original SP
      ldr   sp,[sp,#10*4]
      
      ldmia sp!,{r4-r8,r10-r12,pc}
      

;@ NOTE: See PFIELD_DOLINE_N6 for explanations
PFIELD_DOLINE_N8:
         ;@ (
         ;@    r0=uae_u32 *pixels,
         ;@    r1=int wordcount,
         ;@    r2=uae_u8 *line_data[lineno],
         ;@    r3=uae_u32 max_words
         ;@ )
      stmdb sp!,{r4-r12,lr}
      
      mov   r4,r0                ;@ Save r0 before using it
      mov   r5,r1                ;@ Save r1 before using it
      
      add   r1,r2,r3,lsl #3      ;@ b0 <=> DATA_POINTER (7)
      sub   r1,r1,r3
      add   r9,r2,r3,lsl #2      ;@ b1 <=> DATA_POINTER (6)
      add   r9,r9,r3,lsl #1
      
      add   r0,r2,r3             ;@ b2 <=> DATA_POINTER (5)
      add   r0,r0,r3,lsl #2
      add   r8,r2,r3,lsl #2      ;@ b3 <=> DATA_POINTER (4)
      add   r10,r2,r3            ;@ b4 <=> DATA_POINTER (3)
      add   r10,r10,r3,lsl #1
      add   r11,r2,r3,lsl #1     ;@ b5 <=> DATA_POINTER (2)
      add   r12,r2,r3            ;@ b6 <=> DATA_POINTER (1)
      mov   lr,r2                ;@ b7 <=> DATA_POINTER (0)
      
      ;@ Pointer to the buffer
      ldr   r2,=DoLine_Data
      
      ;@ Store &pixels, wordcount, and original SP
      add   r3,r2,#8*4
      stmia r3,{r4,r5,sp}
      
      ;@ Adjust initial pointers by -4, so that loop can use
      ;@ pre-indexed mode with register update.
      sub   r1,r1,#4
      sub   r9,r9,#4
      sub   r0,r0,#4
      sub   r8,r8,#4
      sub   r10,r10,#4
      sub   r11,r11,#4
      sub   r12,r12,#4
      sub   lr,lr,#4
      
      ;@ Use SP as pointer to bitplane pointers in the buffer
      mov   sp,r2
      
   LINE_N8_LOOP:                 ;@ do { // initial wordcount should always be > 0
      ;@ Get bx and advance bitmap pointers to the next words.
      ldr   r2,[r9,#4]!
      ldr   r4,[r0,#4]!
      ldr   r6,[r8,#4]!
      ldr   r3,[r11,#4]!
      ldr   r5,[r12,#4]!
      ldr   r7,[lr,#4]!
      
      ;@ Store new bitplane pointers over the old values
      stmia sp,{r0,r8,r9,r11,r12,lr}
      
      ;@ Load bx/store new pointers
      ldr   r0,[r1,#4]!
      str   r1,[sp,#6*4]
      ldr   r1,[r10,#4]!
      str   r10,[sp,#7*4]

      ;@ 1st bitmask (0x55555555)
      ldr   r10,[sp,#(11+0)*4]

      ;@ MERGE (b0(r0), b1(r2), 0x55555555, 1);  // MERGE(a,b,mask,shift)
      eor   r8,r0,r2,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r2,r2,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b3(r6), 0x55555555, 1);  // MERGE(a,b,mask,shift)
      eor   r8,r4,r6,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b5(r3), 0x55555555, 1);
      eor   r8,r1,r3,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #1      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x55555555, 1);
      eor   r8,r5,r7,lsr #1      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 2nd bitmask (0x33333333)
      ldr   r10,[sp,#(11+1)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #1      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b2(r4), 0x33333333, 2);
      eor   r8,r0,r4,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r4,r4,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE (b1(r2), b3(r6), 0x33333333, 2);
      eor   r8,r2,r6,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b6(r5), 0x33333333, 2);
      eor   r8,r1,r5,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #2      ;@ b ^= (tmp << shift)
      ;@ MERGE (b5(r3), b7(r7), 0x33333333, 2);
      eor   r8,r3,r7,lsr #2      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 3rd bitmask (0x0f0f0f0f)
      ldr   r10,[sp,#(11+2)*4]
      ;@
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #2      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b4(r1), 0x0f0f0f0f, 4);
      eor   r8,r0,r1,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r1,r1,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b1(r2), b5(r3), 0x0f0f0f0f, 4);
      eor   r8,r2,r3,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b6(r5), 0x0f0f0f0f, 4);
      eor   r8,r4,r5,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #4      ;@ b ^= (tmp << shift)
      ;@ MERGE (b3(r6), b7(r7), 0x0f0f0f0f, 4);
      eor   r8,r6,r7,lsr #4      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 4th bitmask (0x00ff00ff)
      ldr   r10,[sp,#(11+3)*4]
      ;@
      eor   r6,r6,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #4      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b1(r2), 0x00ff00ff, 8);
      eor   r8,r0,r2,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r2,r2,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b2(r4), b3(r6), 0x00ff00ff, 8);
      eor   r8,r4,r6,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r4,r4,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b4(r1), b5(r3), 0x00ff00ff, 8);
      eor   r8,r1,r3,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r3,r3,r8,lsl #8      ;@ b ^= (tmp << shift)
      ;@ MERGE (b6(r5), b7(r7), 0x00ff00ff, 8);
      eor   r8,r5,r7,lsr #8      ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      ;@
      ;@ 5th bitmask (0x0000ffff)
      ldr   r10,[sp,#(11+4)*4]
      ;@
      eor   r5,r5,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #8      ;@ b ^= (tmp << shift)
      
      ;@ MERGE (b0(r0), b2(r4), 0x0000ffff, 16);
      eor   r8,r0,r4,lsr #16     ;@ tmp = (a ^ (b >> shift))
      ;@
      ;@ Get &pixels from the buffer
      ldr   r11,[sp,#8*4]
      ;@
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r0,r0,r8             ;@ a ^= tmp
      eor   r4,r4,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ Get wordcount from the buffer
      ldr   r12,[sp,#9*4]
      
      ;@ MERGE (b1(r2), b3(r6), 0x0000ffff, 16);
      eor   r8,r2,r6,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r2,r2,r8             ;@ a ^= tmp
      eor   r6,r6,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ wordcount--
      subs  r12,r12,#1
      
      ;@ MERGE (b4(r1), b6(r5), 0x0000ffff, 16);
      eor   r8,r1,r5,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r1,r1,r8             ;@ a ^= tmp
      eor   r5,r5,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      ;@ pixels += 8
      ;@ Keep old &pixels for storing result below
      addgt lr,r11,#8*4         
      strgt lr,[sp,#8*4]         ;@ Store &pixels
      
      ;@ MERGE (b5(r3), b7(r7), 0x0000ffff, 16);
      eor   r8,r3,r7,lsr #16     ;@ tmp = (a ^ (b >> shift))
      and   r8,r10,r8            ;@ tmp = mask & tmp
      eor   r3,r3,r8             ;@ a ^= tmp
      eor   r7,r7,r8,lsl #16     ;@ b ^= (tmp << shift)
      
      strgt r12,[sp,#9*4]        ;@ Store wordcount
      
      rev   r0,r0                ;@ Reverse byte order
      rev   r1,r1
      rev   r2,r2
      rev   r3,r3
      rev   r4,r4
      rev   r5,r5
      rev   r6,r6
      rev   r7,r7
      stmia r11,{r0-r7}          ;@ Store bx
      
      ;@ Get bitplane pointers for next iteration.
      ldmgtia sp,{r0,r8,r9,r11,r12,lr}
      ldrgt r1,[sp,#6*4]
      ldrgt r10,[sp,#7*4]
      
      bgt   LINE_N8_LOOP         ;@ } while (wordcount > 0)
      
      ;@ Restore original SP
      ldr   sp,[sp,#10*4]
      
      ldmia sp!,{r4-r12,pc}
      
   DoLine_pData:
      .align 4
      .long DoLine_Data
      
   ;@ ---
      
      .data
      .align 8
   DoLine_Data:
      ;@ 8 bitplane pointers
      .long 0,0,0,0,0,0,0,0
      
      ;@ wordcount and &pixels
      .long 0,0
      
      ;@ Original SP
      .long 0
      
      ;@ Bitmasks
      .long 0x55555555
      .long 0x33333333
      .long 0x0f0f0f0f
      .long 0x00ff00ff
      .long 0x0000ffff
      
;@ ----------------------------------------------------------------
	
