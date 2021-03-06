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
		SDL_Texture *info;
		SDL_bool vsync;
		int mode;
	} display;
	struct sprites {
		SDL_Texture **walls;
		SDL_Texture **floors;
		SDL_Texture **icons;
		SDL_Texture **gear_rarity;
		SDL_Texture **gear_type;
		SDL_Texture **gear_attribute;
		SDL_Texture **junk;
		SDL_Texture **ladders;
		SDL_Texture **arrows;
		SDL_Texture **font;
	} sprites;
	int state;
	SDL_bool running;
	SDL_bool newgame;
	int day;
	int minute;
	long int seed;
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
	int **junk;
	int **junk_face;
	struct coords start;
	int start_face;
	struct coords end;
	int end_face;
};

enum room { WALL, ROOM, UNKNOWN, POTENTIAL, START, END, DOOR };

#endif
