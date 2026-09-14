#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gamestate.h"
#include "state_paused.h"
#include "renderer.h"
#include "input.h"
#include "paths.h"
#include "game_fun.h"
#include "config.h"

static double time;
static vec4 icon_color;
static float rect_w;
static float rect_h;
static float border_radius_icon;

void state_paused_init(void) {
	fill_color(icon_color, 255.0f, 148.0f, 0.1f, 1.0f);
	border_radius_icon = 20.0f;
	rect_w = (float)WIN_W/18.0f;
	rect_h = (float)WIN_H/3.0f;
}

void state_paused_enter(void) {
	time = 0;
}

void state_paused_run(double dt) {

	DrawRoundedRect(((float)WIN_W/3.0f-rect_w/2.0f)+(float)WIN_W/12.0f,(float)WIN_H/3.0f,rect_w,rect_h,icon_color,border_radius_icon);	DrawRoundedRect(((float)WIN_W*2.0f/3.0f-rect_w/2.0f)-(float)WIN_W/12.0f,(float)WIN_H/3.0f,rect_w,rect_h,icon_color,border_radius_icon);
	time += dt;
}


void state_paused_handle_events(GLFWwindow* window) {

    if (Input_KeyPressed(window, GLFW_KEY_ESCAPE) || Input_KeyPressed(window, GLFW_KEY_Q)) {
		state_paused_shutdown();
        glfwSetWindowShouldClose(window, 1);
	}
	if (Input_KeyPressed(window, GLFW_KEY_SPACE)) {
		GameState newstate = STATE_PLAY;
		Game_SetState(newstate);
	}
}

void state_paused_esc(void) {

}

void state_paused_shutdown(void) {

}
