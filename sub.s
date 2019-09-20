.global asmStart
.global asmEnd
        

.org    0x00000000
asmStart:
        dc.l    0xC0000              /* Stack address */
        dc.l    0x100            /* Program start address */

.org 0x100        

sub_Entry_Point:           
 move.b #0x55,0x0FF800F
 move.w #0x101,0x0FF8000
 move.l #0,%D0
loopCnt:        
 add.l #1,%D0
 cmp.l #0x50000,%D0
 bne loopCnt
 move.b #0xAA,0x0FF800F
 move.w #0x201,0x0FF8000
 move.l #0,%D0
loop2Cnt:        
 add.l #1,%D0
 cmp.l #0x50000,%D0
 bne loop2Cnt        
 bra sub_Entry_Point


asmEnd:



        
