#ifndef U8_CTX_H
#define U8_CTX_H

#include <stdio.h>
#include <stdint.h>

#include "core/core.h"
#include "core/regs.h"
#include "core/mem.h"
#include "peripheral.h"

struct u8_sim_ctx {
	struct u8_core core;
	struct u8_periph periph;

	uint8_t *code_mem;
	uint8_t *data_mem;

	FILE *log_file;
};

#endif