#include <stdint.h>
#include <stdarg.h>

#include "cpu.h"
#include "operands.h"

uint64_t oper_get(u8_cpu *cpu, u8_instr_opers_t opers, uint8_t idx) {
	switch(opers.operands[idx].type) {
		Rn: return reg_get_r(cpu, opers.operands[idx].reg_idx);
		ERn: return reg_get_er(cpu, opers.operands[idx].reg_idx);
		XRn: return reg_get_xr(cpu, opers.operands[idx].reg_idx);
		QRn: return reg_get_qr(cpu, opers.operands[idx].reg_idx);

		EA_PTR: {
			uint16_t addr = cpu->mem_read(cpu->cur_dsr, cpu->regs.ea);
			uint64_t val = 0;
			for (int x = 0; x <= 3; x++) val |= cpu->mem_read(cpu->cur_dsr, addr + x) << (8 * x);
			return val;
		}

		ERn_PTR: {
			uint16_t addr = cpu->mem_read(cpu->cur_dsr, reg_get_er(cpu, opers.operands[idx].reg_idx));
			uint64_t val = 0;
			for (int x = 0; x <= 3; x++) val |= cpu->mem_read(cpu->cur_dsr, addr + x) << (8 * x);
			return val;
		}

		ERn_D16: {
			uint16_t addr = cpu->mem_read(cpu->cur_dsr, reg_get_er(cpu, opers.operands[idx].reg_idx));
			addr += opers.long_imm;
			uint64_t val = 0;
			for (int x = 0; x <= 3; x++) val |= cpu->mem_read(cpu->cur_dsr, addr + x) << (8 * x);
			return val;
		}

		BP_D6: {
			uint16_t addr = cpu->mem_read(cpu->cur_dsr, reg_get_er(cpu, 12));
			addr += opers.operands[idx].val;
			uint64_t val = 0;
			for (int x = 0; x <= 3; x++) val |= cpu->mem_read(cpu->cur_dsr, addr + x) << (8 * x);
			return val;
		}

		FP_D6: {
			uint16_t addr = cpu->mem_read(cpu->cur_dsr, reg_get_er(cpu, 14));
			addr += opers.operands[idx].val;
			uint64_t val = 0;
			for (int x = 0; x <= 3; x++) val |= cpu->mem_read(cpu->cur_dsr, addr + x) << (8 * x);
			return val;
		}

		LONG_IMM: return opers.long_imm;
	}
}

void oper_set(u8_cpu *cpu, u8_instr_opers_t opers, uint8_t idx, uint64_t val) {

}