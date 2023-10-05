#include <stdio.h>

#include "core.h"
#include "instr.h"
#include "regs.h"

/*
* TODO: Implement coprocessor stuff
* Operands:
*   Operands are specified by a mask, shift, flags and handler
*   What the flags do are specific to each handler
*     REG_GP: The flags represent the register size, R=1, ER=2, ...
*     REG_CTRL: The flags represent the control register
*       0: ECSR
*       1: ELR
*       2: PSW
*       3: EPSW
*       4: SP
*       5: CSR
*       6: DSR
*     MEM: 0xSDER
*       S: as binary ISSS
*         I: Increment EA?
*         S: Size - 1
*       D: Displacement
*         0: No Displacement
*         1: Disp16 from second word
*         2: Disp from instruction (Only used for Disp6)
*       E: Size in bits for displacement sign extension (Only used D=2)
*       R: Register
*         0: No Register (Used for Dadr by specify Disp16)
*         1: EA
*         2: ER from instruction
*         3: BP (ER12)
*         4: FP (ER14)
*     IMM: 0x000S
*        S: Size in bits for sign extension, if this is non-zero
*           then extract bit S (ie (val >> S) & 1) and repeat to
*           fill uint64_t
*/

/* Instruction Decoding Table */
static struct u8_instr u8_instr_table[] = {
	// Arithmetic Instructions
	{&instr_add,		0x8001,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// Add	Rn, 	Rm
	{&instr_add,		0x1000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00ff, 0,  0x0000, &oper_imm}},		// Add	Rn, 	#imm8
	{&instr_add,		0xf006, 0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00e0, 4,  0x0002, &oper_reg_gp}},	// Add	ERn, 	ERm
	{&instr_add,		0xe080, 0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x007f, 0,  0x0006, &oper_imm}},		// Add	ERn, 	#imm7
	{&instr_addc,		0x8006,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// ADDC	Rn, 	Rm
	{&instr_addc,		0x6000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00ff, 0,  0x0000, &oper_imm}},		// ADDC	Rn, 	#imm8
	{&instr_and,		0x8002,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// AND	Rn, 	Rm
	{&instr_and,		0x2000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00ff, 0,  0x0000, &oper_imm}},		// AND	Rn,		#imm8
	{&instr_cmp,		0x8007,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// CMP	Rn,		Rm
	{&instr_cmp,		0x7000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00ff, 0,  0x0000, &oper_imm}},		// CMP	Rn,		#imm8
	{&instr_cmpc,		0x8005,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// CMPC	Rn,		Rm
	{&instr_cmpc,		0x5000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00ff, 0,  0x0000, &oper_imm}},		// CMPC	Rn,		#imm8
	{&instr_mov,		0xf005,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00e0, 4,  0x0002, &oper_reg_gp}},	// MOV	ERn,	ERm
	{&instr_mov,		0xe000,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x007f, 0,  0x0006, &oper_imm}},		// MOV	ERn,	#imm7
	{&instr_mov,		0x8000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// MOV	Rn,		Rm
	{&instr_mov,		0x0000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00ff, 0,  0x0000, &oper_imm}},		// MOV	Rn,		#imm8
	{&instr_or,			0x8003,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// OR	Rn,		Rm
	{&instr_or,			0x3000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00ff, 0,  0x0000, &oper_imm}},		// OR	Rn,		#imm8
	{&instr_xor,		0x8004,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// XOR	Rn,		Rm
	{&instr_xor,		0x4000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00ff, 0,  0x0000, &oper_imm}},		// XOR	Rn,		#imm8
	{&instr_cmp,		0xf007,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00e0, 4,  0x0002, &oper_reg_gp}},	// CMP	ERn,	ERm
	{&instr_sub,		0x8008,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// SUB	Rn,		Rm
	{&instr_subc,		0x8009,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// SUBC	Rn,		Rm

	// Shift Instructions
	{&instr_shift,		0x800a,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// SLL	Rn,		Rm
	{&instr_shift,		0x900a,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0070, 4,  0x0000, &oper_imm}},		// SLL	Rn,		#width
	{&instr_shift,		0x800b,	0x02,	{0x0f00, 8,  0x0012, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// SLLC	Rn,		Rm
	{&instr_shift,		0x900b,	0x02,	{0x0f00, 8,  0x0012, &oper_reg_gp},		{0x0070, 4,  0x0000, &oper_imm}},		// SLLC	Rn,		#width
	{&instr_shift,		0x800e,	0x05,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// SRA	Rn,		Rm
	{&instr_shift,		0x900e,	0x05,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0070, 4,  0x0000, &oper_imm}},		// SRA	Rn,		#width
	{&instr_shift,		0x800c,	0x01,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// SRL	Rn,		Rm
	{&instr_shift,		0x900c,	0x01,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0070, 4,  0x0000, &oper_imm}},		// SRL	Rn,		#width
	{&instr_shift,		0x800d,	0x03,	{0x0f00, 8,  0x0002, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// SRLC	Rn,		Rm
	{&instr_shift,		0x900d,	0x03,	{0x0f00, 8,  0x0002, &oper_reg_gp},		{0x0070, 4,  0x0000, &oper_imm}},		// SRLC	Rn,		#width

	// DSR Prefix Instructions
	{&instr_dsr,		0xe300,	0x00,	{0x00ff, 0,  0x0000, &oper_imm},		{.handler = NULL}},						// Use #imm8
	{&instr_dsr,		0x900f,	0x00,	{0x00f0, 4,  0x0001, &oper_reg_gp},		{.handler = NULL}},						// Use Rn
	{&instr_dsr,		0xfe9f,	0x00,	{0x0000, 0,  0x0006, &oper_reg_ctrl},						{.handler = NULL}},						// Use DSR register

	// Load/Store Instructions
	{&instr_load,		0x9032,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x0000, 0,  0x1001, &oper_mem}},		// L	ERn,	[EA]
	{&instr_load,		0x9052,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x0000, 0,  0x9001, &oper_mem}},		// L	ERn,	[EA+]
	{&instr_load,		0x9002,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00e0, 4,  0x1002, &oper_mem}},		// L	ERn,	[ERm]
	{&instr_load,		0xa008,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00e0, 4,  0x1102, &oper_mem}},		// L	ERn,	Disp16[ERm]
	{&instr_load,		0xb000,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x003f, 0,  0x1253, &oper_mem}},		// L	ERn,	Disp6[BP]
	{&instr_load,		0xb040,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x003f, 0,  0x1254, &oper_mem}},		// L	ERn,	Disp6[FP]
	{&instr_load,		0x9012,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x0000, 0,  0x1100, &oper_mem}},		// L	ERn,	Dadr
	{&instr_load,		0x9030,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x0001, &oper_mem}},		// L	Rn,		[EA]
	{&instr_load,		0x9050,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x8001, &oper_mem}},		// L	Rn,		[EA+]
	{&instr_load,		0x9000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00e0, 4,  0x0002, &oper_mem}},		// L	Rn,		[ERm]
	{&instr_load,		0x9008,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00e0, 4,  0x0102, &oper_mem}},		// L	Rn,		Disp16[ERm]
	{&instr_load,		0xd000,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x003f, 0,  0x0253, &oper_mem}},		// L	Rn,		Disp6[BP]
	{&instr_load,		0xd040,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x003f, 0,  0x0254, &oper_mem}},		// L	Rn,		Disp6[FP]
	{&instr_load,		0x9010,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x0100, &oper_mem}},		// L	Rn,		Dadr
	{&instr_load,		0x9034,	0x00,	{0x0c00, 8,  0x0004, &oper_reg_gp},		{0x0000, 0,  0x3001, &oper_mem}},		// L	Xr,		[EA]
	{&instr_load,		0x9054,	0x00,	{0x0c00, 8,  0x0004, &oper_reg_gp},		{0x0000, 0,  0xB001, &oper_mem}},		// L	Xr,		[EA+]
	{&instr_load,		0x9036,	0x00,	{0x0800, 8,  0x0008, &oper_reg_gp},		{0x0000, 0,  0x7001, &oper_mem}},		// L	Qr,		[EA]
	{&instr_load,		0x9056,	0x00,	{0x0800, 8,  0x0008, &oper_reg_gp},		{0x0000, 0,  0xF001, &oper_mem}},		// L	Qr,		[EA+]

	{&instr_store,		0x9033,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x0000, 0,  0x1001, &oper_mem}},		// ST	ERn,	[EA]
	{&instr_store,		0x9053,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x0000, 0,  0x9001, &oper_mem}},		// ST	ERn,	[EA+]
	{&instr_store,		0x9003,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00e0, 4,  0x1002, &oper_mem}},		// ST	ERn,	[ERm]
	{&instr_store,		0xa009,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00e0, 4,  0x1102, &oper_mem}},		// ST	ERn,	Disp16[ERm]
	{&instr_store,		0xb080,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x003f, 0,  0x1253, &oper_mem}},		// ST	ERn,	Disp6[BP]
	{&instr_store,		0xb0c0,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x003f, 0,  0x1254, &oper_mem}},		// ST	ERn,	Disp6[FP]
	{&instr_store,		0x9013,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x0000, 0,  0x1100, &oper_mem}},		// ST	ERn,	Dadr
	{&instr_store,		0x9031,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x0001, &oper_mem}},		// ST	Rn,		[EA]
	{&instr_store,		0x9051,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x8001, &oper_mem}},		// ST	Rn,		[EA+]
	{&instr_store,		0x9001,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00e0, 4,  0x0002, &oper_mem}},		// ST	Rn,		[ERm]
	{&instr_store,		0x9009,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x00e0, 4,  0x0102, &oper_mem}},		// ST	Rn,		Disp16[ERm]
	{&instr_store,		0xd080,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x003f, 0,  0x0253, &oper_mem}},		// ST	Rn,		Disp6[BP]
	{&instr_store,		0xd0c0,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x003f, 0,  0x0254, &oper_mem}},		// ST	Rn,		Disp6[FP]
	{&instr_store,		0x9011,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x0100, &oper_mem}},		// ST	Rn,		Dadr
	{&instr_store,		0x9035,	0x00,	{0x0c00, 8,  0x0004, &oper_reg_gp},		{0x0000, 0,  0x3001, &oper_mem}},		// ST	Xr,		[EA]
	{&instr_store,		0x9055,	0x00,	{0x0c00, 8,  0x0004, &oper_reg_gp},		{0x0000, 0,  0xB001, &oper_mem}},		// ST	Xr,		[EA+]
	{&instr_store,		0x9037,	0x00,	{0x0800, 8,  0x0008, &oper_reg_gp},		{0x0000, 0,  0x7001, &oper_mem}},		// ST	Qr,		[EA]
	{&instr_store,		0x9057,	0x00,	{0x0800, 8,  0x0008, &oper_reg_gp},		{0x0000, 0,  0xF001, &oper_mem}},		// ST	Qr,		[EA+]

	// Control Register Access Instructions
	{&instr_add,		0xe100,	0x01,	{0x0000, 0,  0x0024, &oper_reg_ctrl},	{0x00ff, 0,  0x0007, &oper_imm}},		// ADD	SP,		#signed8
	{&instr_mov,		0xa00f,	0x01,	{0x0000, 0,  0x0010, &oper_reg_ctrl},	{0x00f0, 4,  0x0001, &oper_reg_gp}},	// MOV	ECSR,	Rm
	{&instr_mov,		0xa00d,	0x01,	{0x0000, 0,  0x0021, &oper_reg_ctrl},	{0x0f00, 8,  0x0002, &oper_reg_gp}},	// MOV	ELR		ERm
	{&instr_mov,		0xa00c,	0x01,	{0x0000, 0,  0x0013, &oper_reg_ctrl},	{0x00f0, 4,  0x0001, &oper_reg_gp}},	// MOV	EPSW	Rm
	{&instr_mov,		0xa005,	0x01,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x0000, 0,  0x0021, &oper_reg_ctrl}},	// MOV	ERn		ELR
	{&instr_mov,		0xa01a,	0x01,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x0000, 0,  0x0024, &oper_reg_ctrl}},	// MOV	ERn		SP
	{&instr_mov,		0xa00b,	0x01,	{0x0000, 0,  0x0012, &oper_reg_ctrl},	{0x00f0, 4,  0x0001, &oper_reg_gp}},	// MOV	PSW		Rm
	{&instr_mov,		0xe900,	0x01,	{0x0000, 0,  0x0012, &oper_reg_ctrl},	{0x00ff, 0,  0x0000, &oper_imm}},		// MOV	PSW		#unsigned8
	{&instr_mov,		0xa007,	0x01,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x0010, &oper_reg_ctrl}},	// MOV	Rn		ECSR
	{&instr_mov,		0xa004,	0x01,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x0013, &oper_reg_ctrl}},	// MOV	Rn		EPSW
	{&instr_mov,		0xa003,	0x01,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0000, 0,  0x0012, &oper_reg_ctrl}},	// MOV	Rn		PSW
	{&instr_mov,		0xa10a,	0x01,	{0x0000, 0,  0x0024, &oper_reg_ctrl},	{0x00e0, 4,  0x0002, &oper_reg_gp}},	// MOV	SP		ERm

	// Push/Pop Instructions
	{&instr_push,		0xf05e,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{.handler = NULL}},						// PUSH	ERn
	{&instr_push,		0xf07e,	0x00,	{0x0800, 8,  0x0008, &oper_reg_gp},		{.handler = NULL}},						// PUSH	QRn
	{&instr_push,		0xf04e,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{.handler = NULL}},						// PUSH	Rn
	{&instr_push,		0xf06e,	0x00,	{0x0c00, 8,  0x0004, &oper_reg_gp},		{.handler = NULL}},						// PUSH	XRn
	{&instr_push_list,	0xf0ce,	0x00,	{0x0f00, 8,  0x0000, &oper_imm},		{.handler = NULL}},						// PUSH	register_list
	{&instr_pop,		0xf01e,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{.handler = NULL}},						// POP	ERn
	{&instr_pop,		0xf03e,	0x00,	{0x0800, 8,  0x0008, &oper_reg_gp},		{.handler = NULL}},						// POP	QRn
	{&instr_pop,		0xf00e,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{.handler = NULL}},						// POP	Rn
	{&instr_pop,		0xf02e,	0x00,	{0x0c00, 8,  0x0004, &oper_reg_gp},		{.handler = NULL}},						// POP	XRn
	{&instr_pop_list,	0xf08e,	0x00,	{0x0f00, 8,  0x0000, &oper_imm},		{.handler = NULL}},						// POP	register_list

	// TODO: Coprocessor Instructions

	// EA Register Data Transfer Instructions
	{&instr_lea,		0xf00a,	0x00,	{0x00e0, 4,  0x0002, &oper_mem},		{.handler = NULL}},						// LEA	[ERm]
	{&instr_lea,		0xf00b,	0x00,	{0x00e0, 4,  0x0102, &oper_mem},		{.handler = NULL}},						// LEA	Disp16[ERm]
	{&instr_lea,		0xf00c,	0x00,	{0x0000, 0,  0x0100, &oper_mem},		{.handler = NULL}},						// LEA	Dadr

	// ALU Instructions
	{&instr_daa,		0x801f,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{.handler = NULL}},						// DAA	Rn
	{&instr_das,		0x803f,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{.handler = NULL}},						// DAS	Rn
	{&instr_neg,		0x805f,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{.handler = NULL}},						// NEG	Rn

	// Bit Access Instructions
	{&instr_bit,		0xa000,	0x01,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0070, 4,  0x0000, &oper_imm}},		// SB	Rn.bit_offset
	{&instr_bit,		0xa080,	0x01,	{0x0000, 0,  0x0100, &oper_mem},		{0x0070, 4,  0x0000, &oper_imm}},		// SB	Dadr.bit_offset
	{&instr_bit,		0xa002,	0x02,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0070, 4,  0x0000, &oper_imm}},		// RB	Rn.bit_offset
	{&instr_bit,		0xa082,	0x02,	{0x0000, 0,  0x0100, &oper_mem},		{0x0070, 4,  0x0000, &oper_imm}},		// RB	Dadr.bit_offset
	{&instr_bit,		0xa001,	0x00,	{0x0f00, 8,  0x0001, &oper_reg_gp},		{0x0070, 4,  0x0000, &oper_imm}},		// TB	Rn.bit_offset
	{&instr_bit,		0xa081,	0x00,	{0x0000, 0,  0x0100, &oper_mem},		{0x0070, 4,  0x0000, &oper_imm}},		// TB	Dadr.bit_offset

	// PSW Access Instructions
	{&instr_ei,			0xed08,	0x00,	{.handler = NULL},						{.handler = NULL}},						// EI
	{&instr_di,			0xebf7,	0x00,	{.handler = NULL},						{.handler = NULL}},						// DI
	{&instr_sc,			0xed80,	0x00,	{.handler = NULL},						{.handler = NULL}},						// SC
	{&instr_rc,			0xeb7f,	0x00,	{.handler = NULL},						{.handler = NULL}},						// RC
	{&instr_cplc,		0xfecf,	0x00,	{.handler = NULL},						{.handler = NULL}},						// CPLC

	// Conditional Relative Branch Instructions
	{&instr_brel,		0xc000,	0x00,	{0x0f00, 8,	 0x0000, &oper_imm},		{0x00ff, 0,  0x0007, &oper_imm}},		// B*	Radr

	// Sign Extension Instruction
	{&instr_extbw,		0x810f,	0x00,   {0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00e0, 4,  0x0002, &oper_reg_gp}},	// EXTBW ERn

	// Software Interrupt Instructions
	{&instr_swi,		0xe500,	0x00,	{0x003f, 0,  0x0000, &oper_imm},		{.handler = NULL}},						// SWI	#snum
	{&instr_brk,		0xffff,	0x00,	{.handler = NULL},						{.handler = NULL}},						// BRK

	// Branch Instructions
	{&instr_b,			0xf000,	0x00,	{0x0f00, 8,  0x0000, &oper_imm},		{0x0000, 0,  0x0100, &oper_mem}},		// B	Cadr
	{&instr_b,			0xf002,	0x00,	{0x0000, 0,  0x0015, &oper_reg_ctrl},	{0x00e0, 4,  0x0002, &oper_mem}},		// B	ERn
	{&instr_bl,			0xf001,	0x00,	{0x0f00, 8,  0x0000, &oper_imm},		{0x0000, 0,  0x0100, &oper_mem}},		// BL	Cadr
	{&instr_bl,			0xf003,	0x00,	{0x0000, 0,  0x0015, &oper_reg_ctrl},	{0x00e0, 4,  0x0002, &oper_mem}},		// BL	ERn

	// Multiplication and Division Instructions
	{&instr_mul,		0xf004,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// MUL	ERn,	Rm
	{&instr_div,		0xf009,	0x00,	{0x0e00, 8,  0x0002, &oper_reg_gp},		{0x00f0, 4,  0x0001, &oper_reg_gp}},	// DIV	ERn,	Rm

	// Miscellaneous Instructions
	{&instr_inc,		0xfe2f,	0x00,	{0x0000, 0,  0x0001, &oper_mem},		{.handler = NULL}},						// INC	[EA]
	{&instr_dec,		0xfe3f,	0x00,	{0x0000, 0,  0x0001, &oper_mem},		{.handler = NULL}},						// DEC	[EA]
	{&instr_rt,			0xfe1f,	0x00,	{.handler = NULL},						{.handler = NULL}},						// RT
	{&instr_rti,		0xfe0f,	0x00,	{.handler = NULL},						{.handler = NULL}},						// RTI
	{&instr_nop,		0xfe8f,	0x00,	{.handler = NULL},						{.handler = NULL}},						// NOP
};

#define NUM_INSTRS sizeof(u8_instr_table)/sizeof(struct u8_instr)

struct u8_instr *u8_decode(uint16_t instr) {
	for (int x = 0; x < NUM_INSTRS; x++) {
		// Generate the mask
		uint16_t mask = 0;
		if (u8_instr_table[x].op0.handler != NULL) mask |= u8_instr_table[x].op0.mask; 
		if (u8_instr_table[x].op1.handler != NULL) mask |= u8_instr_table[x].op1.mask;
		mask ^= 0xffff;

		if ((instr & mask) == u8_instr_table[x].mask) return &u8_instr_table[x];
	}

	return NULL;
}

/* Operand Handlers */
struct u8_oper oper_reg_gp(struct u8_core *core, struct u8_instr_oper *oper, uint32_t val) {
	return (struct u8_oper){.type=OPER_REG_GP, .size=oper->flags & 0xf, .reg=(val - (oper->flags >> 4)) & 0x0f};
}

struct u8_oper oper_reg_ctrl(struct u8_core *core, struct u8_instr_oper *oper, uint32_t val) {
	return (struct u8_oper){.type=OPER_REG_CTRL, .size=oper->flags >> 4, .reg=oper->flags & 0xf};
}

struct u8_oper oper_mem(struct u8_core *core, struct u8_instr_oper *oper, uint32_t val) {
	// Get displacement
	uint16_t disp = 0;
	switch ((oper->flags & 0x0f00) >> 8) {
		case 0: break;							// No Displacement
		case 1: disp = u8_fetch(core); break;	// Next word
		case 2: disp = val; break;				// Displacement from instruction
	}

	// Sign extension for displacement
	uint8_t sext = (oper->flags & 0x00f0) >> 4;
	if (sext != 0 && (val >> sext) & 1) {
		disp |= (uint64_t)(-1) << sext;
	}

	// Get register from which offset is taken
	uint16_t reg_val = 0;
	switch (oper->flags & 0x000f) {
		case 0: break;	// No Register
		case 1: reg_val = core->regs.ea; break;			// EA
		case 2: reg_val = read_reg_er(core, val); break;	// ER from instruction	
		case 3: reg_val = read_reg_er(core, 12); break;	// BP
		case 4: reg_val = read_reg_er(core, 14); break;	// FP
	}

	uint16_t addr = reg_val + disp;

	// Get operand size
	uint8_t size = ((oper->flags & 0x7000) >> 12) + 1;

	// Increment EA
	if (oper->flags & 0x8000)
		core->regs.ea += size;
	
	return (struct u8_oper){.type=OPER_MEM, .size=size, .addr=addr};
}

struct u8_oper oper_imm(struct u8_core *core, struct u8_instr_oper *oper, uint32_t val) {
	// Do we need sign extension?
	if (oper->flags != 0 && (val >> oper->flags) & 1) {
		val |= (-1) << oper->flags;
	}

	return (struct u8_oper){.type=OPER_IMM, .size=0, .imm=val};
}

uint64_t oper_read(struct u8_core *core, struct u8_oper *oper) {
	switch (oper->type) {
		case OPER_REG_GP: return read_reg(core, oper->reg, oper->size);

		case OPER_REG_CTRL: switch (oper->reg) {
			case 0: return core->regs.ecsr[core->regs.psw & 3];
			case 1: return core->regs.elr[core->regs.psw & 3];
			case 2: return core->regs.psw;
			case 3: return core->regs.epsw[core->regs.psw & 3];
			case 4: return core->regs.sp;
			case 5: return core->regs.csr;
			case 6: return core->regs.dsr;
		}

		case OPER_MEM: return read_mem_data(core, core->cur_dsr, oper->addr, oper->size);

		case OPER_IMM: return (uint64_t) oper->imm;
	}
}

void oper_write(struct u8_core *core, struct u8_oper *oper, uint64_t val) {
	switch (oper->type) {
		case OPER_REG_GP:
			write_reg(core, oper->reg, oper->size, val);
			break;

		case OPER_REG_CTRL: switch (oper->reg) {
			case 0: core->regs.ecsr[core->regs.psw & 3] = val; break;
			case 1: core->regs.elr[core->regs.psw & 3] = val; break;
			case 2: core->regs.psw = val; break;
			case 3: core->regs.epsw[core->regs.psw & 3] = val; break;
			case 4: core->regs.sp = val; break;
			case 5: core->regs.csr = val; break;
		} break;

		case OPER_MEM:
			write_mem_data(core, core->cur_dsr, oper->addr, oper->size, val);
			break;

		case OPER_IMM: break;
	}
}
