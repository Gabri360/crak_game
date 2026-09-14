#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "state_gameover.h"
#include "gamestate.h"
#include "state_play.h"
#include "renderer.h"
#include "input.h"
#include "paths.h"
#include "game_fun.h"
#include "config.h"

static GLuint skull;

void state_gameover_init(void) {
    char sprite_Path[512];
    GetResourcePath("assets/pixelart_skull.png", sprite_Path, sizeof(sprite_Path));
	skull = LoadTexture(sprite_Path);
}

void state_gameover_enter(void) {

}

void state_gameover_update(double dt) {
	dt=dt;
}

void state_gameover_run(double dt) {

	dt=dt;

	DrawSprite(skull, 0, 0, WIN_W, WIN_H);
}

void state_gameover_handle_events(GLFWwindow* window) {

    if (Input_KeyPressed(window, GLFW_KEY_ESCAPE) || Input_KeyPressed(window, GLFW_KEY_Q)) {
		Game_Shutdown();
        glfwSetWindowShouldClose(window, 1);
	}
	if (Input_KeyPressed(window, GLFW_KEY_ENTER)) {
		GameState newstate = STATE_PLAY;
		Game_Init();
		Game_SetState(newstate);
	}

}

void state_gameover_esc(void) {

}

void state_gameover_shutdown(void) {

}
