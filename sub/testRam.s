        .global testRam
        .global giveRam0
        .global giveRam1
        
.set SUB_COMM,0x0FF800F
.set SUB_MEMORYMODE,0xFF8002
.set STATUS_FILLING,1
.set STATUS_CHECKING,2
.set STATUS_PASSED,3
.set STATUS_ERROR,0x0E        

.set RET,1                
.set DMNA,2
.set MODE,4
        
testRam:
        move.b #STATUS_FILLING,SUB_COMM
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

        move.b #STATUS_CHECKING,SUB_COMM
        move.l #0,%d1
        move.l #0x80000,%a0        
readNext:
        cmp.b (%a0),%d1
        beq ok
        move.b #STATUS_ERROR,SUB_COMM
        bra testManager
ok:     add.b #1,%d1
        cmp.b #251,%d1
        bne noReadReset
        move.b #0,%d1
noReadReset:    
        add.l #1,%a0
        cmp.l #0x0C0000,%a0
        bne readNext
        move.b #STATUS_PASSED,SUB_COMM        
        bra testManager

.set MODE1_RAM0,MODE|DMNA        
.set MODE1_RAM1,MODE|DMNA|RET        
        
giveRam0:
        move.l #MODE1_RAM0,SUB_MEMORYMODE
        bra testManager


giveRam1:

        move.l #MODE1_RAM1,SUB_MEMORYMODE
        bra testManager
        
