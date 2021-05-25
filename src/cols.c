#include <string.h>
#include "cols.h"

struct colors {
	int id;
	char *name;
	char RGB[3];
} COLORS[] = {
	{ 0, "black", {0, 0, 0} },
	{ 1, "white", {255, 255, 255} },
	{ 2, "red", { 255, 0, 0 } },
	{ 3, "darkred", { 64, 0, 0 } },
	{ 4, "green", { 0, 255, 0 } },
	{ 5, "darkgreen", { 0, 64, 0 } },
	{ 6, "blue", { 0, 0, 255 } },
	{ 7, "darkblue", {0, 0, 64} },
	{ -1, NULL, {0, 0, 0} }
};
	
char *
get_color(char *col)
{
	short int i;
	int len;
	
	for (i = 0, len = strlen(col); COLORS[i].id != -1; i++) {
		if (strncmp(col, COLORS[i].name, len) == 0) {
			return COLORS[i].RGB;
		}
	}
	/* No such color, return NULL color (black) */
	return COLORS[i].RGB;
}
