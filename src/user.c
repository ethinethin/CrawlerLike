#include <SDL2/SDL.h>
#include "draw.h"
#include "font.h"
#include "main.h"
#include "rand.h"
#include "user.h"

/* Function prototypes */
static void	name_char(struct game *cur_game, struct user *cur_user);
static void	draw_name(struct game *cur_game, char buffer[18]);
static void	point_to_stats(struct user *cur_user);
static void	draw_char_screen(struct game *cur_game, struct user *cur_user, SDL_bool ingame);
static void	draw_stats(struct game *cur_game);
static void	draw_gear(struct game *cur_game);
static void	draw_inv(struct game *cur_game);
static void	draw_sys(struct game *cur_game, struct user *cur_user);
static void	char_screen_click(struct game *cur_game, struct user *cur_user, int x, int y);
static void	rando_name(char name[18]);

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
init_char(struct game *cur_game, struct user *cur_user)
{
	/* Make a new character with default stats */
	cur_user->character = malloc(sizeof(*cur_user->character));
	cur_user->character->name = malloc(sizeof(*cur_user->character->name)*17);
	name_char(cur_game, cur_user);
	cur_user->character->level = 1;
	cur_user->character->major_points = 5;
	cur_user->character->minor_points = 10;
	/* Allocate 10 to each major stat */
	cur_user->character->cur_stats.life = 10;
	cur_user->character->cur_stats.stamina = 10;
	cur_user->character->cur_stats.magic = 10;
	cur_user->character->cur_stats.experience = 0;
	/* Allocate 10 to each minor stat */
	cur_user->character->cur_stats.attack = 10;
	cur_user->character->cur_stats.defense = 10;
	cur_user->character->cur_stats.dodge = 10;
	cur_user->character->cur_stats.power = 10;
	cur_user->character->cur_stats.spirit = 10;
	cur_user->character->cur_stats.avoid = 10;
	/* Allocate 10 to each major stat */
	cur_user->character->max_stats.life = 10;
	cur_user->character->max_stats.stamina = 10;
	cur_user->character->max_stats.magic = 10;
	cur_user->character->max_stats.experience = 100;
	/* Allocate 10 to each minor stat */
	cur_user->character->max_stats.attack = 10;
	cur_user->character->max_stats.defense = 10;
	cur_user->character->max_stats.dodge = 10;
	cur_user->character->max_stats.power = 10;
	cur_user->character->max_stats.spirit = 10;
	cur_user->character->max_stats.avoid = 10;
	/* Start with 100 money */
	cur_user->character->money = 100;
}

void
kill_char(struct user *cur_user)
{
	free(cur_user->character->name);
	free(cur_user->character);
}

static void
name_char(struct game *cur_game, struct user *cur_user)
{
	char buffer[18];
	char symbols[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };
	char letter;
	int cursor;
	int key;
	int x, y;
	SDL_bool loop;
	SDL_bool redraw;
	SDL_bool shift;
	SDL_Event event;
	
	/* Enter input loop */
	buffer[0] = '_'; buffer[1] = '\0';
	cursor = 0;
	/* draw screen */
	render_clear(cur_game, "darkred");
	draw_name(cur_game, buffer);
	render_present(cur_game, SDL_FALSE);
	/* enter input loop */
	redraw = SDL_FALSE;
	shift = SDL_FALSE;
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		SDL_Delay(10);
		if (SDL_PollEvent(&event) == 0) continue;
		switch (event.type) {
			case SDL_QUIT:
				exit(1);
				break;
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				/* If holding shift */
				if (key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
					shift = SDL_TRUE;
				} else if (key == SDLK_BACKSPACE && cursor > 0) {
					buffer[cursor - 1] = '_';
					buffer[cursor] = '\0';
					cursor--;
				} else if (key >= 'a' && key <= 'z' && cursor < 15) {
					if (shift == SDL_TRUE) {
						letter = key - 32;
					} else {
						letter = key;
					}
					buffer[cursor] = letter;
					buffer[cursor + 1] = '_';
					buffer[cursor + 2] = '\0';
					cursor++;
				} else if (key >= '0' && key <= '9' && cursor < 15) {
					if (shift == SDL_TRUE) {
						key = key - 48;
						key = symbols[key];
					}
					buffer[cursor] = key;
					buffer[cursor + 1] = '_';
					buffer[cursor + 2] = '\0';
					cursor++;
				} else if (key >= 32 && key <= 126 && cursor < 15) {
					if (shift == SDL_TRUE) {
						if (key == '\'') key = '"';
						else if (key == ';') key = ':';
						else if (key == ',') key = '<';
						else if (key == '=') key = '+';
						else if (key == '.') key = '>';
						else if (key == '/') key = '?';
						else if (key == '-') key = '_';
						else if (key == '`') key = '~';
						else if (key == '[') key = '{';
						else if (key == '\\') key = '|';
						else if (key == ']') key = '}';
					}
					buffer[cursor] = key;
					buffer[cursor + 1] = '_';
					buffer[cursor + 2] = '\0';
					cursor++;
				} else if (key == SDLK_RETURN && cursor > 0) {
					loop = SDL_FALSE;
				}
				redraw = SDL_TRUE;
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
					shift = SDL_FALSE;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					/* respond to the mouse click */
					x = event.button.x / cur_game->display.scale_w;
					y = event.button.y / cur_game->display.scale_h;
					if (x >= 785 && x <= 835 && y >= 376 && y <= 426) {
						if (cursor > 0) {
							loop = SDL_FALSE;
						}
					} else if (x >= 730 && x <= 780 && y >= 376 && y <= 426) {
						rando_name(buffer);
						cursor = strnlen(buffer, 18);
						buffer[cursor] = '_';
					}
				}
				redraw = SDL_TRUE;
				break;
			default:
				break;
		}
		if (redraw == SDL_TRUE) {
			render_clear(cur_game, "darkred");
			draw_name(cur_game, buffer);
			render_present(cur_game, SDL_FALSE);
			redraw = SDL_FALSE;
		}
	}
	/* Copy the name to the character */
	buffer[cursor] = '\0';
	strncpy(cur_user->character->name, buffer, strnlen(buffer, 16) + 1);
}

static void
draw_name(struct game *cur_game, char buffer[18])
{
	draw_sentence(cur_game, 10, 10, "Name Your Character", 0.25);
	draw_sentence(cur_game, 429, 349, "Name: ", 0.1);
	draw_rect(cur_game, 523, 345, 310, 26, SDL_TRUE, "black");
	draw_rect(cur_game, 523, 345, 310, 26, SDL_FALSE, "white");
	draw_sentence(cur_game, 526, 349, buffer, 0.1);
	draw_sprites(cur_game, cur_game->sprites.icons, 6, 785, 376, 50, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 13, 730, 376, 50, 50, 255, SDL_FALSE);
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
	
	/* Make a fake character and draw the screen */
	draw_char_screen(cur_game, cur_user, ingame);
	
	/* Input loop */
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		SDL_Delay(10);
		if (SDL_PollEvent(&event) == 0) continue;
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					loop = SDL_FALSE;
					break;
			}	
		}
		/* Only output the screen if the user gave input */
		draw_char_screen(cur_game, cur_user, ingame);
	}
}

enum type { TYPE_RECT, TYPE_TEXT, TYPE_ICON };
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
	{ { 10, 79, 528, 109 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL }, 
	{ { 10, 218, 528, 163 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL }, 
	{ { 10, 409, 373, 232 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 393, 409, 490, 232 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 893, 409, 249, 232 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 548, 79, 594, 302 }, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 10, 10, 0, 0}, TYPE_TEXT, 0.20, "Name: %s (Lv. %d)", NULL, NULL, NULL },
	{ { 14, 57, 0, 0}, TYPE_TEXT, 0.1, "Major Stats (%d points)", NULL, NULL, NULL },
	{ { 18, 87, 0, 0}, TYPE_TEXT, 0.1, "Life:       %3d / %3d", NULL, NULL, NULL },
	{ { 18, 113, 0, 0}, TYPE_TEXT, 0.1, "Stamina:    %3d / %3d", NULL, NULL, NULL },
	{ { 18, 139, 0, 0}, TYPE_TEXT, 0.1, "Magic:      %3d / %3d", NULL, NULL, NULL },
	{ { 18, 165, 0, 0}, TYPE_TEXT, 0.1, "Experience: %3d / %3d", NULL, NULL, NULL },
	{ { 14, 196, 0, 0}, TYPE_TEXT, 0.1, "Minor Stats (%d points)", NULL, NULL, NULL },
	{ { 18, 226, 0, 0}, TYPE_TEXT, 0.1, "Attack:     %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 252, 0, 0}, TYPE_TEXT, 0.1, "Defense:    %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 278, 0, 0}, TYPE_TEXT, 0.1, "Dodge:      %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 304, 0, 0}, TYPE_TEXT, 0.1, "Power:      %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 330, 0, 0}, TYPE_TEXT, 0.1, "Spirit:     %3d (%3d)", NULL, NULL, NULL },
	{ { 18, 356, 0, 0}, TYPE_TEXT, 0.1, "Avoid:      %3d (%3d)", NULL, NULL, NULL },
	{ { 14, 387, 0, 0}, TYPE_TEXT, 0.1, "Equipped Gear", NULL, NULL, NULL },
	{ { 401, 387, 0, 0}, TYPE_TEXT, 0.1, "Inventory", NULL, NULL, NULL },
	{ { 901, 387, 0, 0}, TYPE_TEXT, 0.1, "System", NULL, NULL, NULL },
	{ { 552, 57, 0, 0}, TYPE_TEXT, 0.1, "Information", NULL, NULL, NULL },
	{ { 25, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 145, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 265, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 25, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 145, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 265, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 408, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL }, 
	{ { 528, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 648, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 768, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 408, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 528, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 648, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 768, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 908, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 1028, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 908, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 1028, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL }
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
	SDL_Rect screen_dest = { 64* cur_game->display.scale_w, 36 * cur_game->display.scale_h, 1152* cur_game->display.scale_w, 648 * cur_game->display.scale_h };
	
	/* Render to custom target and clear it */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.char_screen_tex);
	SDL_SetRenderDrawColor(cur_game->display.renderer, 16, 16, 16, 255);
	SDL_RenderClear(cur_game->display.renderer);
	/* Point to elements of interest */
	point_to_stats(cur_user);
	/* Draw outline */
	draw_rect(cur_game, 0, 0, screen_src.w, screen_src.h, SDL_FALSE, "white");
	/* Display elements from the above structure */
	for (i = 0; i < 42; i++) {
		if (char_screen_stats[i].type == TYPE_RECT) {
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
	/* Draw icons */
	draw_stats(cur_game);
	draw_gear(cur_game);
	draw_inv(cur_game);
	draw_sys(cur_game, cur_user);

	/* Switch to rendering target and output current output */
	SDL_SetRenderTarget(cur_game->display.renderer, NULL);
	SDL_RenderClear(cur_game->display.renderer);
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.output, &out_src, &out_dest);
	/* Output view if ingame */
	if (cur_game->display.view != NULL && ingame == SDL_TRUE) {
		SDL_RenderCopy(cur_game->display.renderer, cur_game->display.view, &view_src, &view_dest);
	}
	/* Output character screen */
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.char_screen_tex, &screen_src, &screen_dest);
	SDL_RenderPresent(cur_game->display.renderer);
	/* Destroy this target texture */
	//SDL_DestroyTexture(texture);
}

static void
draw_stats(struct game *cur_game)
{
	/* Minor stats */
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 83, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 83, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 109, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 109, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 135, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 135, 25, 25, 255, SDL_FALSE);
	/* Minor stats */
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 222, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 222, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 248, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 248, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 274, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 274, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 300, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 300, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 326, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 326, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 0, 483, 352, 25, 25, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 1, 509, 352, 25, 25, 255, SDL_FALSE);
}

static void
draw_gear(struct game *cur_game)
{
	draw_sprites(cur_game, cur_game->sprites.icons, 8, 25, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 9, 145, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 10, 265, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 11, 25, 529, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 11, 145, 529, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 11, 265, 529, 100, 100, 255, SDL_FALSE);
}

static void
draw_inv(struct game *cur_game)
{
	draw_sprites(cur_game, cur_game->sprites.icons, 12, 408, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 12, 528, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 12, 648, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 12, 768, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 12, 408, 529, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 12, 528, 529, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 12, 648, 529, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 12, 768, 529, 100, 100, 255, SDL_FALSE);
}

static void
draw_sys(struct game *cur_game, struct user *cur_user)
{
	char money[6];
	draw_sprites(cur_game, cur_game->sprites.icons, 5, 908, 419, 100, 100, 255, SDL_FALSE);
	sprintf(money, "%5d", cur_user->character->money);
	draw_sentence(cur_game, 911, 494, money, 0.1);
	draw_sprites(cur_game, cur_game->sprites.icons, 6, 1028, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 4, 908, 529, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 7, 1028, 529, 100, 100, 255, SDL_FALSE);
}

static void
char_screen_click(struct game *cur_game, struct user *cur_user, int x, int y)
{
}

static void
rando_name(char name[18])
{
	char *consonants[] = { "b", "bl", "c", "cl", "cr", "d", "fl", "fr", "g", "gl",
			       "gr", "h", "kl", "kr", "l", "ll", "m", "p", "pl",
			       "pr", "s", "sl", "st", "t", "tr", "v", "w", "wr", "z", "zh" };
	char *vowels[] = { "ae", "ai", "an", "ay", "e", "ee", "en", "i", "o", "on", "oo", "ou", "u", "y" };
	int i;
	
	/* Empty out the string */
	for (i = 0; i < 18; i++) name[i] = '\0';
	/* Add random syllables */
	for (i = 0; i < rand_num(2, 4); i++) {
		strncat(name, consonants[rand_num(0, 29)], 2);
		strncat(name, vowels[rand_num(0, 13)], 2);
	}
	/* Make first letter uppercase */
	name[0] -= 32;
}	
