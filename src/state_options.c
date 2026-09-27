#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "state_options.h"
#include "renderer.h"
#include "input.h"
#include "paths.h"
#include "game_fun.h"
#include "config.h"
#include "state_play.h"
#include "state_statistics.h"

static double game_time;
static GameState prev_state;

static float frame_anim_time;
static vec4 color_frame;
static vec4 color_frame_border;
static float border_radius_frame;
static float border_width_frame;
static float coord_frame[2];
static float dim_frame[2];

static void draw_frame(void) {

	float revealW, revealH;

    if (frame_anim_time < FRAME_ANIM_PHASE1_DURATION) {
        float t = frame_anim_time / FRAME_ANIM_PHASE1_DURATION;
        float eased = EaseOutQuad(t);
        revealW = Lerp(0.0f, dim_frame[0], eased);
        revealH = FRAME_ANIM_LINE_THICKNESS;
    } else {
        revealW = dim_frame[0];
        float t2 = (frame_anim_time - FRAME_ANIM_PHASE1_DURATION) / FRAME_ANIM_PHASE2_DURATION;
        if (t2 > 1.0f) t2 = 1.0f;
        float eased2 = EaseOutQuad(t2);
        revealH = Lerp(FRAME_ANIM_LINE_THICKNESS, dim_frame[1], eased2);
    }

    if (revealW < 1.0f) revealW = 1.0f;
    if (revealH < 1.0f) revealH = 1.0f;

    float centerX = coord_frame[0] + dim_frame[0] / 2.0f;
    float centerY = coord_frame[1] + dim_frame[1] / 2.0f;
    float revealX = centerX - revealW / 2.0f;
    float revealY = centerY - revealH / 2.0f;

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)revealX, (int)((float)WIN_H - (revealY + revealH)), (int)revealW, (int)revealH);

	DrawRoundedRect(revealX,(int)((float)WIN_H - (revealY + revealH)),revealW,revealH,color_frame,border_radius_frame,color_frame_border, border_width_frame);

}


void options_load(void) {
	fill_color(color_frame_border, 20.0f, 20.0f, 20.0f, 1.0f);
	fill_color(color_frame, 20.0f, 20.0f, 20.0f, 0.8f);


	border_radius_frame = 10.0f;
	border_width_frame = 5.0f;

	frame_anim_time = 0.0f;


	coord_frame[0] = coord_frame[1] = 73.0f;
	dim_frame[0] = dim_frame[1] = (float)WIN_W-2.0f*73.0f;
}

void state_options_init(void) {


}

void state_options_enter(void) {
	game_time = 0;
	frame_anim_time = 0.0f;

}

void state_options_update(double dt) {
	game_time += dt;
	frame_anim_time += (float)dt;
}

void state_options_run(void) {

	draw_prev_state(prev_state);
	draw_frame();

    glDisable(GL_SCISSOR_TEST);
}


void state_options_handle_events(GLFWwindow* window) {


	bool ctrlHeld = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;

    if (ctrlHeld && Input_KeyPressed(window, GLFW_KEY_Q)) {
		state_options_shutdown();
        glfwSetWindowShouldClose(window, 1);
	}
	else if (Input_KeyPressed(window, GLFW_KEY_ESCAPE)) {
		GameState newstate = prev_state;
		Game_SetState(newstate);
	}
	else if (Input_KeyPressed(window, GLFW_KEY_R)) {
		GameState newstate = STATE_PLAY;
		Game_Init();
		Game_SetState(newstate);
	}
}

void set_prev_state_options(GameState prev_state_give) {
	prev_state = prev_state_give;
	switch (prev_state) {
    case STATE_PLAY:
        break;
    case STATE_PAUSED:
        break;
    case STATE_GAMEOVER:
        break;
	case STATE_STATISTICS:
		break;
	case STATE_OPTIONS:
		break;
    }
}

void state_options_esc(void) {

}

void state_options_shutdown(void) {

}
