#include <stdint.h>
#include <stdbool.h>

#include "core.h"
#include "mem.h"

uint64_t access_mem(struct u8_core *core, uint8_t seg, uint16_t offset, uint8_t size, enum u8_mem_type type, bool rw, uint64_t val) {
	// Find region
	uint32_t addr = ((uint32_t)seg << 16) | offset;
	for (int i = 0; i < core->mem.num_regions; i++) {
		// Is the address in the current region?
		uint32_t addr_l = core->mem.regions[i].addr_l;
		uint32_t addr_h = core->mem.regions[i].addr_h;

		if (addr >= addr_l && addr <= addr_h &&
		   (core->mem.regions[i].type == type || core->mem.regions[i].type == U8_REGION_BOTH)) {

			if (rw && !core->mem.regions[i].rw) return 0;

			addr -= addr_l;

			// Read each byte in little endian order
			if (!rw) val = 0;
			for (int x = 0; x < size; x++)
				switch (core->mem.regions[i].acc) {
					// Array
					case U8_MACC_ARR: {
						if (rw) {
							core->mem.regions[i].array[addr + x] = (val >> (8 * x)) & 0xff;
						} else {
							val |= (uint64_t)core->mem.regions[i].array[addr + x] << (8 * x);
						}
						break;
					}

					// Function Pointer
					case U8_MACC_FUNC: {
						if (rw) {
							core->mem.regions[i].write(core, seg, addr + x, (val >> (8 * x)) & 0xff);
						} else {
							val |= (uint64_t)core->mem.regions[i].read(core, seg, addr + x) << (8 * x);
						}
						
						break;
					}
				}
			
			return val;
		}
	}

	// Not in any memory regions, return 0
	return 0x00;
}

uint64_t read_mem_data(struct u8_core *core, uint8_t dsr, uint16_t offset, uint8_t size) {
	return access_mem(core, dsr, offset, size, U8_REGION_DATA, false, 0);
}

uint64_t read_mem_code(struct u8_core *core, uint8_t csr, uint16_t offset, uint8_t size) {
	return access_mem(core, csr, offset, size, U8_REGION_CODE, false, 0);
}

void write_mem_data(struct u8_core *core, uint8_t dsr, uint16_t offset, uint8_t size, uint64_t val) {
	access_mem(core, dsr, offset, size, U8_REGION_DATA, true, val);
}

void write_mem_code(struct u8_core *core, uint8_t csr, uint16_t offset, uint8_t size, uint64_t val) {
	access_mem(core, csr, offset, size, U8_REGION_CODE, true, val);
}