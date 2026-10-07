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

static float frame_anim_time;
static vec4 color_frame;
static vec4 color_frame_border;
static float border_radius_frame;
static float border_width_frame;
static float coord_frame[2];
static float dim_frame[2];

static vec4 null_color;
static float paused_icon_dim;

static vec4 shadow_color;
static vec4 text_color;
static vec4 text_border_color;
static float text_border_width_title;
static float shadow_offset;

static void draw_frame(void) {

	float revealW, revealH;

    if (frame_anim_time < FRAME_ANIM_PHASE1_DURATION_OPTIONS) {
        float t = frame_anim_time / FRAME_ANIM_PHASE1_DURATION_OPTIONS;
        float eased = EaseOutQuad(t);
        revealW = Lerp(0.0f, dim_frame[0], eased);
        revealH = FRAME_ANIM_LINE_THICKNESS;
    } else {
        revealW = dim_frame[0];
        float t2 = (frame_anim_time - FRAME_ANIM_PHASE1_DURATION_OPTIONS) / FRAME_ANIM_PHASE2_DURATION_OPTIONS;
        if (t2 > 1.0f) t2 = 1.0f;
        float eased2 = EaseOutQuad(t2);
        revealH = Lerp(FRAME_ANIM_LINE_THICKNESS_OPTIONS, dim_frame[1], eased2);
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

static void draw_paused_icon(void) {
	float padding = 10.0f;
	DrawRoundedRect(coord_frame[0] + dim_frame[0] - paused_icon_dim - padding + shadow_offset, coord_frame[1] + padding + shadow_offset, paused_icon_dim/4.0f,paused_icon_dim,shadow_color,10.0f, null_color, text_border_width_title);
	DrawRoundedRect(coord_frame[0] + dim_frame[0] - paused_icon_dim - padding + paused_icon_dim/2.0f + shadow_offset, coord_frame[1] + padding + shadow_offset,paused_icon_dim/4.0f,paused_icon_dim,shadow_color,10.0f, null_color, text_border_width_title);

	DrawRoundedRect(coord_frame[0] + dim_frame[0] - paused_icon_dim - padding, coord_frame[1] + padding, paused_icon_dim/4.0f ,paused_icon_dim,text_color, 10.0f, text_border_color, text_border_width_title);
	DrawRoundedRect(coord_frame[0] + dim_frame[0] - paused_icon_dim - padding + paused_icon_dim/2.0f, coord_frame[1] + padding,paused_icon_dim/4.0f,paused_icon_dim,text_color, 10.0f, text_border_color, text_border_width_title);
}

static void draw_title(void) {
	char text[32];
	snprintf(text,sizeof(text), "SETTINGS");
	DrawText(text, coord_frame[0] + dim_frame[0]/2.0f - 135.0f + shadow_offset, coord_frame[1] + 60.0f + shadow_offset,1.2f, shadow_color, null_color, text_border_width_title);
	DrawText(text, coord_frame[0] + dim_frame[0]/2.0f - 135.0f, coord_frame[1] + 60.0f,1.2f, text_color, text_border_color, text_border_width_title);
}

void options_load(void) {
	fill_color(color_frame_border, 255.0f, 146.0f, 0.0f, 1.0f);
	fill_color(color_frame, 20.0f, 20.0f, 20.0f, 0.97f);
	fill_color(null_color, 0.0f, 0.0f, 0.0f, 0.0f);
	fill_color(text_color, 255.0f, 146.0f, 0.0f, 1.0f);
	fill_color(text_border_color, 0.1f, 0.1f, 0.1f, 1.0f);
	fill_color(shadow_color, 255.0f, 255.0f, 255.0f, 0.5f);

	text_border_width_title = 3.0f;
	shadow_offset = 5.0f;

	border_radius_frame = 10.0f;
	border_width_frame = 3.0f;
	paused_icon_dim = 60.0f;

	frame_anim_time = 0.0f;


	coord_frame[0] = coord_frame[1] = 12.0f;
	dim_frame[0] = dim_frame[1] = (float)WIN_W-2.0f*12.0f;
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

	draw_prev_state();
	draw_frame();
	draw_paused_icon();
	draw_title();

    glDisable(GL_SCISSOR_TEST);
}


void state_options_handle_events(GLFWwindow* window) {


	bool ctrlHeld = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;

    if (ctrlHeld && Input_KeyPressed(window, GLFW_KEY_Q)) {
		state_options_shutdown();
        glfwSetWindowShouldClose(window, 1);
	}
	else if (Input_KeyPressed(window, GLFW_KEY_ESCAPE) || Input_KeyPressed(window, GLFW_KEY_Q)) {
		Game_SetState(return_pop_prev_state());
	}
	else if (Input_KeyPressed(window, GLFW_KEY_R)) {
		GameState newstate = STATE_PLAY;
		Game_Init();
		Game_SetState(newstate);
	}
}

void state_options_esc(void) {

}

void state_options_shutdown(void) {

}
