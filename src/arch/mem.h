#ifndef U8_MEM_H
#define U8_MEM_H

#include <stdint.h>

struct u8_arch;

struct u8_mem {

};

uint64_t read_mem_data(struct u8_arch *arch, uint8_t dsr, uint16_t addr, uint8_t size);
void write_mem_data(struct u8_arch *arch, uint8_t dsr, uint16_t addr, uint8_t size, uint64_t val);

uint64_t read_mem_code(struct u8_arch *arch, uint8_t csr, uint16_t addr, uint8_t size);
void write_mem_code(struct u8_arch *arch, uint8_t csr, uint16_t addr, uint8_t size, uint64_t val);

void init_mem(uint8_t *code);

#endif