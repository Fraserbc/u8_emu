#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#include "arch/arch.h"
#include "arch/mem.h"
#include "arch/regs.h"

void braille(char *s, uint8_t c) {
	s[0] = 0b11100010;
	s[1] = 0b10100000 | c >> 6;
	s[2] = 0b10000000 | (c & 0b00111111);
}

char *render(struct u8_arch *arch) {
	char *buf = malloc(8 * (48 * 3 + 1) + 1);

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 48; x++) {
			// Get the pixels
			uint8_t pixels[8];

			for (int Py = 0; Py < 4; Py++) {
				uint16_t offset = (y << 6) + (Py << 4) + (x >> 2);
				uint8_t byte = read_mem_data(arch, 0, 0xf800 + offset, 1);

				for (int Px = 0; Px < 2; Px++)	
					pixels[(Py << 1) + Px] = byte >> (7 - (((x & 3) << 1) + Px)) & 1;
			}

			// Convert to braille
			uint8_t weight[] = {0x01, 0x08, 0x02, 0x10, 0x04, 0x20, 0x40, 0x80};
			uint8_t val = 0;
			for (int i = 0; i < 8; i++) val |= pixels[i] * weight[i];

			braille(&buf[(48 * 3 + 1) * y + 3 * x], val);
		}
		buf[(48 * 3 + 1) * (y + 1) - 1] = '\n';
	}
	buf[8 * (48 * 3 + 1)] = '\0';

	return buf;
}

int main(int argc, char **argv) {
	struct u8_arch *arch = malloc(sizeof(struct u8_arch));

	FILE *f = fopen("rom_emu.bin", "r");
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t *data = malloc(fsize);
	fread(data, fsize, 1, f);

	init_mem(data);

	write_mem_data(arch, 0, 0x811d, 1, 1);

	arch->regs.pc = 0x3a6c;
	arch->regs.sp = 0x8dec;

	int count = 0;

	while (1) {
		//printf("ADDR: %x:%04x\n", arch->regs.csr, arch->regs.pc);

		if (arch->regs.pc == 0x2ec0)
			printf("%s", render(arch));

		if (arch->regs.pc == 0x3afa) break;
		//for (int x = 0; x < 16; x++) printf("%02x ", arch->regs.gp[x]);
		//printf("\n");

		u8_step(arch);

		//write(1,"\E[H\E[2J",7);
		//printf("%s\n", render(arch));
	}

	return 0;
}