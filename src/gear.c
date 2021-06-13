#include <stdio.h>
#include <SDL2/SDL.h>
#include "char.h"
#include "draw.h"
#include "home.h"
#include "gear.h"
#include "main.h"
#include "user.h"

/* Function prototypes */
static void		 prep_item_coords(struct user *cur_user);
static int		 find_item(int x, int y);
static SDL_bool		 drag_loop(struct game *cur_game, SDL_Rect *mouse, int i);
static void		 draw_drag(struct game *cur_game, SDL_Rect *mouse, int i);
static void		 handle_transfer(struct game *cur_game, struct user *cur_user, int i, int j);
static int		 gear_type(int id);
static int		 gear_value(int id);
static struct gear 	*find_gear(int id);

enum type { GEAR_WEAPON, GEAR_ARMOR, GEAR_ACCESSORY, GEAR_SKILL, GEAR_ITEM, GEAR_ANY, GEAR_TRASH, GEAR_NULL };

/* Drawing/Clicking item coordinates on character screen */
struct item_coords {
	SDL_Rect coords;
	int *slot;
	int empty;
	int type;
} item_coords[] = {
	{ { 25, 419, 100, 100 }, NULL, 8, GEAR_WEAPON },
	{ { 145, 419, 100, 100 }, NULL, 9, GEAR_ARMOR },
	{ { 265, 419, 100, 100 }, NULL, 10, GEAR_ACCESSORY },
	{ { 25, 529, 100, 100 }, NULL, 11, GEAR_SKILL },
	{ { 145, 529, 100, 100 }, NULL, 11, GEAR_SKILL },
	{ { 265, 529, 100, 100 }, NULL, 11, GEAR_SKILL },
	{ { 408, 419, 100, 100 }, NULL, 12, GEAR_ANY }, 
	{ { 528, 419, 100, 100 }, NULL, 12, GEAR_ANY }, 
	{ { 648, 419, 100, 100 }, NULL, 12, GEAR_ANY }, 
	{ { 768, 419, 100, 100 }, NULL, 12, GEAR_ANY }, 
	{ { 408, 529, 100, 100 }, NULL, 12, GEAR_ANY }, 
	{ { 528, 529, 100, 100 }, NULL, 12, GEAR_ANY }, 
	{ { 648, 529, 100, 100 }, NULL, 12, GEAR_ANY }, 
	{ { 768, 529, 100, 100 }, NULL, 12, GEAR_ANY },
	{ { 908, 529, 100, 100 }, NULL, 0, GEAR_TRASH },
	{ { -1, -1, 0, 0 }, NULL, 0, GEAR_ANY }
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
	handle_transfer(cur_game, cur_user, i, j);
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
	/* Draw empty slot over the icon - need to scale its location and size because it's being rendered directly */
	draw_sprites(cur_game, cur_game->sprites.icons, item_coords[i].empty, (item_coords[i].coords.x + 64) * cur_game->display.scale_w, (item_coords[i].coords.y + 36) * cur_game->display.scale_h, item_coords[i].coords.w * cur_game->display.scale_w, item_coords[i].coords.h * cur_game->display.scale_h, 255, SDL_FALSE);
	/* Draw icon for item being dragged - also scaled*/
	draw_sprites(cur_game, cur_game->sprites.gear, *item_coords[i].slot - 1, (mouse->x - mouse->w + 64) * cur_game->display.scale_w, (mouse->y - mouse->h + 36) * cur_game->display.scale_h, 100 * cur_game->display.scale_w, 100 * cur_game->display.scale_h, 255, SDL_FALSE);
	SDL_RenderPresent(cur_game->display.renderer);
}

static void
handle_transfer(struct game *cur_game, struct user *cur_user, int i, int j)
{
	char message[100];
	int value;
	int tmp;
	int src_type, dest_type;
	
	/* Is the destination type compatible with the source type? */
	src_type = gear_type(*item_coords[i].slot);
	if (item_coords[j].type == GEAR_TRASH) {
		value = gear_value(*item_coords[i].slot);
		if (value != -1) {
			sprintf(message, "This will destroy the item. You will receive %d gold. Okay?", value);
			if (yes_no(cur_game, message, SDL_TRUE, SDL_TRUE) == SDL_TRUE) {
				cur_user->character->money += value;
				del_gear(*item_coords[i].slot);
				*item_coords[i].slot = 0;
			}
		}
		return;
	} else if (item_coords[i].type == GEAR_ANY && item_coords[j].type == GEAR_ANY) {
		src_type = GEAR_ANY;
		dest_type = GEAR_ANY;
	} else if (*item_coords[j].slot == 0) {
		dest_type = item_coords[j].type;
	} else {
		dest_type = gear_type(*item_coords[j].slot);
	}
	/* Make the swap */
	if (dest_type == GEAR_ANY || src_type == dest_type) {
		tmp = *item_coords[i].slot;
		*item_coords[i].slot = *item_coords[j].slot;
		*item_coords[j].slot = tmp;
	}
}

/* Gear table linked list stuff - all below */
struct gear {
	int id;
	int type;
	int sprite;
	int level;
	int value;
	struct stats mods;
	struct gear *next;
};

struct gear *GEAR;
int max_id;

void
init_gear(void)
{
	int i;
	int types[] = { GEAR_WEAPON, GEAR_ARMOR, GEAR_ACCESSORY, GEAR_SKILL, GEAR_SKILL, GEAR_SKILL, GEAR_SKILL, GEAR_SKILL, GEAR_SKILL };
	struct stats mods;

	/* Allocate memory for gear table, point to null, and set first id */
	GEAR = malloc(sizeof(*GEAR));
	GEAR->next = NULL;
	max_id = 1;
	
	/* Temporary - make starting gear */
	zero_stats(&mods);
	for (i = 1; i < 10; i++) {
		add_gear(0, i, types[i - 1], 1, 10, &mods);
	}
}

void
kill_gear(void)
{
	struct gear *tmp;
	struct gear *last;
	
	tmp = GEAR;
	while (tmp->next != NULL) {
		last = tmp;
		tmp = tmp->next;
		free(last);
	}
	free(tmp);
}

void
add_gear(int id, int sprite, int type, int level, int value, struct stats *mods)
{
	struct gear *tmp;
	struct gear *new_gear;
	
	/* Make the new gear */
	new_gear = malloc(sizeof(*new_gear));
	new_gear->sprite = sprite;
	if (id == 0) {
		new_gear->id = max_id;
		max_id += 1;
	} else {
		new_gear->id = id;
	}
	new_gear->type = type;
	new_gear->level = level;
	new_gear->value = value;
	copy_stats(mods, &new_gear->mods);
	new_gear->next = NULL;
	/* Add it to the list */
	tmp = GEAR;
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}
	tmp->next = new_gear;
}

void
del_gear(int id)
{
	struct gear *tmp;
	struct gear *last;
	
	tmp = GEAR;
	while (tmp->next != NULL) {
		last = tmp;
		tmp = tmp->next;
		if (tmp->id == id) {
			last->next = tmp->next;
			free(tmp);
			return;
		}
	}
}

int
gear_sprite(int id)
{
	struct gear *tmp;
	
	tmp = find_gear(id);
	if (tmp != NULL) {
		return tmp->sprite;
	}
	return 0;
}

struct stats *
gear_stats(int id)
{
	struct gear *tmp;
	
	tmp = find_gear(id);
	if (tmp != NULL) {
		return &tmp->mods;
	}
	return NULL;
}

static int
gear_type(int id)
{
	struct gear *tmp;
	
	tmp = find_gear(id);
	if (tmp != NULL) {
		return tmp->type;
	}
	return GEAR_NULL;
}

static int
gear_value(int id)
{
	struct gear *tmp;
	
	tmp = find_gear(id);
	if (tmp != NULL) {
		return tmp->value;
	}
	return -1;
}

static struct gear *
find_gear(int id)
{
	struct gear *tmp;

	tmp = GEAR;
	while (tmp->next != NULL) {
		tmp = tmp->next;
		if (tmp->id == id) {
			return tmp;
		}
	}
	return NULL;
}

void
dump_gear(FILE *fp)
{
	struct gear *tmp;
	
	/* Print out max_id */
	fprintf(fp, "max_id=%d\n", max_id);
	
	tmp = GEAR;
	while (tmp->next != NULL) {
		tmp = tmp->next;
		fprintf(fp, "id=%d\n", tmp->id);
		fprintf(fp, "  type=%d\n", tmp->type);
		fprintf(fp, "  sprite=%d\n", tmp->sprite);
		fprintf(fp, "  level=%d\n", tmp->level);
		fprintf(fp, "  value=%d\n", tmp->value);
		fprintf(fp, "  mods.life=%d\n", tmp->mods.life);
		fprintf(fp, "  mods.stamina=%d\n", tmp->mods.stamina);
		fprintf(fp, "  mods.magic=%d\n", tmp->mods.magic);
		fprintf(fp, "  mods.experience=%d\n", tmp->mods.experience);
		fprintf(fp, "  mods.attack=%d\n", tmp->mods.attack);
		fprintf(fp, "  mods.defense=%d\n", tmp->mods.defense);
		fprintf(fp, "  mods.dodge=%d\n", tmp->mods.dodge);
		fprintf(fp, "  mods.power=%d\n", tmp->mods.power);
		fprintf(fp, "  mods.spirit=%d\n", tmp->mods.spirit);
		fprintf(fp, "  mods.avoid=%d\n", tmp->mods.avoid);
	}
	fprintf(fp, "id=0\n");
}

void
undump_gear(FILE *fp)
{
	struct gear tmp;
	
	/* Set up gear table */
	GEAR = malloc(sizeof(*GEAR));
	GEAR->next = NULL;

	/* Load max_id */
	fscanf(fp, "max_id=%d\n", &max_id);
	while(SDL_TRUE) {
		fscanf(fp, "id=%d\n", &tmp.id);
		if (tmp.id == 0) {
			break;
		}
		fscanf(fp, "  type=%d\n", &tmp.type);
		fscanf(fp, "  sprite=%d\n", &tmp.sprite);
		fscanf(fp, "  level=%d\n", &tmp.level);
		fscanf(fp, "  value=%d\n", &tmp.value);
		fscanf(fp, "  mods.life=%d\n", &tmp.mods.life);
		fscanf(fp, "  mods.stamina=%d\n", &tmp.mods.stamina);
		fscanf(fp, "  mods.magic=%d\n", &tmp.mods.magic);
		fscanf(fp, "  mods.experience=%d\n", &tmp.mods.experience);
		fscanf(fp, "  mods.attack=%d\n", &tmp.mods.attack);
		fscanf(fp, "  mods.defense=%d\n", &tmp.mods.defense);
		fscanf(fp, "  mods.dodge=%d\n", &tmp.mods.dodge);
		fscanf(fp, "  mods.power=%d\n", &tmp.mods.power);
		fscanf(fp, "  mods.spirit=%d\n", &tmp.mods.spirit);
		fscanf(fp, "  mods.avoid=%d\n", &tmp.mods.avoid);
		add_gear(tmp.id, tmp.sprite, tmp.type, tmp.level, tmp.value, &tmp.mods);
	}
}
