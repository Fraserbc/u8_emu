#ifndef U8_INSTR_H
#define U8_INSTR_H

#include <stdlib.h>
#include <stdint.h>

#include "../context.h"

// TODO: Rename these structs, their names are bad

/* Instruction Operand */
enum u8_oper_type {
	OPER_REG_GP,
	OPER_REG_CTRL,
	OPER_MEM,
	OPER_IMM,
};

struct u8_oper {
	enum u8_oper_type type;
	size_t size;
	union {
		uint32_t imm;	// Immediate value
		uint8_t reg;	// Register index
		uint32_t addr;	// Address of value
	};
};

/* Instruction Specification */
struct u8_instr_oper {
	uint32_t mask;
	int8_t shift;
	uint16_t flags;
	struct u8_oper (*handler)(struct u8_sim_ctx *, struct u8_instr_oper *, uint32_t);
};

struct u8_instr {
	void (*handler)(struct u8_sim_ctx *, uint8_t, struct u8_oper *, struct u8_oper *);
	uint16_t mask;
	uint8_t flags;
	struct u8_instr_oper op0;
	struct u8_instr_oper op1;
};

/* Instruction Dispatch */
// TODO: Struct containing handler and instr_oper

struct u8_instr *u8_decode(uint16_t instr);

/* Operand Handlers */
struct u8_oper oper_reg_gp(struct u8_sim_ctx *ctx, struct u8_instr_oper *oper, uint32_t val);	// General purpose registers
struct u8_oper oper_reg_ctrl(struct u8_sim_ctx *ctx, struct u8_instr_oper *oper, uint32_t val);	// Control registers
struct u8_oper oper_mem(struct u8_sim_ctx *ctx, struct u8_instr_oper *oper, uint32_t val);		// Memory access
struct u8_oper oper_imm(struct u8_sim_ctx *ctx, struct u8_instr_oper *oper, uint32_t val);		// Immediate value

uint64_t oper_read(struct u8_sim_ctx *ctx, struct u8_oper *oper);
void oper_write(struct u8_sim_ctx *ctx, struct u8_oper *oper, uint64_t val);

/* Instruction Handlers */
// Arithmetic Instructions
void instr_add(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_addc(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_and(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_cmp(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_cmpc(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_mov(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_or(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_xor(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_sub(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_subc(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Shift Instructions
void instr_shift(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// DSR Prefix Instruction
void instr_dsr(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Load/Store Instructions
void instr_load(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_store(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Push/Pop Instructions
void instr_push(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_push_list(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_pop(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_pop_list(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// TODO: Coprocessor Instructions
// Either just error out or actually implement

// EA Register Data Transfer Instructions
void instr_lea(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// ALU Instructions
void instr_daa(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_das(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_neg(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Bit Access Instructions
void instr_sb(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_rb(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_tb(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// PSW Access Instructions
void instr_ei(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_di(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_sc(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_rc(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_cplc(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Conditional Relative Branch Instructions
void instr_brel(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);	// Bits 8-11 used as immediate operand

// Sign Extension Instruction
void instr_extbw(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Software Interrupt Instructions
void instr_swi(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_brk(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Branch Instructions
void instr_b(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_bl(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Multiplication and Division Instructions
void instr_mul(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_div(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

// Miscellaneous Instructions
void instr_inc(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_dec(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_rt(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_rti(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);
void instr_nop(struct u8_sim_ctx *ctx, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1);

#endif