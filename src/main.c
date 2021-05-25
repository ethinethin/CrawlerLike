#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "draw.h"
#include "main.h"
#include "maze.h"
#include "rand.h"
#include "user.h"

struct game GAME = {
	{ 1280,		/* display.w */
	  720,		/* display.h */
	  1.0,		/* display.scale_w */
	  1.0,		/* display.scale_h */
	  "RandoMaze",	/* display.name */
	  NULL,		/* display.window */
	  NULL,		/* display.renderer */
	  NULL,		/* display.output */
	  NULL,		/* display.view */
	  SDL_FALSE,	/* display.vsync */
	  2 },		/* display.mode */
	{ NULL,		/* sprites.walls */
	  NULL },	/* sprites.arrows */
	SDL_FALSE,	/* running */
	0,		/* num_maps */
	NULL		/* maps */
};

struct user USER;

/* Function prototypes */
static void	new_game(struct game *cur_game, struct user *cur_user, int num_maps, int map_dim_row, int map_dim_col);
static void	exit_game(struct game *cur_game, struct user *cur_user);

int
main()
{
	SDL_Event event;
	struct game *cur_game;
	struct user *cur_user;
	/* Initialize the game */
	cur_game = &GAME;
	cur_user = &USER;
	new_game(cur_game, cur_user, 20, 16, 16);

	/* Enter main game loop */
	while (cur_game->running == SDL_TRUE) {
		/* draw map, player, and render */
		render_clear(cur_game, "darkred");
		draw_screen(cur_game, cur_user);
		render_present(cur_game);
		SDL_Delay(33);
		/* poll for an event */
		if (SDL_PollEvent(&event) == 0) continue;
		if (event.type == SDL_QUIT) { /* exit button pressed */
			cur_game->running = SDL_FALSE;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_UP: /* move forward */
				case SDLK_w:
					move_player(cur_game->maps + cur_user->map, cur_user, 1);
					break;
				case SDLK_DOWN: /* move backward */
				case SDLK_s:
					move_player(cur_game->maps + cur_user->map, cur_user, -1);
					break;
				case SDLK_LEFT: /* turn left */
				case SDLK_a:
					turn_player(cur_user, -1);
					break;
				case SDLK_RIGHT: /* turn right */
				case SDLK_d:
					turn_player(cur_user, 1);
					break;
				case SDLK_SPACE: /* change level */
					change_level(cur_game, cur_game->maps + cur_user->map, cur_user);
					break;
			}
			update_seen(cur_user);
		}			
	}

	/* Free memory and exit */
	exit_game(cur_game, cur_user);
	return 0;
}

static void
new_game(struct game *cur_game, struct user *cur_user, int num_maps, int map_dim_row, int map_dim_col)
{
	int i;

	/* Initialize the display */
	display_init(cur_game);
	/* Seed random number generator */
	seed_rng();
	/* allocate space for maps and seen */
	cur_game->maps = malloc(sizeof(*cur_game->maps)*num_maps);
	cur_game->num_maps = num_maps;
	cur_user->seen = malloc(sizeof(*cur_user->seen)*num_maps);
	cur_user->num_seen = num_maps;
	/* Figure out map sizes needed for given dimensions */
	map_dim_row = map_dim_row * 2 + 1;
	map_dim_col = map_dim_col * 2 + 1;
	/* Initialize all maps and seem with end of previous map as start */
	for (i = 0; i < num_maps; i++) {
		init_map(&cur_game->maps[i], map_dim_row, map_dim_col);
		init_seen(&cur_user->seen[i], map_dim_row, map_dim_col);
		if (i == 0) {
			populate_map(&cur_game->maps[0], map_dim_row/2 + 1, map_dim_col/2 + 1);
			cur_user->map = 0;
			cur_user->row = map_dim_row/2 + 1;
			cur_user->col = map_dim_col/2 + 1;
			cur_user->facing = NORTH;
		} else {
			populate_map(&cur_game->maps[i], cur_game->maps[i - 1].end.row, cur_game->maps[i - 1].end.col);
		}
	}
	/* Update starting square as seen */
	update_seen(cur_user);
	/* Game is now running */
	cur_game->running = SDL_TRUE;
}

static void
exit_game(struct game *cur_game, struct user *cur_user)
{
	int i;
	for (i = 0; i < cur_game->num_maps; i++) {
		kill_map(&cur_game->maps[i]);
		kill_seen(&cur_user->seen[i]);
	}
	display_quit(cur_game);
}
