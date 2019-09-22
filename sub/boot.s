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
    .global _Entry_Point
    .org    0x00000000

_Start_Of_Rom:
_Vecteurs_68K:
        dc.l    0x000C0000              /* Stack address */
        dc.l    testRam            /* Program start address */
        dc.l    _Bus_Error
        dc.l    _Address_Error
        dc.l    _Illegal_Instruction
        dc.l    _Zero_Divide
        dc.l    _Chk_Instruction
        dc.l    _Trapv_Instruction
        dc.l    _Privilege_Violation
        dc.l    _Trace
        dc.l    _Line_1010_Emulation
        dc.l    _Line_1111_Emulation
        dc.l     _Error_Exception, _Error_Exception, _Error_Exception, _Error_Exception
        dc.l     _Error_Exception, _Error_Exception, _Error_Exception, _Error_Exception
        dc.l     _Error_Exception, _Error_Exception, _Error_Exception, _Error_Exception
        dc.l    _Error_Exception, _INT, _EXTINT, _INT
        dc.l    _HINT
        dc.l    _INT
        dc.l    _VINT
        dc.l    _INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT
        dc.l    _INT,_INT,_INT,_INT,_INT,_INT,_INT,_INT


_Entry_Point:
        move  #0x2700,%sr

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
        jmp     main



_Trace:
_Line_1010_Emulation:
_Line_1111_Emulation:
_Error_Exception:
_INT:
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


