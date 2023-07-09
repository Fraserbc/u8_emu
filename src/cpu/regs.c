#include <stdint.h>

#include "cpu.h"

/* Rn */
inline uint8_t reg_get_r(u8_cpu *cpu, uint8_t n) {
	return cpu->regs.gp[n];
}

void reg_set_r(u8_cpu *cpu, uint8_t n, uint8_t val) {
	cpu->regs.gp[n] = val;
}

/* ERn */
inline uint16_t reg_get_er(u8_cpu *cpu, uint8_t n) {
	return reg_get_r(cpu, n) | (uint16_t)reg_get_r(cpu, n + 1) << 8;
}

inline void reg_set_er(u8_cpu *cpu, uint8_t n, uint16_t val) {
	reg_set_r(cpu, n, val & 0xFF);
	reg_set_r(cpu, n + 1, val >> 8);
}

/* XRn */
inline uint32_t reg_get_xr(u8_cpu *cpu, uint8_t n) {
	return reg_get_er(cpu, n) | (uint32_t)reg_get_er(cpu, n + 2) << 16;
}

inline void reg_set_xr(u8_cpu *cpu, uint8_t n, uint32_t val) {
	reg_set_er(cpu, n, val & 0xFFFF);
	reg_set_er(cpu, n + 2, val >> 16);
}

/* QRn */
inline uint64_t reg_get_qr(u8_cpu *cpu, uint8_t n) {
	return reg_get_xr(cpu, n) | (uint64_t)reg_get_xr(cpu, n + 4) << 32;
}

inline void reg_set_qr(u8_cpu *cpu, uint8_t n, uint64_t val) {
	reg_set_xr(cpu, n, val & 0xFFFFFFFF);
	reg_set_xr(cpu, n + 4, val >> 32);
}