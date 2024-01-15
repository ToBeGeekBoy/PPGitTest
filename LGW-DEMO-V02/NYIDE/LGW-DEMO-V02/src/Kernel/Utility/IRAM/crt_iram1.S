##==============================================================================
##
##  crt_iram.S
##
##  iram isr vector code
##
##==============================================================================
#include "nx1_def.h"
#if OPTION_IRAM

#include "iram_interrupt_iram1.h"
#define __ASSEMBLY__

##------------------------------------------------------------------------------

    .section "iram1-vector", "ax"
    .align 2
!========================================================================
! Vector table
!========================================================================
_start:
exception_iram_vector:
    j iram_blank				  !  (0) iram Blank
    j iram_Trap_TLB_Fill          !  (1) iram Trap TLB fill
    j iram_Trap_PTE_Not_Present   !  (2) iram Trap PTE not present
    j iram_Trap_TLB_Misc          !  (3) iram Trap TLB misc
    j iram_Trap_TLB_VLPT_Miss     !  (4) iram Trap TLB VLPT miss
    j iram_Trap_Machine_Error     !  (5) iram Trap Machine error
    j iram_Trap_Debug_Related     !  (6) iram Trap Debug related
    j iram_Trap_General_Exception !  (7) iram Trap General exception
    j iram_Trap_Syscall           !  (8) iram Syscall
    j iram_Trap_Interrupt_HW0     !  (9) iram Interrupt HW0
    j iram_Trap_Interrupt_HW1     ! (10) iram Interrupt HW1
    j iram_Trap_Interrupt_HW2     ! (11) iram Interrupt HW2
    j iram_Trap_Interrupt_HW3     ! (12) iram Interrupt HW3
    j iram_Trap_Interrupt_HW4     ! (13) iram Interrupt HW4
    j iram_Trap_Interrupt_HW5     ! (14) iram Interrupt HW5
    j iram_Trap_Interrupt_HW6     ! (15) iram Interrupt HW6
    j iram_Trap_Interrupt_HW7     ! (16) iram Interrupt HW7
    j iram_Trap_Interrupt_HW8     ! (17) iram Interrupt HW8
    j iram_Trap_Interrupt_HW9     ! (18) iram Interrupt HW9
    j iram_Trap_Interrupt_HW10    ! (19) iram Interrupt HW10
    j iram_Trap_Interrupt_HW11    ! (20) iram Interrupt HW11
    j iram_Trap_Interrupt_HW12    ! (21) iram Interrupt HW12
    j iram_Trap_Interrupt_HW13    ! (22) iram Interrupt HW13
    j iram_Trap_Interrupt_HW14    ! (23) iram Interrupt HW14
    j iram_Trap_Interrupt_HW15    ! (24) iram Interrupt HW15

    j iram_Trap_Interrupt_HW16    ! (25) iram Interrupt HW16
    j iram_Trap_Interrupt_HW17    ! (26) iram Interrupt HW17
    j iram_Trap_Interrupt_HW18    ! (27) iram Interrupt HW18
    j iram_Trap_Interrupt_HW19    ! (28) iram Interrupt HW19
    j iram_Trap_Interrupt_HW20    ! (29) iram Interrupt HW20
    j iram_Trap_Interrupt_HW21    ! (30) iram Interrupt HW21
    j iram_Trap_Interrupt_HW22    ! (31) iram Interrupt HW22
    j iram_Trap_Interrupt_HW23    ! (32) iram Interrupt HW23

	.section .text, "ax"
	.align 2
iram_blank:

exception_iram_vector_end:
/*----------------------------------------------------------------------
   Since N903 doesn't have MMU, make them share the common error handler.
----------------------------------------------------------------------*/
iram_Trap_TLB_Fill:
iram_Trap_PTE_Not_Present:
iram_Trap_TLB_Misc:
iram_Trap_TLB_VLPT_Miss:
iram_Trap_Machine_Error:
iram_Trap_Debug_Related:
iram_Trap_General_Exception:
iram_Trap_Syscall:

    push $r0
    SAVE_ALL_HW_IRAM
    jal ExceptionHandler_IRAM
    RESTORE_ALL_HW_IRAM
    iret


iram_common_ISR_wrapper:
	SAVE_ALL_HW_IRAM
    !gie_enable
	la  $r1, CPUVectorTable_IRAM
	lw  $r1, [$r1+$r0<<2]
	jral $r1
	!gie_disable
	!CallFn	OS_CPU_Int_Comm
	RESTORE_ALL_HW_IRAM
	iret

iram_Trap_Interrupt_HW0:
	push $r0
	li  $r0, 0x0
	j   iram_common_ISR_wrapper

iram_Trap_Interrupt_HW1:
	push $r0
	li   $r0, 0x01
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW2:
	push $r0
	li   $r0, 0x02
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW3:
	push $r0
	li   $r0, 0x03
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW4:
	push $r0
	li   $r0, 0x04
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW5:
	push $r0
	li   $r0, 0x05
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW6:
	push $r0
	li   $r0, 0x06
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW7:
	push $r0
	li   $r0, 0x07
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW8:
	push $r0
	li   $r0, 0x08
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW9:
	push $r0
	li  $r0, 0x09
	j   iram_common_ISR_wrapper

iram_Trap_Interrupt_HW10:
	push $r0
	li   $r0, 0x0A
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW11:
	push $r0
	li   $r0, 0x0B
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW12:
	push $r0
	li   $r0, 0x0C
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW13:
	push $r0
	li   $r0, 0x0D
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW14:
	push $r0
	li   $r0, 0x0E
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW15:
	push $r0
	li   $r0, 0xF
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW16:
	push $r0
	li   $r0, 0x10
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW17:
	push $r0
	li   $r0, 0x11
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW18:
	push $r0
	li   $r0, 0x12
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW19:
	push $r0
	li   $r0, 0x13
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW20:
	push $r0
	li   $r0, 0x14
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW21:
	push $r0
	li   $r0, 0x15
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW22:
	push $r0
	li   $r0, 0x16
	j    iram_common_ISR_wrapper

iram_Trap_Interrupt_HW23:
	push $r0
	li   $r0, 0x17
	j    iram_common_ISR_wrapper


    .end
#endif