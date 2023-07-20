#include <stdint.h>

#include "arch.h"
#include "regs.h"

/* Register Access */
uint64_t read_reg(struct u8_arch *arch, uint8_t reg, uint8_t size) {
	uint64_t val = 0;
	for (int x = 0; x < size; x++)
		val |= (uint64_t)arch->regs.gp[(reg + x) & 0x0f] << (8 * x);

	return val;
}

void write_reg(struct u8_arch *arch, uint8_t reg, uint8_t size, uint64_t val) {
	for (int x = 0; x < size; x++)
		arch->regs.gp[(reg + x) & 0x0f] = (uint8_t)(val >> (8 * x));
}

inline uint8_t read_reg_r(struct u8_arch *arch, uint8_t reg) {
	return (uint8_t)read_reg(arch, reg, 1);
}

inline uint16_t read_reg_er(struct u8_arch *arch, uint8_t reg) {
	return (uint16_t)read_reg(arch, reg, 2);
}

inline uint32_t read_reg_xr(struct u8_arch *arch, uint8_t reg) {
	return (uint32_t)read_reg(arch, reg, 4);
}

inline uint64_t read_reg_qr(struct u8_arch *arch, uint8_t reg) {
	return (uint64_t)read_reg(arch, reg, 8);
}

inline void write_reg_r(struct u8_arch *arch, uint8_t reg, uint8_t val) {
	write_reg(arch, reg, 1, val);
}

inline void write_reg_er(struct u8_arch *arch, uint8_t reg, uint16_t val) {
	write_reg(arch, reg, 2, val);
}

inline void write_reg_xr(struct u8_arch *arch, uint8_t reg, uint32_t val) {
	write_reg(arch, reg, 4, val);
}

inline void write_reg_qr(struct u8_arch *arch, uint8_t reg, uint64_t val) {
	write_reg(arch, reg, 8, val);
}