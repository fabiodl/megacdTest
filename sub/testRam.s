        .global testRam

.set SUB_COMM,0x0FF800F
.set STATE_FILLING,1
.set STATE_CHECKING,2
.set STATE_ERROR,0x0E
.set STATE_PASSED,3        
        
testRam:
        move.b #STATE_FILLING,SUB_COMM
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

        move.b #STATE_CHECKING,SUB_COMM
        move.l #0,%d1
        move.l #0x80000,%a0        
readNext:
        cmp.b (%a0),%d1
        beq ok
        move.b #STATE_ERROR,SUB_COMM
        jmp testManager
ok:     add.b #1,%d1
        cmp.b #251,%d1
        bne noReadReset
        move.b #0,%d1
noReadReset:    
        add.l #1,%a0
        cmp.l #0x0C0000,%a0
        bne readNext
        move.b #STATE_PASSED,SUB_COMM        
        jmp testManager
