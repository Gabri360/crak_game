#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gamestate.h"
#include "state_play.h"
#include "state_paused.h"
#include "state_gameover.h"

GameState currentState = STATE_PLAY;



void Game_Init(void) {
    state_play_init();
    state_paused_init();
    state_gameover_init();
}

void Game_state_enter(void) {
	switch (currentState) {
    case STATE_PLAY:
        state_play_enter();
        break;
    case STATE_PAUSED:
		state_paused_enter();
        break;
    case STATE_GAMEOVER:
		state_gameover_enter();
        break;
    }
}

void Game_Update(double dt) {
    switch (currentState) {
    case STATE_PLAY:
        state_play_update(dt);
        break;
    case STATE_PAUSED:
        break;
    case STATE_GAMEOVER:
		state_gameover_update(dt);
        break;
    }
}

void Game_Run(double dt) {
    switch (currentState) {
    case STATE_PLAY:
        state_play_run(dt);
        break;
    case STATE_PAUSED:
		state_paused_run(dt);
        break;
    case STATE_GAMEOVER:
		state_gameover_run(dt);
        break;
    }
}

void Game_handle_events(GLFWwindow* window) {
	switch (currentState) {
    case STATE_PLAY:
        state_play_handle_events(window);
        break;
    case STATE_PAUSED:
		state_paused_handle_events(window);
        break;
    case STATE_GAMEOVER:
		state_gameover_handle_events(window);
        break;
    }
}

void Game_state_esc(void) {
	switch (currentState) {
    case STATE_PLAY:
        state_play_esc();
        break;
    case STATE_PAUSED:
		state_paused_esc();
        break;
    case STATE_GAMEOVER:
		state_gameover_esc();
        break;
    }
}

void Game_Shutdown(void) {
    state_play_shutdown();
	state_paused_shutdown();
	state_gameover_shutdown();
}


void Game_SetState(GameState newState) {
	Game_state_esc();
    currentState = newState;
	Game_state_enter();
}
