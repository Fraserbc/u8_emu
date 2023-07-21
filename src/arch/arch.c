#include "arch.h"
#include "instr.h"
#include "../context.h"
#include "../mem.h"

#include <stdio.h>

/* Execute one instruction */
void u8_step(struct u8_sim_ctx *ctx) {
	// Fetch the next instruction
	uint16_t instr_word = u8_fetch(ctx);

	// Decode the instruction
	struct u8_instr *instr = u8_decode(instr_word);

	if (instr == NULL) {
		printf("ERR: Invalid instruction %04x\n", instr_word);
		return;
	}

	// Operands
	struct u8_oper op0, op1;
	if (instr->op0.handler != NULL)
		op0 = instr->op0.handler(ctx, &instr->op0,
			(instr_word & instr->op0.mask) >> instr->op0.shift);

	if (instr->op1.handler != NULL)
		op1 = instr->op1.handler(ctx, &instr->op1,
			(instr_word & instr->op1.mask) >> instr->op1.shift);
	
	// Call the instruction handler
	instr->handler(ctx, instr->flags, &op0, &op1);
}

uint16_t u8_fetch(struct u8_sim_ctx *ctx) {
	// Read from code memory at PC
	uint16_t instr = read_mem_code(ctx, ctx->regs.csr, ctx->regs.pc, 2);
	ctx->regs.pc += 2;

	return instr;
}