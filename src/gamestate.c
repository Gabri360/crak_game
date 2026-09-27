#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gamestate.h"
#include "state_play.h"
#include "state_paused.h"
#include "state_gameover.h"
#include "state_statistics.h"
#include "state_options.h"

#include <stdio.h>

static GameState currentState = STATE_PLAY;

static GameState state_queue[5];
static int count_queue;

void Game_load(void) {
	play_load_texture_and_constant();
	gameover_load();
	paused_load();
	statistics_load();
	options_load();
}

void Game_Init(void) {
    state_play_init();
    state_paused_init();
    state_gameover_init();
	statistics_init();
	state_options_init();
	count_queue = 0;
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
	case STATE_STATISTICS:
		statistics_enter();
		break;
	case STATE_OPTIONS:
		state_options_enter();
		break;
    }
}

void Game_Update(double dt) {
    switch (currentState) {
    case STATE_PLAY:
        state_play_update(dt);
        break;
    case STATE_PAUSED:
		state_paused_update(dt);
        break;
    case STATE_GAMEOVER:
		state_gameover_update(dt);
        break;
	case STATE_STATISTICS:
		statistics_update(dt);
		break;
	case STATE_OPTIONS:
		state_options_update(dt);
		break;
    }
}

void Game_Run(void) {
    switch (currentState) {
    case STATE_PLAY:
        state_play_run();
        break;
    case STATE_PAUSED:
		state_paused_run();
        break;
    case STATE_GAMEOVER:
		state_gameover_run();
        break;
	case STATE_STATISTICS:
		statistics_run();
		break;
	case STATE_OPTIONS:
		state_options_run();
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
	case STATE_STATISTICS:
		statistics_handle_events(window);
		break;
	case STATE_OPTIONS:
		state_options_handle_events(window);
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
	case STATE_STATISTICS:
		statistics_esc();
		break;
	case STATE_OPTIONS:
		state_options_esc();
		break;
    }
}

void Game_Shutdown(void) {
    state_play_shutdown();
	state_paused_shutdown();
	state_gameover_shutdown();
	statistics_shutdown();
	state_options_shutdown();
}

static void reset_queue(void) {
	for (int i=0;i<count_queue;i++) {
		state_queue[i] = STATE_PLAY;
	}
	count_queue = 0;
}
static void add_queue(GameState newState) {
	int is_in_queue = 0;
	for (int i=0;i<count_queue;i++) {
		if (state_queue[i] == newState) {
			is_in_queue = 1;
		}
		if (is_in_queue && i != count_queue-1) {
			state_queue[i] = state_queue[i+1];
		}
	}
	if (is_in_queue) {
		state_queue[count_queue-1] = newState;
	}
	else {
		state_queue[count_queue] = newState;
		count_queue++;
	}
}

void Game_SetState(GameState newState) {
	if (newState == STATE_PLAY) {
		reset_queue();
	}
	else {
		add_queue(newState);
	}
	Game_state_esc();
    currentState = newState;
	Game_state_enter();
}

void draw_prev_state(void) {
	if (count_queue - 2 < 0) {
		state_play_run();
	}
	else switch (state_queue[count_queue-2]) {
    case STATE_PLAY:
        state_play_run();
        break;
    case STATE_PAUSED:
		state_paused_run();
        break;
    case STATE_GAMEOVER:
		state_gameover_run();
        break;
	case STATE_STATISTICS:
		statistics_run();
		break;
	case STATE_OPTIONS:
		state_options_run();
		break;
    }
}

GameState return_pop_prev_state(void) {
	count_queue--;
	if (count_queue-1 < 0) {
		return STATE_PLAY;
	}
	else {
		return state_queue[count_queue-1];
	}
}

GameState return_prev_state(void) {
	if (count_queue-2 < 0) {
		return STATE_PLAY;
	}
	else {
		return state_queue[count_queue-2];
	}
}
