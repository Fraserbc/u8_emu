#ifndef U8_REGS_H
#define U8_REGS_H

#include <stdint.h>

struct u8_sim_ctx;

/* CPU Registers */
struct u8_regs {
	uint8_t gp[16];		// General purpose registers
	uint16_t pc;		// Program counter
	uint8_t csr;		// Code segment register
	uint8_t lcsr;
	uint8_t ecsr[3];	// CSR backup registers
	uint16_t lr;		// Link register
	uint16_t elr[3];	// Link backup registers
	uint8_t psw;		// Program status word
	uint8_t epsw[3];	// PSW backup registers
	uint16_t sp;		// Stack Pointer
	uint16_t ea;		// EA Register
	uint8_t dsr;		// Data segment register
};

/* Register Access */
uint64_t read_reg(struct u8_sim_ctx *ctx, uint8_t reg, uint8_t size) ;
void write_reg(struct u8_sim_ctx *ctx, uint8_t reg, uint8_t size, uint64_t val);

uint8_t read_reg_r(struct u8_sim_ctx *ctx, uint8_t reg);
uint16_t read_reg_er(struct u8_sim_ctx *ctx, uint8_t reg);
uint32_t read_reg_xr(struct u8_sim_ctx *ctx, uint8_t reg);
uint64_t read_reg_qr(struct u8_sim_ctx *ctx, uint8_t reg);

void write_reg_r(struct u8_sim_ctx *ctx, uint8_t reg, uint8_t val);
void write_reg_er(struct u8_sim_ctx *ctx, uint8_t reg, uint16_t val);
void write_reg_xr(struct u8_sim_ctx *ctx, uint8_t reg, uint32_t val);
void write_reg_qr(struct u8_sim_ctx *ctx, uint8_t reg, uint64_t val);

#endif