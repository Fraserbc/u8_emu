#pragma once

#include "cpu.h"
#include "instr.h"

typedef enum {
	Rn,
	ERn,
	XRn,
	QRn,
	EA_PTR,
	ERn_PTR,
	ERn_D16,
	BP_D6,
	FP_D6,
	LONG_IMM
} u8_oper_type;

typedef struct {
	u8_oper_type type;
	uint8_t reg_idx;
	uint64_t val;
} u8_instr_oper_t;

typedef struct {
	u8_instr_oper_t operands[2];
	uint64_t long_imm;		// Long immediate value
} u8_instr_opers_t;

uint64_t oper_get(u8_cpu *cpu, u8_instr_opers_t opers, uint8_t idx);
void oper_set(u8_cpu *cpu, u8_instr_opers_t opers, uint8_t idx, uint64_t val);