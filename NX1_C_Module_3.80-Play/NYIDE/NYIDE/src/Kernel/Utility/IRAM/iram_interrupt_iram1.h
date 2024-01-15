#ifndef IRAM_INTERRUPT_H
#define IRAM_INTERRUPT_H

	.macro gie_disable
		setgie.d
		dsb
	.endm

	.macro gie_enable
		setgie.e
		dsb
	.endm

	/* align $sp to 8B boundary */
	.macro align_sp_8_iraam R0="$r2", R1="$r3"
		move    \R0, $sp        !keep original $sp to be pushed
#if !defined(NDS32_BASELINE_V3M) || 1 //could be optimized out
 #ifndef NDS32_EXT_PERF
		andi    \R1, $sp, #4    ! R1 = $sp.bit2 // 0 or 4
		subri   \R1, \R1, #4    ! R1 = 4 - R1   // 4 or 0
		sub     $sp, $sp, \R1   ! $sp -= R1     //-4 or 0
		push    \R0
 #else
		addi    $sp, $sp, #-4   ! $sp -= 4
		bclr    $sp, $sp, #2    ! $sp.bit2 = 0
		sw      \R0, [$sp]
 #endif
#endif
	.endm

	.macro push_d0d1_iram R0="$r2", R1="$r3", R2="$r4", R3="$r5"
#ifdef NDS32_DX_REGS
		mfusr   \R0, $d1.lo
		mfusr   \R1, $d1.hi
		mfusr   \R2, $d0.lo
		mfusr   \R3, $d0.hi
		pushm   \R0, \R3
#endif
	.endm

	.macro pop_d0d1_iram R0="$r2", R1="$r3", R2="$r4", R3="$r5"
#ifdef NDS32_DX_REGS
		popm    \R0, \R3
		mtusr   \R0, $d1.lo
		mtusr   \R1, $d1.hi
		mtusr   \R2, $d0.lo
		mtusr   \R3, $d0.hi
#endif
	.endm

	.macro push_ifc_lp_iram R0="$r2"
#ifdef NDS32_EXT_IFC
		mfusr   \R0, $IFC_LP
		push    \R0
#endif
	.endm

	.macro pop_ifc_lp_iram R0="$r2"
#ifdef NDS32_EXT_IFC
	pop     \R0
	mtusr   \R0, $IFC_LP
#endif
	.endm

	.macro SAVE_FPU_REGS_00_iram
		addi    $sp, $sp, -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_01_iram
		addi    $sp, $sp, -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_02_iram
		addi    $sp, $sp, -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd10, [$sp], -8
		fsdi.bi $fd8, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_03_iram
		addi    $sp, $sp, -8
		fsdi.bi $fd30, [$sp], -8
		fsdi.bi $fd28, [$sp], -8
		fsdi.bi $fd26, [$sp], -8
		fsdi.bi $fd24, [$sp], -8
		fsdi.bi $fd22, [$sp], -8
		fsdi.bi $fd20, [$sp], -8
		fsdi.bi $fd18, [$sp], -8
		fsdi.bi $fd16, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd10, [$sp], -8
		fsdi.bi $fd8, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro push_fpu_iraam
#if defined(NDS32_EXT_FPU_CONFIG_0)
		SAVE_FPU_REGS_00_iram
#elif defined(NDS32_EXT_FPU_CONFIG_1)
		SAVE_FPU_REGS_01_iram
#elif defined(NDS32_EXT_FPU_CONFIG_2)
		SAVE_FPU_REGS_02_iram
#elif defined(NDS32_EXT_FPU_CONFIG_3)
		SAVE_FPU_REGS_03_iram
#else
#endif
	.endm

	.macro RESTORE_FPU_REGS_00_iram
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_01_iram
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_02_iram
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd8, [$sp], 8
		fldi.bi $fd10, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd14, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_03_iram
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd8, [$sp], 8
		fldi.bi $fd10, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd16, [$sp], 8
		fldi.bi $fd18, [$sp], 8
		fldi.bi $fd20, [$sp], 8
		fldi.bi $fd22, [$sp], 8
		fldi.bi $fd24, [$sp], 8
		fldi.bi $fd26, [$sp], 8
		fldi.bi $fd28, [$sp], 8
		fldi.bi $fd30, [$sp], 8
	.endm

	.macro pop_fpu_IRAM
#if defined(NDS32_EXT_FPU_CONFIG_0)
		RESTORE_FPU_REGS_00_iram
#elif defined(NDS32_EXT_FPU_CONFIG_1)
		RESTORE_FPU_REGS_01_iram
#elif defined(NDS32_EXT_FPU_CONFIG_2)
		RESTORE_FPU_REGS_02_iram
#elif defined(NDS32_EXT_FPU_CONFIG_3)
		RESTORE_FPU_REGS_03_iram
#else
#endif
	.endm



	.macro SAVE_ALL_iram
		pushm $r2, $r30
		push_d0d1_iram
		push_ifc_lp_iram
		mfsr    $r0, $PSW
		mfsr    $r1, $IPC
		mfsr    $r2, $IPSW
		pushm   $r0, $r2

		/* Descend interrupt level */
		align_sp_8_iraam
		push_fpu_iraam
	.endm

	.macro RESTORE_ALL
		pop_fpu_IRAM
		lwi     $sp, [$sp]
		popm    $r0, $r2	/* popm $r1 ~ $r2*/
		mtsr    $r1, $IPC
		mtsr    $r2, $IPSW
		pop_ifc_lp_iram
		pop_d0d1_iram
		popm	$r2, $r30
	.endm

	.macro CallFn fn

!#if defined(NDS32_ABI_2) || defined(NDS32_ABI_2FP)
!		addi    $sp, $sp, -4
!#else
!		addi    $sp, $sp, -28
!#endif

		bal	\fn

!#if defined(NDS32_ABI_2) || defined(NDS32_ABI_2FP)
!		addi    $sp, $sp, 4
!#else
!		addi    $sp, $sp, 28
!#endif
	.endm

    .macro IntlSwitch lv
		mfsr	$a0, $PSW
		addi    $r0, $r0, #-2
		mtsr    $r0, $PSW
		isb

	.endm    

	.macro SAVE_ALL_HW_IRAM      !should not touch $r0
		/* push caller-saved gpr */
		pushm	$r1, $r5
		pushm	$r15, $r30
		push_d0d1_iram
		push_ifc_lp_iram


		/* push $PSW, $IPC, $IPSW */
		mfsr    $r1, $PSW
		mfsr    $r2, $IPC
		mfsr    $r3, $IPSW
		pushm   $r1, $r3

		/* You can use -1 if you want to
		 * descend interrupt level and enable gie or
		 * you can enable gie when you finish your top
		 * half isr. */
		/* Descend interrupt level */
		addi    $r1, $r1, #-2
		mtsr    $r1, $PSW

		align_sp_8_iraam
		push_fpu_iraam
	.endm

	.macro RESTORE_ALL_HW_IRAM
		/* pop fpu*/
		pop_fpu_IRAM
		lwi $sp, [$sp]

		/* pop $PSW, $IPC, $IPSW*/
		popm    $r0, $r2
		gie_disable
		mtsr    $r0, $PSW
		mtsr    $r1, $IPC
		mtsr    $r2, $IPSW

		pop_ifc_lp_iram
		/* pop d0d1 */
		pop_d0d1_iram

		/* pop gpr */
		popm    $r15,$r30
		popm    $r1, $r5
		pop     $r0
	.endm
#endif //#ifndef IRAM_INTERRUPT_H