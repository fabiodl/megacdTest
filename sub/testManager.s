.set SUB_COMM_CMD,0x0FF800E
.set SUB_COMM_STATUS,0x0FF800F

.set CMD_NONE,0x00
        
.set STATUS_IDLE,0x00
.set STATUS_CMDREAD,0x80
.set STATUS_CMDEXEC,0x81

.global testManager



        
cmdTable: 
        dc.l testManager         /*0x00 CMD_NONE*/
        dc.l resetStatus         /*0x01*/
        dc.l testWordRam         /*0x02*/
        dc.l testBackupRam       /*0x03*/
        dc.l giveBank0           /*0x04*/
        dc.l giveBank1           /*0x05*/
        dc.l give2M              /*0x06*/
        dc.l to2MMode            /*0x07*/
        dc.l init_c              /*0x08*/
        dc.l waitInterrupt       /*0x09*/
        dc.l copyProgToWord      /*0x0A*/
        dc.l testWordZeroFilling /*0x0B*/
        dc.l testWordFFFilling   /*0x0C*/
     

        
        
testManager:
        move.l #0,%d0
        move.b SUB_COMM_CMD,%d0
        cmp.b #CMD_NONE,%d0
        beq testManager
        move.b #STATUS_CMDREAD,SUB_COMM_STATUS
waitCmdRemoval:
        cmp.b #CMD_NONE,SUB_COMM_CMD
        bne waitCmdRemoval
        move.b #STATUS_CMDEXEC,SUB_COMM_STATUS        

        lea cmdTable,%a0
        asl.l  #2,%d0
        movea.l (0,%a0,%d0),%a1
        jmp (%a1)
        

resetStatus:
        move.b #STATUS_IDLE,SUB_COMM_STATUS
        bra testManager



