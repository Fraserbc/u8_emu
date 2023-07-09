#pragma once

#include <stdint.h>

#include "cpu.h"

/* Arithmetic Instructions */
#define U8_ADD_R_R       0x00
#define U8_ADD_R_IMM     0x01
#define U8_ADD_ER_ER     0x02
#define U8_ADD_ER_IMM    0x03
#define U8_ADDC_R_R      0x04
#define U8_ADDC_R_IMM    0x05
#define U8_AND_R_R       0x06
#define U8_AND_R_IMM     0x07
#define U8_CMP_R_R       0x08
#define U8_CMP_R_IMM     0x09
#define U8_CMPC_R_R      0x0A
#define U8_CMPC_R_IMM    0x0B
#define U8_MOV_ER_ER     0x0C
#define U8_MOV_ER_IMM    0x0D
#define U8_MOV_R_R       0x0E
#define U8_MOV_R_IMM     0x0F
#define U8_OR_R_R        0x10
#define U8_OR_R_IMM      0x11
#define U8_XOR_R_R       0x12
#define U8_XOR_R_IMM     0x13
#define U8_CMP_ER_ER     0x14
#define U8_SUB_R_R       0x15
#define U8_SUBC_R_R      0x16

/* Shift Instructions */
#define U8_SLL_R_R       0x17
#define U8_SLL_R_WIDTH   0x18
#define U8_SLLC_R_R      0x19
#define U8_SLLC_R_WIDTH  0x1A
#define U8_SRA_R_R       0x1B
#define U8_SRA_R_WIDTH   0x1C
#define U8_SRL_R_R       0x1D
#define U8_SRL_R_WIDTH   0x1E
#define U8_SRLC_R_R      0x1F
#define U8_SRLC_R_WIDTH  0x20

/* Load/Store Instructions */
#define U8_L_ER_EA       0x21
#define U8_L_ER_EA_P     0x22
#define U8_L_ER_ER       0x23
#define U8_L_ER_ER_D16   0x24
#define U8_L_ER_BP_D6    0x25
#define U8_L_ER_FP_D6    0x26
#define U8_L_ER_DA       0x27
#define U8_L_R_EA        0x28
#define U8_L_R_EA_P      0x29
#define U8_L_R_ER        0x2A
#define U8_L_R_ER_D16    0x2B
#define U8_L_R_BP_D6     0x2C
#define U8_L_R_FP_D6     0x2D
#define U8_L_R_DA        0x2E
#define U8_L_XR_EA       0x2F
#define U8_L_XR_EA_P     0x30
#define U8_L_QR_EA       0x31
#define U8_L_QR_EA_P     0x32

#define U8_ST_ER_EA      0x33
#define U8_ST_ER_EA_P    0x34
#define U8_ST_ER_ER      0x35
#define U8_ST_ER_ER_D16  0x36
#define U8_ST_ER_BP_D6   0x37
#define U8_ST_ER_FP_D6   0x38
#define U8_ST_ER_DA      0x39
#define U8_ST_R_EA       0x3A
#define U8_ST_R_EA_P     0x3B
#define U8_ST_R_ER       0x3C
#define U8_ST_R_ER_D16   0x3D
#define U8_ST_R_BP_D6    0x3E
#define U8_ST_R_FP_D6    0x3F
#define U8_ST_R_DA       0x40
#define U8_ST_XR_EA      0x41
#define U8_ST_XR_EA_P    0x42
#define U8_ST_QR_EA      0x43
#define U8_ST_QR_EA_P    0x44

/* Control Register Access Instructions */
#define U8_ADD_SP_IMM    0x45
#define U8_MOV_ECSR_R    0x46
#define U8_MOV_ELR_ER    0x47
#define U8_MOV_EPSW_R    0x48
#define U8_MOV_ERn_ELR   0x49
#define U8_MOV_ERn_SP    0x4A
#define U8_MOV_PSW_R     0x4B
#define U8_MOV_PSW_IMM   0x4C
#define U8_MOV_Rn_ECSR   0x4D
#define U8_MOV_Rn_EPSW   0x4E
#define U8_MOV_Rn_PSW    0x4F
#define U8_MOV_SP_ER     0x50

/* Push/Pop Instructions */
#define U8_PUSH_ER       0x51
#define U8_PUSH_QR       0x52
#define U8_PUSH_R        0x53
#define U8_PUSH_XR       0x54
#define U8_PUSH_REGLIST  0x55
#define U8_POP_ER        0x56
#define U8_POP_QR        0x57
#define U8_POP_R         0x58
#define U8_POP_XR        0x59
#define U8_POP_REGLIST   0x5A

/* Coprocessor Data Transfer Instructions */
#define U8_MOV_CR_R      0x5B
#define U8_MOV_CER_EA    0x5C
#define U8_MOV_CER_EA_P  0x5D
#define U8_MOV_CR_EA     0x5E
#define U8_MOV_CR_EA_P   0x5F
#define U8_MOV_CXR_EA    0x60
#define U8_MOV_CXR_EA_P  0x61
#define U8_MOV_CQR_EA    0x62
#define U8_MOV_CQR_EA_P  0x63
#define U8_MOV_R_CR      0x64
#define U8_MOV_EA_CER    0x65
#define U8_MOV_EA_P_CER  0x66
#define U8_MOV_EA_CR     0x67
#define U8_MOV_EA_P_CR   0x68
#define U8_MOV_EA_CXR    0x69
#define U8_MOV_EA_P_CXR  0x6A
#define U8_MOV_EA_CQR    0x6B
#define U8_MOV_EA_P_CQR  0x6C

/* EA Register Data Transfer Instructions */
#define U8_LEA_ER        0x6D
#define U8_LEA_ER_D16    0x6E
#define U8_LEA_ER_DA     0x6F

/* ALU Instructions */
#define U8_DAA_R         0x70
#define U8_DAS_R         0x71
#define U8_NEG_R         0x72

/* Bit Access Instructions */
#define U8_SB_R          0x73
#define U8_SB_DA         0x74
#define U8_RB_R          0x75
#define U8_RB_DA         0x76
#define U8_TB_R          0x77
#define U8_TB_DA         0x78

/* PSW Access Instructions */
#define U8_EI            0x79
#define U8_DI            0x7A
#define U8_SC            0x7B
#define U8_RC            0x7C
#define U8_CPLC          0x7D

/* Conditional Relative Branch Instructions */
#define U8_BGE_DA        0x7E
#define U8_BLT_DA        0x7F
#define U8_BGT_DA        0x80
#define U8_BLE_DA        0x81
#define U8_BGES_DA       0x82
#define U8_BLTS_DA       0x83
#define U8_BGTS_DA       0x84
#define U8_BLES_DA       0x85
#define U8_BNE_DA        0x86
#define U8_BEQ_DA        0x87
#define U8_BNV_DA        0x88
#define U8_BOV_DA        0x89
#define U8_BPS_DA        0x8A
#define U8_BNS_DA        0x8B
#define U8_BAL_DA        0x8C

/* Sign Extension Instruction */
#define U8_EXTBW_ER      0x8D

/* Software Interrupt Instruction */
#define U8_SW_NUM        0x8E
#define U8_BRK           0x8F

/* Branch Instructions */
#define U8_B_DA          0x90
#define U8_B_ER          0x91
#define U8_BL_DA         0x92
#define U8_BL_ER         0x93

/* Multiplication and Division Instructions */
#define U8_MUL_ER_R      0x94
#define U8_DIV_ER_R      0x95

/* Miscellaneous */
#define U8_INC_EA        0x96
#define U8_DEC_EA        0x97
#define U8_RT            0x98
#define U8_RTI           0x99
#define U8_NOP           0x9A

/* DSR Prefix Instructions */
#define U8_DSR_IMM       0x9B
#define U8_DSR_R         0x9C
#define U8_DSR_CUR       0x9D

/* Instruction Flags */
#define U8_INSTR_INC_EA  0x01

/* Operand Handler */
typedef struct {
	uint64_t (*get)(u8_cpu *, uint8_t, uint16_t);
	void (*set)(u8_cpu *, uint8_t, uint64_t, uint16_t);
} u8_oper_handler_t;

/* Instruction Operand */
typedef struct {
	uint32_t mask;
	u8_oper_handler_t *handler;
} u8_instr_oper_t;

/* Instruction */
typedef struct {
	void (*handler)(u8_cpu *, void *, void *);
	uint8_t len;
	uint8_t flags;
	uint32_t instr_mask;
	u8_instr_oper_t op0;
	u8_instr_oper_t op1;
} u8_instr_t;