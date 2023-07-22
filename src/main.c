#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <memory.h>

#include "context.h"
#include "mem.h"
#include "arch/arch.h"
#include "arch/regs.h"

void braille(char *s, uint8_t c) {
	s[0] = 0b11100010;
	s[1] = 0b10100000 | c >> 6;
	s[2] = 0b10000000 | (c & 0b00111111);
}

void render(struct u8_sim_ctx *ctx, WINDOW *win) {
	for (int y = 0; y < 8; y++) {
		wmove(win, y, 0);
		for (int x = 0; x < 48; x++) {
			// Get the pixels
			uint8_t pixels[8];

			for (int Py = 0; Py < 4; Py++) {
				uint16_t offset = (y << 6) + (Py << 4) + (x >> 2);
				uint8_t byte = read_mem_data(ctx, 0, 0xf800 + offset, 1);

				for (int Px = 0; Px < 2; Px++)	
					pixels[(Py << 1) + Px] = byte >> (7 - (((x & 3) << 1) + Px)) & 1;
			}

			// Convert to braille
			uint8_t weight[] = {0x01, 0x08, 0x02, 0x10, 0x04, 0x20, 0x40, 0x80};
			uint8_t val = 0;
			for (int i = 0; i < 8; i++) val |= pixels[i] * weight[i];

			char buf[4] = {'\0'};
			braille(buf, val);
			wprintw(win, "%s", buf);
		}
	}

	wrefresh(win);
}

uint8_t rom_window(struct u8_sim_ctx *ctx, uint8_t seg, uint16_t offset) {
	return ctx->code_mem[(seg << 16) | offset];
}

uint8_t data_mem_read(struct u8_sim_ctx *ctx, uint8_t seg, uint16_t offset) {
	if (seg != 0x00) return 0x00;
	return ctx->data_mem[offset - 0x8000];
}

void data_mem_write(struct u8_sim_ctx *ctx, uint8_t seg, uint16_t offset, uint8_t val) {
	if (seg != 0x00) return;
	ctx->data_mem[offset - 0x8000] = val;
}

uint8_t code_mem_0(struct u8_sim_ctx *ctx, uint8_t seg, uint16_t offset) {
	return ctx->code_mem[(seg << 16) | offset];
}

uint8_t code_mem_8(struct u8_sim_ctx *ctx, uint8_t seg, uint16_t offset) {
	return ctx->code_mem[offset];
}

int main(int argc, char **argv) {
	// Initialise simulator
	struct u8_sim_ctx *ctx = malloc(sizeof(struct u8_sim_ctx));

	// Load the rom
	FILE *f = fopen("rom_emu.bin", "r");
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	ctx->code_mem = malloc(fsize);
	fread(ctx->code_mem, fsize, 1, f);

	// Setup memory regions
	ctx->data_mem = malloc(0x8000);
	memset(ctx->data_mem, 0, 0x8000);

	ctx->mem.num_regions = 5;
	ctx->mem.regions = malloc(sizeof(struct u8_mem_reg) * 5);

	ctx->mem.regions[0].type = U8_REGION_BOTH;
	ctx->mem.regions[0].addr_l = 0x00000;
	ctx->mem.regions[0].addr_h = 0x07FFF;
	ctx->mem.regions[0].read = &rom_window;
	ctx->mem.regions[0].write = NULL;

	ctx->mem.regions[1].type = U8_REGION_DATA;
	ctx->mem.regions[1].addr_l = 0x08000;
	ctx->mem.regions[1].addr_h = 0x0FFFF;
	ctx->mem.regions[1].read = &data_mem_read;
	ctx->mem.regions[1].write = &data_mem_write;

	ctx->mem.regions[2].type = U8_REGION_CODE;
	ctx->mem.regions[2].addr_l = 0x08000;
	ctx->mem.regions[2].addr_h = 0x0FFFF;
	ctx->mem.regions[2].read = &code_mem_0;
	ctx->mem.regions[2].write = NULL;

	ctx->mem.regions[3].type = U8_REGION_BOTH;
	ctx->mem.regions[3].addr_l = 0x10000;
	ctx->mem.regions[3].addr_h = 0x1FFFF;
	ctx->mem.regions[3].read = &code_mem_0;
	ctx->mem.regions[3].write = NULL;

	ctx->mem.regions[4].type = U8_REGION_DATA;
	ctx->mem.regions[4].addr_l = 0x80000;
	ctx->mem.regions[4].addr_h = 0x8FFFF;
	ctx->mem.regions[4].read = &code_mem_8;
	ctx->mem.regions[4].write = NULL;

	// Initialise peripherals
	init_periph(ctx);

	// Controls writing to buffer or real screen
	write_mem_data(ctx, 0, 0x811d, 1, 1);

	write_mem_data(ctx, 0, 0x8e00, 1, 1);

	// Set PC and SP
	ctx->regs.pc = 0x39aa;
	ctx->regs.sp = 0x8dec;
	ctx->regs.lr = 0xffff;

	int count = 0;

	FILE *log_file = fopen("sim.log", "w");
	ctx->log_file = log_file;

	bool log = false;

	while (true) {
		// Log the PCs to a file
		/*if (log) {
			fprintf(log_file, "ADDR: %x:%04x SP: %04x\n", ctx->regs.csr, ctx->regs.pc, ctx->regs.sp);
			fprintf(log_file, "REGS: ");
			for (int i = 0; i < 16; i++) fprintf(log_file, "%02x ", ctx->regs.gp[i]);
			fprintf(log_file, "\nSTACK: ");
			for (int i = 0; i < 24; i++) fprintf(log_file, "%02x ", read_mem_data(ctx, 0, ctx->regs.sp + i, 1));
			fprintf(log_file, "\n");
			
			fprintf(log_file, "%s SP @ %x:%04x\n", ctx->regs.sp & 1 ? "Unaligned" : "Aligned", ctx->regs.csr, ctx->regs.pc);

			fflush(log_file);
		}*/

		if (ctx->regs.pc == 0xFFFF) break;

		//if (ctx->regs.pc == 0xb3e0) ctx->regs.pc = 0xb41a;

		if (ctx->regs.pc == 0xb404) {
			wprintw(ctx->periph.cons_win, "Keycode: %02x\n", read_reg_r(ctx, 0));
			wrefresh(ctx->periph.cons_win);
		}

		//if (ctx->regs.pc == 0x7142) log = true;
		//if (ctx->regs.pc == 0x71a6) log = false;

		// Hook the render function
		if (ctx->regs.pc == 0x2ec0 || count % 1000 == 0) render(ctx, ctx->periph.lcd_win);
		count++;

		update_keyboard(ctx);

		u8_step(ctx);
	}

	wprintw(ctx->periph.cons_win, "Emulator exited cleanly\n");
	wrefresh(ctx->periph.cons_win);

	//fflush(log_file);
	//fclose(log_file);

	while(1) {}
	while (getch() == ERR) {}

	endwin();

	return 0;
}