#include <SDL2/SDL.h>
#include "draw.h"
#include "font.h"
#include "main.h"
#include "user.h"

/* Function prototypes */
static void	draw_char_screen(struct game *cur_game, struct user *cur_user, SDL_bool ingame);
static void	char_screen_click(struct game *cur_game, struct user *cur_user, int x, int y);


void
init_seen(struct seen *cur_seen, int rows, int cols)
{
	int i, j;
	
	/* Set up seen dimensions */
	cur_seen->rows = rows;
	cur_seen->cols = cols;
	/* Allocate memory and set to 0 */
	cur_seen->tiles = malloc(sizeof(*cur_seen->tiles) * rows);
	for (i = 0; i < rows; i++) {
		*(cur_seen->tiles + i) = malloc(sizeof(**cur_seen->tiles) * cols);
		for (j = 0; j < cols; j++) {
			*(*(cur_seen->tiles + i) + j) = 0;
		}
	}
}

void
kill_seen(struct seen *cur_seen)
{
	int i;

	/* Free memory */
	for (i = 0; i < cur_seen->rows; i++) {
		free(*(cur_seen->tiles + i));
	}
	free(cur_seen->tiles);
}

void
update_seen(struct user *cur_user)
{
	*(*(cur_user->seen[cur_user->map].tiles + cur_user->row) + cur_user->col) = 1;
}

void
init_char(struct user *cur_user)
{
	/* Make a fake character */
	cur_user->character = malloc(sizeof(*cur_user->character));
	cur_user->character->name = malloc(11);
	strncpy(cur_user->character->name, "Mr. Fake", 10);
	cur_user->character->level = 1;
	cur_user->character->cur_stats.life = 10;
	cur_user->character->cur_stats.stamina = 10;
	cur_user->character->cur_stats.magic = 10;
	cur_user->character->cur_stats.experience = 0;
	cur_user->character->cur_stats.attack = 10;
	cur_user->character->cur_stats.defense = 10;
	cur_user->character->cur_stats.dodge = 10;
	cur_user->character->cur_stats.power = 10;
	cur_user->character->cur_stats.spirit = 10;
	cur_user->character->cur_stats.avoid = 10;
	cur_user->character->max_stats.life = 10;
	cur_user->character->max_stats.stamina = 10;
	cur_user->character->max_stats.magic = 10;
	cur_user->character->max_stats.experience = 100;
	cur_user->character->max_stats.attack = 10;
	cur_user->character->max_stats.defense = 10;
	cur_user->character->max_stats.dodge = 10;
	cur_user->character->max_stats.power = 10;
	cur_user->character->max_stats.spirit = 10;
	cur_user->character->max_stats.avoid = 10;
	cur_user->character->money = 1000;
}

void
kill_char(struct user *cur_user)
{
	free(cur_user->character->name);
	free(cur_user->character);
}

SDL_bool
move_player(struct map *cur_map, struct user *cur_user, int move)
{
	int row_d, col_d;
	
	/* Set the direction to be changed based on facing direction */
	row_d = 0;
	col_d = 0;
	if (cur_user->facing == NORTH) row_d = -1;
	if (cur_user->facing == EAST) col_d = 1;
	if (cur_user->facing == SOUTH) row_d = 1;
	if (cur_user->facing == WEST) col_d = -1;
	/* Multiply by move, which is negative 1 if the player is backing up */
	row_d *= move;
	col_d *= move;
	/* See if the move is okay */
	if (*(*(cur_map->tiles + cur_user->row + row_d) + cur_user->col + col_d) == ROOM ||
	    *(*(cur_map->tiles + cur_user->row + row_d) + cur_user->col + col_d) == DOOR) {
		cur_user->row += row_d * 2;
		cur_user->col += col_d * 2;
		return SDL_TRUE;
	}
	return SDL_FALSE;
}

void
turn_player(struct user *cur_user, int turn)
{
	cur_user->facing += turn;
	if (cur_user->facing < NORTH) cur_user->facing = WEST;
	if (cur_user->facing > WEST) cur_user->facing = NORTH;
}

void
change_level(struct game *cur_game, struct map *cur_map, struct user *cur_user)
{
	if (*(*(cur_map->tiles + cur_user->row) + cur_user->col) == START) {
		cur_user->map -= 1;
	} else if (*(*(cur_map->tiles + cur_user->row) + cur_user->col) == END) {
		cur_user->map += 1;
	}
	if (cur_user->map > cur_game->num_maps - 1) cur_user->map = cur_game->num_maps - 1;
	if (cur_user->map < 0) cur_user->map = 0;
}

void
char_screen(struct game *cur_game, struct user *cur_user, SDL_bool ingame)
{
	SDL_bool loop;
	SDL_Event event;
	
	/* Make a fake character */
	init_char(cur_user);
	
	/* Input loop */
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		draw_char_screen(cur_game, cur_user, ingame);
		SDL_Delay(10);
		if (SDL_PollEvent(&event) == 0) continue;
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					loop = SDL_FALSE;
					break;
			}	
		}
	}
	
	/* Kill fake character */
	kill_char(cur_user);
}

static void
draw_char_screen(struct game *cur_game, struct user *cur_user, SDL_bool ingame)
{//1152x648 @ 64, 36
	char line[100];
	SDL_Rect out_src = { 0, 0, 1280, 720 };
	SDL_Rect out_dest = { 0, 0, cur_game->display.w, cur_game->display.h };
	SDL_Rect view_src = { 341, 10, 929, 700 };
	SDL_Rect view_dest = { 341 * cur_game->display.scale_w, 10 * cur_game->display.scale_h, 929 * cur_game->display.scale_w, 700  * cur_game->display.scale_h };	
	SDL_Rect screen_src = { 0, 0, 1152, 648 };
	SDL_Rect screen_dest = { 64, 36, 1152, 648 };
	SDL_Texture *texture;
	int points_1 = 5;
	int points_2 = 10;
	
	/* Create a custom texture and render to it */
	texture = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1152, 648);
	SDL_SetRenderTarget(cur_game->display.renderer, texture);
	/* Draw outline */
	draw_rect(cur_game, 0, 0, 1152, 648, SDL_TRUE, "black");
	draw_rect(cur_game, 0, 0, 1152, 648, SDL_FALSE, "white");
	/* Display character name */
	sprintf(line, "Name: %s (Lv. %d)", cur_user->character->name, cur_user->character->level);
	draw_sentence_xlimited(cur_game, 10, 10, line, 0.25, 1152);
	/* Draw 4 major stats */
	sprintf(line, "Major Stats (%d points)", points_1);
	draw_sentence_xlimited(cur_game, 18, 82, line, 0.125, 1152);
	draw_rect(cur_game, 10, 108, 608, 114, SDL_FALSE, "white");
	sprintf(line, "Life:       %3d / %3d", cur_user->character->cur_stats.life, cur_user->character->max_stats.life);
	draw_sentence_xlimited(cur_game, 18, 116, line, 0.125, 1152);
	sprintf(line, "Stamina:    %3d / %3d", cur_user->character->cur_stats.stamina, cur_user->character->max_stats.stamina);
	draw_sentence_xlimited(cur_game, 18, 142, line, 0.125, 1152);
	sprintf(line, "Magic:      %3d / %3d", cur_user->character->cur_stats.magic, cur_user->character->max_stats.magic);
	draw_sentence_xlimited(cur_game, 18, 168, line, 0.125, 1152);
	sprintf(line, "Experience: %3d / %3d", cur_user->character->cur_stats.experience, cur_user->character->max_stats.experience);
	draw_sentence_xlimited(cur_game, 18, 194, line, 0.125, 1152);
	/* Display 6 minor stats */
	sprintf(line, "Minor Stats (%d points)", points_2);
	draw_sentence_xlimited(cur_game, 18, 242, line, 0.125, 1152);
	draw_rect(cur_game, 10, 268, 608, 171, SDL_FALSE, "white");
	sprintf(line, "Attack:     %3d (%3d)", cur_user->character->cur_stats.attack, cur_user->character->max_stats.attack);
	draw_sentence_xlimited(cur_game, 18, 276, line, 0.125, 1152);
	sprintf(line, "Defense:    %3d (%3d)", cur_user->character->cur_stats.defense, cur_user->character->max_stats.defense);
	draw_sentence_xlimited(cur_game, 18, 302, line, 0.125, 1152);
	sprintf(line, "Dodge:      %3d (%3d)", cur_user->character->cur_stats.dodge, cur_user->character->max_stats.dodge);
	draw_sentence_xlimited(cur_game, 18, 328, line, 0.125, 1152);
	sprintf(line, "Power:      %3d (%3d)", cur_user->character->cur_stats.power, cur_user->character->max_stats.power);
	draw_sentence_xlimited(cur_game, 18, 354, line, 0.125, 1152);
	sprintf(line, "Spirit:     %3d (%3d)", cur_user->character->cur_stats.spirit, cur_user->character->max_stats.spirit);
	draw_sentence_xlimited(cur_game, 18, 380, line, 0.125, 1152);
	sprintf(line, "Avoid:      %3d (%3d)", cur_user->character->cur_stats.avoid, cur_user->character->max_stats.avoid);
	draw_sentence_xlimited(cur_game, 18, 406, line, 0.125, 1152);
	/* Display equipped 6 slots */
	sprintf(line, "Equipped Gear");
	draw_sentence_xlimited(cur_game, 18, 449, line, 0.125, 1152);
	draw_rect(cur_game, 10, 485, 373, 154, SDL_FALSE, "white");
	/* Display inventory 8 slots */
	sprintf(line, "Inventory");
	draw_sentence_xlimited(cur_game, 401, 449, line, 0.125, 1152);
	draw_rect(cur_game, 393, 485, 670, 154, SDL_FALSE, "white");
	//393, 485, 670, 150
	
	/* Switch to rendering target and output current output */
	SDL_SetRenderTarget(cur_game->display.renderer, NULL);
	SDL_RenderClear(cur_game->display.renderer);
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.output, &out_src, &out_dest);
	/* Output view if ingame */
	if (cur_game->display.view != NULL && ingame == SDL_TRUE) {
		SDL_RenderCopy(cur_game->display.renderer, cur_game->display.view, &view_src, &view_dest);
	}
	/* Output character screen */
	SDL_RenderCopy(cur_game->display.renderer, texture, &screen_src, &screen_dest);
	SDL_RenderPresent(cur_game->display.renderer);
	/* Destroy this target texture */
	SDL_DestroyTexture(texture);
}

static void
char_screen_click(struct game *cur_game, struct user *cur_user, int x, int y)
{
}
