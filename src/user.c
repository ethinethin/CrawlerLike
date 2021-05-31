#include <SDL2/SDL.h>
#include "draw.h"
#include "font.h"
#include "main.h"
#include "user.h"

/* Function prototypes */
static void	draw_char_screen(struct game *cur_game, struct user *cur_user, SDL_bool ingame);
static void	char_screen_click(struct game *cur_game, struct user *cur_user, int x, int y);
static void	point_to_stats(struct user *cur_user);


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
	strncpy(cur_user->character->name, "Mr. Faker", 10);
	cur_user->character->level = 999;
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

enum type { TYPE_RECT, TYPE_TEXT, TYPE_SPRITE };
struct char_screen_stats {
	SDL_Rect rect;
	int type;
	float scale;
	char *line;
	void *val1;
	void *val2;
	void *val3;
} char_screen_stats[] = {
	{ { 0, 0, 1152, 648 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 10, 94, 528, 104 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL }, 
	{ { 10, 228, 528, 146 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL }, 
	{ { 10, 404, 373, 234 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 393, 404, 490, 234 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 893, 404, 249, 234 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 548, 94, 594, 280 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 10, 10, 0, 0}, TYPE_TEXT, 0.20, "Name: %s (Lv. %d)", NULL, NULL, NULL },
	{ { 14, 72, 0, 0}, TYPE_TEXT, 0.1, "Major Stats (%d points)", NULL, NULL, NULL },
	{ { 18, 102, 0, 0}, TYPE_TEXT, 0.1, "Life:       %3d / %3d", NULL, NULL, NULL },
	{ { 18, 124, 0, 0}, TYPE_TEXT, 0.1, "Stamina:    %3d / %3d", NULL, NULL, NULL },
	{ { 18, 146, 0, 0}, TYPE_TEXT, 0.1, "Magic:      %3d / %3d", NULL, NULL, NULL },
	{ { 18, 168, 0, 0}, TYPE_TEXT, 0.1, "Experience: %3d / %3d", NULL, NULL, NULL },
	{ { 14, 206, 0, 0}, TYPE_TEXT, 0.1, "Minor Stats (%d points)", NULL, NULL, NULL },
	{ { 18, 236, 0, 0}, TYPE_TEXT, 0.1, "Attack:     %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 258, 0, 0}, TYPE_TEXT, 0.1, "Defense:    %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 280, 0, 0}, TYPE_TEXT, 0.1, "Dodge:      %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 302, 0, 0}, TYPE_TEXT, 0.1, "Power:      %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 324, 0, 0}, TYPE_TEXT, 0.1, "Spirit:     %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 346, 0, 0}, TYPE_TEXT, 0.1, "Avoid:      %3d (%3d)", NULL, NULL, NULL },
	{ { 14, 382, 0, 0}, TYPE_TEXT, 0.1, "Equipped Gear", NULL, NULL, NULL },
	{ { 401, 382, 0, 0}, TYPE_TEXT, 0.1, "Inventory", NULL, NULL, NULL },
	{ { 901, 382, 0, 0}, TYPE_TEXT, 0.1, "System", NULL, NULL, NULL },
	{ { 552, 72, 0, 0}, TYPE_TEXT, 0.1, "Information", NULL, NULL, NULL },
	{ { 25, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 145, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 265, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 25, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 145, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 265, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 408, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL }, 
	{ { 528, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 648, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 768, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 408, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 528, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 648, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 768, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 908, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 1028, 415, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 908, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 1028, 525, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL }
};

int points_1 = 5;
int points_2 = 10;
static void
point_to_stats(struct user *cur_user)
{
	/* Have the structure above point to the appropriate variables */
	char_screen_stats[7].val3 = cur_user->character->name; char_screen_stats[7].val1 = &cur_user->character->level;
	char_screen_stats[8].val1 = &points_1;
	char_screen_stats[9].val1 = &cur_user->character->cur_stats.life; char_screen_stats[9].val2 = &cur_user->character->max_stats.life; 
	char_screen_stats[10].val1 = &cur_user->character->cur_stats.stamina; char_screen_stats[10].val2 = &cur_user->character->max_stats.stamina; 
	char_screen_stats[11].val1 = &cur_user->character->cur_stats.magic; char_screen_stats[11].val2 = &cur_user->character->max_stats.magic; 
	char_screen_stats[12].val1 = &cur_user->character->cur_stats.experience; char_screen_stats[12].val2 = &cur_user->character->max_stats.experience; 
	char_screen_stats[13].val1 = &points_2;
	char_screen_stats[14].val1 = &cur_user->character->cur_stats.attack; char_screen_stats[14].val2 = &cur_user->character->max_stats.attack; 
	char_screen_stats[15].val1 = &cur_user->character->cur_stats.defense; char_screen_stats[15].val2 = &cur_user->character->max_stats.defense; 
	char_screen_stats[16].val1 = &cur_user->character->cur_stats.dodge; char_screen_stats[16].val2 = &cur_user->character->max_stats.dodge; 
	char_screen_stats[17].val1 = &cur_user->character->cur_stats.power; char_screen_stats[17].val2 = &cur_user->character->max_stats.power; 
	char_screen_stats[18].val1 = &cur_user->character->cur_stats.spirit; char_screen_stats[18].val2 = &cur_user->character->max_stats.spirit; 
	char_screen_stats[19].val1 = &cur_user->character->cur_stats.avoid; char_screen_stats[19].val2 = &cur_user->character->max_stats.avoid; 
}

static void
draw_char_screen(struct game *cur_game, struct user *cur_user, SDL_bool ingame)
{
	char line[100];
	int i;
	SDL_Rect out_src = { 0, 0, 1280, 720 };
	SDL_Rect out_dest = { 0, 0, cur_game->display.w, cur_game->display.h };
	SDL_Rect view_src = { 341, 10, 929, 700 };
	SDL_Rect view_dest = { 341 * cur_game->display.scale_w, 10 * cur_game->display.scale_h, 929 * cur_game->display.scale_w, 700  * cur_game->display.scale_h };	
	SDL_Rect screen_src = { 0, 0, 1152, 648 };
	SDL_Rect screen_dest = { 64, 36, 1152, 648 };
	SDL_Texture *texture;
	
	/* Create a custom texture and render to it */
	texture = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1152, 648);
	SDL_SetRenderTarget(cur_game->display.renderer, texture);
	/* Point to elements of interest */
	point_to_stats(cur_user);
	/* Display elements from the above structure */
	for (i = 0; i < 42; i++) {
		if (char_screen_stats[i].type == TYPE_RECT) {
			draw_rect(cur_game, char_screen_stats[i].rect.x, char_screen_stats[i].rect.y, char_screen_stats[i].rect.w, char_screen_stats[i].rect.h, SDL_TRUE, "black");
			draw_rect(cur_game, char_screen_stats[i].rect.x, char_screen_stats[i].rect.y, char_screen_stats[i].rect.w, char_screen_stats[i].rect.h, SDL_FALSE, "white");
		} else if (char_screen_stats[i].val3 != NULL) {
			sprintf(line, char_screen_stats[i].line, (char *) char_screen_stats[i].val3, *((int *) char_screen_stats[i].val1));
			draw_sentence_xlimited(cur_game, char_screen_stats[i].rect.x, char_screen_stats[i].rect.y, line, char_screen_stats[i].scale, 1152);
		} else if (char_screen_stats[i].val2 != NULL) {
			sprintf(line, char_screen_stats[i].line, *((int *) char_screen_stats[i].val1), *((int *) char_screen_stats[i].val2));
			draw_sentence_xlimited(cur_game, char_screen_stats[i].rect.x, char_screen_stats[i].rect.y, line, char_screen_stats[i].scale, 1152);
		} else if (char_screen_stats[i].val1 != NULL) {
			sprintf(line, char_screen_stats[i].line, *((int *) char_screen_stats[i].val1));
			draw_sentence_xlimited(cur_game, char_screen_stats[i].rect.x, char_screen_stats[i].rect.y, line, char_screen_stats[i].scale, 1152);
		} else {
			draw_sentence_xlimited(cur_game, char_screen_stats[i].rect.x, char_screen_stats[i].rect.y, char_screen_stats[i].line, char_screen_stats[i].scale, 1152);
		}
	}
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
