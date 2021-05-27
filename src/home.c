#include <SDL2/SDL.h>
#include "draw.h"
#include "font.h"
#include "home.h"
#include "main.h"
#include "user.h"

/* Function prototypes */
static void		draw_title(struct game *cur_game, int state);
static SDL_bool		title_click(struct game *cur_game, struct user *cur_user, int state, int x, int y);
static void		saveload(struct game *cur_game, struct user *cur_user);
static void		draw_saveload(struct game *cur_game);
static SDL_bool		options(struct game *cur_game);
static void		draw_options(struct game *cur_game);
static SDL_bool		yes_no(struct game *cur_game, char *message);
static void		draw_yesno(struct game *cur_game, char *message);

void
title(struct game *cur_game, struct user *cur_user, int state)
{
	SDL_Event event;
	SDL_bool loop;
	
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		/* draw title screen and render */
		render_clear(cur_game, "darkblue");
		draw_title(cur_game, state);
		render_present(cur_game);
		SDL_Delay(10);
		/* poll for an event */
		if (SDL_PollEvent(&event) == 0) continue;
		if (event.type == SDL_QUIT) { /* exit button pressed */
			cur_game->running = SDL_FALSE;
			loop = SDL_FALSE;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					loop = SDL_FALSE;
					break;
			}
		} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			loop = title_click(cur_game, cur_user, state,
					   event.button.x / cur_game->display.scale_w,
					   event.button.y / cur_game->display.scale_h);
		}
	}
}

static void
draw_title(struct game *cur_game, int state)
{
	draw_sentence(cur_game, 20, 20, "CrawlerLike", 0.5);
	draw_sentence(cur_game, 100, 187, "New Game", 0.1);
	draw_sentence(cur_game, 100, 212, "Load Game", 0.1);
	draw_sentence(cur_game, 100, 237, "Save Game", 0.1);
	draw_sentence(cur_game, 100, 262, "Options", 0.1);
	draw_sentence(cur_game, 100, 287, "Exit", 0.1);
	if (state == LOADED) draw_sentence(cur_game, 10, 679, "Back to Game", 0.15);
}

static SDL_bool
title_click(struct game *cur_game, struct user *cur_user, int state, int x, int y)
{
	if (x >= 10 && x <= 340 && y >= 679 && y < 709 && state == LOADED) {
		return SDL_FALSE;
	} else if (x >= 100 && x <= 280) {
		if (y >= 187 && y < 207) {
			if (state == LOADED && yes_no(cur_game, "Game in progress will be lost.\nContinue?") == SDL_TRUE) {
				/* Make a new game */
				printf("Making a new game!\n");
			}
		} else if (y >= 212 && y < 232) {
			printf("Load game clicked\n");
			/* I don't have saving and loading implemented yet */
		} else if (y >= 237 && y < 257) {
			printf("Save game clicked\n");
			/* I don't have saving and loading implemented yet */
		} else if (y >= 262 && y < 282) {
			return options(cur_game);
		} else if (y >= 287 && y < 397) {
			cur_game->running = SDL_FALSE;
			return SDL_FALSE;
		}
	}
	return SDL_TRUE;
}

static void
saveload(struct game *cur_game, struct user *cur_user)
{
	if (cur_game->running == SDL_TRUE || cur_user->map >= 0) return;
	/* I don't have saving and loading implemented yet */
}

static void
draw_saveload(struct game *cur_game)
{
	if (cur_game->running == SDL_TRUE) return;
	/* I don't have saving and loading implemented yet */
}


static SDL_bool
options(struct game *cur_game)
{
	SDL_Event event;
	SDL_bool loop;
	
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		/* draw options screen and render */
		render_clear(cur_game, "darkblue");
		draw_options(cur_game);
		render_present(cur_game);
		SDL_Delay(10);
		/* poll for an event */
		if (SDL_PollEvent(&event) == 0) continue;
		if (event.type == SDL_QUIT) { /* exit button pressed */
			cur_game->running = SDL_FALSE;
			return SDL_FALSE;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					loop = SDL_FALSE;
					break;
			}
		} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			/* Clicking in options at
				event.button.x / cur_game->display.scale_w
				event.button.y / cur_game->display.scale_h
			*/
		}
	}
	return SDL_TRUE;
}

static void
draw_options(struct game *cur_game)
{
	draw_sentence(cur_game, 20, 20, "Options", 0.5);
	draw_sentence(cur_game, 100, 187, "Display Mode", 0.15);
	draw_sentence(cur_game, 100, 228, "  Fullscreen", 0.1);
	draw_sentence(cur_game, 100, 253, "  Window", 0.1);
	draw_sentence(cur_game, 100, 303, "VSync", 0.15);
	draw_sentence(cur_game, 100, 344, "  On", 0.1);
	draw_sentence(cur_game, 100, 369, "  Off", 0.1);
	draw_sentence(cur_game, 600, 187, "Resolution", 0.15);
	draw_sentence(cur_game, 600, 228, "  Default", 0.1);
	draw_sentence(cur_game, 600, 253, "  1280x720", 0.1);
	draw_sentence(cur_game, 600, 278, "  1360x768 ", 0.1);
	draw_sentence(cur_game, 600, 303, "  1366x768", 0.1);
	draw_sentence(cur_game, 600, 328, "  1440x900 ", 0.1);
	draw_sentence(cur_game, 600, 353, "  1600x900 ", 0.1);
	draw_sentence(cur_game, 600, 378, "  1600x1080 ", 0.1);
	draw_sentence(cur_game, 600, 403, "  1680x1050 ", 0.1);
	draw_sentence(cur_game, 600, 428, "  1920x1080 ", 0.1);
	draw_sentence(cur_game, 600, 453, "  1920x1200 ", 0.1);
	draw_sentence(cur_game, 600, 478, "  2560x1440 ", 0.1);
	draw_sentence(cur_game, 600, 503, "  2560x1600 ", 0.1);
	draw_sentence(cur_game, 600, 528, "  3840x2160 ", 0.1);
	draw_sentence(cur_game, 10, 679, "Back to Title Screen", 0.15);
}


static SDL_bool
yes_no(struct game *cur_game, char *message)
{
	SDL_Event event;
	SDL_bool loop;
	SDL_bool answer;
	
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		/* draw yesno screen - this renders a yes no box on top of the previously rendered window */
		draw_yesno(cur_game, message);
		SDL_Delay(10);
		/* poll for an event */
		if (SDL_PollEvent(&event) == 0) continue;
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_y:
					answer = SDL_TRUE;
					loop = SDL_FALSE;
					break;
				default:
					answer = SDL_FALSE;
					loop = SDL_FALSE;
					break;
			}
		} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			/* Clicking in options at
				event.button.x / cur_game->display.scale_w
				event.button.y / cur_game->display.scale_h
			*/
		}
	}
	return answer;
}

static void
draw_yesno(struct game *cur_game, char *message)
{
	SDL_Rect src = { 0, 0, 1280, 720 };
	SDL_Rect dest = { 0, 0, cur_game->display.w, cur_game->display.h };

	/* Reset render target to the renderer */
	SDL_SetRenderTarget(cur_game->display.renderer, NULL);
	/* Clear the renderer */
	SDL_RenderClear(cur_game->display.renderer);
	/* Copy the output texture to the renderer */
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.output, &src, &dest);
	/* Render a yesno box - scaled */
	draw_rect(cur_game,
		  340 * cur_game->display.scale_w, 240 * cur_game->display.scale_h,
		  600 * cur_game->display.scale_w, 200 * cur_game->display.scale_w,
		  SDL_TRUE, "black");
	draw_rect(cur_game,
		  340 * cur_game->display.scale_w, 240 * cur_game->display.scale_h,
		  600 * cur_game->display.scale_w, 200 * cur_game->display.scale_w,
		  SDL_FALSE, "white");
	draw_sentence(cur_game, 350 * cur_game->display.scale_w, 250 * cur_game->display.scale_h, message, 0.1 * cur_game->display.scale_w);
	draw_sentence(cur_game, 380 * cur_game->display.scale_w, 330 * cur_game->display.scale_h, "      Yes          No", 0.1 * cur_game->display.scale_w);
	/* Present */
	SDL_RenderPresent(cur_game->display.renderer);
}
