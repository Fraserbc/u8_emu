#include <stdint.h>

#include "mem.h"

uint8_t read_mem_data_8(struct u8_mem *mem, uint8_t dsr, uint16_t addr) {

}

inline uint16_t read_mem_data_16(struct u8_mem *mem, uint8_t dsr, uint16_t addr) {
	return (uint16_t)read_mem_data_8(mem, dsr, addr) | (uint16_t)read_mem_data_8(mem, dsr, addr + 1) << 8;
}

inline uint32_t read_mem_data_32(struct u8_mem *mem, uint8_t dsr, uint16_t addr) {
	return (uint32_t)read_mem_data_16(mem, dsr, addr) | (uint32_t)read_mem_data_16(mem, dsr, addr + 2) << 16;
}

inline uint64_t read_mem_data_64(struct u8_mem *mem, uint8_t dsr, uint16_t addr) {
	return (uint64_t)read_mem_data_32(mem, dsr, addr) | (uint64_t)read_mem_data_32(mem, dsr, addr + 4) << 32;
}

uint8_t code_mem[] = { 0x94, 0x52, 0xDE, 0xAD};

uint8_t read_mem_code_8(struct u8_mem *mem, uint8_t csr, uint16_t addr) {
	return code_mem[addr];
}

inline uint16_t read_mem_code_16(struct u8_mem *mem, uint8_t csr, uint16_t addr) {
	return (uint16_t)read_mem_code_8(mem, csr, addr) << 8 | (uint16_t)read_mem_code_8(mem, csr, addr + 1);
}