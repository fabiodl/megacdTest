        .global testRam

.set SUB_COMM,0x0FF800F

        
testRam:
        move.b #1,SUB_COMM
        move.l #0,%d1
        move.l #0x80000,%a0
        
fillNext:       
        move.b %d1,(%a0)
        add.b #1,%d1
        cmp.b #251,%d1
        bne noFillReset
        move.b #0,%d1        
noFillReset:
        add.l #1,%a0
        cmp.l #0x0C0000,%a0        
        bne fillNext

        move.b #2,SUB_COMM
        move.l #0,%d1
        move.l #0x80000,%a0        
readNext:
        cmp.b (%a0),%d1
        beq ok
        move.b #0x0E,SUB_COMM
        jmp _Entry_Point
ok:     add.b #1,%d1
        cmp.b #251,%d1
        bne noReadReset
        move.b #0,%d1
noReadReset:    
        add.l #1,%a0
        cmp.l #0x0C0000,%a0
        bne readNext
        move.b #0x03,SUB_COMM        
        jmp _Entry_Point
