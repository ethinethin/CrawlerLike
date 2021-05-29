#include <SDL2/SDL.h>
#include "font.h"
#include "main.h"

/* Function prototypes */
static void	draw_char(struct game *cur_game, int x, int y, int letter, float scale);

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

	for (i = 0, len = strlen(sentence); i < len; i++) {
		if (sentence[i] != '\n') {
			draw_char(cur_game, x, y, sentence[i] - 32, scale);
			x += 192 * scale;
		}
		if (x >= (max_x - 192 * scale) || sentence[i] == '\n') {
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
