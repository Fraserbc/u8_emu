#ifndef U8_MEM_H
#define U8_MEM_H

#include <stdint.h>
#include <stdbool.h>

struct u8_core;

enum u8_mem_type {
	U8_REGION_BOTH,
	U8_REGION_DATA,
	U8_REGION_CODE
};

enum u8_mem_access {
	U8_MACC_ARR,
	U8_MACC_FUNC
};

struct u8_mem_reg {
	enum u8_mem_type type;
	bool rw;
	uint32_t addr_m;
	uint32_t addr_v;

	enum u8_mem_access acc;
	union {
		uint8_t *array;
		struct {
			uint8_t (*read)(struct u8_core *, uint8_t, uint16_t);
			void (*write)(struct u8_core *, uint8_t, uint16_t, uint8_t);
		};
	};
};

struct u8_mem {
	int num_regions;
	struct u8_mem_reg *regions;
};

uint64_t read_mem_data(struct u8_core *core, uint8_t dsr, uint16_t offset, uint8_t size);
void write_mem_data(struct u8_core *core, uint8_t dsr, uint16_t offset, uint8_t size, uint64_t val);

uint64_t read_mem_code(struct u8_core *core, uint8_t csr, uint16_t offset, uint8_t size);
void write_mem_code(struct u8_core *core, uint8_t csr, uint16_t offset, uint8_t size, uint64_t val);

void init_mem(uint8_t *code);

#endif