#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "core.h"
#include "mem.h"

static uint64_t access_mem(struct u8_core *core, uint8_t seg, uint16_t offset, uint8_t size, bool rw, uint64_t val) {
	if (!(size && (((size - 1) & size) == 0))) __builtin_unreachable();
	// Find region
	uint32_t addr = ((uint32_t)seg << 16) | offset;
	for (int i = 0; i < core->mem.num_regions; i++) {
		// Is the address in the current region?
		uint32_t addr_l = core->mem.regions[i].addr_l;
		uint32_t addr_h = core->mem.regions[i].addr_h;

		if (addr >= addr_l && addr <= addr_h && (core->mem.regions[i].type <= U8_REGION_DATA)) {
			if (rw && !core->mem.regions[i].rw) return 0;

			addr -= addr_l;

			// Read each byte in little endian order
			if (!rw) val = 0;
			if (__builtin_expect(core->mem.regions[i].acc == U8_MACC_ARR, 1)) {
#if defined (__LITTLE_ENDIAN__) || defined (__x86_64__) || defined (__i686__) || defined (__aarch64__)
				if (__builtin_expect(rw, 0)) {
				core->mem.regions[i].dirty &= 2;
					memcpy(&core->mem.regions[i].array[addr], &val, size);
				} else {
					if (__builtin_expect(size == 1, 1)) {
						return core->mem.regions[i].array[addr];
					} else {
						memcpy(&val, &core->mem.regions[i].array[addr], size);
					}
				}
			return val;
#else
#error big endian is not big fast!
#endif
			} else {
				if (rw) {
					for (int x = 0; x < size; x++) {
						core->mem.regions[i].write(core, seg, addr + x, (val >> (8 * x)) & 0xff);
					}
				} else {
					for (int x = 0; x < size; x++) {
						val |= (uint64_t)core->mem.regions[i].read(core, seg, addr + x) << (8 * x);
					}
				}
			}

			return val;
		}
	}

	// Not in any memory regions, return 0
	return 0x00;
}

uint64_t read_mem_data(struct u8_core *core, uint8_t dsr, uint16_t offset, uint8_t size) {
	return access_mem(core, dsr, offset, size, false, 0);
}

uint64_t read_mem_code(struct u8_core *core, uint8_t csr, uint16_t offset, uint8_t size) {
	if (__builtin_expect(size, 2)) {
		uint32_t addr = ((uint32_t)csr << 16) | offset;

		for (int i = 0; i < core->codemem.num_regions; i++) {
			struct u8_mem_reg region = core->codemem.regions[i];
			if (__builtin_expect(region.addr_l <= addr && addr + size <= region.addr_h, 1)) {
				uint64_t out;
				memcpy(&out, &region.array[addr - region.addr_l], 2);
				return out;
			}
		}
	} else {
		// slow path
		uint32_t addr = ((uint32_t)csr << 16) | offset;

		for (int i = 0; i < core->codemem.num_regions; i++) {
			struct u8_mem_reg region = core->codemem.regions[i];
			if (__builtin_expect(region.addr_l <= addr && addr + size <= region.addr_h, 1)) {
				uint64_t out;
				memcpy(&out, &region.array[addr - region.addr_l], size);
				return out;
			}
		}
	}
	return 0;
}

void write_mem_data(struct u8_core *core, uint8_t dsr, uint16_t offset, uint8_t size, uint64_t val) {
	access_mem(core, dsr, offset, size, true, val);
}
