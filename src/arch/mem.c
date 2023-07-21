#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "arch.h"
#include "mem.h"

uint8_t *code_mem;
uint8_t *data_mem;

void init_mem(uint8_t *code) {
	code_mem = code;
	data_mem = malloc(0x8000);
}

// Data memory reads
int count = 0;

uint64_t read_mem_data(struct u8_arch *arch, uint8_t dsr, uint16_t addr, uint8_t size) {
	switch (dsr) {
		case 0: {
			if (addr & 0x8000) {
				uint64_t val = 0;
				for (int x = 0; x < size; x++)
					val |= (uint64_t)data_mem[addr - 0x8000 + x] << (8 * x);
				return val;
			} else
				return read_mem_code(arch, 0, addr, size);
		};
		case 1: return read_mem_code(arch, 1, addr, size);
		case 8: return read_mem_code(arch, 0, addr, size);
		default: return 0x00;
	}
}

// Data memory writes
void write_mem_data(struct u8_arch *arch, uint8_t dsr, uint16_t addr, uint8_t size, uint64_t val) {
	if (dsr != 0 || ((addr & 0x8000) == 0)) return;
	for (int x = 0; x < size; x++)
		data_mem[addr - 0x8000 + x] = (val >> (8 * x)) & 0xff;
}

uint64_t read_mem_code(struct u8_arch *arch, uint8_t csr, uint16_t addr, uint8_t size) {
	uint64_t val = 0;
	for (int x = 0; x < size; x++)
		val |= (uint64_t)code_mem[(csr << 16) + addr + x] << (8 * x);
	return val;
}

void write_mem_code(struct u8_arch *arch, uint8_t csr, uint16_t addr, uint8_t size, uint64_t val) {
	return;
}