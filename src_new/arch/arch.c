#include "arch.h"
#include "instr.h"
#include "mem.h"

#include <stdio.h>

/* Execute one instruction */
void u8_step(struct u8_arch *arch) {
	// Fetch the next instruction
	uint16_t instr_word = u8_fetch(arch);

	// Decode the instruction
	struct u8_instr *instr = u8_decode(instr_word);

	uint32_t val;

	// Operand 0
	val = instr_word;
	val &= instr->op0.mask;
	val >>= instr->op0.shift;

	struct u8_oper op0 = instr->op0.handler(arch, &instr->op0, val);

	// Operand 1
	val = instr_word;
	val &= instr->op1.mask;
	val >>= instr->op1.shift;
	
	struct u8_oper op1 = instr->op1.handler(arch, &instr->op0, val);
	
	// Call the instruction handler
	instr->handler(arch, &op0, &op1);
}

uint16_t u8_fetch(struct u8_arch *arch) {
	// Read from code memory at PC
	uint16_t instr = read_mem_code_16(&arch->mem, arch->regs.csr, arch->regs.pc);
	arch->regs.pc += 2;

	return instr;
}