#ifndef GAMESTATE_H
#define GAMESTATE_H

typedef enum {
    STATE_PLAY,
    STATE_PAUSED,
    STATE_GAMEOVER
} GameState;

void Game_Init(void);
void Game_state_enter(void);
void Game_Update(double dt);
void Game_Run(void);
void Game_handle_events(GLFWwindow* window);
void Game_Shutdown(void);
void Game_state_esc(void);
void Game_SetState(GameState newState);
#endif
