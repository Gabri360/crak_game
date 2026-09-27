#ifndef GAMESTATE_H
#define GAMESTATE_H

typedef enum {
    STATE_PLAY,
    STATE_PAUSED,
    STATE_GAMEOVER,
	STATE_STATISTICS,
	STATE_OPTIONS
} GameState;


void Game_load(void);
void Game_Init(void);
void Game_state_enter(void);
void Game_Update(double dt);
void Game_Run(void);
void Game_handle_events(GLFWwindow* window);
void Game_Shutdown(void);
void Game_state_esc(void);
void Game_SetState(GameState newState);
void draw_prev_state(void);
GameState return_pop_prev_state(void);
GameState return_prev_state(void);

#endif
