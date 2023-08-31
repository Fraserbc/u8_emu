#include <ncurses.h>
#include <locale.h>

#include "context.h"
#include "core/regs.h"
#include "peripheral.h"

struct keypad_key {
	int x0, y0, x1, y1;
	char name[6];
	uint8_t ki, ko;
};

struct keypad_key keys[] ={
	{0,   0, 6,   2, "Shift", 0x80, 0x01},	// Shift
	{7,   0, 13,  2, "Alpha", 0x80, 0x02},	// Alpha
	{18,  0, 24,  3, "Up",    0x80, 0x04},	// Up
	{25,  2, 28,  4, "Right", 0x80, 0x08},	// Down
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

void init_periph(struct u8_sim_ctx *ctx) {
	// Initialise ncurses
	setlocale(LC_ALL, "");
	initscr();
	noecho();
	cbreak();
	mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED, NULL);
	curs_set(0);

	refresh();

	// Get window size
	int screen_x, screen_y;
	getmaxyx(stdscr, screen_y, screen_x);

	// Setup window borders
	WINDOW *lcd_border = newwin(10, 50, 0, 0);
	box(lcd_border, 0, 0);
	wrefresh(lcd_border);

	WINDOW *keypad_border = newwin(30, 45, 0, 50);
	box(keypad_border, 0, 0);
	wrefresh(keypad_border);

	WINDOW *cons_border = newwin(screen_y - 10, 50, 10, 0);
	box(cons_border, 0, 0);
	wrefresh(cons_border);

	// Setup windows
	ctx->periph.lcd_win = newwin(8, 48, 1, 1);

	// Keypad window
	ctx->periph.keypad_win = newwin(28, 43, 1, 51);
	nodelay(ctx->periph.keypad_win, true);
	keypad(ctx->periph.keypad_win, true);

	wprintw(ctx->periph.keypad_win, "%s", keypad_text);
	wrefresh(ctx->periph.keypad_win);

	// Console window
	ctx->periph.cons_win = newwin(screen_y - 12, 48, 11, 1);
	scrollok(ctx->periph.cons_win, true);

	refresh();

	// TODO: Initialise the keyboard memory region

	// TODO: Initialise the screen memory region
}

uint8_t last_ready = 0;
void update_keyboard(struct u8_sim_ctx *ctx) {
	uint8_t ready = read_mem_data(ctx, 0, 0x8e00, 1);

	if ((last_ready == 0) && (ready == 1)) {
		write_mem_data(ctx, 0, 0x8e01, 1, 0);
		write_mem_data(ctx, 0, 0x8e02, 1, 0);
	}

	last_ready = ready;

	int ch = wgetch(ctx->periph.keypad_win);
	if (ch == KEY_MOUSE) {
		MEVENT event;
		if (getmouse(&event) == OK) {
			if ((event.bstate & BUTTON1_PRESSED) && ready) {
				int x = event.x;
				int y = event.y;
				if (wmouse_trafo(ctx->periph.keypad_win, &y, &x, false)) {
					for (int i = 0; i < NUM_KEYS; i++)
						if ((x >= keys[i].x0) && (x <= keys[i].x1) && (y >= keys[i].y0) && (y <= keys[i].y1)) {
							write_mem_data(ctx, 0, 0x8e01, 1, keys[i].ki);
							write_mem_data(ctx, 0, 0x8e02, 1, keys[i].ko);
						}
				}
			}
		}
	}
}