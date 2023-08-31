#ifndef U8_PERIPH_H
#define U8_PERIPH_H

#include <ncurses.h>

struct u8_sim_ctx;

struct u8_periph {
	WINDOW *lcd_win;
	WINDOW *keypad_win;
	WINDOW *cons_win;
};

void init_periph(struct u8_sim_ctx *ctx);
void update_keyboard(struct u8_sim_ctx *ctx);

#endif