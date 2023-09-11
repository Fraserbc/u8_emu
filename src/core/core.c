#include <stdio.h>
#include <string.h>

#include "core.h"
#include "instr.h"
#include "mem.h"

void u8_reset(struct u8_core *core) {
	// Set all registers to 0
	memset(&core->regs, 0, sizeof(struct u8_regs));
	core->cur_dsr = 0;

	// Set SP and PC
	core->regs.sp = read_mem_data(core, 0, 0x0000, 2);
	core->regs.pc = read_mem_data(core, 0, 0x0002, 2);
}

/* Execute one instruction */
void u8_step(struct u8_core *core) {
	// Fetch the next instruction
	uint16_t instr_word = u8_fetch(core);

	// Decode the instruction
	struct u8_instr *instr = u8_decode(instr_word);

	if (instr == NULL) {
		printf("ERR: Invalid instruction %04x\n", instr_word);
		return;
	}

	// Operands
	struct u8_oper op0, op1;
	if (instr->op0.handler != NULL)
		op0 = instr->op0.handler(core, &instr->op0,
			(instr_word & instr->op0.mask) >> instr->op0.shift);

	if (instr->op1.handler != NULL)
		op1 = instr->op1.handler(core, &instr->op1,
			(instr_word & instr->op1.mask) >> instr->op1.shift);
	
	// Call the instruction handler
	instr->handler(core, instr->flags, &op0, &op1);
}

uint16_t u8_fetch(struct u8_core *core) {
	// Read from code memory at PC
	uint16_t instr = read_mem_code(core, core->regs.csr, core->regs.pc, 2);
	core->regs.pc += 2;

	return instr;
}