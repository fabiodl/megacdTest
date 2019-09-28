.section .text.keepboot

*-------------------------------------------------------
*
*       Sega startup code for the GNU Assembler
*       Translated from:
*       Sega startup code for the Sozobon C compiler
*       Written by Paul W. Lee
*       Modified by Charles Coty
*       Modified by Stephane Dallongeville
*
*-------------------------------------------------------

    .globl 	_hard_reset
    .global testManager
    .org    0x00000000

.set SUB_COMM_CMD,0x0FF800E
.set SUB_COMM_STATUS,0x0FF800F
.set CMD_NONE,0
.set CMD_RESETSTATUS,1        
.set CMD_TESTRAM,2
.set CMD_GIVERAM0,3
.set CMD_GIVERAM1,4       
.set CMD_2MMODE,5
.set CMD_INITC,6
.set CMD_WAITINTERRUPT,7

        
.set STATUS_IDLE,0x00
.set STATUS_CMDREAD,0x80
.set STATUS_CMDEXEC,0x81
.set STATUS_INITC,4
.set STATUS_C_OK,5        
        
_Start_Of_Rom:
_Vecteurs_68K:
        dc.l    0x000C0000              /* Stack address 0*/  
        dc.l    testManager            /* Program start address 4*/
        dc.l    _Bus_Error               /*8 */
        dc.l    _Address_Error                      
        dc.l    _Illegal_Instruction
        dc.l    _Zero_Divide
        dc.l    _Chk_Instruction
        dc.l    _Trapv_Instruction
        dc.l    _Privilege_Violation
        dc.l    _Trace                /*24*/
        dc.l    _Line_1010_Emulation  /*28*/
        dc.l    _Line_1111_Emulation  /*2C*/
        dc.l     _Error_Exception, _Error_Exception, _Error_Exception, _Error_Exception  /*...3C*/
        dc.l     _Error_Exception, _Error_Exception, _Error_Exception, _Error_Exception  /*...4C*/
        dc.l     _Error_Exception, _Error_Exception, _Error_Exception, _Error_Exception  /*...5C*/
        dc.l    _Error_Exception /*60*/
        dc.l    _INT1, _INT2, _INT3 
        dc.l    _HINT
        dc.l    _INT
        dc.l    _VINT
        dc.l    _INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT


init_c:
        move.b #STATUS_INITC,SUB_COMM_STATUS
        move  #0x2700,%sr  /*in supervisor mode, all interrupts masked*/

* clear Data RAM
        move.l   #0x080000,%a0
        move.w   #0,%d0
ClearRam:
        move.w  %d0,(%a0)+
        cmp.l   #0x0C0000,%a0
        bne     ClearRam


* copy initialized variables from ROM to Work RAM
        lea     _stext,%a0
        lea     0x080000,%a1
        move.l  #_sdata,%d0

* fix for last byte to initialize
        addq.l  #1,%d0
        lsr.l   #1,%d0
        beq     NoCopy

        subq.w  #1,%d0
CopyVar:
        move.w  (%a0)+,(%a1)+
        dbra    %d0,CopyVar

NoCopy:

* Jump to initialisation process...
        move.w #0x2000,%sr      /*in supervisor mode, no interrupt mask*/
        move.b #STATUS_C_OK,SUB_COMM_STATUS
        bra testManager


_INT2:
  jsr onInterrupt2
  rte      


        
_INT3:
  jsr onInterrupt3
  rte  

        
_Trace:
_Line_1010_Emulation:
_Line_1111_Emulation:
_Error_Exception:
_INT:
_INT1:
_EXTINT:
_HINT:
_VINT:
_Bus_Error:
_Address_Error:
_Illegal_Instruction:
_Zero_Divide:
_Chk_Instruction:
_Trapv_Instruction:
_Privilege_Violation:        
        rte

testManager:
        move.b SUB_COMM_CMD,%d0
        cmp.b #CMD_NONE,%d0
        beq testManager
        move.b #STATUS_CMDREAD,SUB_COMM_STATUS
waitCmdRemoval:
        cmp.b #CMD_NONE,SUB_COMM_CMD
        bne waitCmdRemoval
        move.b #STATUS_CMDEXEC,SUB_COMM_STATUS
        
        cmp.b #CMD_TESTRAM,%d0
        beq testRam        
              
        cmp.b #CMD_GIVERAM0,%d0
        beq giveRam0       
        
        cmp.b #CMD_GIVERAM1,%d0
        beq giveRam1        

        cmp.b #CMD_2MMODE,%d0
        beq to2MMode
        
        cmp.b #CMD_RESETSTATUS,%d0
        beq resetStatus

        
        cmp.b #CMD_INITC,%d0
        beq init_c

        cmp.b #CMD_WAITINTERRUPT,%d0                
        beq waitInterrupt
        
        
        
        bra testManager

resetStatus:
        move.b #STATUS_IDLE,SUB_COMM_STATUS
        bra testManager
