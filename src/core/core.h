#ifndef U8_CORE_H
#define U8_CORE_H

#include "../context.h"

void u8_step(struct u8_sim_ctx *ctx);
uint16_t u8_fetch(struct u8_sim_ctx *ctx);

#endif