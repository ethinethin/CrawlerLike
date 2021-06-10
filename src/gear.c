#include <stdio.h>
#include <SDL2/SDL.h>
#include "draw.h"
#include "home.h"
#include "gear.h"
#include "main.h"
#include "user.h"

/* Function prototypes */
static void		prep_item_coords(struct user *cur_user);
static int		find_item(int x, int y);
static SDL_bool		drag_loop(struct game *cur_game, SDL_Rect *mouse, int i);
static void		draw_drag(struct game *cur_game, SDL_Rect *mouse, int i);
static void		handle_transfer(int i, int j);

struct item_coords {
	SDL_Rect coords;
	int *slot;
	int empty;
} item_coords[] = {
	{ { 25, 419, 100, 100 }, NULL, 8 },
	{ { 145, 419, 100, 100 }, NULL, 9 },
	{ { 265, 419, 100, 100 }, NULL, 10 },
	{ { 25, 529, 100, 100 }, NULL, 11 },
	{ { 145, 529, 100, 100 }, NULL, 11 },
	{ { 265, 529, 100, 100 }, NULL, 11 },
	{ { 408, 419, 100, 100 }, NULL, 12 }, 
	{ { 528, 419, 100, 100 }, NULL, 12 }, 
	{ { 648, 419, 100, 100 }, NULL, 12 }, 
	{ { 768, 419, 100, 100 }, NULL, 12 }, 
	{ { 408, 529, 100, 100 }, NULL, 12 }, 
	{ { 528, 529, 100, 100 }, NULL, 12 }, 
	{ { 648, 529, 100, 100 }, NULL, 12 }, 
	{ { 768, 529, 100, 100 }, NULL, 12 },
	{ { -1, -1, 0, 0 }, NULL, 0 }
};

static void
prep_item_coords(struct user *cur_user)
{
	int i;
	
	if (item_coords[0].slot != NULL && item_coords[0].slot == &cur_user->character->gear[0]) return;
	for (i = 0; i < 8; i++) {
		if (i < 3) {
			item_coords[i].slot = &cur_user->character->gear[i];
			item_coords[i + 3].slot = &cur_user->character->skills[i];
		}
		item_coords[i + 6].slot = &cur_user->character->inventory[i];
	}
}

SDL_bool
drag_mouse(struct game *cur_game, struct user *cur_user, int x, int y)
{
	SDL_Rect mouse;
	int i, j;
	
	/* Check if you're in a click & drag area - return x and y */
	if (!(x >= 25 && x <= 868 && y >= 419 && y <= 629)) {
		return SDL_FALSE;
	}
	
	/* Point to the right inventory slots */
	prep_item_coords(cur_user);
	/* They tried to click in the inventory or gear - figure out where */
	i = find_item(x, y);
	/* Make sure you're pointing to a slot that has an item */
	if (i == -1 || *item_coords[i].slot == 0) return SDL_FALSE;
	/* Handle the dragging */
	mouse.x = x; mouse.y = y; mouse.w = x - item_coords[i].coords.x; mouse.h = y - item_coords[i].coords.y;
	drag_loop(cur_game, &mouse, i);
	/* If they didn't drag anywhere new, do nothing */
	if (x == mouse.x && y == mouse.y) {
		return SDL_FALSE;
	}
	/* If they did, find out where */
	j = find_item(mouse.x, mouse.y);
	if (j == -1) return SDL_FALSE;
	
	/* Handle the item transfer */
	handle_transfer(i, j);
	return SDL_TRUE;
}

static int
find_item(int x, int y)
{
	int i;
	
	for (i = 0; item_coords[i].coords.x != -1; i++) {
		if (x >= item_coords[i].coords.x && x <= item_coords[i].coords.x + item_coords[i].coords.w &&
		    y >= item_coords[i].coords.y && y <= item_coords[i].coords.y + item_coords[i].coords.h) {
		    	/* Found it ! */
		    	return i;
		}
	}
	return -1;
}

static SDL_bool
drag_loop(struct game *cur_game, SDL_Rect *mouse, int i)
{
	SDL_bool loop;
	SDL_Event event;
	
	/* Draw screen */
	draw_drag(cur_game, mouse, i);
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		/* Draw screen */
		draw_drag(cur_game, mouse, i);
		SDL_WaitEvent(&event);
		switch (event.type) {
			case SDL_MOUSEBUTTONUP:
				loop = SDL_FALSE;
				break;
			case SDL_MOUSEMOTION:
				mouse->x = event.motion.x / cur_game->display.scale_w - 64;
				mouse->y = event.motion.y / cur_game->display.scale_h - 36;
				break;
			default:
				return SDL_FALSE;
		}
	}
	return SDL_TRUE;
}

static void
draw_drag(struct game *cur_game, SDL_Rect *mouse, int i)
{
	SDL_Rect out_src = { 0, 0, 1280, 720 };
	SDL_Rect out_dest = { 0, 0, cur_game->display.w, cur_game->display.h };
	SDL_Rect view_src = { 341, 10, 929, 700 };
	SDL_Rect view_dest = { 341 * cur_game->display.scale_w, 10 * cur_game->display.scale_h, 929 * cur_game->display.scale_w, 700  * cur_game->display.scale_h };	
	SDL_Rect screen_src = { 0, 0, 1152, 648 };
	SDL_Rect screen_dest = { 64 * cur_game->display.scale_w, 36 * cur_game->display.scale_h, 1152* cur_game->display.scale_w, 648 * cur_game->display.scale_h };
	
	/* Draw everything already on screen */
	SDL_SetRenderTarget(cur_game->display.renderer, NULL);
	SDL_RenderClear(cur_game->display.renderer);
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.output, &out_src, &out_dest);
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.view, &view_src, &view_dest);
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.char_screen_tex, &screen_src, &screen_dest);
	/* Draw empty slot over the icon */
	draw_sprites(cur_game, cur_game->sprites.icons, item_coords[i].empty, item_coords[i].coords.x + 64, item_coords[i].coords.y + 36, item_coords[i].coords.w, item_coords[i].coords.h, 255, SDL_FALSE);
	/* Draw icon for item being dragged */
	draw_sprites(cur_game, cur_game->sprites.gear, *item_coords[i].slot - 1, mouse->x - mouse->w + 64, mouse->y - mouse->h + 36, 100, 100, 255, SDL_FALSE);
	SDL_RenderPresent(cur_game->display.renderer);
}

static void
handle_transfer(int i, int j)
{
	int tmp;
	/* Just swap for now */
	tmp = *item_coords[i].slot;
	*item_coords[i].slot = *item_coords[j].slot;
	*item_coords[j].slot = tmp;
}
