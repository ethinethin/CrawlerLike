#include <stdio.h>
#include "main.h"
#include "save.h"
#include "user.h"

void
save_opts(struct game *cur_game)
{
	FILE *fp;
	
	fp = fopen("options.ini", "w");
	if (fp == NULL) {
		printf("Could not open options.ini\n");
		exit(1);
	}
	
	/* Save the settings from the options screen */
	fprintf(fp, "res=%dx%d\n", cur_game->display.w, cur_game->display.h);
	fprintf(fp, "vsync=%d\n", cur_game->display.vsync);
	fprintf(fp, "displaymode=%d\n", cur_game->display.mode);
	fclose(fp);
}

void
load_opts(struct game *cur_game)
{
	FILE *fp;
	
	fp = fopen("options.ini", "r");
	if (fp == NULL) {
		/* File does not exist, keep the default values */
		return;
	}
	
	/* Load the settings */
	fscanf(fp, "res=%dx%d\n", &cur_game->display.w, &cur_game->display.h);
	cur_game->display.scale_w = cur_game->display.w/1280.0;
	cur_game->display.scale_h = cur_game->display.h/720.0;
	fscanf(fp, "vsync=%d\n", &cur_game->display.vsync);
	fscanf(fp, "displaymode=%d\n", &cur_game->display.mode);
	fclose(fp);
}
