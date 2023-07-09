#pragma once
#include <stdint.h>

/* CPU Registers */
typedef struct {
	uint8_t gp[16];	// General purpose registers
	uint16_t pc;		// Program counter
	uint8_t csr;		// Code segment register
	uint8_t lcsr[3];	// CSR backup registers
	uint16_t lr;		// Link register
	uint16_t elr[3];	// Link backup registers
	uint8_t psw;		// Program status word
	uint8_t epsw[3];	// PSW backup registers
	uint16_t sp;		// Stack Pointer
	uint16_t ea;		// EA Register
	uint8_t dsr;		// Data segment register
} u8_cpu_regs;

/* CPU State */
typedef struct {
	u8_cpu_regs regs;

	uint8_t cur_dsr;

	uint8_t (*mem_read)(uint8_t, uint16_t);
	uint8_t (*mem_write)(uint8_t, uint16_t, uint8_t);
} u8_cpu;

/* Register Functions */
extern uint8_t reg_get_r(u8_cpu *cpu, uint8_t n);
extern void reg_set_r(u8_cpu *cpu, uint8_t n, uint8_t val);
extern uint16_t reg_get_er(u8_cpu *cpu, uint8_t n);
extern void reg_set_er(u8_cpu *cpu, uint8_t n, uint16_t val);
extern uint32_t reg_get_xr(u8_cpu *cpu, uint8_t n);
extern void reg_set_xr(u8_cpu *cpu, uint8_t n, uint32_t val);
extern uint64_t reg_get_qr(u8_cpu *cpu, uint8_t n);
extern void reg_set_qr(u8_cpu *cpu, uint8_t n, uint64_t val);