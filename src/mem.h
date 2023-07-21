#ifndef U8_MEM_H
#define U8_MEM_H

#include <stdint.h>

struct u8_sim_ctx;

enum u8_mem_type {
	U8_REGION_BOTH,
	U8_REGION_DATA,
	U8_REGION_CODE
};

struct u8_mem_reg {
	enum u8_mem_type type;
	uint32_t addr_l;
	uint32_t addr_h;
	uint8_t (*read)(struct u8_sim_ctx *, uint8_t, uint16_t);
	void (*write)(struct u8_sim_ctx *, uint8_t, uint16_t, uint8_t);
};

struct u8_mem {
	int num_regions;
	struct u8_mem_reg *regions;
};

uint64_t read_mem_data(struct u8_sim_ctx *ctx, uint8_t dsr, uint16_t offset, uint8_t size);
void write_mem_data(struct u8_sim_ctx *ctx, uint8_t dsr, uint16_t offset, uint8_t size, uint64_t val);

uint64_t read_mem_code(struct u8_sim_ctx *ctx, uint8_t csr, uint16_t offset, uint8_t size);
void write_mem_code(struct u8_sim_ctx *ctx, uint8_t csr, uint16_t offset, uint8_t size, uint64_t val);

void init_mem(uint8_t *code);

#endif