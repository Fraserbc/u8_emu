#ifndef U8_CORE_H
#define U8_CORE_H

#include <stdint.h>

#include "regs.h"
#include "mem.h"

struct u8_core {
	struct u8_regs regs;
	uint8_t cur_dsr;
	struct u8_mem mem;
};

void u8_step(struct u8_core *core);
uint16_t u8_fetch(struct u8_core *core);

#endif