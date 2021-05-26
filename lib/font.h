#ifndef FONT_H
#define FONT_H

#include "main.h"

extern void	load_font(struct game *cur_game);
extern void	unload_font(struct game *cur_game);
extern void	set_font_alpha(int alpha);
extern void	draw_sentence(struct game *cur_game, int x, int y, char *sentence, float scale);

#endif
