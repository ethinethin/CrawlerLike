#include <SDL2/SDL.h>
#include "char.h"
#include "cols.h"
#include "draw.h"
#include "font.h"
#include "gear.h"
#include "main.h"
#include "maze.h"
#include "user.h"
#include "wall.h"

/* Function prototypes */
static void		  load_sprites(struct game *cur_game);
static SDL_Texture	**load_image(struct game *cur_game, char *file, int num, int w, int h);
static void		  unload_sprites(struct game *cur_game);
static void		  unload_image(SDL_Texture **sprites, int num);

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
	/* Create info window texture */
	cur_game->display.info = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 592, 300);
	clear_info(cur_game);
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
	SDL_DestroyTexture(cur_game->display.info);
	SDL_DestroyRenderer(cur_game->display.renderer);
	cur_game->display.renderer = NULL;
	SDL_DestroyWindow(cur_game->display.window);
	cur_game->display.window = NULL;
}

static void
load_sprites(struct game *cur_game)
{
	cur_game->sprites.walls = load_image(cur_game, "art/walls.bmp", 24, 64, 64);
	cur_game->sprites.floors = load_image(cur_game, "art/floors.bmp", 3, 300, 230);
	cur_game->sprites.icons = load_image(cur_game, "art/icons.bmp", 17, 50, 50);
	cur_game->sprites.gear_rarity = load_image(cur_game, "art/gear_rarity.bmp", 6, 50, 50);
	cur_game->sprites.gear_type = load_image(cur_game, "art/gear_type.bmp", 4, 50, 50);
	cur_game->sprites.gear_attribute = load_image(cur_game, "art/gear_attribute.bmp", 6, 50, 50);
	cur_game->sprites.junk = load_image(cur_game, "art/junk.bmp", 24, 64, 64);
	cur_game->sprites.ladders = load_image(cur_game, "art/ladders.bmp", 8, 64, 128);
	cur_game->sprites.arrows = load_image(cur_game, "art/arrows.bmp", 6, 16, 16);
}

static SDL_Texture **
load_image(struct game *cur_game, char *file, int num, int w, int h)
{
	int i;
	SDL_Rect rect;
	SDL_Surface *image;
	SDL_Surface *tmp;
	SDL_Texture **sprites;
	
	/* Load walls: Set up rectangle, load image, and allocate memory */
	rect.x = 0; rect.y = 0; rect.w = w; rect.h = h;
	image = SDL_LoadBMP(file);
	sprites = malloc(sizeof(*sprites) * num);
	/* Blit each sprite and convert to a texture */
	for (i = 0; i < num; i++) {
		tmp = SDL_CreateRGBSurface(0, w, h, 24, 0, 0, 0, 0);
		SDL_SetColorKey(tmp, 1, 0xFF00FF);
		rect.x = i * w;
		SDL_BlitSurface(image, &rect, tmp, NULL);
		*(sprites + i) = SDL_CreateTextureFromSurface(cur_game->display.renderer, tmp);
		SDL_FreeSurface(tmp);
	}
	SDL_FreeSurface(image);
	return sprites;
}

static void
unload_sprites(struct game *cur_game)
{
	unload_image(cur_game->sprites.walls, 24);
	unload_image(cur_game->sprites.floors, 3);
	unload_image(cur_game->sprites.icons, 17);
	unload_image(cur_game->sprites.gear_rarity, 6);
	unload_image(cur_game->sprites.gear_type, 4);
	unload_image(cur_game->sprites.gear_attribute, 6);
	unload_image(cur_game->sprites.junk, 24);
	unload_image(cur_game->sprites.ladders, 8);
	unload_image(cur_game->sprites.arrows, 6);
}

static void
unload_image(SDL_Texture **sprites, int num)
{
	int i;
	for (i = 0; i < num; i++) {
		SDL_DestroyTexture(*(sprites + i));
	}
	free(sprites);
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
	SDL_SetRenderDrawColor(cur_game->display.renderer, 0, 0, 0, 255);
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
draw_sprites(struct game *cur_game, SDL_Texture **sprites, int sprite_id, int x, int y, int w, int h, int alpha, SDL_bool hflip, SDL_bool vflip)
{
	SDL_Rect rect = {x, y, w, h};
	SDL_RendererFlip flip_dir;
	
	if (hflip && !vflip) {
		flip_dir = SDL_FLIP_HORIZONTAL;
	} else if (hflip && vflip) {
		flip_dir = SDL_FLIP_HORIZONTAL|SDL_FLIP_VERTICAL;
	} else if (!hflip && vflip) {
		flip_dir = SDL_FLIP_VERTICAL;
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
	int hh;
	int mm;
	char text[100];

	/* Draw map in top left corner */
	draw_map(cur_game, cur_user, 10, 10);
	/* Output some text */
	sprintf(text, "Dungeon Level: %2d", cur_user->map + 1);
	draw_sentence(cur_game, 10, 340, text, 0.1);
	hh = cur_game->minute/6;
	mm = cur_game->minute - hh * 6;
	sprintf(text, "Day: %3d, (%2d:%02d)", cur_game->day, hh, mm*10);
	draw_sentence(cur_game, 10, 362, text, 0.1);
	/* Draw player info */
	draw_rect(cur_game, 10, 386, 319, 324, SDL_TRUE, "black");
	draw_rect(cur_game, 10, 386, 319, 324, SDL_FALSE, "white");
	sprintf(text, "%s", cur_user->character->name);
	draw_sentence(cur_game, 20, 396, text, 0.1);
	sprintf(text, "Lv. %d", cur_user->character->level);
	draw_sentence(cur_game, 20, 418, text, 0.1);
	draw_meters(cur_game, cur_user);	
}
