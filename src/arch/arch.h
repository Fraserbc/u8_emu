#ifndef U8_ARCH_H
#define U8_ARCH_H

#include "regs.h"
#include "mem.h"

struct u8_arch {
	struct u8_regs regs;
	uint8_t cur_dsr;
	struct u8_mem mem;
};

void u8_step(struct u8_arch *arch);
uint16_t u8_fetch(struct u8_arch *arch);

#endif