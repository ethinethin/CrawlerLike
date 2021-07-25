#include <SDL2/SDL.h>
#include "char.h"
#include "draw.h"
#include "font.h"
#include "gear.h"
#include "home.h"
#include "main.h"
#include "rand.h"
#include "user.h"

/* Function prototypes */
static void	name_char(struct game *cur_game, struct user *cur_user);
static void	draw_name(struct game *cur_game, char buffer[18]);
static SDL_bool	changes_made(struct user *cur_user, int points[2]);
static void	point_to_stats(struct user *cur_user, struct stats *temp_stats[3], int points[2]);
static void	draw_char_screen(struct game *cur_game, struct user *cur_user, struct stats *temp_stats[3], int points[2]);
static void	point_to_arrows(struct stats *temp_stats[3]);
static void	draw_stat_arrows(struct game *cur_game, struct stats *temp_stats[3], int points[2]);
static void	draw_gear(struct game *cur_game, struct user *cur_user);
static void	draw_inv(struct game *cur_game, struct user *cur_user);
static void	draw_sys(struct game *cur_game, struct user *cur_user);
static int	char_screen_click(struct game *cur_game, struct user *cur_user, int points[2], int x, int y);
static void	rando_name(char name[18]);

void
init_char(struct game *cur_game, struct user *cur_user)
{
	int i;
	
	/* Make a new character with default stats */
	cur_user->character = malloc(sizeof(*cur_user->character));
	cur_user->character->name = malloc(sizeof(*cur_user->character->name)*17);
	/* Zero out stats */
	zero_stats(&cur_user->character->cur_stats);
	zero_stats(&cur_user->character->max_stats);
	zero_stats(&cur_user->character->mod_stats);
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
	cur_user->character->mod_stats.attack = 10;
	cur_user->character->mod_stats.defense = 10;
	cur_user->character->mod_stats.dodge = 10;
	cur_user->character->mod_stats.power = 10;
	cur_user->character->mod_stats.spirit = 10;
	cur_user->character->mod_stats.avoid = 10;
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
	/* TEMPORARY - gear, skills, and inventory */
	/* I will eventually add a selection of starting skills and probably
	 * a set starting gear piece */
	for (i = 0; i < 8; i++) {
		if (i < 3) {
			cur_user->character->gear[i] = i + 1;
			cur_user->character->skills[i] = i + 3 + 1;
			cur_user->character->inventory[i] = i + 6 + 1;
		} else {
			cur_user->character->inventory[i] = 0;
		}
	}
	/* END TEMPORARY */
	
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

enum char_clicks { CLICK_NULL, CLICK_STAT, CLICK_YES, CLICK_CANCEL };
void
char_screen(struct game *cur_game, struct user *cur_user)
{
	int points[2];
	int x, y;
	int results;
	SDL_bool loop;
	SDL_bool changes;
	SDL_Event event;
	struct stats increase;
	struct stats *temp_stats[4] = { &increase,
					&cur_user->character->cur_stats,
					&cur_user->character->max_stats,
					&cur_user->character->mod_stats };
	
	/* Zero out increase and allocate points */
	zero_stats(&increase);
	points[0] = cur_user->character->major_points;
	points[1] = cur_user->character->minor_points;
	/* Draw the screen */
	draw_char_screen(cur_game, cur_user, temp_stats, points);
	
	/* Input loop */
	loop = SDL_TRUE;
	while (loop == SDL_TRUE) {
		SDL_Delay(10);
		if (SDL_PollEvent(&event) == 0) continue;
		changes = changes_made(cur_user, points);
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					if (changes == SDL_TRUE) {
						if (yes_no(cur_game, "Abandon changes?", SDL_TRUE, SDL_TRUE, SDL_FALSE) == SDL_TRUE) {
							/* Niclear option - return out of loop, changes will be lost */
							return;
						}
					} else {
						loop = SDL_FALSE;
					}
					break;
				case SDLK_RETURN:
					if (changes == SDL_FALSE ||
					    yes_no(cur_game, "Finalize character changes?", SDL_TRUE, SDL_TRUE, SDL_FALSE) == SDL_TRUE) {
					    	/* Break loop, changes will be kept */
						loop = SDL_FALSE;
						break;
					}
					break;
			}
		} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			/* Respond to mouse click */ // I'm doing this wrong ... need to think about it
			x = (event.button.x) / cur_game->display.scale_w - 64;
			y = (event.button.y) / cur_game->display.scale_h - 36;
			results = char_screen_click(cur_game, cur_user, points, x, y);
			if ((results == CLICK_YES || results == CLICK_CANCEL) && changes == SDL_FALSE) {
				/* No changes, return out of function */
				return;
			} else if (results == CLICK_YES && changes == SDL_TRUE) {
				/* Accept changes? */
				if (yes_no(cur_game, "Finalize character changes?", SDL_TRUE, SDL_TRUE, SDL_FALSE) == SDL_TRUE) {
					/* Commit changes and return */
					cur_user->character->major_points = points[0];
					cur_user->character->minor_points = points[1];
					return;
				}
			} else if (results == CLICK_CANCEL && changes == SDL_TRUE) {
				/* Abandon all change? */
				if (yes_no(cur_game, "Abandon changes?", SDL_TRUE, SDL_TRUE, SDL_FALSE) == SDL_TRUE) {
					/* Nuclear option - break out of the loop to undo changes */
					loop = SDL_FALSE;
				}
			}
		} else if (event.type == SDL_MOUSEMOTION) {
			x = (event.motion.x) / cur_game->display.scale_w - 64;
			y = (event.motion.y) / cur_game->display.scale_h - 36;
			gear_mouseover(cur_game, cur_user, x, y);
		}
		/* Only output the screen if the user gave input */
		draw_char_screen(cur_game, cur_user, temp_stats, points);
	}
	/* Uncommit changes */
	cur_user->character->max_stats.life -= increase.life;
	cur_user->character->max_stats.stamina -= increase.stamina;
	cur_user->character->max_stats.magic -= increase.magic;
	cur_user->character->mod_stats.attack -= increase.attack;
	cur_user->character->max_stats.attack -= increase.attack;
	cur_user->character->mod_stats.defense -= increase.defense;
	cur_user->character->max_stats.defense -= increase.defense;
	cur_user->character->mod_stats.dodge -= increase.dodge;
	cur_user->character->max_stats.dodge -= increase.dodge;
	cur_user->character->mod_stats.power -= increase.power;
	cur_user->character->max_stats.power -= increase.power;
	cur_user->character->mod_stats.spirit -= increase.spirit;
	cur_user->character->max_stats.spirit -= increase.spirit;
	cur_user->character->mod_stats.avoid -= increase.avoid;
	cur_user->character->max_stats.avoid -= increase.avoid;
}

void
zero_stats(struct stats *cur_stats)
{
	cur_stats->life = 0;
	cur_stats->stamina = 0;
	cur_stats->magic = 0;
	cur_stats->experience = 0;
	cur_stats->attack = 0;
	cur_stats->defense = 0;
	cur_stats->dodge = 0;
	cur_stats->power = 0;
	cur_stats->spirit = 0;
	cur_stats->avoid = 0;
}

void
copy_stats(struct stats *src, struct stats *dest)
{
	dest->life = src->life;
	dest->stamina = src->stamina;
	dest->magic = src->magic;
	dest->experience = src->experience;
	dest->attack = src->attack;
	dest->defense = src->defense;
	dest->dodge = src->dodge;
	dest->power = src->power;
	dest->spirit = src->spirit;
	dest->avoid = src->avoid;
}

static SDL_bool
changes_made(struct user *cur_user, int points[2])
{
	if (cur_user->character->major_points == points[0] &&
	    cur_user->character->minor_points == points[1]) {
		return SDL_FALSE;
	} else {
		return SDL_TRUE;
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
	{ { 1028, 419, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL  },
	{ { 908, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL },
	{ { 1028, 529, 100, 100}, TYPE_RECT, 0, NULL, NULL, NULL, NULL  }
};

static void
point_to_stats(struct user *cur_user, struct stats *temp_stats[3], int points[2])
{
	/* Have the structure above point to the appropriate variables */
	char_screen_stats[7].val3 = cur_user->character->name; char_screen_stats[7].val1 = &cur_user->character->level;
	char_screen_stats[8].val1 = &points[0];
	char_screen_stats[9].val1 = &temp_stats[1]->life; char_screen_stats[9].val2 = &temp_stats[2]->life;
	char_screen_stats[10].val1 = &temp_stats[1]->stamina; char_screen_stats[10].val2 = &temp_stats[2]->stamina;
	char_screen_stats[11].val1 = &temp_stats[1]->magic; char_screen_stats[11].val2 = &temp_stats[2]->magic;
	char_screen_stats[12].val1 = &cur_user->character->cur_stats.experience; char_screen_stats[12].val2 = &cur_user->character->max_stats.experience; 
	char_screen_stats[13].val1 = &points[1];
	char_screen_stats[14].val1 = &temp_stats[3]->attack; char_screen_stats[14].val2 = &temp_stats[2]->attack;
	char_screen_stats[15].val1 = &temp_stats[3]->defense; char_screen_stats[15].val2 = &temp_stats[2]->defense;
	char_screen_stats[16].val1 = &temp_stats[3]->dodge; char_screen_stats[16].val2 = &temp_stats[2]->dodge;
	char_screen_stats[17].val1 = &temp_stats[3]->power; char_screen_stats[17].val2 = &temp_stats[2]->power;
	char_screen_stats[18].val1 = &temp_stats[3]->spirit; char_screen_stats[18].val2 = &temp_stats[2]->spirit;
	char_screen_stats[19].val1 = &temp_stats[3]->avoid; char_screen_stats[19].val2 = &temp_stats[2]->avoid;
}

static void
draw_char_screen(struct game *cur_game, struct user *cur_user, struct stats *temp_stats[3], int points[2])
{
	char line[100];
	int i;
	SDL_Rect out_src = { 0, 0, 1280, 720 };
	SDL_Rect out_dest = { 0, 0, cur_game->display.w, cur_game->display.h };
	SDL_Rect view_src = { 341, 10, 929, 700 };
	SDL_Rect view_dest = { 341 * cur_game->display.scale_w, 10 * cur_game->display.scale_h, 929 * cur_game->display.scale_w, 700  * cur_game->display.scale_h };	
	SDL_Rect screen_src = { 0, 0, 1152, 648 };
	SDL_Rect screen_dest = { 64 * cur_game->display.scale_w, 36 * cur_game->display.scale_h, 1152 * cur_game->display.scale_w, 648 * cur_game->display.scale_h };
	SDL_Rect info_src = { 0, 0, 592, 300 };
	SDL_Rect info_dest = { 549, 80, 592, 300 };
	
	/* Render to custom target and clear it */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.char_screen_tex);
	SDL_SetRenderDrawColor(cur_game->display.renderer, 16, 16, 16, 255);
	SDL_RenderClear(cur_game->display.renderer);
	/* Point to elements of interest */
	point_to_stats(cur_user, temp_stats, points);
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
	draw_stat_arrows(cur_game, temp_stats, points);
	draw_gear(cur_game, cur_user);
	draw_inv(cur_game, cur_user);
	draw_sys(cur_game, cur_user);
	/* Output info */
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.info, &info_src, &info_dest);
	/* Switch to rendering target and output current output */
	SDL_SetRenderTarget(cur_game->display.renderer, NULL);
	SDL_RenderClear(cur_game->display.renderer);
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.output, &out_src, &out_dest);
	/* Output view */
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.view, &view_src, &view_dest);
	/* Output character screen */
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.char_screen_tex, &screen_src, &screen_dest);
	SDL_RenderPresent(cur_game->display.renderer);
}

enum arrow_type { ARROW_PLUS_MAJOR, ARROW_PLUS_MINOR, ARROW_MINUS_MAJOR, ARROW_MINUS_MINOR, ARROW_NULL };
struct char_screen_stats arrows[] = {
	{ { 483, 83, 25, 25 }, ARROW_PLUS_MAJOR, 0, NULL, NULL, NULL, NULL },
	{ { 483, 109, 25, 25 }, ARROW_PLUS_MAJOR, 0, NULL, NULL, NULL, NULL },
	{ { 483, 135, 25, 25 }, ARROW_PLUS_MAJOR, 0, NULL, NULL, NULL, NULL },
	{ { 483, 222, 25, 25 }, ARROW_PLUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 483, 248, 25, 25 }, ARROW_PLUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 483, 274, 25, 25 }, ARROW_PLUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 483, 300, 25, 25 }, ARROW_PLUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 483, 326, 25, 25 }, ARROW_PLUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 483, 352, 25, 25 }, ARROW_PLUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 83, 25, 25 }, ARROW_MINUS_MAJOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 109, 25, 25 }, ARROW_MINUS_MAJOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 135, 25, 25 }, ARROW_MINUS_MAJOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 222, 25, 25 }, ARROW_MINUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 248, 25, 25 }, ARROW_MINUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 274, 25, 25 }, ARROW_MINUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 300, 25, 25 }, ARROW_MINUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 326, 25, 25 }, ARROW_MINUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { 509, 352, 25, 25 }, ARROW_MINUS_MINOR, 0, NULL, NULL, NULL, NULL },
	{ { -1, -1, -1, -1 }, ARROW_NULL, 0, NULL, NULL, NULL, NULL }
};

static void
point_to_arrows(struct stats *temp_stats[3])
{
	arrows[0].val1 = &temp_stats[0]->life;		arrows[9].val1 = &temp_stats[0]->life;
	arrows[0].val2 = &temp_stats[1]->life;		arrows[9].val2 = &temp_stats[1]->life;
	arrows[0].val3 = &temp_stats[2]->life;		arrows[9].val3 = &temp_stats[2]->life;
	arrows[1].val1 = &temp_stats[0]->stamina;	arrows[10].val1 = &temp_stats[0]->stamina;
	arrows[1].val2 = &temp_stats[1]->stamina;	arrows[10].val2 = &temp_stats[1]->stamina;
	arrows[1].val3 = &temp_stats[2]->stamina;	arrows[10].val3 = &temp_stats[2]->stamina;
	arrows[2].val1 = &temp_stats[0]->magic;		arrows[11].val1 = &temp_stats[0]->magic;
	arrows[2].val2 = &temp_stats[1]->magic;		arrows[11].val2 = &temp_stats[1]->magic;
	arrows[2].val3 = &temp_stats[2]->magic;		arrows[11].val3 = &temp_stats[2]->magic;
	arrows[3].val1 = &temp_stats[0]->attack;	arrows[12].val1 = &temp_stats[0]->attack;
	arrows[3].val2 = &temp_stats[3]->attack;	arrows[12].val2 = &temp_stats[3]->attack;
	arrows[3].val3 = &temp_stats[2]->attack;	arrows[12].val3 = &temp_stats[2]->attack;
	arrows[4].val1 = &temp_stats[0]->defense;	arrows[13].val1 = &temp_stats[0]->defense;
	arrows[4].val2 = &temp_stats[3]->defense;	arrows[13].val2 = &temp_stats[3]->defense;
	arrows[4].val3 = &temp_stats[2]->defense;	arrows[13].val3 = &temp_stats[2]->defense;
	arrows[5].val1 = &temp_stats[0]->dodge;		arrows[14].val1 = &temp_stats[0]->dodge;
	arrows[5].val2 = &temp_stats[3]->dodge;		arrows[14].val2 = &temp_stats[3]->dodge;
	arrows[5].val3 = &temp_stats[2]->dodge;		arrows[14].val3 = &temp_stats[2]->dodge;
	arrows[6].val1 = &temp_stats[0]->power;		arrows[15].val1 = &temp_stats[0]->power;
	arrows[6].val2 = &temp_stats[3]->power;		arrows[15].val2 = &temp_stats[3]->power;
	arrows[6].val3 = &temp_stats[2]->power;		arrows[15].val3 = &temp_stats[2]->power;
	arrows[7].val1 = &temp_stats[0]->spirit;	arrows[16].val1 = &temp_stats[0]->spirit;
	arrows[7].val2 = &temp_stats[3]->spirit;	arrows[16].val2 = &temp_stats[3]->spirit;
	arrows[7].val3 = &temp_stats[2]->spirit;	arrows[16].val3 = &temp_stats[2]->spirit;
	arrows[8].val1 = &temp_stats[0]->avoid;		arrows[17].val1 = &temp_stats[0]->avoid;
	arrows[8].val2 = &temp_stats[3]->avoid;		arrows[17].val2 = &temp_stats[3]->avoid;
	arrows[8].val3 = &temp_stats[2]->avoid;		arrows[17].val3 = &temp_stats[2]->avoid;
}

static void
draw_stat_arrows(struct game *cur_game, struct stats *temp_stats[3], int points[2])
{
	int i;
	int sprite;
	
	point_to_arrows(temp_stats);
	for (i = 0; arrows[i].rect.x != -1; i++) {
		if (arrows[i].type == ARROW_PLUS_MAJOR && points[0] > 0) {
			sprite = 0;
		} else if (arrows[i].type == ARROW_PLUS_MAJOR && points[0] <= 0) {
			sprite = 2;
		} else if (arrows[i].type == ARROW_PLUS_MINOR && points[1] > 0) {
			sprite = 0;
		} else if (arrows[i].type == ARROW_PLUS_MINOR && points[1] <= 0) { 
			sprite = 2;
		} else if ((arrows[i].type == ARROW_MINUS_MAJOR || arrows[i].type == ARROW_MINUS_MINOR) &&
			   *((int *) arrows[i].val1) > 0) {
			sprite = 1;
		} else if ((arrows[i].type == ARROW_MINUS_MAJOR || arrows[i].type == ARROW_MINUS_MINOR) &&
			   *((int *) arrows[i].val1) <= 0) {
			sprite = 3;
		} else {
			continue;
		}
		draw_sprites(cur_game, cur_game->sprites.icons, sprite, arrows[i].rect.x, arrows[i].rect.y, arrows[i].rect.w, arrows[i].rect.h, 255, SDL_FALSE);
	}	
}

static void
draw_gear(struct game *cur_game, struct user *cur_user)
{
	int i;
	int x[] = { 25, 145, 265 };
	
	for (i = 0; i < 3; i++) {
		/* Draw gear */
		draw_sprites(cur_game, cur_game->sprites.icons, 8 + i, x[i], 419, 100, 100, 255, SDL_FALSE);
		if (cur_user->character->gear[i] != 0) {
			draw_sprites(cur_game, cur_game->sprites.gear_rarity, gear_rarity(cur_user->character->gear[i]), x[i], 419, 100, 100, 255, SDL_FALSE);
			draw_sprites(cur_game, cur_game->sprites.gear_type, gear_type(cur_user->character->gear[i]), x[i], 419, 100, 100, 255, SDL_FALSE);
			draw_sprites(cur_game, cur_game->sprites.gear_attribute, gear_attribute(cur_user->character->gear[i]), x[i], 419, 100, 100, 255, SDL_FALSE);
			draw_sprites(cur_game, cur_game->sprites.icons, 16, x[i], 419, 100, 100, 255, SDL_FALSE);
		}
		/* Draw skills */
		draw_sprites(cur_game, cur_game->sprites.icons, 11, x[i], 529, 100, 100, 255, SDL_FALSE);
		if (cur_user->character->skills[i] != 0) {
			draw_sprites(cur_game, cur_game->sprites.gear_rarity, gear_rarity(cur_user->character->skills[i]), x[i], 529, 100, 100, 255, SDL_FALSE);
			draw_sprites(cur_game, cur_game->sprites.gear_type, gear_type(cur_user->character->skills[i]), x[i], 529, 100, 100, 255, SDL_FALSE);
			draw_sprites(cur_game, cur_game->sprites.gear_attribute, gear_attribute(cur_user->character->skills[i]), x[i], 529, 100, 100, 255, SDL_FALSE);
			draw_sprites(cur_game, cur_game->sprites.icons, 16, x[i], 529, 100, 100, 255, SDL_FALSE);
		}
	}
}

static void
draw_inv(struct game *cur_game, struct user *cur_user)
{
	int i, j;
	int x[] = { 408, 528, 648, 768 };
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			draw_sprites(cur_game, cur_game->sprites.icons, 12, x[j], 419 + i * 110, 100, 100, 255, SDL_FALSE);
			if (cur_user->character->inventory[j + i * 4] != 0) {
				draw_sprites(cur_game, cur_game->sprites.gear_rarity, gear_rarity(cur_user->character->inventory[j + i * 4]), x[j], 419 + i * 110, 100, 100, 255, SDL_FALSE);
				draw_sprites(cur_game, cur_game->sprites.gear_type, gear_type(cur_user->character->inventory[j + i * 4]), x[j], 419 + i * 110, 100, 100, 255, SDL_FALSE);
				draw_sprites(cur_game, cur_game->sprites.gear_attribute, gear_attribute(cur_user->character->inventory[j + i * 4]), x[j], 419 + i * 110, 100, 100, 255, SDL_FALSE);
				draw_sprites(cur_game, cur_game->sprites.icons, 16, x[j], 419 + i * 110, 100, 100, 255, SDL_FALSE);
			}
		}
	}
}

static void
draw_sys(struct game *cur_game, struct user *cur_user)
{
	char money[6];
	draw_sprites(cur_game, cur_game->sprites.icons, 5, 908, 419, 100, 100, 255, SDL_FALSE);
	if (cur_user->character->money == 0) {
		sprintf(money, "    0");
	} else {
		sprintf(money, "%5d", cur_user->character->money);
	}
	draw_sentence(cur_game, 915, 494, money, 0.095);
	draw_sprites(cur_game, cur_game->sprites.icons, 6, 1028, 419, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 4, 908, 529, 100, 100, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 7, 1028, 529, 100, 100, 255, SDL_FALSE);
}

static int
char_screen_click(struct game *cur_game, struct user *cur_user, int points[2], int x, int y)
{
	int i;
	SDL_Event event;
	
	/* First, poll for the next event.. if it is not mouse button up, they're dragging */
	while (SDL_PollEvent(&event) == 0) {
		SDL_Delay(10);
	}
	if (event.type != SDL_MOUSEBUTTONUP) {
		/* They might be trying to drag something */
		if (drag_mouse(cur_game, cur_user, x, y) == SDL_TRUE) {
			return CLICK_NULL;
		}
	}
	/* Single clicked, so try to handle it */
	for (i = 0; arrows[i].rect.x != -1; i++) {
		if (x >= arrows[i].rect.x && x <= arrows[i].rect.x + arrows[i].rect.w && 
		    y >= arrows[i].rect.y && y <= arrows[i].rect.y + arrows[i].rect.h) {
			switch(arrows[i].type) {
				case ARROW_PLUS_MAJOR:
					if (points[0] > 0) {
						points[0] -= 1;
						*((int *) arrows[i].val1) += 1;
						*((int *) arrows[i].val2) += 1;
						*((int *) arrows[i].val3) += 1;
						return CLICK_STAT;
					}
					break;
				case ARROW_PLUS_MINOR:
					if (points[1] > 0) {
						points[1] -= 1;
						*((int *) arrows[i].val1) += 1;
						*((int *) arrows[i].val2) += 1;
						*((int *) arrows[i].val3) += 1;
						return CLICK_STAT;
					}
					break;
				case ARROW_MINUS_MAJOR:
					if (*((int *) arrows[i].val1) > 0) {
						*((int *) arrows[i].val1) -= 1;
						*((int *) arrows[i].val2) -= 1;
						*((int *) arrows[i].val3) -= 1;
						points[0] += 1;	
						return CLICK_STAT;
					}
					break;
				case ARROW_MINUS_MINOR:
					if (*((int *) arrows[i].val1) > 0) {
						*((int *) arrows[i].val1) -= 1;
						*((int *) arrows[i].val2) -= 1;
						*((int *) arrows[i].val3) -= 1;
						points[1] += 1;	
						return CLICK_STAT;
					}
					break;
			}
		}
	}
	/* Check if ok or cancel were clicked */
	if (x >= 1028 && x <= 1128 && y >= 419 && y <= 519) {
		return CLICK_YES;
	} else if (x >= 1028 && x <= 1128 && y >= 529 && y <= 629) {
		return CLICK_CANCEL;
	} else {
		return CLICK_NULL;
	}
}

static void
rando_name(char name[18])
{
	char *consonants[] = { "b", "bl", "c", "cl", "cr", "d", "fl", "fr", "g", "gl",
			       "gr", "h", "kl", "kr", "l", "ll", "m", "p", "pl",
			       "pr", "s", "sl", "st", "t", "tr", "v", "w", "wr", "z", "zh" };
	char *vowels[] = { "ae", "ai", "ar", "ay", "e", "ee", "en", "i", "o", "on", "oo", "ou", "u", "y" };
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

void
update_stats(struct user *cur_user)
{
	int i;
	struct stats *mods_gear;
	struct stats *mods_skills;
	struct stats empty;
	
	/* Subtract major modifiers from major max */
	cur_user->character->max_stats.life -= cur_user->character->mod_stats.life;
	cur_user->character->max_stats.stamina -= cur_user->character->mod_stats.stamina;
	cur_user->character->max_stats.magic -= cur_user->character->mod_stats.magic;
	/* Zero out major stat modifiers and reset minor stat modifiers */
	cur_user->character->mod_stats.life = 0;
	cur_user->character->mod_stats.stamina = 0;
	cur_user->character->mod_stats.magic = 0;
	cur_user->character->mod_stats.attack = cur_user->character->max_stats.attack;
	cur_user->character->mod_stats.defense = cur_user->character->max_stats.defense;
	cur_user->character->mod_stats.dodge = cur_user->character->max_stats.dodge;
	cur_user->character->mod_stats.power = cur_user->character->max_stats.power;
	cur_user->character->mod_stats.spirit = cur_user->character->max_stats.spirit;
	cur_user->character->mod_stats.avoid = cur_user->character->max_stats.avoid;
	/* Go through each gear/skill slot and add stats to cur_stats */
	zero_stats(&empty);
	for (i = 0; i < 3; i += 1) {
		mods_gear = cur_user->character->gear[i] == 0 ? &empty : gear_stats(cur_user->character->gear[i]);
		mods_skills = cur_user->character->skills[i] == 0 ? &empty : gear_stats(cur_user->character->skills[i]);
		cur_user->character->mod_stats.life += mods_gear->life + mods_skills->life;
		cur_user->character->mod_stats.stamina += mods_gear->stamina + mods_skills->stamina;
		cur_user->character->mod_stats.magic += mods_gear->magic + mods_skills->magic;
		cur_user->character->mod_stats.attack += mods_gear->attack + mods_skills->attack;
		cur_user->character->mod_stats.defense += mods_gear->defense + mods_skills->defense;
		cur_user->character->mod_stats.dodge += mods_gear->dodge + mods_skills->dodge;
		cur_user->character->mod_stats.power += mods_gear->power + mods_skills->power;
		cur_user->character->mod_stats.spirit += mods_gear->spirit + mods_skills->spirit;
		cur_user->character->mod_stats.avoid += mods_gear->avoid + mods_skills->avoid;
	}
	/* Re-add major stats to max */
	cur_user->character->max_stats.life += cur_user->character->mod_stats.life;
	cur_user->character->max_stats.stamina += cur_user->character->mod_stats.stamina;
	cur_user->character->max_stats.magic += cur_user->character->mod_stats.magic;
	/* Check if current major stats are over max and reduce if necessary */
	if (cur_user->character->cur_stats.life > cur_user->character->max_stats.life) {
		cur_user->character->cur_stats.life = cur_user->character->max_stats.life;
	}
	if (cur_user->character->cur_stats.stamina > cur_user->character->max_stats.stamina) {
		cur_user->character->cur_stats.stamina = cur_user->character->max_stats.stamina;
	}
	if (cur_user->character->cur_stats.magic > cur_user->character->max_stats.magic) {
		cur_user->character->cur_stats.magic = cur_user->character->max_stats.magic;
	}
}

void
draw_meters(struct game *cur_game, struct user *cur_user)
{
	int meter;
	float meter_per;
	
	/* Full meter rectangle horizontal size = 235 */
	/* Draw life */
	meter_per = ((float) cur_user->character->cur_stats.life) / ((float) cur_user->character->max_stats.life);
	meter = 235 * meter_per;
	draw_sentence(cur_game, 30, 444, "LP", 0.1);
	draw_rect(cur_game, 75, 445, meter, 16, SDL_TRUE, "lightred");
	draw_sprites(cur_game, cur_game->sprites.icons, 14, 73, 445, 50, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 15, 123, 445, 139, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 14, 262, 445, 50, 50, 255, SDL_TRUE);
	/* Draw stamina */
	meter_per = ((float) cur_user->character->cur_stats.stamina) / ((float) cur_user->character->max_stats.stamina);
	meter = 235 * meter_per;
	draw_sentence(cur_game, 30, 466, "SP", 0.1);
	draw_rect(cur_game, 75, 467, meter, 16, SDL_TRUE, "lightgreen");
	draw_sprites(cur_game, cur_game->sprites.icons, 14, 73, 467, 50, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 15, 123, 467, 139, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 14, 262, 467, 50, 50, 255, SDL_TRUE);
	/* Draw magic */
	meter_per = ((float) cur_user->character->cur_stats.magic) / ((float) cur_user->character->max_stats.magic);
	meter = 235 * meter_per;
	draw_sentence(cur_game, 30, 488, "MP", 0.1);
	draw_rect(cur_game, 75, 489, meter, 16, SDL_TRUE, "lightblue");
	draw_sprites(cur_game, cur_game->sprites.icons, 14, 73, 489, 50, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 15, 123, 489, 139, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 14, 262, 489, 50, 50, 255, SDL_TRUE);
	/* Draw experience */
	meter_per = ((float) cur_user->character->cur_stats.experience) / ((float) cur_user->character->max_stats.experience);
	meter = 235 * meter_per;
	draw_sentence(cur_game, 30, 510, "XP", 0.1);
	draw_rect(cur_game, 75, 511, meter, 16, SDL_TRUE, "lightyellow");
	draw_sprites(cur_game, cur_game->sprites.icons, 14, 73, 511, 50, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 15, 123, 511, 139, 50, 255, SDL_FALSE);
	draw_sprites(cur_game, cur_game->sprites.icons, 14, 262, 511, 50, 50, 255, SDL_TRUE);
}
