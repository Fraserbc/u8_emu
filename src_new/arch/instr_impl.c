#include <stdio.h>

#include "arch.h"
#include "instr.h"

#define INSTR_NOT_IMPL(name) printf("ERR: '%s' not implemented\n", name)

// Arithmetic Instructions
void instr_add(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("ADD");
}

void instr_addc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("ADDC");
}

void instr_and(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("AND");
}

void instr_cmp(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("CMP");
}

void instr_cmpc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("CMPC");
}

void instr_mov(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("MOV");
}

void instr_or(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("OR");
}

void instr_xor(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("XOR");
}

void instr_sub(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SUB");
}

void instr_subc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SUBC");
}

// Shift Instructions
void instr_sll(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SLL");
}

void instr_sllc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SLLC");
}

void instr_sra(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SRA");
}

void instr_srl(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SRL");
}

void instr_srlc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SRLC");
}

// DSR Prefix Instruction
void instr_dsr(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("DSR Prefix");
}

// Load/Store Instructions
void instr_load(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("L");
}

void instr_store(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("ST");
}

// Push/Pop Instructions
void instr_push(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("PUSH");
}

void instr_push_list(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("PUSH List");
}

void instr_pop(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("POP");
}

void instr_pop_list(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("POP List");
}

// TODO: Coprocessor Instructions
// Either just error out or actually implement

// EA Register Data Transfer Instructions
void instr_lea(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("LEA");
}

// ALU Instructions
void instr_daa(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("DAA");
}

void instr_das(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("DAS");
}

void instr_neg(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("NEG");
}

// Bit Access Instructions
void instr_sb(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SB");
}

void instr_rb(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("RB");
}

void instr_tb(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("TB");
}

// PSW Access Instructions
void instr_ei(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("EI");
}

void instr_di(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("DI");
}

void instr_sc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SC");
}

void instr_rc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("RC");
}

void instr_cplc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("CPLC");
}

// Conditional Relative Branch Instructions
void instr_brel(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("BREL");
}

// Sign Extension Instruction
void instr_extbw(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("EXTBW");
}

// Software Interrupt Instructions
void instr_swi(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SWI");
}

void instr_brk(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("BRK");
}

// Branch Instructions
void instr_b(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("B");
}

void instr_bl(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("BL");
}

// Multiplication and Division Instructions
void instr_mul(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("MUL");
}

void instr_div(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("DIV");
}

// Miscellaneous Instructions
void instr_inc(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("INC");
}

void instr_dec(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("DEC");
}

void instr_rt(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("RT");
}

void instr_rti(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("RTI");
}

void instr_nop(struct u8_arch *arch, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("NOP");
}
