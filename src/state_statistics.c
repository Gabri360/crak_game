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
#include "history.h"
#include <stdio.h>


static float frame_anim_time;
static vec4 color_frame;
static vec4 color_frame_border;
static float border_radius_frame;
static float border_width_frame;

static vec4 null_color;
static double game_time;
static float coord_frame[2];
static float dim_frame[2];

static float coord_graph[2];
static float dim_graph[2];
static vec4 color_graf_border;
static vec4 color_axis;
static vec4 text_color;
static vec4 text_border_color;
static float text_border_width;
static vec4 color_histo_data;
static vec4 color_histo_data_border;

#define max_history_load 9000
static ScoreEntry history[max_history_load];
static size_t count_history_load;

static int histogram_time_data[20];
static int max_time_data;

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

static void load_histogram_time(void) {

	for(int k=0;k<20;k++) {
		histogram_time_data[k]=0;
	}
	for(int i=0;i<(int)count_history_load;i++) {
		if(history[i].game_time != 20)
			histogram_time_data[(int)history[i].game_time] += 1;
		else
			histogram_time_data[19] += 1;
	}
	max_time_data = histogram_time_data[0];
	for(int j=1;j<20;j++) {
		if(histogram_time_data[j]>max_time_data)
			max_time_data = histogram_time_data[j];
	}
}

static void draw_time_data(void) {

	float padding = 60.0f;
	float pos_tar;
	float spacing = (dim_graph[0] - 2.0f*padding) / 21.0f;
	float unitH = (dim_graph[1]- 20.0f - 2.0f*padding)/(float)max_time_data;
	for(int i=0;i<20;i++) {
		pos_tar = (float)i*spacing;
		DrawRoundedRect(coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding-(float)histogram_time_data[i]*unitH, spacing, (float)histogram_time_data[i]*unitH, color_histo_data, 2.0f, color_histo_data_border, 2.0f);
	}

}

static void draw_axis_time(void) {
	float padding = 60.0f;
	char text[32];
	DrawLine(coord_graph[0] + padding, coord_graph[1] + padding, coord_graph[0] + padding, coord_graph[1] + dim_graph[1] - padding + 10.0f, 4.0f, color_axis);
	DrawLine(coord_graph[0] + padding - 10.0f, coord_graph[1] + dim_graph[1] - padding, coord_graph[0] + dim_graph[0] - padding, coord_graph[1] + dim_graph[1] - padding, 4.0f, color_axis);
	DrawETriangle(coord_graph[0] + padding - 4.0f, coord_graph[1] + padding, 8.0f, 2.0f, color_axis, 'u');
	DrawETriangle(coord_graph[0] + dim_graph[0] - padding, coord_graph[1] + dim_graph[1] - padding - 4.0f, 8.0f, 2.0f, color_axis, 'r');
	float pos_tar;
	float spacing = (dim_graph[0] - 2.0f*padding) / 21.0f;
	for(int i=1;i<21;i++) {
		pos_tar = (float)i * spacing;
		DrawLine(coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding - 5.0f, coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding + 5.0f, 4.0f, color_axis);

		snprintf(text, sizeof(text), "%d", i);
		DrawText(text, coord_graph[0] + padding + pos_tar, coord_graph[1] + dim_graph[1] - padding + 25.0f, 0.3f, text_color, text_border_color, text_border_width);
	}
	snprintf(text, sizeof(text), "Count");
	DrawText(text, coord_graph[0] + 35.0f, coord_graph[1] + padding - 20.0f, 0.3f, text_color, text_border_color, text_border_width);
}


static void draw_histo_time(void) {
	DrawRoundedRect(coord_frame[0] + 50.0f, coord_frame[1] + 50.0f,dim_graph[0],dim_graph[1], null_color, 5.0f, color_graf_border, 5.0f);
	draw_axis_time();
	draw_time_data();
}

void statistics_load(void) {
	fill_color(color_frame_border, 20.0f, 20.0f, 20.0f, 1.0f);
	fill_color(color_frame, 20.0f, 20.0f, 20.0f, 0.8f);
	fill_color(null_color, 0.0f, 0.0f, 0.0f, 0.0f);
	fill_color(color_graf_border, 255.0f, 145.0f, 0.1f, 0.5f);
	fill_color(color_axis, 255.0f, 145.0f, 0.1f, 1.0f);
	fill_color(text_border_color, 0.1f, 0.1f, 0.1f, 1.0f);
	fill_color(text_color, 245.0f, 140.0f, 0.1f, 1.0f);
	fill_color(color_histo_data, 0.0f, 0.0f, 255.0f, 1.0f);
	fill_color(color_histo_data_border, 0.0f, 0.0f, 0.0f,1.0f);

	text_border_width = 3.5f;
	border_radius_frame = 10.0f;
	border_width_frame = 5.0f;

	frame_anim_time = 0.0f;
	game_time = 0.0f;

	coord_frame[0] = coord_frame[1] = 73.0f;
	dim_frame[0] = dim_frame[1] = (float)WIN_W-2.0f*coord_frame[0];

	coord_graph[0] = coord_graph[1] = 73.0f + 50.0f;
	dim_graph[0] = dim_graph[1] = dim_frame[0]-2.0f*(coord_graph[0]-coord_frame[0]);

	count_history_load = History_LoadAll(history, max_history_load);

	load_histogram_time();
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

	draw_histo_time();
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
