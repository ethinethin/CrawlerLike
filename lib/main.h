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
		SDL_bool vsync;
		int mode;
	} display;
	struct sprites {
		SDL_Texture **walls;
		SDL_Texture **arrows;
		SDL_Texture **font;
	} sprites;
	SDL_bool running;
	int num_maps;
	struct map *maps;
};

struct coords {
	int row;
	int col;
};

struct map {
	int rows;
	int cols;
	int **tiles;
	struct coords start;
	struct coords end;
};

enum room { WALL, ROOM, UNKNOWN, POTENTIAL, START, END };

#endif
