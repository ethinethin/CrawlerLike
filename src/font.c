#include <SDL2/SDL.h>
#include "font.h"
#include "main.h"

/* Function prototypes */
static void	draw_char(struct game *cur_game, int x, int y, int letter, float scale);
static void	format_text(char *text, char formatted[1024], int n);

void
load_font(struct game *cur_game)
{
	int i, j;
	SDL_Surface *image, *tmp;
	SDL_Rect rect = {0, 0, 192, 208};

	/* Allocate memory for 95 font characters */
	cur_game->sprites.font = malloc(sizeof(*cur_game->sprites.font)*95);
	/* Load sprite sheet */
	image = SDL_LoadBMP("art/font.bmp");
	/* Load all sprites */
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			if (i*10+j == 95) break;
			rect.x = j*192;
			rect.y = i*208;
			tmp = SDL_CreateRGBSurface(0, 192, 208, 24, 0x00, 0x00, 0x00, 0x00);
			SDL_SetColorKey(tmp, 1, 0xFF00FF);
			SDL_BlitSurface(image, &rect, tmp, NULL);
			cur_game->sprites.font[i*10+j] = SDL_CreateTextureFromSurface(cur_game->display.renderer, tmp);
			SDL_SetTextureBlendMode(cur_game->sprites.font[i*10+j], SDL_BLENDMODE_BLEND);
			SDL_FreeSurface(tmp);		
		}
	}
	SDL_FreeSurface(image);
}

void
unload_font(struct game *cur_game)
{
	int i;

	/* Free all font characters */
	for (i = 0; i < 95; i++) {
		SDL_DestroyTexture(cur_game->sprites.font[i]);
	}
	free(cur_game->sprites.font);
}


int FONT_ALPHA = 255;
void
set_font_alpha(int alpha)
{
	FONT_ALPHA = alpha;
}

static void
draw_char(struct game *cur_game, int x, int y, int letter, float scale)
{
	SDL_SetTextureAlphaMod(cur_game->sprites.font[letter], FONT_ALPHA);
	SDL_Rect rect = {x, y, 192 * scale, 208 * scale};
	SDL_RenderCopyEx(cur_game->display.renderer, cur_game->sprites.font[letter], NULL, &rect, 0, NULL, 0);
}

void
draw_sentence_xlimited(struct game *cur_game, int x, int y, char *sentence, float scale, int max_x)
{
	int i;
	int start_x = x;
	int len;
	char formatted[1024];

	/* Format the text*/
	format_text(sentence, formatted, (max_x - x)/(scale * 192) - 1);
	/* Output the text */
	for (i = 0, len = strlen(formatted); i < len; i++) {
		if (formatted[i] != '\n') {
			draw_char(cur_game, x, y, formatted[i] - 32, scale);
			x += 192 * scale;
		}
		if (x >= (max_x - 192 * scale) || formatted[i] == '\n') {
			x = start_x;
			y = y + 208 * scale;
		}
	}
}

void
draw_sentence(struct game *cur_game, int x, int y, char *sentence, float scale)
{
	draw_sentence_xlimited(cur_game, x, y, sentence, scale, 1280);
}

static void
format_text(char *text, char formatted[1024], int n)
{
	int form_i;
	int len;
	int text_i;
	int x_pos;
	
	len = strlen(text);
	/* If the string fits in a single line, just copy it */
	if (len <= n) {
		strncpy(formatted, text, len + 1);
		return;
	}
	/* Reformat text to fit window */
	for (text_i = 0, form_i = 0, x_pos = 0; text_i < len; text_i++, form_i++, x_pos++) {
		/* Check if at end of line */
		if (x_pos > n || text[text_i] == '\n') {
			/* If you're on a space, replace a new line */
			if (text[text_i] == ' ' || text[text_i] == '\n') {
				formatted[form_i] = '\n';
			} else {
				/* Go backwords looking for a space */
				while (text[text_i] != ' ') {
					text_i--; form_i--;
				}
				formatted[form_i] = '\n';
			}
			x_pos = 0;
		} else {
			formatted[form_i] = text[text_i];
		}
	}
	formatted[form_i] = '\0';
}
