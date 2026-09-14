#ifndef STATE_PLAY_H
#define STATE_PLAY_H


void state_play_init(void);
void state_play_enter(void);
void state_play_update(double dt);
void state_play_run(void);
void state_play_handle_events(GLFWwindow* window);
void state_play_esc(void);
void state_play_shutdown(void);

#endif
