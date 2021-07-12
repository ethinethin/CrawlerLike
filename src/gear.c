#include <stdio.h>
#include <SDL2/SDL.h>
#include "char.h"
#include "draw.h"
#include "font.h"
#include "home.h"
#include "gear.h"
#include "main.h"
#include "rand.h"
#include "user.h"

/* Function prototypes */
static void		 prep_item_coords(struct user *cur_user);
static int		 find_item(int x, int y);
static SDL_bool		 drag_loop(struct game *cur_game, SDL_Rect *mouse, int i);
static void		 draw_drag(struct game *cur_game, SDL_Rect *mouse, int i);
static SDL_bool		 handle_transfer(struct game *cur_game, struct user *cur_user, int i, int j);
static void		 prep_info(struct game *cur_game, struct stats *mouse_over, struct stats *equipped_gear[3], int type);
static void		 stat_string_skill(struct stats *mouse_over, struct stats diff[3], char stats[1024]);
static void		 stat_string_gear(struct stats *mouse_over, struct stats diff[3], char stats[1024]);
static void		 draw_info(struct game *cur_game, char info[1024]);
static int		 add_gear(int id, void *new_gear);
static void		 del_gear(int id);
static int		 gear_rarity(int id);
static int		 gear_type(int id);
static int		 gear_value(int id);
static struct gear 	*find_gear(int id);
static int		 new_rarity(int level);
static int		 new_type(void);
static void		 gen_stats(struct stats *mods, int rarity, int level);
static int		 calc_value(struct gear *cur_gear);

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
	for (i = 0; i < 8; i += 1) {
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
	if (handle_transfer(cur_game, cur_user, i, j) == SDL_TRUE) {
		update_stats(cur_user);
	}
	return SDL_TRUE;
}

static int
find_item(int x, int y)
{
	int i;
	
	for (i = 0; item_coords[i].coords.x != -1; i += 1) {
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
	draw_sprites(cur_game, cur_game->sprites.gear, gear_sprite(*item_coords[i].slot) - 1, (mouse->x - mouse->w + 64) * cur_game->display.scale_w, (mouse->y - mouse->h + 36) * cur_game->display.scale_h, 100 * cur_game->display.scale_w, 100 * cur_game->display.scale_h, 255, SDL_FALSE);
	SDL_RenderPresent(cur_game->display.renderer);
}

static SDL_bool
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
			if (yes_no(cur_game, message, SDL_TRUE, SDL_TRUE, SDL_FALSE) == SDL_TRUE) {
				cur_user->character->money += value;
				del_gear(*item_coords[i].slot);
				*item_coords[i].slot = 0;
				return SDL_TRUE;
			}
		}
		return SDL_FALSE;
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
		return SDL_TRUE;
	} else {
		return SDL_FALSE;
	}
}

void
gear_mouseover(struct game *cur_game, struct user *cur_user, int x, int y)
{
	int i;
	int type;
	static SDL_bool zeroed = SDL_FALSE;
	static struct stats zero;
	struct stats *mouse_over = NULL;
	struct stats *equipped_gear[3] = { &zero, &zero, &zero };
	
	/* Clear info window */
	clear_info(cur_game);
	/* Point to the right inventory slots and zero out stats zero */
	prep_item_coords(cur_user);
	if (zeroed == SDL_FALSE) {
		zero_stats(&zero);
		zeroed = SDL_TRUE;
	}
	for (i = 0; item_coords[i].coords.x != -1; i++) {
		if (x >= item_coords[i].coords.x &&
		    x <= item_coords[i].coords.x + item_coords[i].coords.w &&
		    y >= item_coords[i].coords.y &&
		    y <= item_coords[i].coords.y + item_coords[i].coords.h) {
		    	if (item_coords[i].slot != NULL && *item_coords[i].slot != 0) {
		    		mouse_over = gear_stats(*item_coords[i].slot);
		    		type = gear_type(*item_coords[i].slot);
		    		if (type == GEAR_WEAPON) {
		    			if (cur_user->character->gear[0] != 0) {
		    				equipped_gear[0] = gear_stats(cur_user->character->gear[0]);
		    			}
		    		} else if (type == GEAR_ARMOR) {
		       			if (cur_user->character->gear[1] != 0) {
		    				equipped_gear[0] = gear_stats(cur_user->character->gear[1]);
		    			}
		    		} else if (type == GEAR_ACCESSORY) {
		    			if (cur_user->character->gear[2] != 0) {
		    				equipped_gear[0] = gear_stats(cur_user->character->gear[2]);
		    			}
		    		} else if (type == GEAR_SKILL) {
		    			if (cur_user->character->skills[0] != 0) {
		    				equipped_gear[0] = gear_stats(cur_user->character->skills[0]);
		    			}
		    			if (cur_user->character->skills[1] != 0) {
		    				equipped_gear[1] = gear_stats(cur_user->character->skills[1]);
		    			}
		    			if (cur_user->character->skills[2] != 0) {
		    				equipped_gear[2] = gear_stats(cur_user->character->skills[2]);
		    			}
		    		}
		    	}
		    	if (mouse_over != NULL) {
		    		prep_info(cur_game, mouse_over, equipped_gear, type);
		    	}
		    	break;
		}
	}
}

static void
prep_info(struct game *cur_game, struct stats *mouse_over, struct stats *equipped_gear[3], int type)
{
	int i;
	struct stats diff[3];
	char stats[1024];

	/* Zero out differences */
	zero_stats(&diff[0]);
	zero_stats(&diff[1]);
	zero_stats(&diff[2]);
	/* Loop through each equipped gear and calculate diff */
	for (i = 0; i < (type == GEAR_SKILL ? 3 : 1); i += 1) {
		diff[i].life = mouse_over->life - equipped_gear[i]->life;
		diff[i].stamina = mouse_over->stamina - equipped_gear[i]->stamina;
		diff[i].magic = mouse_over->magic - equipped_gear[i]->magic;
		diff[i].attack = mouse_over->attack - equipped_gear[i]->attack;
		diff[i].defense = mouse_over->defense - equipped_gear[i]->defense;
		diff[i].dodge = mouse_over->dodge - equipped_gear[i]->dodge;
		diff[i].power = mouse_over->power - equipped_gear[i]->power;
		diff[i].spirit = mouse_over->spirit - equipped_gear[i]->spirit;
		diff[i].avoid = mouse_over->avoid - equipped_gear[i]->avoid;
	}
	/* Put in name of item */
	if (type == GEAR_WEAPON) {
		sprintf(stats, "Weapon\n------\n");
	} else if (type == GEAR_ARMOR) {
		sprintf(stats, "Armor\n-----\n");
	} else if (type == GEAR_ACCESSORY) {
		sprintf(stats, "Accessory\n---------\n");
	} else {
		sprintf(stats, "Skill\n-----\n");
	}
	/* Build format string with stats */
	if (type == GEAR_SKILL) {
		stat_string_skill(mouse_over, diff, stats);
	} else {
		stat_string_gear(mouse_over, diff, stats);
	}
	/* Now finally draw it */
	draw_info(cur_game, stats);
}

static void
stat_string_skill(struct stats *mouse_over, struct stats diff[3], char stats[1024])
{
	if (mouse_over->life != 0 || diff[0].life != 0 || diff[1].life != 0 || diff[2].life != 0) {
		sprintf(stats, "%sLife: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->life, diff[0].life, diff[1].life, diff[2].life);
	}
	if (mouse_over->stamina != 0 || diff[0].stamina != 0 || diff[1].stamina != 0 || diff[2].stamina != 0) {
		sprintf(stats, "%sStamina: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->stamina, diff[0].stamina, diff[1].stamina, diff[2].stamina);
	}
	if (mouse_over->magic != 0 || diff[0].magic != 0 || diff[1].magic != 0 || diff[2].magic != 0) {
		sprintf(stats, "%sMagic: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->magic, diff[0].magic, diff[1].magic, diff[2].magic);
	}
	if (mouse_over->attack != 0 || diff[0].attack != 0 || diff[1].attack != 0 || diff[2].attack != 0) {
		sprintf(stats, "%sAttack: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->attack, diff[0].attack, diff[1].attack, diff[2].attack);
	}
	if (mouse_over->defense != 0 || diff[0].defense != 0 || diff[1].defense != 0 || diff[2].defense != 0) {
		sprintf(stats, "%sDefense: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->defense, diff[0].defense, diff[1].defense, diff[2].defense);
	}
	if (mouse_over->dodge != 0 || diff[0].dodge != 0 || diff[1].dodge != 0 || diff[2].dodge != 0) {
		sprintf(stats, "%sDodge: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->dodge, diff[0].dodge, diff[1].dodge, diff[2].dodge);
	}
	if (mouse_over->power != 0 || diff[0].power != 0 || diff[1].power != 0 || diff[2].power != 0) {
		sprintf(stats, "%sPower: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->power, diff[0].power, diff[1].power, diff[2].power);
	}
	if (mouse_over->spirit != 0 || diff[0].spirit != 0 || diff[1].spirit != 0 || diff[2].spirit != 0) {
		sprintf(stats, "%sSpirit: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->spirit, diff[0].spirit, diff[1].spirit, diff[2].spirit);
	}
	if (mouse_over->avoid != 0 || diff[0].avoid != 0 || diff[1].avoid != 0 || diff[2].avoid != 0) {
		sprintf(stats, "%sAvoid: %d (%+d) (%+d) (%+d)\n", stats, mouse_over->avoid, diff[0].avoid, diff[1].avoid, diff[2].avoid);
	}
}

static void
stat_string_gear(struct stats *mouse_over, struct stats diff[3], char stats[1024])
{
	if (mouse_over->life != 0 || diff[0].life != 0) {
		sprintf(stats, "%sLife: %d (%+d)\n", stats, mouse_over->life, diff[0].life);
	}
	if (mouse_over->stamina != 0 || diff[0].stamina != 0) {
		sprintf(stats, "%sStamina: %d (%+d)\n", stats, mouse_over->stamina, diff[0].stamina);
	}
	if (mouse_over->magic != 0 || diff[0].magic != 0) {
		sprintf(stats, "%sMagic: %d (%+d)\n", stats, mouse_over->magic, diff[0].magic);
	}
	if (mouse_over->attack != 0 || diff[0].attack != 0) {
		sprintf(stats, "%sAttack: %d (%+d)\n", stats, mouse_over->attack, diff[0].attack);
	}
	if (mouse_over->defense != 0 || diff[0].defense != 0) {
		sprintf(stats, "%sDefense: %d (%+d)\n", stats, mouse_over->defense, diff[0].defense);
	}
	if (mouse_over->dodge != 0 || diff[0].dodge != 0) {
		sprintf(stats, "%sDodge: %d (%+d)\n", stats, mouse_over->dodge, diff[0].dodge);
	}
	if (mouse_over->power != 0 || diff[0].power != 0) {
		sprintf(stats, "%sPower: %d (%+d)\n", stats, mouse_over->power, diff[0].power);
	}
	if (mouse_over->spirit != 0 || diff[0].spirit != 0) {
		sprintf(stats, "%sSpirit: %d (%+d)\n", stats, mouse_over->spirit, diff[0].spirit);
	}
	if (mouse_over->avoid != 0 || diff[0].avoid != 0) {
		sprintf(stats, "%sAvoid: %d (%+d)\n", stats, mouse_over->avoid, diff[0].avoid);
	}
}

static void
draw_info(struct game *cur_game, char info[1024])
{
	/* Output stats to the info texture */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.info);
	draw_rect(cur_game, 0, 0, 594, 302, SDL_TRUE, "darkgrey");
	draw_sentence(cur_game, 10, 10, info, 0.1);
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.char_screen_tex);
}

void
clear_info(struct game *cur_game)
{
	/* Draw a black rectangle on top of the info texture */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.info);
	draw_rect(cur_game, 0, 0, 594, 302, SDL_TRUE, "darkgrey");
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.char_screen_tex);
}

/* Gear table linked list stuff - all below */
struct gear {
	int id;
	int rarity;
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
	struct gear *new_gear;
	struct stats mods;

	/* Allocate memory for gear table, point to null, and set first id */
	GEAR = malloc(sizeof(*GEAR));
	GEAR->next = NULL;
	max_id = 1;
	
	/* Temporary - make starting gear */
	zero_stats(&mods);
	for (i = 1; i < 10; i += 1) {
		new_gear = malloc(sizeof(*new_gear));
		new_gear->sprite = i;
		new_gear->rarity = RAR_TRASH;
		new_gear->type = types[i - 1];
		new_gear->level = 0;
		new_gear->value = 10;
		copy_stats(&mods, &new_gear->mods);
		add_gear(0, new_gear);
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

static int
add_gear(int id, void *new_gear_void)
{
	struct gear *tmp;
	struct gear *new_gear;
	
	/* Cast void pointer as struct gear */
	new_gear = (struct gear *) new_gear_void;
	/* Add item id and increment max_id  */
	if (id == 0) {
		new_gear->id = max_id;
		max_id += 1;
	} else {
		new_gear->id = id;
	}
	new_gear->next = NULL;
	/* Add it to the list */
	tmp = GEAR;
	while (tmp->next != NULL) {
		tmp = tmp->next;
	}
	tmp->next = new_gear;
	/* Return item id */
	return new_gear->id;
}

static void
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
gear_rarity(int id)
{
	struct gear *tmp;
	
	tmp = find_gear(id);
	if (tmp != NULL) {
		return tmp->rarity;
	}
	return GEAR_NULL;
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

int
create_gear(int level)
{
	struct gear *new_gear;
	
	/* Allocate memory for it */
	new_gear = malloc(sizeof(*new_gear));	
	/* Determine rarity */
	new_gear->rarity = new_rarity(level);
	/* Determine type */
	new_gear->type = new_type();
	/* Assign sprite and level */
	new_gear->sprite = new_gear->type + 1;
	new_gear->level = level;
	/* Generate random stats */
	zero_stats(&new_gear->mods);
	gen_stats(&new_gear->mods, new_gear->rarity, new_gear->level);
	/* Calculate and assign value */
	new_gear->value = calc_value(new_gear);
	/* Add it to the gear table */
	return add_gear(0, new_gear);
}

static int
new_rarity(int level)
{
	//if (rand_num(1, 6250) <= level) RAR_UNIQUE
	if (rand_num(1, 1250) <= level) {
		return RAR_RAREPLUS;
	} else if (rand_num(1, 250) <= level) {
		return RAR_RARE;
	} else if (rand_num(1, 50) <= level) {
		return RAR_UNCOMMON;
	} else {
		return RAR_COMMON;
	}
}

static int
new_type(void)
{
	int prob;
	
	/* Generate a random number and assign gear type */
	prob = rand_num(0, 99);
	if (prob < 25) {
		return GEAR_WEAPON;
	} else if (prob < 50) {
		return GEAR_ARMOR;
	} else if (prob < 75) {
		return GEAR_ACCESSORY;
	} else {
		return GEAR_SKILL;
	}
}

static void
gen_stats(struct stats *mods, int rarity, int level)
{
	int stat;
	int amount;
	int *stats[9] = { &mods->life, &mods->stamina, &mods->magic,
			  &mods->attack, &mods->defense, &mods->dodge,
			  &mods->power, &mods->spirit, &mods->avoid };
	
	while (rarity > 0) {
		/* Decide which stat you're increasing */
		if (rand_num(0, 2) == 0) {
			stat = rand_num(0, 2);
			amount = rand_num(1 * level, 2 * level);
		} else {
			stat = rand_num(3, 8);
			amount = rand_num(1 * level, 3 * level);
		}
		/* Assign stat increase to appropriate stat */
		*stats[stat] += amount;
		/* Increase stat by random amount */
		rarity -= 1;
	}
}

static int
calc_value(struct gear *cur_gear)
{
	int value;
	
	/* Add up major and minor stats */
	value = (cur_gear->mods.life + cur_gear->mods.stamina + cur_gear->mods.magic) * 4;
	value += (cur_gear->mods.attack + cur_gear->mods.defense + cur_gear->mods.dodge) * 2;
	value += (cur_gear->mods.power + cur_gear->mods.spirit + cur_gear->mods.avoid) * 2;
	/* Multiply based on rarity */
	if (value < 10) {
		value = 10;
	}
	return value;
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
		fprintf(fp, "  rarity=%d\n", tmp->rarity);
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
	int id;
	struct gear *tmp;
	
	/* Set up gear table */
	GEAR = malloc(sizeof(*GEAR));
	GEAR->next = NULL;

	/* Load max_id */
	fscanf(fp, "max_id=%d\n", &max_id);
	while(SDL_TRUE) {
		fscanf(fp, "id=%d\n", &id);
		if (id == 0) break;
		tmp = malloc(sizeof(*tmp));
		fscanf(fp, "  rarity=%d\n", &tmp->rarity);
		fscanf(fp, "  type=%d\n", &tmp->type);
		fscanf(fp, "  sprite=%d\n", &tmp->sprite);
		fscanf(fp, "  level=%d\n", &tmp->level);
		fscanf(fp, "  value=%d\n", &tmp->value);
		fscanf(fp, "  mods.life=%d\n", &tmp->mods.life);
		fscanf(fp, "  mods.stamina=%d\n", &tmp->mods.stamina);
		fscanf(fp, "  mods.magic=%d\n", &tmp->mods.magic);
		fscanf(fp, "  mods.experience=%d\n", &tmp->mods.experience);
		fscanf(fp, "  mods.attack=%d\n", &tmp->mods.attack);
		fscanf(fp, "  mods.defense=%d\n", &tmp->mods.defense);
		fscanf(fp, "  mods.dodge=%d\n", &tmp->mods.dodge);
		fscanf(fp, "  mods.power=%d\n", &tmp->mods.power);
		fscanf(fp, "  mods.spirit=%d\n", &tmp->mods.spirit);
		fscanf(fp, "  mods.avoid=%d\n", &tmp->mods.avoid);
		add_gear(id, tmp);
	}
}
