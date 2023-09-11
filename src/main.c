#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <memory.h>

#include "context.h"
#include "core/core.h"
#include "core/mem.h"
#include "core/regs.h"

void braille(char *s, uint8_t c) {
	s[0] = 0b11100010;
	s[1] = 0b10100000 | c >> 6;
	s[2] = 0b10000000 | (c & 0b00111111);
}

void render(struct u8_sim_ctx *ctx) {
	for (int y = 0; y < 8; y++) {
		wmove(ctx->periph.lcd_win, y, 0);
		for (int x = 0; x < 48; x++) {
			// Get the pixels
			uint8_t pixels[8];

			for (int Py = 0; Py < 4; Py++) {
				uint16_t offset = (y << 6) + (Py << 4) + (x >> 2);
				uint8_t byte = read_mem_data(&ctx->core, 0, 0xf800 + offset, 1);

				for (int Px = 0; Px < 2; Px++)	
					pixels[(Py << 1) + Px] = byte >> (7 - (((x & 3) << 1) + Px)) & 1;
			}

			// Convert to braille
			uint8_t weight[] = {0x01, 0x08, 0x02, 0x10, 0x04, 0x20, 0x40, 0x80};
			uint8_t val = 0;
			for (int i = 0; i < 8; i++) val |= pixels[i] * weight[i];

			char buf[4] = {'\0'};
			braille(buf, val);
			wprintw(ctx->periph.lcd_win, "%s", buf);
		}
	}

	wrefresh(ctx->periph.lcd_win);
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
	memset(ctx->data_mem, 0, 0xE00);

	uint8_t *emu_kb = malloc(0x30);
	memset(emu_kb, 0, 0x30);

	uint8_t *sfr = malloc(0x1000);
	memset(sfr, 0, 0x1000);

	ctx->core.mem.num_regions = 7;
	ctx->core.mem.regions = malloc(sizeof(struct u8_mem_reg) * 7);

	ctx->core.mem.regions[0] = (struct u8_mem_reg){
		.type = U8_REGION_BOTH,
		.rw = false,
		.addr_l = 0x00000,
		.addr_h = 0x07FFF,
		.acc = U8_MACC_ARR,
		.array = ctx->code_mem
	};

	ctx->core.mem.regions[1] = (struct u8_mem_reg){
		.type = U8_REGION_DATA,
		.rw = true,
		.addr_l = 0x08000,
		.addr_h = 0x08E00,
		.acc = U8_MACC_ARR,
		.array = ctx->data_mem
	};

	ctx->core.mem.regions[2] = (struct u8_mem_reg){
		.type = U8_REGION_DATA,
		.rw = true,
		.addr_l = 0x08E00,
		.addr_h = 0x08E30,
		.acc = U8_MACC_ARR,
		.array = emu_kb
	};

	ctx->core.mem.regions[3] = (struct u8_mem_reg){
		.type = U8_REGION_DATA,
		.rw = true,
		.addr_l = 0x0F000,
		.addr_h = 0x0FFFF,
		.acc = U8_MACC_ARR,
		.array = sfr
	};

	ctx->core.mem.regions[4] = (struct u8_mem_reg){
		.type = U8_REGION_CODE,
		.rw = false,
		.addr_l = 0x08000,
		.addr_h = 0x0FFFF,
		.acc = U8_MACC_ARR,
		.array = (uint8_t *)(ctx->code_mem + 0x8000)
	};

	ctx->core.mem.regions[5] = (struct u8_mem_reg){
		.type = U8_REGION_BOTH,
		.rw = false,
		.addr_l = 0x10000,
		.addr_h = 0x1FFFF,
		.acc = U8_MACC_ARR,
		.array = (uint8_t *)(ctx->code_mem + 0x10000)
	};

	ctx->core.mem.regions[6] = (struct u8_mem_reg){
		.type = U8_REGION_DATA,
		.rw = false,
		.addr_l = 0x80000,
		.addr_h = 0x8FFFF,
		.acc = U8_MACC_ARR,
		.array = ctx->code_mem
	};

	// Initialise peripherals
	init_periph(ctx);

	// Reset the core
	u8_reset(&ctx->core);

	// Open the log file
	FILE *log_file = fopen("sim.log", "w");
	ctx->log_file = log_file;

	int count = 0;
	while (true) {
		// Log the PCs to a file
		/*fprintf(log_file, "ADDR: %x:%04x SP: %04x PSW %04x\n", ctx->regs.csr, ctx->regs.pc, ctx->regs.sp, ctx->regs.psw);
		fprintf(log_file, "REGS: ");
		for (int i = 0; i < 16; i++) fprintf(log_file, "%02x ", ctx->regs.gp[i]);
		fprintf(log_file, "\nSTACK: ");
		for (int i = 0; i < 32; i++) fprintf(log_file, "%02x ", read_mem_data(ctx, 0, ctx->regs.sp + i, 1));
		fprintf(log_file, "\n");
	

		fflush(log_file);

		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x8c64) {
			wprintw(ctx->periph.cons_win, "REG 0: ");
			for (int x = 0; x < 0xB; x++) wprintw(ctx->periph.cons_win, "%02x ", read_mem_data(ctx, 0, 0x8000 + x, 1));
			wprintw(ctx->periph.cons_win, "\nREG 1: ");
			for (int x = 0; x < 0xB; x++) wprintw(ctx->periph.cons_win, "%02x ", read_mem_data(ctx, 0, 0x8010 + x, 1));
			wprintw(ctx->periph.cons_win, "\n");
			wrefresh(ctx->periph.cons_win);
		}

		if(ctx->regs.csr == 0x01 && ctx->regs.pc >= 0x89a0 && ctx->regs.pc <= 0x89cc) {
			wprintw(ctx->periph.cons_win, "%04x %016lx %016lx %02x\n", ctx->regs.pc, read_reg_qr(ctx, 0), read_reg_qr(ctx, 8), ctx->regs.psw);
			wrefresh(ctx->periph.cons_win);
			//getch();
		}

		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x83d6) {
				wprintw(ctx->periph.cons_win, "\nNext digit (%x):", read_reg_r(ctx, 1));
				wrefresh(ctx->periph.cons_win);
		}

		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x83f0) {
				wprintw(ctx->periph.cons_win, " %02x", read_reg_r(ctx, 2));
				wrefresh(ctx->periph.cons_win);
		}

		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x8414) wprintw(ctx->periph.cons_win, "E");
		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x83F8) wprintw(ctx->periph.cons_win, "G");
		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x8456) wprintw(ctx->periph.cons_win, "O");
		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x8436) wprintw(ctx->periph.cons_win, "T");

		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x84d0) {
			fprintf(ctx->log_file, "%016lx,%016lx,", read_reg_qr(ctx, 0), read_reg_qr(ctx, 8));
			fflush(ctx->log_file);
		}

		if (ctx->regs.csr == 0x01 && ctx->regs.pc == 0x84f2) {
			fprintf(ctx->log_file, "%016lx\n", read_reg_qr(ctx, 0));
			fflush(ctx->log_file);
		}

		if (ctx->regs.csr == 0x00 && ctx->regs.pc == 0x4ffa) {
			wprintw(ctx->periph.cons_win, "Here! er0: %04x psw: %02x\n", read_reg_er(ctx, 0), ctx->regs.psw);
			wrefresh(ctx->periph.cons_win);
		}

		if (ctx->regs.csr == 0x00 && ctx->regs.pc == 0x28e0) {
			wprintw(ctx->periph.cons_win, "LR %x%04x Char: %02x\n", ctx->regs.lcsr, ctx->regs.lr, read_reg_r(ctx, 0));
			wrefresh(ctx->periph.cons_win);
		}

		if (ctx->regs.csr == 0x00 && ctx->regs.pc == 0x2be4 && (read_reg_er(ctx, 2) == 0xffcc)) {
				wprintw(ctx->periph.cons_win, "What??? %x%04x\n", ctx->regs.lcsr, ctx->regs.lr);
				wrefresh(ctx->periph.cons_win);
		}

		if (ctx->regs.csr == 0x00 && ctx->regs.pc == 0x2bfc) {
				wprintw(ctx->periph.cons_win, "HIT: %04x\n", read_reg_er(ctx, 2));
				wrefresh(ctx->periph.cons_win);
		}

		if (ctx->regs.csr == 0x00 && ctx->regs.pc == 0x2c0a && ctx->regs.gp[2] == 0xd0) {
				wprintw(ctx->periph.cons_win, "%04x\n", read_reg_er(ctx, 10));
				wrefresh(ctx->periph.cons_win);
		}

		//if (ctx->regs.pc == 0x3c52) break;

		if (count % 10000 == 0) {
			for (int i = 0; i < 10; i++) wprintw(ctx->periph.cons_win, "%02x ", read_mem_data(ctx, 0, 0x8230 + i, 1));
			wprintw(ctx->periph.cons_win, "\n");
			wrefresh(ctx->periph.cons_win);
		}

		//if (ctx->regs.pc == 0xb3e0) ctx->regs.pc = 0xb41a;

		if (ctx->regs.pc == 0xb404) {
			wprintw(ctx->periph.cons_win, "Keycode: %02x\n", read_reg_r(ctx, 0));
			wrefresh(ctx->periph.cons_win);
		}*/

		// Hook the render function
		if (ctx->core.regs.pc == 0x2ec0 || count % 1000 == 0) render(ctx);
		count++;

		update_keyboard(ctx);

		u8_step(&ctx->core);
	}

	render(ctx);

	wprintw(ctx->periph.cons_win, "Emulator exited cleanly\n");
	wrefresh(ctx->periph.cons_win);

	fflush(log_file);
	fclose(log_file);

	while (getch() != '\n') {}

	endwin();

	return 0;
}
