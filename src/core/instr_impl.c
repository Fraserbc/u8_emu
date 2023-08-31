#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "core.h"
#include "instr.h"
#include "mem.h"

#define INSTR_NOT_IMPL(name) printf("ERR: '%s' not implemented @ %04x\n", name, core->regs.pc); exit(0)

// Common psw update routines
void psw_set_zs(struct u8_core *core, uint8_t size, uint64_t val) {
	// Mask the flags
	uint8_t psw = core->regs.psw;
	psw &= 0b10011111;

	uint8_t s_bit = size * 8 - 1;	// Sign bit

	psw |= (val == 0) << 6;		// Zero
	psw |= (val >> s_bit) << 5;	// Sign

	// Set the new flags
	core->regs.psw = psw;
}

uint64_t add_impl(uint64_t val0, uint64_t val1, size_t size, uint8_t *flags) {
	// Full add
	uint64_t full = val0 + val1;
	bool carry = full >> size * 8;
	full &= (1 << size * 8) - 1;
	bool zero = full == 0;

	uint8_t s_bit = size * 8 - 1;			// Sign bit
	bool sign = full >> s_bit;

	bool oflow = (val0 >> s_bit) == (val1 >> s_bit) ? (val0 >> s_bit) != (full >> s_bit) : 0;

	// Half add
	uint8_t hc_bit = size * 8 - 4;
	uint64_t hc_mask = (1 << hc_bit) - 1;
	uint64_t half = (val0 & hc_mask) + (val1 & hc_mask);

	bool hc = half >> hc_bit;

	// Set flags
	*flags = (carry << 7) | (zero << 6) | (sign << 5) | (oflow << 4) | (hc << 2);

	return full;
}

uint64_t sub_impl(uint64_t val0, uint64_t val1, size_t size, uint8_t *flags) {
	// Sign extend
	uint64_t m = 1 << (size * 8 - 1);
	val0 = (val0 ^ m) - m;
	val1 = (val1 ^ m) - m;

	// Full sub
	uint64_t full = val0 - val1;
	uint64_t sign_nt = full >> (size * 8);

	full &= (1 << size * 8) - 1;

	bool zero = full == 0;
	bool sign = full >> (size * 8 - 1);
	bool borrow = val1 > val0;

	sign_nt += sign;
	sign_nt &= (uint64_t)(-1) >> (size * 8);

	bool oflow = sign_nt != 0;

	// Half sub
	uint8_t hc_bit = size * 8 - 4;
	uint64_t hc_mask = (1 << hc_bit) - 1;

	bool hc = (val1 & hc_mask) > (val0 & hc_mask);

	// Set flags
	*flags = (borrow << 7) | (zero << 6) | (sign << 5) | (oflow << 4) | (hc << 2);

	return full;
}

// Arithmetic Instructions
void instr_add(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t mask = (1 << 8 * op0->size) - 1;

	uint64_t val0 = oper_read(core, op0) & mask;
	uint64_t val1 = oper_read(core, op1) & mask;

	uint8_t psw_new;
	uint64_t res = add_impl(val0, val1, op0->size, &psw_new);

	// Set the new flags
	if (flags == 0) {
		core->regs.psw &= 0b00001011;
		core->regs.psw |= psw_new;
	}

	oper_write(core, op0, res);
}

/*void instr_addc(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t mask = (1 << 8 * op0->size) - 1;

	uint64_t val0 = oper_read(core, op0) & mask;
	uint64_t val1 = oper_read(core, op1) & mask;

	bool carry = core->regs.psw >> 7;

	uint8_t psw_new;
	uint64_t res = add_impl(val0, val1 + carry, op0->size, &psw_new);

	// Set the new flags
	core->regs.psw &= (psw_new | 0b10111111) & 0b01001011;
	core->regs.psw |= psw_new & 0b10111111;

	oper_write(core, op0, res);
}*/

void instr_addc(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t mask = (1 << 8 * op0->size) - 1;

	uint64_t val0 = oper_read(core, op0) & mask;
	uint64_t val1 = oper_read(core, op1) & mask;

	bool carry = core->regs.psw >> 7;

	uint64_t res;
	uint8_t psw0, psw1;
	res = add_impl(val0, val1, op0->size, &psw0);
	res = add_impl(res, carry, op0->size, &psw1);

	uint8_t psw_new = (psw0 & 0b10111111) | (psw1 & 0b10111111) | (psw1 & 0b01000000);

	// Set the new flags
	core->regs.psw &= (psw_new | 0b10111111) & 0b01001011;
	core->regs.psw |= psw_new & 0b10111111;

	oper_write(core, op0, res);
}

void instr_cmp(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t mask = (1 << 8 * op0->size) - 1;

	uint64_t val0 = oper_read(core, op0) & mask;
	uint64_t val1 = oper_read(core, op1) & mask;

	uint8_t psw_new;
	sub_impl(val0, val1, op0->size, &psw_new);

	// Set the new flags
	core->regs.psw &= 0b00001011;
	core->regs.psw |= psw_new;
}

void instr_cmpc(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t mask = (1 << 8 * op0->size) - 1;

	bool carry = core->regs.psw >> 7;

	uint64_t val0 = oper_read(core, op0) & mask;
	uint64_t val1 = (oper_read(core, op1) + carry) & mask;

	uint8_t psw_new;
	sub_impl(val0, val1, op0->size, &psw_new);

	// Set the new flags
	core->regs.psw &= (psw_new | 0b10111111) & 0b01001011;
	core->regs.psw |= psw_new & 0b10111111;
}

void instr_sub(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t mask = (1 << 8 * op0->size) - 1;

	uint64_t val0 = oper_read(core, op0) & mask;
	uint64_t val1 = oper_read(core, op1) & mask;

	uint8_t psw_new;
	uint64_t res = sub_impl(val0, val1, op0->size, &psw_new);

	// Set the new flags
	core->regs.psw &= 0b00001011;
	core->regs.psw |= psw_new;

	oper_write(core, op0, res);
}

void instr_subc(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t mask = (1 << 8 * op0->size) - 1;

	uint64_t val0 = oper_read(core, op0) & mask;
	uint64_t val1 = oper_read(core, op1) & mask;

	bool carry = core->regs.psw >> 7;

	uint8_t psw_new;
	uint64_t res = sub_impl(val0, val1 + carry, op0->size, &psw_new);

	core->regs.psw &= (psw_new | 0b10111111) & 0b01001011;
	core->regs.psw |= psw_new & 0b10111111;

	oper_write(core, op0, res);
}

// Shift Instructions
void instr_shift(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op0);
	uint64_t shift = oper_read(core, op1);

	// Bits that don't change during continued shifts
	uint64_t keep_mask = (1 << (op0->size - 1) * 8) - 1;
	if (flags & 1) keep_mask ^= -1;
	uint64_t keep = val & keep_mask;

	// Is this an arithmetic shift?
	if (flags & 4) {
		// Sign extend
		uint64_t m = 1 << (op0->size * 8 - 1);
		val = (val ^ m) - m;
	}

	// Limit shift size
	shift &= 0b111;

	// Mask the flags
	uint8_t psw = core->regs.psw;
	psw &= 0b01111111;

	// Apply shift
	if (flags & 1) {
		psw |= ((val >> (shift - 1)) & 1) << 7;
		val >>= shift;
	} else {
		psw |= ((val >> (op0->size * 8 - shift)) & 1) << 7;
		val <<= shift;
	}

	core->regs.psw = psw;

	// This is a continued shift, keep some bits
	if (flags & 2) {
		val &= -1 ^ keep_mask;
		val |= keep;
	}

	oper_write(core, op0, val);
}

void instr_mov(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op1);

	// Do we update the flags?
	if (flags == 0) {
		psw_set_zs(core, op0->size, val);
	}

	oper_write(core, op0, val);
}

void instr_or(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val0 = oper_read(core, op0);
	uint64_t val1 = oper_read(core, op1);

	uint64_t result = val0 | val1;

	// Set the flags
	psw_set_zs(core, op0->size, result);

	oper_write(core, op0, result);
}

void instr_and(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val0 = oper_read(core, op0);
	uint64_t val1 = oper_read(core, op1);

	uint64_t result = val0 & val1;

	// Set the flags
	psw_set_zs(core, op0->size, result);

	oper_write(core, op0, result);
}

void instr_xor(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val0 = oper_read(core, op0);
	uint64_t val1 = oper_read(core, op1);

	uint64_t result = val0 ^ val1;

	// Set the flags
	psw_set_zs(core, op0->size, result);

	oper_write(core, op0, result);
}

// DSR Prefix Instruction
void instr_dsr(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->cur_dsr = oper_read(core, op0);

	u8_step(core);

	core->cur_dsr = 0;
}

// Load/Store Instructions
void instr_load(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op1);

	psw_set_zs(core, op0->size, val);

	oper_write(core, op0, val);
}

void instr_store(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	oper_write(core, op1, oper_read(core, op0));
}

// Push/Pop Instructions
void instr_push(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.sp -= (op0->size == 1) ? 2 : op0->size;
	write_mem_data(core, core->cur_dsr, core->regs.sp, op0->size, oper_read(core, op0));
}

void instr_push_list(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t list = oper_read(core, op0);

	if (list & 0x2) { // ELR
		core->regs.sp -= 2;
		write_mem_data(core, core->cur_dsr, core->regs.sp, 1, core->regs.ecsr[core->regs.psw & 3]);
		core->regs.sp -= 2;
		write_mem_data(core, core->cur_dsr, core->regs.sp, 2, core->regs.elr[core->regs.psw & 3]);
	}

	if (list & 0x4) { // EPSW
		core->regs.sp -= 2;
		write_mem_data(core, core->cur_dsr, core->regs.sp, 1, core->regs.epsw[core->regs.psw & 3]);
	}

	if (list & 0x8) { // LR
		core->regs.sp -= 2;
		write_mem_data(core, core->cur_dsr, core->regs.sp, 1, core->regs.lcsr);
		core->regs.sp -= 2;
		write_mem_data(core, core->cur_dsr, core->regs.sp, 2, core->regs.lr);
	}	

	if (list & 0x1) { // EA
		core->regs.sp -= 2;
		write_mem_data(core, core->cur_dsr, core->regs.sp, 2, core->regs.ea);
	}
}

void instr_pop(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	oper_write(core, op0, read_mem_data(core, core->cur_dsr, core->regs.sp, op0->size));
	core->regs.sp += (op0->size == 1) ? 2 : op0->size;
}

void instr_pop_list(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t list = oper_read(core, op0);

	if (list & 0x1) { // EA
		core->regs.ea = read_mem_data(core, core->cur_dsr, core->regs.sp, 2);
		core->regs.sp += 2;
	}

	if (list & 0x8) { // LR
		core->regs.lr = read_mem_data(core, core->cur_dsr, core->regs.sp, 2);
		core->regs.sp += 2;
		core->regs.lcsr = read_mem_data(core, core->cur_dsr, core->regs.sp, 1);
		core->regs.sp += 2;
	}

	if (list & 0x4) { // PSW
		core->regs.psw = read_mem_data(core, core->cur_dsr, core->regs.sp, 1);
		core->regs.sp += 2;
	}

	if (list & 0x2) { // PC
		core->regs.pc = read_mem_data(core, core->cur_dsr, core->regs.sp, 2);
		core->regs.sp += 2;
		core->regs.csr = read_mem_data(core, core->cur_dsr, core->regs.sp, 1);
		core->regs.sp += 2;
	}
}

// TODO: Coprocessor Instructions
// Either just error out or actually implement

// EA Register Data Transfer Instructions
void instr_lea(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.ea = op0->addr;
}

// ALU Instructions
void instr_daa(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op0);

	bool c = (core->regs.psw & 0x80) >> 7;
	bool hc = (core->regs.psw & 0x04) >> 2;

	uint8_t psw = core->regs.psw & 0b10011011;

	if (hc || (val & 0x0f) > 0x09) {
		val += 0x06;
		psw |= 1 << 2;
	}

	if (c || (val & 0xf0) > 0x90 || (val & 0x100)) {
		val += 0x60;
		psw |= 1 << 7;
	}

	val &= 0xff;

	psw |= (val == 0) << 6;
	psw |= (val >> 7) << 5;

	core->regs.psw = psw;

	oper_write(core, op0, val);
}

void instr_das(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op0);

	bool c = (core->regs.psw & 0x80) >> 7;
	bool hc = (core->regs.psw & 0x04) >> 2;

	uint8_t psw = core->regs.psw & 0b10011011;

	if (hc || (val & 0x0f) > 0x09) {
		val -= 0x06;
		psw |= 1 << 2;
	}

	if (c || (val & 0xf0) > 0x90 || (val & 0x100)) {
		val -= 0x60;
		psw |= 1 << 7;
	}

	val &= 0xff;

	psw |= (val == 0) << 6;
	psw |= (val >> 7) << 5;

	core->regs.psw = psw;

	oper_write(core, op0, val);
}

void instr_neg(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op0);

	uint8_t psw_new;
	uint64_t res = sub_impl(0, val, op0->size, &psw_new);
	
	core->regs.psw &= 0b00001011;
	core->regs.psw |= psw_new;

	oper_write(core, op0, res);
}

// Bit Access Instructions
void instr_bit(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op0);
	uint64_t bit_idx = oper_read(core, op1);

	// Update flags
	bool bit = (val >> bit_idx) & 1;
	core->regs.psw &= 0b10111111;
	core->regs.psw |= (uint8_t)(~bit) << 6;

	switch (flags) {
		case 1: val |= 1 << bit_idx; break;				// SB
		case 2: val &= (1 << bit_idx) ^ (-1); break;	// RB
		default: return;								// TB
	}

	oper_write(core, op0, val);
}

// PSW Access Instructions
void instr_ei(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.psw |= 0b00001000;
}

void instr_di(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.psw &= 0b11110111;
}

void instr_sc(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.psw |= 0b10000000;
}

void instr_rc(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.psw &= 0b01111111;
}

void instr_cplc(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.psw ^= 0b10000000;
}

// Conditional Relative Branch Instructions
void instr_brel(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint8_t psw = core->regs.psw;
	bool c = (psw & 0x80) >> 7;
	bool z = (psw & 0x40) >> 6;
	bool s = (psw & 0x20) >> 5;
	bool ov = (psw & 0x10) >> 4;
	bool hc = (psw & 0x04) >> 2;

	switch (oper_read(core, op0)) {
		case 0b0000: {	// BGE
			if (!(c == 0)) return;
			break;
		}

		case 0b0001: {	// BLT
			if (!(c == 1)) return;
			break;
		}

		case 0b0010: {	// BGT
			if (!((c == 0)&&(z == 0))) return;
			break;
		}

		case 0b0011: {	// BLE
			if (!((c == 1)||(z == 1))) return;
			break;
		}

		case 0b0100: {	// BGES
			if (!((ov ^ s) == 0)) return;
			break;
		}

		case 0b0101: {	// BLTS
			if (!((ov ^ s) == 1)) return;
			break;
		}

		case 0b0110: {	// BGTS
			if (!(((ov ^ s) | z) == 0)) return;
			break;
		}

		case 0b0111: {	// BLES
			if (!(((ov ^ s) | z) == 1)) return;
			break;
		}

		case 0b1000: {	// BNE
			if (!(z == 0)) return;
			break;
		}

		case 0b1001: {	// BEQ
			if (!(z == 1)) return;
			break;
		}

		case 0b1010: {	// BNV
			if (!(ov == 0)) return;
			break;
		}

		case 0b1011: {	// BOV
			if (!(ov == 1)) return;
			break;
		}

		case 0b1100: {	// BPS
			if (!(s == 0)) return;
			break;
		}

		case 0b1101: {	// BNS
			if (!(s == 1)) return;
			break;
		}

		case 0b1110: {	// BAL
			break;
		}
	}

	// Apply the offset
	int64_t offset = oper_read(core, op1);
	core->regs.pc += offset << 1;
}

// Sign Extension Instruction
void instr_extbw(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op1) & ((1 << op1->size * 4) - 1);
	
	uint64_t m = 1 << (op1->size * 4 - 1);
	val = (val ^ m) - m;

	psw_set_zs(core, op1->size, val);

	oper_write(core, op1, val);
}

// Software Interrupt Instructions
void instr_swi(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("SWI");
}

void instr_brk(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	INSTR_NOT_IMPL("BRK");
}

// Branch Instructions
void instr_b(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.csr = oper_read(core, op0);
	core->regs.pc = op1->addr;
}

void instr_bl(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.lcsr = core->regs.csr;
	core->regs.lr = core->regs.pc;
	core->regs.csr = oper_read(core, op0);
	core->regs.pc = op1->addr;
}

// Multiplication and Division Instructions
void instr_mul(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t mask = (1 << 4 * op0->size) - 1;

	uint64_t val0 = oper_read(core, op0) & mask;
	uint64_t val1 = oper_read(core, op1) & mask;

	uint64_t res = val0 * val1;

	res &= (1 << 8 * op0->size) - 1;

	core->regs.psw &= 0b10111111;
	if (res == 0) core->regs.psw |= 0b01000000;	// Zero flag

	oper_write(core, op0, res);
}

void instr_div(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val0 = oper_read(core, op0);
	uint64_t val1 = oper_read(core, op1);

	core->regs.psw &= 0b00111111;

	uint64_t res = val0 / val1;

	if (val1 == 0) core->regs.psw |= 0b10000000;
	if (res == 0) core->regs.psw |= 0b01000000;

	oper_write(core, op0, res);
	oper_write(core, op1, val0 % val1);
}

// Miscellaneous Instructions
void instr_inc(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op0);

	uint8_t psw_new;
	uint64_t res = add_impl(val, 1, 1, &psw_new);

	// Set the new flags
	core->regs.psw &= 0b10001011;
	core->regs.psw |= psw_new & 0b01110100;	// Mask the carry flag	

	oper_write(core, op0, res);
}

void instr_dec(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint64_t val = oper_read(core, op0);

	uint8_t psw_new;
	uint64_t res = sub_impl(val, 1, 1, &psw_new);

	// Set the new flags
	core->regs.psw &= 0b10001011;
	core->regs.psw |= psw_new & 0b01110100;	// Mask the carry flag	

	oper_write(core, op0, res);
}

void instr_rt(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	core->regs.csr = core->regs.lcsr;
	core->regs.pc = core->regs.lr;
}

void instr_rti(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	uint8_t elevel = core->regs.psw & 3;
	core->regs.csr = core->regs.ecsr[elevel];
	core->regs.pc = core->regs.elr[elevel];
	core->regs.psw = core->regs.epsw[elevel];
}

void instr_nop(struct u8_core *core, uint8_t flags, struct u8_oper *op0, struct u8_oper *op1) {
	return;
}
