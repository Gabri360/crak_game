#ifndef STATE_PAUSED_H
#define STATE_PAUSED_H


void state_paused_init(void);
void state_paused_enter(void);
void state_paused_update(double dt);
void state_paused_run(void);
void state_paused_handle_events(GLFWwindow* window);
void state_paused_esc(void);
void state_paused_shutdown(void);

#endif
