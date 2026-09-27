#ifndef STATE_OPTIONS_H
#define STATE_OPTIONS_H


#include "gamestate.h"

void options_load(void);
void state_options_init(void);
void state_options_enter(void);
void state_options_update(double dt);
void state_options_run(void);
void state_options_handle_events(GLFWwindow* window);
void state_options_esc(void);
void state_options_shutdown(void);
void set_prev_state_options(GameState prev_state_give);

#endif
