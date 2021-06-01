#include <SDL2/SDL.h>
#include "cols.h"
#include "draw.h"
#include "font.h"
#include "main.h"
#include "maze.h"
#include "user.h"
#include "wall.h"

/* Function prototypes */
static void	load_sprites(struct game *cur_game);
static void	unload_sprites(struct game *cur_game);

void
display_init(struct game *cur_game)
{
	SDL_DisplayMode dm;
	
	/* Create the main window */
	if (cur_game->display.mode == FULLSCREEN_DESKTOP) {
		SDL_GetDesktopDisplayMode(0, &dm);
		cur_game->display.w = dm.w;
		cur_game->display.h = dm.h;
		cur_game->display.window = SDL_CreateWindow(cur_game->display.name, 0, 0, cur_game->display.w, cur_game->display.h, SDL_WINDOW_FULLSCREEN_DESKTOP);
	} else if (cur_game->display.mode == FULLSCREEN) {
		cur_game->display.window = SDL_CreateWindow(cur_game->display.name, 0, 0, cur_game->display.w, cur_game->display.h, SDL_WINDOW_FULLSCREEN);
	} else {
		cur_game->display.window = SDL_CreateWindow(cur_game->display.name, 0, 0, cur_game->display.w, cur_game->display.h, SDL_WINDOW_SHOWN);
	}
	/* Set scale based on window width and height */
	cur_game->display.scale_w = cur_game->display.w / 1280.0;
	cur_game->display.scale_h = cur_game->display.h / 720.0;
	/* Create renderer */
	if (cur_game->display.vsync == SDL_TRUE) {
		cur_game->display.renderer = SDL_CreateRenderer(cur_game->display.window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_PRESENTVSYNC);
	} else {
		cur_game->display.renderer = SDL_CreateRenderer(cur_game->display.window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
	}
	SDL_SetRenderDrawBlendMode(cur_game->display.renderer, SDL_BLENDMODE_BLEND);
	/* Create output texture */
	cur_game->display.output = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1280, 720);
	/* Create char screen texture */
	cur_game->display.char_screen_tex = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1152, 648);
	/* Create view texture */
	cur_game->display.view = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1280, 720);
	/* Load sprites and font */
	load_sprites(cur_game);
	load_font(cur_game);
}

void
display_quit(struct game *cur_game)
{
	unload_sprites(cur_game);
	unload_font(cur_game);
	SDL_DestroyTexture(cur_game->display.view);
	SDL_DestroyTexture(cur_game->display.char_screen_tex);
	SDL_DestroyTexture(cur_game->display.output);
	SDL_DestroyRenderer(cur_game->display.renderer);
	cur_game->display.renderer = NULL;
	SDL_DestroyWindow(cur_game->display.window);
	cur_game->display.window = NULL;
}

/* Sprite information */
int num_walls = 24;
int sprite_w = 64;
int sprite_h = 64;
static void
load_sprites(struct game *cur_game)
{
	int i;
	SDL_Rect rect;
	SDL_Surface *image;
	SDL_Surface *tmp;
	SDL_Texture **sprites;
	
	/* Load walls: Set up rectangle, load image, and allocate memory */
	rect.x = 0; rect.y = 0; rect.w = sprite_w; rect.h = sprite_h;
	image = SDL_LoadBMP("art/walls.bmp");
	sprites = malloc(sizeof(*sprites) * num_walls);
	/* Blit each sprite and convert to a texture */
	for (i = 0; i < num_walls; i++) {
		tmp = SDL_CreateRGBSurface(0, sprite_w, sprite_h, 24, 0, 0, 0, 0);
		SDL_SetColorKey(tmp, 1, 0xFF00FF);
		rect.x = i * sprite_w;
		SDL_BlitSurface(image, &rect, tmp, NULL);
		*(sprites + i) = SDL_CreateTextureFromSurface(cur_game->display.renderer, tmp);
		SDL_FreeSurface(tmp);
	}
	SDL_FreeSurface(image);
	cur_game->sprites.walls = sprites;
	/* Load arrows: Set up rectangle, load image, and allocate memory */
	rect.x = 0; rect.y = 0; rect.w = 16; rect.h = 16;
	image = SDL_LoadBMP("art/arrows.bmp");
	sprites = malloc(sizeof(*sprites) * 4);
	/* Blit each sprite and convert to a texture */
	for (i = 0; i < 4; i++) {
		tmp = SDL_CreateRGBSurface(0, 16, 16, 24, 0, 0, 0, 0);
		SDL_SetColorKey(tmp, 1, 0xFF00FF);
		rect.x = i * 16;
		SDL_BlitSurface(image, &rect, tmp, NULL);
		*(sprites + i) = SDL_CreateTextureFromSurface(cur_game->display.renderer, tmp);
		SDL_FreeSurface(tmp);
	}
	SDL_FreeSurface(image);
	cur_game->sprites.arrows = sprites;

}

static void
unload_sprites(struct game *cur_game)
{
	int i;
	
	/* Unload walls */
	for (i = 0; i < num_walls; i++) {
		SDL_DestroyTexture(*(cur_game->sprites.walls + i));
	}
	free(cur_game->sprites.walls);
	/* Unload arrows */
	for (i = 0; i < 4; i++) {
		SDL_DestroyTexture(*(cur_game->sprites.arrows + i));
	}
	free(cur_game->sprites.arrows);
}


void
render_clear(struct game *cur_game, char *col)
{
	char *RGB;
	/* Set the rendering target to the output texture and clear it */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.output);
	RGB = get_color(col);
	SDL_SetRenderDrawColor(cur_game->display.renderer, RGB[0], RGB[1], RGB[2], 255);
	SDL_RenderClear(cur_game->display.renderer);
}

void
render_present(struct game *cur_game, SDL_bool ingame)
{
	SDL_Rect src = { 0, 0, 1280, 720 };
	SDL_Rect dest = { 0, 0, cur_game->display.w, cur_game->display.h };
	SDL_Rect view = { 341, 10, 929, 700 };
	SDL_Rect view_out = { 341 * cur_game->display.scale_w, 10 * cur_game->display.scale_h, 929 * cur_game->display.scale_w, 700  * cur_game->display.scale_h };	

	/* Reset render target to the renderer */
	SDL_SetRenderTarget(cur_game->display.renderer, NULL);
	/* Clear the renderer */
	SDL_RenderClear(cur_game->display.renderer);
	/* Copy the output texture to the renderer */
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.output, &src, &dest);
	/* Copy the view */
	if (ingame == SDL_TRUE) {
		SDL_RenderCopy(cur_game->display.renderer, cur_game->display.view, &view, &view_out);
	}
	/* Present */
	SDL_RenderPresent(cur_game->display.renderer);
}

void
draw_line(struct game *cur_game, int x1, int y1, int x2, int y2, char *col)
{
	char *color;
	
	color = get_color(col);
	SDL_SetRenderDrawColor(cur_game->display.renderer, color[0], color[1], color[2], 255);
	SDL_SetRenderDrawColor(cur_game->display.renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(cur_game->display.renderer, x1, y1, x2, y2);
}

void
draw_rect(struct game *cur_game, int x, int y, int w, int h, SDL_bool fill, char *col)
{
	char *color;
	SDL_Rect coords = { x, y, w, h };
	
	color = get_color(col);
	SDL_SetRenderDrawColor(cur_game->display.renderer, color[0], color[1], color[2], 255);
	
	/* Draw filled rectangle? */
	if (fill) {
		SDL_RenderFillRect(cur_game->display.renderer, &coords);
	} else {
		SDL_RenderDrawRect(cur_game->display.renderer, &coords);
	}
}

void
draw_sprites(struct game *cur_game, SDL_Texture **sprites, int sprite_id, int x, int y, int w, int h, int alpha, SDL_bool flip)
{
	SDL_Rect rect = {x, y, w, h};
	SDL_RendererFlip flip_dir;
	
	if (flip == SDL_TRUE) {
		flip_dir = SDL_FLIP_HORIZONTAL;
	} else {
		flip_dir = SDL_FLIP_NONE;
	}
	if (alpha != 255) SDL_SetTextureAlphaMod(*(sprites + sprite_id), alpha);
	SDL_RenderCopyEx(cur_game->display.renderer, *(sprites + sprite_id), NULL, &rect, 0, NULL, flip_dir);
	//SDL_RenderCopy(cur_game->display.renderer, *(sprites + sprite_id), NULL, &rect);
	if (alpha != 255) SDL_SetTextureAlphaMod(*(sprites + sprite_id), 255);
}

void
draw_screen(struct game *cur_game, struct user *cur_user)
{
	char text[100];

	/* Draw map in top left corner */
	draw_map(cur_game, cur_user, 10, 10);
	/* Output some text */
	sprintf(text, "Dungeon Level: %2d", cur_user->map + 1);
	draw_sentence(cur_game, 10, 340, text, 0.1);
}
