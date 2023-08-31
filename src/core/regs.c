#include <stdint.h>

#include "regs.h"

#include "../context.h"

/* Register Access */
uint64_t read_reg(struct u8_sim_ctx *ctx, uint8_t reg, uint8_t size) {
	uint64_t val = 0;
	for (int x = 0; x < size; x++)
		val |= (uint64_t)ctx->regs.gp[(reg + x) & 0x0f] << (8 * x);

	return val;
}

void write_reg(struct u8_sim_ctx *ctx, uint8_t reg, uint8_t size, uint64_t val) {
	for (int x = 0; x < size; x++)
		ctx->regs.gp[(reg + x) & 0x0f] = (uint8_t)(val >> (8 * x));
}

uint8_t read_reg_r(struct u8_sim_ctx *ctx, uint8_t reg) {
	return (uint8_t)read_reg(ctx, reg, 1);
}

uint16_t read_reg_er(struct u8_sim_ctx *ctx, uint8_t reg) {
	return (uint16_t)read_reg(ctx, reg, 2);
}

uint32_t read_reg_xr(struct u8_sim_ctx *ctx, uint8_t reg) {
	return (uint32_t)read_reg(ctx, reg, 4);
}

uint64_t read_reg_qr(struct u8_sim_ctx *ctx, uint8_t reg) {
	return (uint64_t)read_reg(ctx, reg, 8);
}

void write_reg_r(struct u8_sim_ctx *ctx, uint8_t reg, uint8_t val) {
	write_reg(ctx, reg, 1, val);
}

void write_reg_er(struct u8_sim_ctx *ctx, uint8_t reg, uint16_t val) {
	write_reg(ctx, reg, 2, val);
}

void write_reg_xr(struct u8_sim_ctx *ctx, uint8_t reg, uint32_t val) {
	write_reg(ctx, reg, 4, val);
}

void write_reg_qr(struct u8_sim_ctx *ctx, uint8_t reg, uint64_t val) {
	write_reg(ctx, reg, 8, val);
}