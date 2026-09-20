#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "state_statistics.h"
#include "gamestate.h"
#include "state_play.h"
#include "renderer.h"
#include "input.h"
#include "paths.h"
#include "game_fun.h"
#include "config.h"

static float frame_anim_time;
static vec4 color_frame;
static vec4 color_frame_border;
static float border_radius_frame;
static float border_width_frame;

static double game_time;
static float coord_frame[2];

static void draw_frame(void) {
	float dim_frame[2] = {(float)WIN_W-2.0f*coord_frame[0], (float)WIN_H-2.0f*coord_frame[1]};

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

void statistics_load(void) {
	fill_color(color_frame_border, 20.0f, 20.0f, 20.0f, 1.0f);
	fill_color(color_frame, 20.0f, 20.0f, 20.0f, 0.8f);

	border_radius_frame = 10.0f;
	border_width_frame = 5.0f;

	frame_anim_time = 0.0f;
	game_time = 0.0f;


	coord_frame[0] = coord_frame[1] = 73.0f;
}

void statistics_init(void) {

}

void statistics_enter(void) {
	game_time = 0.0f;
	frame_anim_time = 0.0f;
}

void statistics_update(double dt) {
	game_time += dt;
	frame_anim_time += (float)dt;

	state_play_update(dt);
}

void statistics_run(void) {

	state_play_run();
	draw_frame();


    glDisable(GL_SCISSOR_TEST);
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
