#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "home.h"
#include "main.h"
#include "maze.h"
#include "save.h"
#include "user.h"

/* Function prototypes */
static void	save_info(struct game *cur_game, struct user *cur_user);
static void	save_map(struct game *cur_game);
static void	load_map(struct game *cur_game);
static void	save_player(struct user *cur_user);
static void	load_player(struct user *cur_user);
static void	save_char(struct user *cur_user);
static void	load_char(struct user *cur_user);

SDL_bool checked_directories = SDL_FALSE;
static void
check_directories(void)
{
	DIR *dp;

	/* Check if save directories exist */
	dp = opendir("save");
	if (dp != NULL) {
		closedir(dp);
	} else {
		mkdir("save", 0744);
	}
	/* Only need to check once */
	checked_directories = SDL_TRUE;
}

void
save_opts(struct game *cur_game)
{
	char filename[20] = "save/options.txt";
	FILE *fp;
	
	/* First time this is accessed, make sure the directories exist */
	if (checked_directories == SDL_FALSE) check_directories();
	
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("Could not open options.txt\n");
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
	char filename[20] = "save/options.txt";
	FILE *fp;
	
	/* First time this is accessed, make sure the directories exist */
	if (checked_directories == SDL_FALSE) check_directories();
	
	fp = fopen(filename, "r");
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

static void
save_info(struct game *cur_game, struct user *cur_user)
{
	char filename[20] = "save/info.txt";
	FILE *fp;
	time_t timestamp;
 
	time(&timestamp);
	
	fp = fopen(filename, "w");
	/* This may be important later */
	fprintf(fp, "Game saved: %s", ctime(&timestamp));
	fprintf(fp, "Num maps: %d\n", cur_game->num_maps);
	fprintf(fp, "Name: %s\n", cur_user->character->name);
	fclose(fp);
}

void
load_info(struct savefile_info *save)
{
	char filename[20] = "save/info.txt";
	FILE *fp;

	/* First time this is accessed, make sure the directories exist */
	if (checked_directories == SDL_FALSE) check_directories();
	/* Open savefile info file and pull out information */
	fp = fopen(filename, "r");
	if (fp == NULL) {
		save->exists = SDL_FALSE;
	} else {
		save->exists = SDL_TRUE;
		fscanf(fp, "%[^\n]%*c", save->line1);
		fscanf(fp, "%[^\n]%*c", save->line2);
		fscanf(fp, "%[^\n]%*c", save->line3);
		fclose(fp);
	}	
}

void
save_all(struct game *cur_game, struct user *cur_user)
{
	save_info(cur_game, cur_user);
	save_map(cur_game);
	save_player(cur_user);
	save_char(cur_user);
}

void
load_all(struct game *cur_game, struct user *cur_user)
{
	load_map(cur_game);
	load_player(cur_user);
	load_char(cur_user);
	cur_game->state = LOADED;
}

static void
save_map(struct game *cur_game)
{
	char filename[20] = "save/maps.txt";
	int map, i, j;
	FILE *fp;
	
	fp = fopen(filename, "w");
	/* Output dimensions */
	fprintf(fp, "num_maps=%d\n", cur_game->num_maps);
	fprintf(fp, "rows=%d\n", cur_game->maps[0].rows);
	fprintf(fp, "cols=%d\n", cur_game->maps[0].cols);
	/* Output each map */
	for (map = 0; map < cur_game->num_maps; map++) {
		fprintf(fp, "sprite=%d\n", cur_game->maps[map].sprite);
		for (i = 0; i < cur_game->maps[map].rows; i++) {
			for (j = 0; j < cur_game->maps[map].cols; j++) {
				fprintf(fp, "%d", *(*(cur_game->maps[map].tiles + i) + j));
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

static void
load_map(struct game *cur_game)
{
	char filename[20] = "save/maps.txt";
	int map, i, j;
	int rows, cols;
	FILE *fp;
	
	fp = fopen(filename, "r");
	/* Read in dimensions */
	fscanf(fp, "num_maps=%d\n", &cur_game->num_maps);
	fscanf(fp, "rows=%d\n", &rows);
	fscanf(fp, "cols=%d\n", &cols);
	/* Allocate memory */
	cur_game->maps = malloc(sizeof(*cur_game->maps) * cur_game->num_maps);
	/* Input each map */
	for (map = 0; map < cur_game->num_maps; map++) {
		init_map(&cur_game->maps[map], rows, cols);
		fscanf(fp, "sprite=%d\n", &cur_game->maps[map].sprite);
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				*(*(cur_game->maps[map].tiles + i) + j) = fgetc(fp) - 48;
			}
		}
		fscanf(fp, "\n", NULL);
	}
	fclose(fp);
}

static void
save_player(struct user *cur_user)
{
	char filename[20] = "save/user.txt";
	int map, i, j;
	FILE *fp;
	
	fp = fopen(filename, "w");
	/* Output current location */
	fprintf(fp, "map=%d\n", cur_user->map);
	fprintf(fp, "row=%d\n", cur_user->row);
	fprintf(fp, "col=%d\n", cur_user->col);
	fprintf(fp, "facing=%d\n", cur_user->facing);
	/* Output seen dimensions */
	fprintf(fp, "num_seen=%d\n", cur_user->num_seen);
	fprintf(fp, "seen_rows=%d\n", cur_user->seen[0].rows);
	fprintf(fp, "seen_cols=%d\n", cur_user->seen[0].cols);
	/* Output all seen data */
	for (map = 0; map < cur_user->num_seen; map++) {
		for (i = 0; i < cur_user->seen[0].rows; i++) {
			for (j = 0; j < cur_user->seen[0].cols; j++) {
				fprintf(fp, "%d", *(*(cur_user->seen[map].tiles + i) + j));
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

static void
load_player(struct user *cur_user)
{
	char filename[20] = "save/user.txt";
	int map, i, j;
	int rows, cols;
	FILE *fp;
	
	fp = fopen(filename, "r");
	/* Input player location */
	fscanf(fp, "map=%d\n", &cur_user->map);
	fscanf(fp, "row=%d\n", &cur_user->row);
	fscanf(fp, "col=%d\n", &cur_user->col);
	fscanf(fp, "facing=%d\n", &cur_user->facing);
	/* Input seen dimensions */
	fscanf(fp, "num_seen=%d\n", &cur_user->num_seen);
	fscanf(fp, "seen_rows=%d\n", &rows);
	fscanf(fp, "seen_cols=%d\n", &cols);
	/* Allocate memory */
	cur_user->seen = malloc(sizeof(*cur_user->seen) * cur_user->num_seen);
	/* Input all seen data */
	for (map = 0; map < cur_user->num_seen; map++) {
		init_seen(&cur_user->seen[map], rows, cols);
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				*(*(cur_user->seen[map].tiles + i) + j) = fgetc(fp) - 48;
			}
		}
		fscanf(fp, "\n", NULL);
	}
	fclose(fp);
}

static void
save_char(struct user *cur_user)
{
	char filename[20] = "save/char.txt";
	FILE *fp;
	
	fp = fopen(filename, "w");
	/* Output character parameters */
	fprintf(fp, "name=%s\n", cur_user->character->name);
	fprintf(fp, "level=%d\n", cur_user->character->level);
	fprintf(fp, "money=%d\n", cur_user->character->money);
	fprintf(fp, "major_points=%d\n", cur_user->character->major_points);
	fprintf(fp, "minor_points=%d\n", cur_user->character->minor_points);
	/* Stats */
	fprintf(fp, "stats_0=%d,%d\n", cur_user->character->cur_stats.life, cur_user->character->max_stats.life);
	fprintf(fp, "stats_1=%d,%d\n", cur_user->character->cur_stats.stamina, cur_user->character->max_stats.stamina);
	fprintf(fp, "stats_2=%d,%d\n", cur_user->character->cur_stats.magic, cur_user->character->max_stats.magic);
	fprintf(fp, "stats_3=%d,%d\n", cur_user->character->cur_stats.experience, cur_user->character->max_stats.experience);
	fprintf(fp, "stats_4=%d,%d\n", cur_user->character->cur_stats.attack, cur_user->character->max_stats.attack);
	fprintf(fp, "stats_5=%d,%d\n", cur_user->character->cur_stats.defense, cur_user->character->max_stats.defense);
	fprintf(fp, "stats_6=%d,%d\n", cur_user->character->cur_stats.dodge, cur_user->character->max_stats.dodge);
	fprintf(fp, "stats_7=%d,%d\n", cur_user->character->cur_stats.power, cur_user->character->max_stats.power);
	fprintf(fp, "stats_8=%d,%d\n", cur_user->character->cur_stats.spirit, cur_user->character->max_stats.spirit);
	fprintf(fp, "stats_9=%d,%d\n", cur_user->character->cur_stats.avoid, cur_user->character->max_stats.avoid);	
	fprintf(fp, "gear=%d,%d,%d\n", cur_user->character->gear[0], cur_user->character->gear[1], cur_user->character->gear[2]);
	fprintf(fp, "skills=%d,%d,%d\n", cur_user->character->skills[0], cur_user->character->skills[1], cur_user->character->skills[2]);
	fprintf(fp, "inventory=%d,%d,%d,%d,%d,%d,%d,%d\n", cur_user->character->inventory[0], cur_user->character->inventory[1], cur_user->character->inventory[2], cur_user->character->inventory[3], cur_user->character->inventory[4], cur_user->character->inventory[5], cur_user->character->inventory[6], cur_user->character->inventory[7]);
	fclose(fp);
}

static void
load_char(struct user *cur_user)
{
	char filename[20] = "save/char.txt";
	FILE *fp;
	
	/* Allocate memory */
	cur_user->character = malloc(sizeof(*cur_user->character));
	cur_user->character->name = malloc(sizeof(*cur_user->character->name)*17);
	/* Open and load character parameters */
	fp = fopen(filename, "r");
	fscanf(fp, "name=%[^\n]%*c", cur_user->character->name);
	fscanf(fp, "level=%d\n", &cur_user->character->level);
	fscanf(fp, "money=%d\n", &cur_user->character->money);
	fscanf(fp, "major_points=%d\n", &cur_user->character->major_points);
	fscanf(fp, "minor_points=%d\n", &cur_user->character->minor_points);
	/* Stats */
	fscanf(fp, "stats_0=%d,%d\n", &cur_user->character->cur_stats.life, &cur_user->character->max_stats.life);
	fscanf(fp, "stats_1=%d,%d\n", &cur_user->character->cur_stats.stamina, &cur_user->character->max_stats.stamina);
	fscanf(fp, "stats_2=%d,%d\n", &cur_user->character->cur_stats.magic, &cur_user->character->max_stats.magic);
	fscanf(fp, "stats_3=%d,%d\n", &cur_user->character->cur_stats.experience, &cur_user->character->max_stats.experience);
	fscanf(fp, "stats_4=%d,%d\n", &cur_user->character->cur_stats.attack, &cur_user->character->max_stats.attack);
	fscanf(fp, "stats_5=%d,%d\n", &cur_user->character->cur_stats.defense, &cur_user->character->max_stats.defense);
	fscanf(fp, "stats_6=%d,%d\n", &cur_user->character->cur_stats.dodge, &cur_user->character->max_stats.dodge);
	fscanf(fp, "stats_7=%d,%d\n", &cur_user->character->cur_stats.power, &cur_user->character->max_stats.power);
	fscanf(fp, "stats_8=%d,%d\n", &cur_user->character->cur_stats.spirit, &cur_user->character->max_stats.spirit);
	fscanf(fp, "stats_9=%d,%d\n", &cur_user->character->cur_stats.avoid, &cur_user->character->max_stats.avoid);
	fscanf(fp, "gear=%d,%d,%d\n", &cur_user->character->gear[0], &cur_user->character->gear[1], &cur_user->character->gear[2]);
	fscanf(fp, "skills=%d,%d,%d\n", &cur_user->character->skills[0], &cur_user->character->skills[1], &cur_user->character->skills[2]);
	fscanf(fp, "inventory=%d,%d,%d,%d,%d,%d,%d,%d\n", &cur_user->character->inventory[0], &cur_user->character->inventory[1], &cur_user->character->inventory[2], &cur_user->character->inventory[3], &cur_user->character->inventory[4], &cur_user->character->inventory[5], &cur_user->character->inventory[6], &cur_user->character->inventory[7]);
	fclose(fp);
}
