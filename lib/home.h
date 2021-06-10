#ifndef HOME_H
#define HOME_H

#include "main.h"
#include "user.h"

extern void		title(struct game *cur_game, struct user *cur_user);
extern SDL_bool		yes_no(struct game *cur_game, char *message, SDL_bool view, SDL_bool char_sheet);

enum game_states { UNLOADED, LOADED };
enum results { RESULTS_YES, RESULTS_NO, RESULTS_NULL };

#endif
