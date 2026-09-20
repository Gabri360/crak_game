#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "state_statistics.h"
#include "gamestate.h"
#include "input.h"
#include "state_play.h"



void statistics_init(void) {

}

void statistics_enter(void) {

}

void statistics_update(double dt) {

}

void statistics_run(void) {
	state_play_run();
}

void statistics_handle_events(GLFWwindow* window) {
    if (Input_KeyPressed(window, GLFW_KEY_ESCAPE) || Input_KeyPressed(window, GLFW_KEY_Q)) {
		Game_Shutdown();
        glfwSetWindowShouldClose(window, 1);
	}
	if (Input_KeyPressed(window, GLFW_KEY_S)) {
		GameState newstate = STATE_PLAY;
		Game_SetState(newstate);
	}
}

void statistics_esc(void) {

}

void statistics_shutdown(void) {

}
