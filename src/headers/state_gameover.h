#ifndef STATE_GAMEOVER_H
#define STATE_GAMEOVER_H



void gameover_load(void);
void state_gameover_init(void);
void state_gameover_enter(void);
void state_gameover_update(double dt);
void state_gameover_run(void);
void state_gameover_handle_events(GLFWwindow* window);
void state_gameover_esc(void);
void state_gameover_shutdown(void);

#endif
