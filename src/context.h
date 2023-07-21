#ifndef U8_CTX_H
#define U8_CTX_H

#include <stdint.h>

#include "arch/regs.h"
#include "mem.h"
#include "peripheral.h"

struct u8_sim_ctx {
	struct u8_regs regs;
	uint8_t cur_dsr;
	struct u8_mem mem;
	struct u8_periph periph;

	uint8_t *code_mem;
	uint8_t *data_mem;
};

#endif