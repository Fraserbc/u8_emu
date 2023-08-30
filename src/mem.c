#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "context.h"
#include "mem.h"

uint64_t access_mem(struct u8_sim_ctx *ctx, uint8_t seg, uint16_t offset, uint8_t size, enum u8_mem_type type, bool rw, uint64_t val) {
	// Find region
	uint32_t addr = ((uint32_t)seg << 16) | offset;
	for (int i = 0; i < ctx->mem.num_regions; i++) {
		// Is the address in the current region?
		if ((addr & ctx->mem.regions[i].addr_m) == ctx->mem.regions[i].addr_v &&
		   (ctx->mem.regions[i].type == type || ctx->mem.regions[i].type == U8_REGION_BOTH)) {

			if (rw && !ctx->mem.regions[i].rw) return 0;

			addr &= 0xFFFFF ^ ctx->mem.regions[i].addr_m;

			// Read each byte in little endian order
			if (!rw) val = 0;
			for (int x = 0; x < size; x++)
				switch (ctx->mem.regions[i].acc) {
					// Array
					case U8_MACC_ARR: {
						if (rw) {
							ctx->mem.regions[i].array[addr + x] = (val >> (8 * x)) & 0xff;
						} else {
							val |= (uint64_t)ctx->mem.regions[i].array[addr + x] << (8 * x);
						}
						break;
					}

					// Function Pointer
					case U8_MACC_FUNC: {
						if (rw) {
							ctx->mem.regions[i].write(ctx, seg, addr + x, (val >> (8 * x)) & 0xff);
						} else {
							val |= (uint64_t)ctx->mem.regions[i].read(ctx, seg, addr + x) << (8 * x);
						}
						
						break;
					}
				}
			
			return val;
		}
	}

	// Not in any memory regions, return 0
	return 0x00;
}

uint64_t read_mem_data(struct u8_sim_ctx *ctx, uint8_t dsr, uint16_t offset, uint8_t size) {
	return access_mem(ctx, dsr, offset, size, U8_REGION_DATA, FALSE, 0);
}

uint64_t read_mem_code(struct u8_sim_ctx *ctx, uint8_t csr, uint16_t offset, uint8_t size) {
	return access_mem(ctx, csr, offset, size, U8_REGION_CODE, FALSE, 0);
}

void write_mem_data(struct u8_sim_ctx *ctx, uint8_t dsr, uint16_t offset, uint8_t size, uint64_t val) {
	access_mem(ctx, dsr, offset, size, U8_REGION_DATA, TRUE, val);
}

void write_mem_code(struct u8_sim_ctx *ctx, uint8_t csr, uint16_t offset, uint8_t size, uint64_t val) {
	access_mem(ctx, csr, offset, size, U8_REGION_CODE, TRUE, val);
}