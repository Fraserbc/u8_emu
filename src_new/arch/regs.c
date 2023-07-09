#include <stdint.h>

#include "regs.h"

/* Register Access */
inline uint8_t read_reg_r(struct u8_cpu_regs *regs, uint8_t reg) {
	return regs->gp[reg];
}

inline uint16_t read_reg_er(struct u8_cpu_regs *regs, uint8_t reg) {
	return (uint16_t)read_reg_r(regs, reg) | (uint16_t)read_reg_r(regs, reg + 1) << 8;
}

inline uint32_t read_reg_xr(struct u8_cpu_regs *regs, uint8_t reg) {
	return (uint32_t)read_reg_er(regs, reg) | (uint32_t)read_reg_er(regs, reg + 2) << 8;
}

inline uint64_t read_reg_qr(struct u8_cpu_regs *regs, uint8_t reg) {
	return (uint64_t)read_reg_xr(regs, reg) | (uint64_t)read_reg_xr(regs, reg + 4) << 16;
}

inline void write_reg_r(struct u8_cpu_regs *regs, uint8_t reg, uint8_t val) {
	regs->gp[reg] = val;
}

inline void write_reg_er(struct u8_cpu_regs *regs, uint8_t reg, uint16_t val) {
	write_reg_r(regs, reg, val & 0xFF);
	write_reg_r(regs, reg + 1, val >> 8);
}

inline void write_reg_xr(struct u8_cpu_regs *regs, uint8_t reg, uint32_t val) {
	write_reg_er(regs, reg, val & 0xFFFF);
	write_reg_er(regs, reg + 2, val >> 16);
}

inline void write_reg_qr(struct u8_cpu_regs *regs, uint8_t reg, uint64_t val) {
	write_reg_xr(regs, reg, val & 0xFFFFFFFF);
	write_reg_xr(regs, reg + 4, val >> 32);
}