#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "draw.h"
#include "home.h"
#include "main.h"
#include "maze.h"
#include "rand.h"
#include "save.h"
#include "user.h"
#include "wall.h"

struct game GAME = {
	{ 1280,		/* display.w */
	  720,		/* display.h */
	  1.0,		/* display.scale_w */
	  1.0,		/* display.scale_h */
	  "CrawlerLike",/* display.name */
	  NULL,		/* display.window */
	  NULL,		/* display.renderer */
	  NULL,		/* display.output */
	  NULL,		/* display.view */
	  NULL,		/* display.char_screen_tex */
	  SDL_FALSE,	/* display.vsync */
	  2 },		/* display.mode */
	{ NULL,		/* sprites.walls */
	  NULL,		/* sprites.floors */
	  NULL,		/* sprites.icons */
	  NULL,		/* sprites.arrows */
	  NULL },	/* sprites.font */
	UNLOADED,	/* state */
	SDL_FALSE,	/* running */
	0,		/* num_maps */
	NULL		/* maps */
};

struct user USER;

/* Function prototypes */

int
main()
{
	SDL_bool redraw;
	SDL_Event event;
	struct game *cur_game;
	struct user *cur_user;
	
	/* Initialize the game */
	SDL_Init(SDL_INIT_EVERYTHING);
	cur_game = &GAME;
	cur_user = &USER;
	load_opts(cur_game);
	display_init(cur_game);
	seed_rng();
	cur_game->running = SDL_TRUE;
	title(cur_game, cur_user);
	
	/* If the game is running after title screen, output the view for the first time */
	if (cur_game->running == SDL_TRUE) {
		render_clear(cur_game, "darkred");
		draw_view(cur_game, cur_user);
		draw_screen(cur_game, cur_user);
		render_present(cur_game, SDL_TRUE);
	}
	/* Enter main game loop */
	while (cur_game->running == SDL_TRUE) {
		/* draw map, player, and render */
		SDL_Delay(10);
		/* poll for an event */
		if (SDL_PollEvent(&event) == 0) continue;
		if (event.type == SDL_QUIT) { /* exit button pressed */
			title(cur_game, cur_user);
			redraw = SDL_TRUE;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE: /* open title screen */
					title(cur_game, cur_user);
					break;
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
				case SDLK_c: /* character screen */
					char_screen(cur_game, cur_user, SDL_TRUE);
					break;
			}
			redraw = SDL_TRUE;
		}
		/* Output the view if the player did something */
		if (redraw == SDL_TRUE && cur_game->running == SDL_TRUE) {
			update_seen(cur_user);
			render_clear(cur_game, "darkred");
			draw_view(cur_game, cur_user);
			draw_screen(cur_game, cur_user);
			render_present(cur_game, SDL_TRUE);
			redraw = SDL_FALSE;
		}
	}

	/* Kill display, SDL, and exit */
	display_quit(cur_game);
	SDL_Quit();
	return 0;
}
