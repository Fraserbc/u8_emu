#ifndef U8_MEM_H
#define U8_MEM_H

struct u8_mem {

};

uint8_t read_mem_data_8(struct u8_mem *mem, uint8_t dsr, uint16_t addr);
uint16_t read_mem_data_16(struct u8_mem *mem, uint8_t dsr, uint16_t addr);
uint32_t read_mem_data_32(struct u8_mem *mem, uint8_t dsr, uint16_t addr);
uint64_t read_mem_data_64(struct u8_mem *mem, uint8_t dsr, uint16_t addr);

uint8_t read_mem_code_8(struct u8_mem *mem, uint8_t csr, uint16_t addr);
uint16_t read_mem_code_16(struct u8_mem *mem, uint8_t csr, uint16_t addr);

#endif