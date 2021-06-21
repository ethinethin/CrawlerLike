#include <string.h>
#include <SDL2/SDL.h>
#include "char.h"
#include "draw.h"
#include "font.h"
#include "gear.h"
#include "home.h"
#include "main.h"
#include "maze.h"
#include "rand.h"
#include "save.h"
#include "user.h"

struct menu_item {
	SDL_Rect rect;
	int state;
	char *sentence;
	float scale;
	SDL_bool clickable;
	int results;
};
enum gamestate { MENU_LOADED, MENU_UNLOADED, MENU_BOTH };

/* Function prototypes */
static void		prep_menu_items(struct menu_item *cur_menu);
static void		draw_menu(struct game *cur_game, struct menu_item cur_menu[]);
static int		click_menu(struct game *cur_game, struct menu_item cur_menu[], int x, int y);
static void		draw_title(struct game *cur_game, SDL_bool save_exists);
static void		title_click(struct game *cur_game, struct user *cur_user, int x, int y, int *whichscreen, SDL_bool save_exists);
static void		draw_new(struct game *cur_game);
static void		new_click(struct game *cur_game, struct user *cur_user, int *whichscreen, int x, int y);
static void		draw_options(struct game *cur_game);
static void		options_click(struct game *cur_game, int x, int y, int *whichscreen);
static SDL_bool		change_resolution(struct game *cur_game, int w, int h);
static void		draw_yesno(struct game *cur_game, char *message, SDL_bool view, SDL_bool char_sheet, SDL_bool msgbox);
static int		yesno_click(int x, int y, SDL_bool msgbox);
static void		new_game(struct game *cur_game, struct user *cur_user, int num_maps, int map_dim_row, int map_dim_col);
static void		exit_game(struct game *cur_game, struct user *cur_user);

enum titlescreen { TITLE, NEWGAME, OPTIONS, GAMESCREEN };

void
title(struct game *cur_game, struct user *cur_user)
{
	int x, y;
	int whichscreen;
	SDL_Event event;
	SDL_bool loop;
	SDL_bool redraw;
	struct savefile_info info;
	
	
	/* See if there is a game to load */
	load_info(&info);

	whichscreen = TITLE;
	/* Draw the title screen */
	render_clear(cur_game, "lightblue");
	draw_title(cur_game, info.exists);
	render_present(cur_game, SDL_FALSE);
	redraw = SDL_FALSE;
	loop = SDL_TRUE;
	while (loop == SDL_TRUE && whichscreen != GAMESCREEN) {
		SDL_Delay(10);
		/* poll for an event */
		if (SDL_PollEvent(&event) == 0) continue;
		if (event.type == SDL_QUIT) { /* exit button pressed */
			redraw = SDL_TRUE;
			if (whichscreen == TITLE) {
				if (cur_game->state == UNLOADED) {
					loop = SDL_FALSE;
					cur_game->running = SDL_FALSE;
				} else if (yes_no(cur_game, "Save and quit game in progress?", SDL_FALSE, SDL_FALSE, SDL_FALSE) == SDL_TRUE) {
					save_all(cur_game, cur_user);
					exit_game(cur_game, cur_user);
					cur_game->running = SDL_FALSE;
					loop = SDL_FALSE;
				}
			} else {
				whichscreen = TITLE;
			}
		} else if (event.type == SDL_KEYDOWN) {
			redraw = SDL_TRUE;
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					if (whichscreen == TITLE && cur_game->state == LOADED) loop = SDL_FALSE;
					else whichscreen = TITLE;
					break;
			}
		} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			redraw = SDL_TRUE;
			x = event.button.x / cur_game->display.scale_w;
			y = event.button.y / cur_game->display.scale_h;
			/* Poll until you get mouse up event or the next click might get lost */
			while (event.type != SDL_MOUSEBUTTONUP) SDL_PollEvent(&event);
			if (whichscreen == TITLE) title_click(cur_game, cur_user, x, y, &whichscreen, info.exists);
			else if (whichscreen == OPTIONS) options_click(cur_game, x, y, &whichscreen);
			else if (whichscreen == NEWGAME) new_click(cur_game, cur_user, &whichscreen, x, y);
		}
		/* if you're staying in the loop, draw appropriate screen */
		if (redraw == SDL_TRUE && loop == SDL_TRUE && whichscreen != GAMESCREEN) {
			render_clear(cur_game, "lightblue");
			if (whichscreen == TITLE) draw_title(cur_game, info.exists);
			else if (whichscreen == OPTIONS) draw_options(cur_game);
			else if (whichscreen == NEWGAME) draw_new(cur_game);
			render_present(cur_game, SDL_FALSE);
			redraw = SDL_FALSE;
		}
	}
}

static void
prep_menu_items(struct menu_item *cur_menu)
{
	int i;
	
	/* If the menu hasn't been prepped for clicking, prep it here */
	if (cur_menu[0].rect.w != 0) return;
	for (i = 0; cur_menu[i].rect.x != -1; i++) {
		cur_menu[i].rect.w = strlen(cur_menu[i].sentence) * 192 * cur_menu[i].scale;
		cur_menu[i].rect.h = (int) (208.0 * cur_menu[i].scale);
	}
}

static void
draw_menu(struct game *cur_game, struct menu_item cur_menu[])
{
	int i;
	SDL_bool draw;

	for (i = 0; cur_menu[i].rect.x != -1; i++) {
		draw = SDL_FALSE;
		if (cur_menu[i].state == MENU_BOTH) {
			draw = SDL_TRUE;
		} else if (cur_menu[i].state == MENU_LOADED && cur_game->state == LOADED) {
			draw = SDL_TRUE;
		} else if (cur_menu[i].state == MENU_UNLOADED && cur_game->state == UNLOADED) {
			draw = SDL_TRUE;
		}
		if (draw == SDL_TRUE) draw_sentence(cur_game, cur_menu[i].rect.x, cur_menu[i].rect.y, cur_menu[i].sentence, cur_menu[i].scale);
	}
}

static int
click_menu(struct game *cur_game, struct menu_item cur_menu[], int x, int y)
{
	int i;
	
	for (i = 0; cur_menu[i].rect.x != -1; i++) {
		if (cur_menu[i].clickable == SDL_TRUE && (cur_menu[i].state == MENU_BOTH ||
		    (cur_menu[i].state == MENU_LOADED && cur_game->state == LOADED) ||
		    (cur_menu[i].state == MENU_UNLOADED && cur_game->state == UNLOADED))) {
			if (x >= cur_menu[i].rect.x && x <= cur_menu[i].rect.x + cur_menu[i].rect.w &&
			    y >= cur_menu[i].rect.y && y <= cur_menu[i].rect.y + cur_menu[i].rect.h) {
				break;
			}
		}
	}
	return i;
}

enum title_menu_results { TITLE_NULL, TITLE_NEW, TITLE_LOAD, TITLE_OPTS, TITLE_BACK, TITLE_EXIT };
struct menu_item title_menu[] = {
	{ {20, 20, 0, 0}, MENU_BOTH, "CrawlerLike", 0.5, SDL_FALSE, TITLE_NULL },
	{ {100, 187, 0, 0}, MENU_BOTH, "New Game", 0.1, SDL_TRUE, TITLE_NEW },
	{ {100, 212, 0, 0}, MENU_UNLOADED, "Continue Game", 0.1, SDL_TRUE, TITLE_LOAD },
	{ {100, 287, 0, 0}, MENU_UNLOADED, "Exit", 0.1, SDL_TRUE, TITLE_EXIT }, 
	{ {100, 287, 0, 0}, MENU_LOADED, "Save and Exit", 0.1, SDL_TRUE, TITLE_EXIT },
	{ {10, 679, 0, 0}, MENU_LOADED, "Back to Game", 0.15, SDL_TRUE, TITLE_BACK },
	{ {100, 262, 0, 0}, MENU_BOTH, "Options", 0.1, SDL_TRUE, TITLE_OPTS },
	{ {-1, -1, 0, 0}, MENU_BOTH, NULL, 0, SDL_FALSE, TITLE_NULL }
};

static void
draw_title(struct game *cur_game, SDL_bool save_exists)
{
	/* If no save game exists, get rid of the Continue option */
	if (save_exists == SDL_FALSE) title_menu[2].sentence = "";
	prep_menu_items(title_menu);
	draw_menu(cur_game, title_menu);
}

static void
title_click(struct game *cur_game, struct user *cur_user, int x, int y, int *whichscreen, SDL_bool save_exists)
{
	int results;
	
	results = click_menu(cur_game, title_menu, x, y);
	switch(title_menu[results].results) {
		case TITLE_BACK:
			*whichscreen = GAMESCREEN;
			break;
		case TITLE_NEW:
			if (cur_game->state == UNLOADED && save_exists == SDL_FALSE) {
				*whichscreen = NEWGAME;
			} else if (cur_game->state == UNLOADED && save_exists == SDL_TRUE) {
				if (yes_no(cur_game, "Starting a new game will erase\nthe current quicksave.\n\nIs this okay?", SDL_FALSE, SDL_FALSE, SDL_FALSE) == SDL_TRUE) {
					*whichscreen = NEWGAME;
					return;
				}
			} else if (cur_game->state == LOADED) {
				if (yes_no(cur_game, "Game in progress will be erasedto start a new game.\n\nIs this okay?", SDL_FALSE, SDL_FALSE, SDL_FALSE) == SDL_TRUE) {
					*whichscreen = NEWGAME;
					return;
				}
			}
			break;
		case TITLE_LOAD:
			if (cur_game->state == UNLOADED && save_exists == SDL_TRUE) {
				*whichscreen = GAMESCREEN;
				load_all(cur_game, cur_user);
			}
			break;
		case TITLE_OPTS:
			*whichscreen = OPTIONS;
			break;
		case TITLE_EXIT:
			if (cur_game->state == UNLOADED) {
				cur_game->running = SDL_FALSE;
				*whichscreen = GAMESCREEN;
			} else if (cur_game->state == LOADED) {
				if (yes_no(cur_game, "Save game in progress to the\nquicksave slot and exit?", SDL_FALSE, SDL_FALSE, SDL_FALSE) == SDL_TRUE) {
					save_all(cur_game, cur_user);
					cur_game->running = SDL_FALSE;
					exit_game(cur_game, cur_user);
					*whichscreen = GAMESCREEN;
				}
			}
			break;
	}
}

enum new_menu_results { NEW_NULL, NEW_EASY, NEW_NORMAL, NEW_HARD, NEW_ENDLESS, NEW_BACK };
struct menu_item new_menu[] = {
	{ {20, 20, 0, 0}, MENU_BOTH, "New Game", 0.5, SDL_FALSE, NEW_NULL },
	{ {100, 187, 0, 0}, MENU_BOTH, "Easy", 0.1, SDL_TRUE, NEW_EASY },
	{ {100, 212, 0, 0}, MENU_BOTH, "Normal", 0.1, SDL_TRUE, NEW_NORMAL },
	{ {100, 237, 0, 0}, MENU_BOTH, "Hard", 0.1, SDL_TRUE, NEW_HARD },
	{ {100, 262, 0, 0}, MENU_BOTH, "Endless", 0.1, SDL_TRUE, NEW_ENDLESS },
	{ {10, 679, 0, 0}, MENU_BOTH, "Back to Title Screen", 0.15, SDL_TRUE, NEW_BACK },
	{ {-1, -1, 0, 0}, MENU_BOTH, NULL, 0, SDL_FALSE, NEW_NULL }
};

static void
draw_new(struct game *cur_game)
{
	prep_menu_items(new_menu);
	draw_menu(cur_game, new_menu);
}

static void
new_click(struct game *cur_game, struct user *cur_user, int *whichscreen, int x, int y)
{
	int levels;
	int results;
	
	results = click_menu(cur_game, new_menu, x, y);
	switch(new_menu[results].results) {
		case NEW_EASY:
			levels = 25;
			break;
		case NEW_NORMAL:
			levels = 50;
			break;
		case NEW_HARD:
			levels = 100;
			break;
		case NEW_ENDLESS:
			levels = 101;
			break;
		case NEW_BACK:
			*whichscreen = TITLE;
		default: /* Fall through */
			return;
	}
	if (cur_game->state == LOADED) exit_game(cur_game, cur_user);
	new_game(cur_game, cur_user, levels, 16, 16);
	*whichscreen = GAMESCREEN;
}

static void
draw_options(struct game *cur_game)
{
	draw_sentence(cur_game, 20, 20, "Options", 0.5);
	draw_sentence(cur_game, 100, 187, "Display Mode", 0.15);
	draw_sentence(cur_game, 100, 228, "  Fullscreen", 0.1);
	if (cur_game->display.mode == FULLSCREEN || cur_game->display.mode == FULLSCREEN_DESKTOP) draw_sentence(cur_game, 100, 228, ">", 0.1);
	draw_sentence(cur_game, 100, 253, "  Window", 0.1);
	if (cur_game->display.mode == WINDOW) draw_sentence(cur_game, 100, 253, ">", 0.1);
	draw_sentence(cur_game, 100, 303, "VSync", 0.15);
	draw_sentence(cur_game, 100, 344, "  On", 0.1);
	if (cur_game->display.vsync == SDL_TRUE) draw_sentence(cur_game, 100, 344, ">", 0.1);
	draw_sentence(cur_game, 100, 369, "  Off", 0.1);
	if (cur_game->display.vsync == SDL_FALSE) draw_sentence(cur_game, 100, 369, ">", 0.1);
	draw_sentence(cur_game, 600, 187, "Resolution", 0.15);
	draw_sentence(cur_game, 600, 228, "  Default", 0.1);
	draw_sentence(cur_game, 600, 253, "  1280x720", 0.1);
	if (cur_game->display.w == 1280 && cur_game->display.h == 720) draw_sentence(cur_game, 600, 253, ">", 0.1);
	draw_sentence(cur_game, 600, 278, "  1360x768 ", 0.1);
	if (cur_game->display.w == 1360 && cur_game->display.h == 768) draw_sentence(cur_game, 600, 278, ">", 0.1);
	draw_sentence(cur_game, 600, 303, "  1366x768", 0.1);
	if (cur_game->display.w == 1366 && cur_game->display.h == 768) draw_sentence(cur_game, 600, 303, ">", 0.1);
	draw_sentence(cur_game, 600, 328, "  1440x900 ", 0.1);
	if (cur_game->display.w == 1440 && cur_game->display.h == 900) draw_sentence(cur_game, 600, 328, ">", 0.1);
	draw_sentence(cur_game, 600, 353, "  1600x900 ", 0.1);
	if (cur_game->display.w == 1600 && cur_game->display.h == 900) draw_sentence(cur_game, 600, 353, ">", 0.1);
	draw_sentence(cur_game, 600, 378, "  1600x1080 ", 0.1);
	if (cur_game->display.w == 1600 && cur_game->display.h == 1080) draw_sentence(cur_game, 600, 378, ">", 0.1);
	draw_sentence(cur_game, 600, 403, "  1680x1050 ", 0.1);
	if (cur_game->display.w == 1680 && cur_game->display.h == 1050) draw_sentence(cur_game, 600, 403, ">", 0.1);
	draw_sentence(cur_game, 600, 428, "  1920x1080 ", 0.1);
	if (cur_game->display.w == 1920 && cur_game->display.h == 1080) draw_sentence(cur_game, 600, 428, ">", 0.1);
	draw_sentence(cur_game, 600, 453, "  1920x1200 ", 0.1);
	if (cur_game->display.w == 1920 && cur_game->display.h == 1200) draw_sentence(cur_game, 600, 453, ">", 0.1);
	draw_sentence(cur_game, 600, 478, "  2560x1440 ", 0.1);
	if (cur_game->display.w == 2560 && cur_game->display.h == 1440) draw_sentence(cur_game, 600, 478, ">", 0.1);
	draw_sentence(cur_game, 600, 503, "  2560x1600 ", 0.1);
	if (cur_game->display.w == 2560 && cur_game->display.h == 1600) draw_sentence(cur_game, 600, 503, ">", 0.1);
	draw_sentence(cur_game, 600, 528, "  3840x2160 ", 0.1);
	if (cur_game->display.w == 3840 && cur_game->display.h == 2160) draw_sentence(cur_game, 600, 528, ">", 0.1);
	draw_sentence(cur_game, 10, 679, "Back to Title Screen", 0.15);
}

static void
options_click(struct game *cur_game, int x, int y, int *whichscreen)
{
	SDL_bool changed = SDL_FALSE;
	
	if (x >= 10 && x < 570 && y >= 681 && y < 710) {
		*whichscreen = TITLE;
	} else if (x >= 135 && x < 328) {
		if (y >= 228 && y < 248 && cur_game->display.mode == WINDOW) {
			cur_game->display.mode = FULLSCREEN_DESKTOP;
			changed = SDL_TRUE;
		} else if (y >= 253 && y < 273 && cur_game->display.mode != WINDOW) {
			cur_game->display.mode = WINDOW;
			changed = SDL_TRUE;
		} else if (y >= 344 && y < 364 && cur_game->display.vsync == SDL_FALSE) {
			cur_game->display.vsync = SDL_TRUE;
			changed = SDL_TRUE;
		} else if (y >= 369 && y < 389 && cur_game->display.vsync == SDL_TRUE) {
			cur_game->display.vsync = SDL_FALSE;
			changed = SDL_TRUE;
		}
	} else if (x >= 633 && x < 814) {
		if (y >= 228 && y < 248) {
			if (cur_game->display.mode == WINDOW && (cur_game->display.w != 1280 || cur_game->display.h != 720)) {
				changed = change_resolution(cur_game, 1280, 720); 
			} else if (cur_game->display.mode == FULLSCREEN) {
				cur_game->display.mode = FULLSCREEN_DESKTOP; changed = SDL_TRUE;
			}
		} else if (y >= 253 && y < 273) {
			changed = change_resolution(cur_game, 1280, 720);
		} else if (y >= 278 && y < 298) {
			changed = change_resolution(cur_game, 1360, 768);
		} else if (y >= 303 && y < 323) {
			changed = change_resolution(cur_game, 1366, 768);
		} else if (y >= 328 && y < 348) {
			changed = change_resolution(cur_game, 1440, 900);
		} else if (y >= 353 && y < 373) {
			changed = change_resolution(cur_game, 1600, 900);
		} else if (y >= 378 && y < 398) {
			changed = change_resolution(cur_game, 1600, 1080);
		} else if (y >= 403 && y < 423) {
			changed = change_resolution(cur_game, 1680, 1050);
		} else if (y >= 428 && y < 448) {
			changed = change_resolution(cur_game, 1920, 1080);
		} else if (y >= 453 && y < 473) {
			changed = change_resolution(cur_game, 1920, 1200);
		} else if (y >= 478 && y < 498) {
			changed = change_resolution(cur_game, 2560, 1440);
		} else if (y >= 503 && y < 523) {
			changed = change_resolution(cur_game, 2560, 1600);
		} else if (y >= 528 && y < 548) {
			changed = change_resolution(cur_game, 3840, 2160);
		}
	}
	if (changed == SDL_TRUE) {
		display_quit(cur_game);
		display_init(cur_game);
		save_opts(cur_game);
	}
}

static SDL_bool
change_resolution(struct game *cur_game, int w, int h)
{
	if (cur_game->display.w == w && cur_game->display.h == h) {
		/* Clicking on already-set resolution */
		return SDL_FALSE;
	} else {
		/* If they are in fullscreen desktop mode, change to fullscreen mode */
		if (cur_game->display.mode == FULLSCREEN_DESKTOP) {
			cur_game->display.mode = FULLSCREEN;
		}
		cur_game->display.w = w;
		cur_game->display.h = h;
		return SDL_TRUE;
	}
}

SDL_bool
yes_no(struct game *cur_game, char *message, SDL_bool view, SDL_bool char_sheet, SDL_bool msgbox)
{
	SDL_Event event;
	SDL_bool loop;
	int answer;
	
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		/* draw yesno screen - this renders a yes no box on top of the previously rendered window */
		draw_yesno(cur_game, message, view, char_sheet, msgbox);
		/* Wait for an event */
		SDL_WaitEvent(&event);
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_y:
				case SDLK_RETURN:
					answer = RESULTS_YES;
					loop = SDL_FALSE;
					break;
				case SDLK_n:
				case SDLK_ESCAPE:
					answer = RESULTS_NO;
					loop = SDL_FALSE;
					break;
				default:
					break;
			}
		} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			answer = yesno_click(event.button.x / cur_game->display.scale_w,
					     event.button.y / cur_game->display.scale_h, msgbox);
			if (answer == RESULTS_NULL) continue;
			loop = SDL_FALSE;
		}
	}
	if (answer == RESULTS_YES) {
		return SDL_TRUE;
	} else {
		return SDL_FALSE;
	}
}

static void
draw_yesno(struct game *cur_game, char *message, SDL_bool view, SDL_bool char_sheet, SDL_bool msgbox)
{
	SDL_Rect out_src = { 0, 0, 1280, 720 };
	SDL_Rect out_dest = { 0, 0, cur_game->display.w, cur_game->display.h };
	SDL_Rect view_src = { 341, 10, 929, 700 };
	SDL_Rect view_dest = { 341 * cur_game->display.scale_w, 10 * cur_game->display.scale_h, 929 * cur_game->display.scale_w, 700  * cur_game->display.scale_h };	
	SDL_Rect char_src = { 0, 0, 1152, 648 };
	SDL_Rect char_dest = { 64 * cur_game->display.scale_w, 36 * cur_game->display.scale_h, 1152* cur_game->display.scale_w, 648 * cur_game->display.scale_h };

	/* Reset render target to the renderer */
	SDL_SetRenderTarget(cur_game->display.renderer, NULL);
	/* Clear the renderer */
	SDL_RenderClear(cur_game->display.renderer);
	/* Copy the output texture to the renderer */
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.output, &out_src, &out_dest);
	/* Draw others? */
	if (view == SDL_TRUE) {
		SDL_RenderCopy(cur_game->display.renderer, cur_game->display.view, &view_src, &view_dest);
	}
	if (char_sheet == SDL_TRUE) {
		SDL_RenderCopy(cur_game->display.renderer, cur_game->display.char_screen_tex, &char_src, &char_dest);
	}
	/* Render a yesno box - scaled */
	draw_rect(cur_game,
		  340 * cur_game->display.scale_w, 240 * cur_game->display.scale_h,
		  600 * cur_game->display.scale_w, 170 * cur_game->display.scale_h,
		  SDL_TRUE, "lightblue");
	draw_rect(cur_game,
		  340 * cur_game->display.scale_w, 240 * cur_game->display.scale_h,
		  600 * cur_game->display.scale_w, 170 * cur_game->display.scale_h,
		  SDL_FALSE, "white");
	draw_sentence_xlimited(cur_game, 350 * cur_game->display.scale_w, 250 * cur_game->display.scale_h, message,
			       0.1 * cur_game->display.scale_w, (340 + 600) * cur_game->display.scale_w);
	if (msgbox == SDL_TRUE) {
		draw_sentence_xlimited(cur_game, 480 * cur_game->display.scale_w, 380 * cur_game->display.scale_h, "Okay",
				       0.1 * cur_game->display.scale_w, (340 + 600) * cur_game->display.scale_w);
	} else {
		draw_sentence_xlimited(cur_game, 480 * cur_game->display.scale_w, 380 * cur_game->display.scale_h, "Yes",
				       0.1 * cur_game->display.scale_w, (340 + 600) * cur_game->display.scale_w);
		draw_sentence_xlimited(cur_game, 680 * cur_game->display.scale_w, 380 * cur_game->display.scale_h, "No",
				       0.1 * cur_game->display.scale_w, (340 + 600) * cur_game->display.scale_w);
	}
	/* Present */
	SDL_RenderPresent(cur_game->display.renderer);
}

static int
yesno_click(int x, int y, SDL_bool msgbox)
{
	if (y >= 380 && y < 400) {
		if (x >= 480 && x < 552) {
			return RESULTS_YES;
		} else if (x >= 680 && x < 725 && msgbox == SDL_FALSE) {
			return RESULTS_NO;
		}
	}
	return RESULTS_NULL;
}

static void
new_game(struct game *cur_game, struct user *cur_user, int num_maps, int map_dim_row, int map_dim_col)
{
	int i;

	/* allocate space for maps and seen */
	cur_game->maps = malloc(sizeof(*cur_game->maps)*num_maps);
	cur_game->num_maps = num_maps;
	cur_user->seen = malloc(sizeof(*cur_user->seen)*num_maps);
	cur_user->num_seen = num_maps;
	/* Figure out map sizes needed for given dimensions */
	map_dim_row = map_dim_row * 2 + 1;
	map_dim_col = map_dim_col * 2 + 1;
	/* Seed random number generator */
	cur_game->seed = seed_rng();
	/* Initialize all maps and seen and generate all maps */
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
	/* Initialize the new character */
	init_char(cur_game, cur_user);
	/* Update starting square as seen */
	update_seen(cur_user);
	/* Setup the gear table */
	init_gear();
	/* Signify that this is a new game */
	cur_game->newgame = SDL_TRUE;
	/* Game is now loaded and running */
	cur_game->state = LOADED;
}

static void
exit_game(struct game *cur_game, struct user *cur_user)
{
	int i;
	/* This might get called before a game is loaded, so just ignore it */
	if (cur_game->state == UNLOADED) return;
	
	/* Destroy the character */
	kill_char(cur_user);
	/* Kill the map and seen */
	for (i = 0; i < cur_game->num_maps; i++) {
		kill_map(&cur_game->maps[i]);
		kill_seen(&cur_user->seen[i]);
	}
	free(cur_game->maps);
	free(cur_user->seen);
	/* Kill the gear table */
	kill_gear();
	cur_game->state = UNLOADED;
}
