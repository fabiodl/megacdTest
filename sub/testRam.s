        .global testWordRam
        .global giveBank0
        .global giveBank1
        .global give2M
        .global to2MMode
        .global testBackupRam
        .global copyProgToWord
        .global testWordZeroFilling
        .global testWordFFFilling
        
.set SUB_COMM_STATUS,0x0FF800F
.set SUB_MEMORYMODE,0xFF8002
.set STATUS_FILLING,0x01
.set STATUS_CHECKING,0x02
.set STATUS_PASSED,0x03
.set STATUS_CHECKREQ,0x09        
.set STATUS_ERROR,0x0E        

.set RET,1                
.set DMNA,2
.set MODE,4
        
testWordRam:
        move.b #STATUS_FILLING,SUB_COMM_STATUS
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

        move.b #STATUS_CHECKING,SUB_COMM_STATUS
        move.l #0,%d1
        move.l #0x80000,%a0        
readNext:
        cmp.b (%a0),%d1
        beq ok
        move.b #STATUS_ERROR,SUB_COMM_STATUS
        bra testManager
ok:     add.b #1,%d1
        cmp.b #251,%d1
        bne noReadReset
        move.b #0,%d1
noReadReset:    
        add.l #1,%a0
        cmp.l #0x0C0000,%a0
        bne readNext
        move.b #STATUS_PASSED,SUB_COMM_STATUS        
        bra testManager

        
giveBank0:
        move.w #MODE,SUB_MEMORYMODE /*MODE*/
        move.b #0x0A,SUB_COMM_STATUS
        bra testManager


giveBank1:

        move.w #MODE+RET,SUB_MEMORYMODE  /*MODE|RET*/
        move.b #0x0B,SUB_COMM_STATUS
        bra testManager
        

to2MMode:
        move.w #0,SUB_MEMORYMODE /*2M mode, to SUB*/
        move.b #0x0C,SUB_COMM_STATUS
        bra testManager

give2M:
        move.w #RET,SUB_MEMORYMODE /*2M mode, to MAIN*/
        move.b #0x0D,SUB_COMM_STATUS
        bra testManager
        


testBackupRam:  
        move.b #STATUS_FILLING,SUB_COMM_STATUS
        move.l #1,%d1
        move.l #0xFE0001,%a0
        
brFillNext:       
        move.b %d1,(%a0)
        add.b #1,%d1
        cmp.b #251,%d1
        bne brNoFillReset
        move.b #0,%d1        
brNoFillReset:
        add.l #2,%a0
        cmp.l #0xFE4001,%a0        
        bne brFillNext

        move.b #STATUS_CHECKING,SUB_COMM_STATUS
        move.l #1,%d1
        move.l #0xFE0001,%a0        
brReadNext:
        cmp.b (%a0),%d1
        beq brOk
        move.b #STATUS_ERROR,SUB_COMM_STATUS
        bra testManager
brOk:   add.b #1,%d1
        cmp.b #251,%d1
        bne brNoReadReset
        move.b #0,%d1
brNoReadReset:    
        add.l #2,%a0
        cmp.l #0xFE4001,%a0
        bne brReadNext
        move.b #STATUS_PASSED,SUB_COMM_STATUS        
        bra testManager

        
copyProgToWord:
        move.b #STATUS_FILLING,SUB_COMM_STATUS
        move.l #0x000000,%a0
        move.l #0x080000,%a1
cppwNext:       
        move.l (%a0)+,(%a1)+
        cmp.l  #0x0C0000,%a1
        bne cppwNext
        move.b #STATUS_CHECKREQ,SUB_COMM_STATUS
        bra testManager
        
testWordZeroFilling:
        move.w #0,%d0
        bra testWordFilling


testWordFFFilling:
        move.w #0xFFFF,%d0
        bra testWordFilling

testWordFilling:        
        move.b #STATUS_FILLING,SUB_COMM_STATUS
        move.l #0x080000,%a0
fillWNext:
        move.w %d0,(%a0)+
        cmp.l  #0x0C0000,%a0
        bne fillWNext
        move.b #STATUS_CHECKING,SUB_COMM_STATUS
        move.l #0x080000,%a0
checkFillWNext:  
        cmp.w (%a0)+,%d0
        bne fillWIsBad
        cmp.l  #0x0C0000,%a0
        bne checkFillWNext
        move.b #STATUS_PASSED,SUB_COMM_STATUS
        bra testManager        
fillWIsBad:
        move.b #STATUS_ERROR,SUB_COMM_STATUS
        bra testManager

        
