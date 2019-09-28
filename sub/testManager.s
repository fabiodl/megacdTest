.set SUB_COMM_CMD,0x0FF800E
.set SUB_COMM_STATUS,0x0FF800F
.set CMD_NONE,0
.set CMD_RESETSTATUS,1        
.set CMD_TESTWORDRAM,2
.set CMD_TESTBACKUPRAM,3
.set CMD_GIVEBANK0,4
.set CMD_GIVEBANK1,5
.set CMD_GIVE2M,6        
.set CMD_2MMODE,7
.set CMD_INITC,8
.set CMD_WAITINTERRUPT,9

        
.set STATUS_IDLE,0x00
.set STATUS_CMDREAD,0x80
.set STATUS_CMDEXEC,0x81

.global testManager

        
testManager:
        move.b SUB_COMM_CMD,%d0
        cmp.b #CMD_NONE,%d0
        beq testManager
        move.b #STATUS_CMDREAD,SUB_COMM_STATUS
waitCmdRemoval:
        cmp.b #CMD_NONE,SUB_COMM_CMD
        bne waitCmdRemoval
        move.b #STATUS_CMDEXEC,SUB_COMM_STATUS
        
        cmp.b #CMD_TESTWORDRAM,%d0
        beq testWordRam        

        cmp.b #CMD_TESTBACKUPRAM,%d0
        beq testBackupRam
        
        cmp.b #CMD_GIVEBANK0,%d0
        beq giveBank0       
        
        cmp.b #CMD_GIVEBANK1,%d0
        beq giveBank1

        cmp.b #CMD_GIVE2M,%d0
        beq give2M

        cmp.b #CMD_2MMODE,%d0
        beq to2MMode
        
        cmp.b #CMD_RESETSTATUS,%d0
        beq resetStatus

        
        cmp.b #CMD_INITC,%d0
        /*move.b #0xFA,0x0FF800F*/
        beq init_c

        cmp.b #CMD_WAITINTERRUPT,%d0                
        beq waitInterrupt
        
        
        
        bra testManager

resetStatus:
        move.b #STATUS_IDLE,SUB_COMM_STATUS
        bra testManager
