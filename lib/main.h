#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>

struct game {
	struct display {
		int w;
		int h;
		float scale_w;
		float scale_h;
		const char *name;
		SDL_Window *window;
		SDL_Renderer *renderer;
		SDL_Texture *output;
		SDL_Texture *view;
		SDL_Texture *char_screen_tex;
		SDL_bool vsync;
		int mode;
	} display;
	struct sprites {
		SDL_Texture **walls;
		SDL_Texture **floors;
		SDL_Texture **icons;
		SDL_Texture **arrows;
		SDL_Texture **font;
	} sprites;
	int state;
	SDL_bool running;
	int num_maps;
	struct map *maps;
};

enum display_mode { FULLSCREEN_DESKTOP, FULLSCREEN, WINDOW };

struct coords {
	int row;
	int col;
};

struct map {
	int sprite;
	int rows;
	int cols;
	int **tiles;
	struct coords start;
	struct coords end;
};

enum room { WALL, ROOM, UNKNOWN, POTENTIAL, START, END, DOOR };

#endif
