#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>
#include "main.h"
#include "user.h"

extern void	display_init(struct game *cur_game);
extern void	display_quit(struct game *cur_game);
extern void	render_clear(struct game *cur_game, char *col);
extern void	render_present(struct game *cur_game, SDL_bool ingame);
extern void	draw_line(struct game *cur_game, int x1, int y1, int x2, int y2, char *col);
extern void	draw_rect(struct game *cur_game, int x, int y, int w, int h, SDL_bool fill, char *col);
extern void	draw_sprites(struct game *cur_game, SDL_Texture **sprites, int sprite_id, int x, int y, int w, int h, int alpha, SDL_bool hflip, SDL_bool vflip);
extern void	draw_screen(struct game *cur_game, struct user *cur_user);

#endif
