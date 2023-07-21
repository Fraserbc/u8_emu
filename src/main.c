#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#include <ncurses.h>
#include <locale.h>

#include "arch/arch.h"
#include "arch/mem.h"
#include "arch/regs.h"

void braille(char *s, uint8_t c) {
	s[0] = 0b11100010;
	s[1] = 0b10100000 | c >> 6;
	s[2] = 0b10000000 | (c & 0b00111111);
}

void render(struct u8_arch *arch, WINDOW *win) {
	for (int y = 0; y < 8; y++) {
		wmove(win, y, 0);
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

			char buf[4] = {'\0'};
			braille(buf, val);
			wprintw(win, "%s", buf);
		}
	}

	wrefresh(win);
}

struct keypad_key {
	int x0, y0, x1, y1;
	char name[6];
	uint8_t ki, ko;
};

struct keypad_key keys[] ={
	{0,   0, 6,   2, "Shift", 0x80, 0x01},	// Shift
	{7,   0, 13,  2, "Alpha", 0x80, 0x02},	// Alpha
	{18,  0, 24,  3, "Up",    0x80, 0x04},	// Up
	{25,  2, 28,  4, "Down",  0x80, 0x08},	// Down
	{29,  0, 35,  2, "Mode",  0x80, 0x10},	// Mode
	{36,  0, 42,  2, "On"},		// On

	{0,   4, 6,   6, "Abs",   0x40, 0x01},	// Abs
	{7,   4, 13,  6, "x^3",   0x40, 0x02},	// x^3
	{14,  2, 17,  4, "Left",  0x40, 0x04},	// Left
	{18,  4, 24,  6, "Down",  0x40, 0x08},	// Right
	{29,  4, 35,  6, "x^-1",  0x40, 0x10},	// x^-1
	{36,  4, 42,  6, "log_x", 0x40, 0x20},	// log_x

	{0,   7, 6,   9, "a/b",   0x20, 0x01},	// a/b
	{7,   7, 13,  9, "sqrt",  0x20, 0x02},	// sqrt(
	{14,  7, 20,  9, "x^2",   0x20, 0x04},	// x^2
	{22,  7, 28,  9, "x^n",   0x20, 0x08},	// x^n
	{29,  7, 35,  9, "log",   0x20, 0x10},	// log
	{36,  7, 42,  9, "ln",    0x20, 0x20},	// ln

	{0,  10, 6,  12, "(-)",   0x10, 0x01},	// (-)
	{7,  10, 13, 12, "dms",   0x10, 0x02},	// dms
	{14, 10, 20, 12, "hyp",   0x10, 0x04},	// hyp
	{22, 10, 28, 12, "sin",   0x10, 0x08},	// sin
	{29, 10, 35, 12, "cos",   0x10, 0x10},	// cos
	{36, 10, 42, 12, "tan",   0x10, 0x20},	// tan

	{0,  13, 6,  15, "RCL",   0x08, 0x01},	// RCL
	{7,  13, 13, 15, "ENG",   0x08, 0x02},	// ENG
	{14, 13, 20, 15, "(",     0x08, 0x04},	// (
	{22, 13, 28, 15, ")",     0x08, 0x08},	// )
	{29, 13, 35, 15, "S<=>D", 0x08, 0x10},	// S<=>D
	{36, 13, 42, 15, "M+",    0x08, 0x20},	// M+

	{4,  16, 10, 18, "7",     0x04, 0x01},	// 7
	{11, 16, 17, 18, "8",     0x04, 0x02},	// 8
	{18, 16, 24, 18, "9",     0x04, 0x04},	// 9
	{25, 16, 31, 18, "DEL",   0x04, 0x08},	// DEL
	{32, 16, 38, 18, "AC",    0x04, 0x10},	// AC

	{4,  19, 10, 21, "4",     0x02, 0x01},	// 4
	{11, 19, 17, 21, "5",     0x02, 0x02},	// 5
	{18, 19, 24, 21, "6",     0x02, 0x04},	// 6
	{25, 19, 31, 21, "X",     0x02, 0x08},	// X
	{32, 19, 38, 21, "div",   0x02, 0x10},	// div

	{4,  22, 10, 24, "1",     0x01, 0x01},	// 1
	{11, 22, 17, 24, "2",     0x01, 0x02},	// 2
	{18, 22, 24, 24, "3",     0x01, 0x04},	// 3
	{25, 22, 31, 24, "+",     0x01, 0x08},	// +
	{32, 22, 38, 24, "-",     0x01, 0x10},	// -

	{4,  25, 10, 27, "0",     0x10, 0x40},	// 0
	{11, 25, 17, 27, ".",     0x08, 0x40},	// .
	{18, 25, 24, 27, "x10",   0x04, 0x40},	// x10
	{25, 25, 31, 27, "Ans",   0x02, 0x40},	// Ans
	{32, 25, 38, 27, "=",     0x01, 0x40},	// =
};

#define NUM_KEYS sizeof(keys) / sizeof(struct keypad_key)

const char *keypad_text = "┏━━━━━┓┏━━━━━┓    ┏━━━━━┓    ┏━━━━━┓┏━━━━━┓"
						  "┃SHIFT┃┃ALPHA┃    ┃  ▲  ┃    ┃MODE ┃┃ ON  ┃"
						  "┗━━━━━┛┗━━━━━┛┏━━━┛     ┗━━━┓┗━━━━━┛┗━━━━━┛"
						  "              ┃ ◀         ▶ ┃              "
						  "┏━━━━━┓┏━━━━━┓┗━━━┓     ┏━━━┛┏━━━━━┓┏━━━━━┓"
						  "┃ Abs ┃┃  x³ ┃    ┃  ▼  ┃    ┃ x⁻¹ ┃┃logₓ ┃"
						  "┗━━━━━┛┗━━━━━┛    ┗━━━━━┛    ┗━━━━━┛┗━━━━━┛"
						  "┏━━━━━┓┏━━━━━┓┏━━━━━┓ ┏━━━━━┓┏━━━━━┓┏━━━━━┓"
						  "┃ a/b ┃┃sqrt(┃┃  x² ┃ ┃  xⁿ ┃┃ log ┃┃ ln( ┃"
						  "┗━━━━━┛┗━━━━━┛┗━━━━━┛ ┗━━━━━┛┗━━━━━┛┗━━━━━┛"
						  "┏━━━━━┓┏━━━━━┓┏━━━━━┓ ┏━━━━━┓┏━━━━━┓┏━━━━━┓"
						  "┃ (-) ┃┃ °'\" ┃┃ hyp ┃ ┃ sin ┃┃ cos ┃┃ tan ┃"
						  "┗━━━━━┛┗━━━━━┛┗━━━━━┛ ┗━━━━━┛┗━━━━━┛┗━━━━━┛"
						  "┏━━━━━┓┏━━━━━┓┏━━━━━┓ ┏━━━━━┓┏━━━━━┓┏━━━━━┓"
						  "┃ RCL ┃┃ ENG ┃┃  (  ┃ ┃  )  ┃┃S<=>D┃┃ M+  ┃"
						  "┗━━━━━┛┗━━━━━┛┗━━━━━┛ ┗━━━━━┛┗━━━━━┛┗━━━━━┛"
						  "    ┏━━━━━┓┏━━━━━┓┏━━━━━┓┏━━━━━┓┏━━━━━┓    "
						  "    ┃  7  ┃┃  8  ┃┃  9  ┃┃ DEL ┃┃  AC ┃    "
						  "    ┗━━━━━┛┗━━━━━┛┗━━━━━┛┗━━━━━┛┗━━━━━┛    "
						  "    ┏━━━━━┓┏━━━━━┓┏━━━━━┓┏━━━━━┓┏━━━━━┓    "
						  "    ┃  4  ┃┃  5  ┃┃  6  ┃┃  X  ┃┃  ÷  ┃    "
						  "    ┗━━━━━┛┗━━━━━┛┗━━━━━┛┗━━━━━┛┗━━━━━┛    "
						  "    ┏━━━━━┓┏━━━━━┓┏━━━━━┓┏━━━━━┓┏━━━━━┓    "
						  "    ┃  1  ┃┃  2  ┃┃  3  ┃┃  +  ┃┃  -  ┃    "
						  "    ┗━━━━━┛┗━━━━━┛┗━━━━━┛┗━━━━━┛┗━━━━━┛    "
						  "    ┏━━━━━┓┏━━━━━┓┏━━━━━┓┏━━━━━┓┏━━━━━┓    "
						  "    ┃  0  ┃┃  .  ┃┃ x10 ┃┃ Ans ┃┃  =  ┃    "
						  "    ┗━━━━━┛┗━━━━━┛┗━━━━━┛┗━━━━━┛┗━━━━━┛    ";

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

	arch->regs.lr = 0xffff;

	// Ncurses initialisation
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	nodelay(stdscr, true);
	cbreak();
	mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED, NULL);
	curs_set(0);

	refresh();

	// Create borders
	WINDOW *lcd_border = newwin(10, 50, 0, 0);
	box(lcd_border, 0, 0);
	wrefresh(lcd_border);

	WINDOW *keypad_border = newwin(30, 45, 0, 50);
	box(keypad_border, 0, 0);
	wrefresh(keypad_border);

	WINDOW *cons_border = newwin(20, 50, 10, 0);
	box(cons_border, 0, 0);
	wrefresh(cons_border);

	// LCD Window
	WINDOW *lcd_win = newwin(8, 48, 1, 1);

	// Keypad window
	WINDOW *keypad_win = newwin(28, 43, 1, 51);
	nodelay(keypad_win, true);
	keypad(keypad_win, true);

	wprintw(keypad_win, "%s", keypad_text);
	wrefresh(keypad_win);

	// Console window
	WINDOW *cons_win = newwin(18, 48, 11, 1);

	uint8_t last_ready = 0;

	FILE *log_file = fopen("sim.log", "w");
	while (true) {
		// Log the PCs to a file
		fprintf(log_file, "ADDR: %04x\n", arch->regs.pc);
		//for (int i = 0; i < 16; i++) fprintf(log_file, "%02x ", arch->regs.gp[i]);
		/*fprintf(log_file, "\nPSW: %02x\n", arch->regs.psw);*/
		//fflush(log_file);

		// Hook the render function
		if (arch->regs.pc == 0x2ec0) render(arch, lcd_win);

		if (arch->regs.pc == 0xFFFF) break;

		uint8_t ready = read_mem_data(arch, 0, 0x8e00, 1);

		if ((last_ready == 0) && (ready == 1)) {
			write_mem_data(arch, 0, 0x8e01, 1, 0);
			write_mem_data(arch, 0, 0x8e02, 1, 0);
		}

		last_ready = ready;

		int ch = wgetch(keypad_win);
		if (ch == KEY_MOUSE && ready) {
			MEVENT event;
			if (getmouse(&event) == OK) {
				int x = event.x;
				int y = event.y;
				if (wmouse_trafo(keypad_win, &y, &x, false)) {
					for (int i = 0; i < NUM_KEYS; i++)
						if ((x >= keys[i].x0) && (x <= keys[i].x1) && (y >= keys[i].y0) && (y <= keys[i].y1)) {
							write_mem_data(arch, 0, 0x8e01, 1, keys[i].ki);
							write_mem_data(arch, 0, 0x8e02, 1, keys[i].ko);
						}
				}
			}
		}

		u8_step(arch);
	}

	wprintw(cons_win, "Emulator exited cleanly\n");
	wrefresh(cons_win);

	fflush(log_file);

	while (getch() == ERR) {}

	endwin();

	return 0;
}