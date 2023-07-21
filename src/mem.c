#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "context.h"
#include "mem.h"

uint64_t read_mem(struct u8_sim_ctx *ctx, uint8_t seg, uint16_t offset, uint8_t size, enum u8_mem_type type) {
	// Find region
	uint32_t addr = ((uint32_t)seg << 16) | offset;
	for (int i = 0; i < ctx->mem.num_regions; i++) {
		if (addr >= ctx->mem.regions[i].addr_l && addr <= ctx->mem.regions[i].addr_h && ctx->mem.regions[i].type != type) {
			if (ctx->mem.regions[i].read == NULL) return 0x00;
			uint64_t val = 0;
			for (int x = 0; x < size; x++)
				val |= (uint64_t)ctx->mem.regions[i].read(ctx, seg, offset + x) << (8 * x);
			return val;
		}
	}

	// Not in any memory regions, return 0
	return 0x00;
}

uint64_t read_mem_data(struct u8_sim_ctx *ctx, uint8_t dsr, uint16_t offset, uint8_t size) {
	return read_mem(ctx, dsr, offset, size, U8_REGION_CODE);
}

uint64_t read_mem_code(struct u8_sim_ctx *ctx, uint8_t csr, uint16_t offset, uint8_t size) {
	return read_mem(ctx, csr, offset, size, U8_REGION_DATA);
}

void write_mem(struct u8_sim_ctx *ctx, uint8_t seg, uint16_t offset, uint8_t size, uint64_t val, enum u8_mem_type type) {
	// Find region
	uint32_t addr = (seg << 16) | offset;
	for (int i = 0; i < ctx->mem.num_regions; i++) {
		if (addr >= ctx->mem.regions[i].addr_l && addr <= ctx->mem.regions[i].addr_h && ctx->mem.regions[i].type != type) {
			if (ctx->mem.regions[i].write == NULL) return;
			for (int x = 0; x < size; x++)
				ctx->mem.regions[i].write(ctx, seg, offset + x, (val >> (8 * x)) & 0xff);
		}
	}
}

void write_mem_data(struct u8_sim_ctx *ctx, uint8_t dsr, uint16_t offset, uint8_t size, uint64_t val) {
	write_mem(ctx, dsr, offset, size, val, U8_REGION_CODE);
}

void write_mem_code(struct u8_sim_ctx *ctx, uint8_t csr, uint16_t offset, uint8_t size, uint64_t val) {
	write_mem(ctx, csr, offset, size, val, U8_REGION_DATA);
}