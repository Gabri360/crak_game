#ifndef STATE_GAMEOVER_H
#define STATE_GAMEOVER_H

#include "gamestate.h"

void gameover_load(void);
void state_gameover_init(void);
void state_gameover_enter(void);
void state_gameover_update(double dt);
void state_gameover_run(void);
void state_gameover_handle_events(GLFWwindow* window);
void set_prev_state_gameover(GameState prev_state_give);
void state_gameover_esc(void);
void state_gameover_shutdown(void);

#endif
