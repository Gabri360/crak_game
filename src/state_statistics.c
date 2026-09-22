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


static float frame_anim_time;
static vec4 color_frame;
static vec4 color_frame_border;
static float border_radius_frame;
static float border_width_frame;

static vec4 black;
static vec4 blank;
static vec4 blank2;
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
static vec4 color_leg;

#define max_history_load 9000
static ScoreEntry history[max_history_load];
static size_t count_history_load;

static int histogram_time_data[20];
static int max_time_data;
static int histogram_score_data[300];
static int record;
static int max_score_data;


static float mean_score;
static float stdev_score;
static float mean_time;
static float stdev_time;
static float leg_padding;
static float dim_leg[2];
static float pos_leg[2];

static float bar_pos;
static vec4 color_bar;
static vec4 color_bar_bg;
static float bar_padding;
static float dim_bar[2];

static float barPosX;
static float barStartX;
static float barEndX;
static float bar_moveElapsed;
static int isMoving;

static float pos_schede;
static float pos_schedeStartX;
static float pos_schedeEndX;

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

static void load_histogram(void) {

	for(int k=0;k<20;k++) {
		histogram_time_data[k] = 0;
	}
	for(int p=1;p<record+1;p++) {
		histogram_score_data[p] = 0;
	}

	for(int i=0;i<(int)count_history_load;i++) {
		if(history[i].game_time != 20)
			histogram_time_data[(int)history[i].game_time] += 1;
		else
			histogram_time_data[19] += 1;

		histogram_score_data[history[i].score] += 1;
	}
	max_time_data = histogram_time_data[0];
	for(int j=1;j<20;j++) {
		if(histogram_time_data[j]>max_time_data)
			max_time_data = histogram_time_data[j];
	}

	max_score_data = histogram_score_data[1];
	for(int q=2;q<record+1;q++) {
		if(histogram_score_data[q]>max_score_data)
			max_score_data = histogram_score_data[q];
	}

}

static void analisis(void) {
	mean_time = 0.0f;
	stdev_time = 0.0f;
	mean_score = 0.0f;
	stdev_score = 0.0f;
	for(int i=0;i<(int)count_history_load;i++) {
		mean_time += (float)history[i].game_time;
		mean_score += (float)history[i].score;
	}
	mean_score /= (float)count_history_load;
	mean_time /= (float)count_history_load;

	for(int j=0;j<(int)count_history_load;j++) {
		stdev_time += ((float)history[j].game_time - mean_time) * ((float)history[j].game_time - mean_time);
		stdev_score += ((float)history[j].score - mean_time) * ((float)history[j].score - mean_time);
	}
	stdev_score = sqrtf(stdev_score/(float)count_history_load);
	stdev_time = sqrtf(stdev_time/(float)count_history_load);
}

static void draw_score_data(void) {


	float padding = 60.0f;
	char text[32];
	float pos_tar;
	float spacing = (dim_graph[0] - 2.0f*padding - 5.0f) / (((float)(record+2))/2.0f);
	float unitH = (dim_graph[1]- 20.0f - 2.0f*padding)/(float)max_score_data;
	for(int i=0;i<record+2;i=i+2) {
		pos_tar = (float)i*spacing/2.0f;
		DrawRoundedRect(coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding-(float)histogram_score_data[i]*unitH -(float)histogram_score_data[i+1]*unitH+ dim_frame[1], spacing, (float)histogram_score_data[i]*unitH + (float)histogram_score_data[i+1]*unitH, color_histo_data, 2.0f, color_histo_data_border, 2.0f);

		if (histogram_score_data[i] != 0) {
		snprintf(text, sizeof(text), "%d", histogram_score_data[i] + histogram_score_data[i+1]);
		DrawText(text, coord_graph[0] + padding + pos_tar + 7.0f, coord_graph[1] + dim_graph[1] - padding-(float)histogram_score_data[i]*unitH - (float)histogram_score_data[i+1]*unitH - 5.0f + dim_frame[1], 0.18f, blank,color_histo_data, text_border_width);
		}
	}

	DrawRoundedRect(pos_leg[0], pos_leg[1] + dim_frame[1], dim_leg[0], dim_leg[1], color_leg, 5.0f, color_graf_border, 2.0f);

	snprintf(text, sizeof(text), "Mean  =  %.2f", mean_score);
	DrawText(text, pos_leg[0] + 20.0f, pos_leg[1] + 30.0f + dim_frame[1], 0.3f, blank, black, 2.0f);
	snprintf(text, sizeof(text), "Stderr =  %.2f", stdev_score);
	DrawText(text, pos_leg[0] + 20.0f, pos_leg[1] + 60.0f + dim_frame[1], 0.3f, blank, black, 2.0f);
}

static void draw_time_data(void) {

	float padding = 60.0f;
	char text[32];
	float pos_tar;
	float spacing = (dim_graph[0] - 2.0f*padding) / 21.0f;
	float unitH = (dim_graph[1]- 20.0f - 2.0f*padding)/(float)max_time_data;
	for(int i=0;i<20;i++) {
		pos_tar = (float)i*spacing;
		DrawRoundedRect(coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding-(float)histogram_time_data[i]*unitH, spacing, (float)histogram_time_data[i]*unitH, color_histo_data, 2.0f, color_histo_data_border, 2.0f);

		if (histogram_time_data[i] != 0) {
		snprintf(text, sizeof(text), "%d", histogram_time_data[i]);
		DrawText(text, coord_graph[0] + padding + pos_tar + 10.0f, coord_graph[1] + dim_graph[1] - padding-(float)histogram_time_data[i]*unitH - 5.0f, 0.3f, blank,color_histo_data, text_border_width);
		}
	}

	DrawRoundedRect(pos_leg[0], pos_leg[1], dim_leg[0], dim_leg[1], color_leg, 5.0f, color_graf_border, 2.0f);

	snprintf(text, sizeof(text), "Mean  =  %.2f", mean_time);
	DrawText(text, pos_leg[0] + 20.0f, pos_leg[1] + 30.0f, 0.3f, blank, black, 2.0f);
	snprintf(text, sizeof(text), "Stderr =  %.2f", stdev_time);
	DrawText(text, pos_leg[0] + 20.0f, pos_leg[1] + 60.0f, 0.3f, blank, black, 2.0f);
}

static void draw_axis(float pos, char t) {
	float padding = 60.0f;
	char text[32];
	DrawLine(coord_graph[0] + padding, coord_graph[1] + padding + pos, coord_graph[0] + padding, coord_graph[1] + dim_graph[1] - padding + 10.0f + pos, 4.0f, color_axis);
	DrawLine(coord_graph[0] + padding - 10.0f, coord_graph[1] + dim_graph[1] - padding + pos, coord_graph[0] + dim_graph[0] - padding, coord_graph[1] + dim_graph[1] - padding + pos, 4.0f, color_axis);
	DrawETriangle(coord_graph[0] + padding - 4.0f, coord_graph[1] + padding + pos, 8.0f, 2.0f, color_axis, 'u');
	DrawETriangle(coord_graph[0] + dim_graph[0] - padding, coord_graph[1] + dim_graph[1] - padding - 4.0f + pos, 8.0f, 2.0f, color_axis, 'r');
	float pos_tar;



	if (t == 't') {

		float spacing = (dim_graph[0] - 2.0f*padding) / 21.0f;
		for(int i=1;i<21;i++) {
			pos_tar = (float)i * spacing;
			DrawLine(coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding - 5.0f + pos, coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding + 5.0f + pos, 4.0f, color_axis);

			snprintf(text, sizeof(text), "%d", i);
			DrawText(text, coord_graph[0] + padding + pos_tar, coord_graph[1] + dim_graph[1] - padding + 25.0f + pos, 0.3f, text_color, text_border_color, text_border_width);
		}
	} else if (t == 's') {

		float spacing = (dim_graph[0] - 2.0f*padding -5.0f) / (((float)(record+2))/2.0f);
		for(int i=1;i<((record+2)/2);i++) {
			pos_tar = (float)i * spacing;
			DrawLine(coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding - 5.0f + pos, coord_graph[0] + padding + pos_tar + 5.0f, coord_graph[1] + dim_graph[1] - padding + 5.0f + pos, 4.0f, color_axis);

			if(i%2) {
				snprintf(text, sizeof(text), "%d", i*2);
				DrawText(text, coord_graph[0] + padding + pos_tar, coord_graph[1] + dim_graph[1] - padding + 25.0f + pos, 0.2f, text_color, text_border_color, text_border_width);
			}
		}
	}


	snprintf(text, sizeof(text), "Count");
	DrawText(text, coord_graph[0] + 35.0f, coord_graph[1] + padding - 20.0f + pos, 0.3f, text_color, text_border_color, text_border_width);

	if (t == 't') {
		snprintf(text, sizeof(text), "Time");
	}
	else if(t == 's') {

		snprintf(text, sizeof(text), "Score");
	}
	DrawText(text,coord_graph[0] + dim_graph[0] - padding + 5.0f , coord_graph[1] + dim_graph[1] - padding + 25.0f + pos, 0.3f, text_color, text_border_color, text_border_width);

}


static void draw_histo_time(void) {
	DrawRoundedRect(coord_frame[0] + 50.0f, coord_frame[1] + 50.0f + pos_schede*dim_frame[1],dim_graph[0],dim_graph[1], null_color, 5.0f, color_graf_border, 5.0f);
	draw_axis(0.0f, 't');
	draw_time_data();
}

static void draw_histo_score(void) {
	DrawRoundedRect(coord_frame[0] + 50.0f, coord_frame[1] + 50.0f + dim_frame[1] + pos_schede*dim_frame[1],dim_graph[0],dim_graph[1], null_color, 5.0f, color_graf_border, 5.0f);
	draw_axis(dim_frame[1], 's');
	draw_score_data();
}


static void draw_bar(void) {
	DrawRoundedRect(coord_frame[0] + dim_frame[0] - 40.0f, coord_frame[1] + bar_padding, dim_bar[0], dim_bar[1], color_bar_bg, 5.0f, black, 2.0f);
	DrawRoundedRect(coord_frame[0] + dim_frame[0] - 40.0f, coord_frame[1] + bar_padding + barPosX*dim_bar[1] / 2.0f, dim_bar[0], dim_bar[1] / 2.0f, color_bar, 5.0f, blank2, 2.0f);
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
	fill_color(blank, 255.0f, 255.0f, 255.0f, 1.0f);
	fill_color(blank2, 255.0f, 255.0f, 255.0f, 0.8f);
	fill_color(color_leg, 20.0f, 20.0f, 20.0f, 0.9f);
	fill_color(black, 0.0f, 0.0f, 0.0f, 1.0f);
	fill_color(color_bar, 255.0f, 145.0f, 0.1f, 0.5f);
	fill_color(color_bar_bg, 20.0f, 20.0f, 20.0f, 0.9f);





	text_border_width = 3.5f;
	border_radius_frame = 10.0f;
	border_width_frame = 5.0f;

	frame_anim_time = 0.0f;
	game_time = 0.0f;

	coord_frame[0] = coord_frame[1] = 73.0f;
	dim_frame[0] = dim_frame[1] = (float)WIN_W-2.0f*73.0f;

	pos_schede = 0.0f;

	coord_graph[0] =coord_frame[0] + 50.0f;
	coord_graph[1] = coord_frame[0] + 50.0f + pos_schede;
	dim_graph[0] = dim_graph[1] = dim_frame[0]-2.0f*(50.0f);



	bar_pos = 0.0f;
	barPosX = bar_pos;
	bar_padding = 200.0f;
	dim_bar[0] = 27.0f;
	dim_bar[1] = dim_frame[1] - 2*bar_padding;

	isMoving = 0;
	leg_padding = 20.0f;
	dim_leg[0] = 150.0f;
	dim_leg[1] = 80.0f;
	pos_leg[0] = coord_frame[0] + dim_graph[0] - dim_leg[0] + 50.0f - leg_padding;
	pos_leg[1] = coord_frame[1] + 50.0f + leg_padding + pos_schede;


	count_history_load = History_LoadAll(history, max_history_load);

	record = max_score(history, max_history_load);
	load_histogram();
	analisis();
}

void statistics_init(void) {

}

void statistics_enter(void) {
	game_time = 0.0f;
	frame_anim_time = 0.0f;

	count_history_load = History_LoadAll(history, max_history_load);

	record = max_score(history, max_history_load);
	load_histogram();
	analisis();
}

void statistics_update(double dt) {
	game_time += dt;
	frame_anim_time += (float)dt;

	state_play_update(dt);

	if (isMoving) {
		bar_moveElapsed += (float)dt;
		float t = bar_moveElapsed / (float)STATISTICS_MOVE_DURATION;
		if (t >= 1.0f) {
			t = 1.0f;
			isMoving = 0;
		}
		barPosX = Lerp(barStartX, barEndX, EaseOutBack(t));
		pos_schede = Lerp(pos_schedeStartX, pos_schedeEndX, EaseOutBack(t));

		coord_graph[1] = coord_frame[0] + 50.0f + pos_schede *  dim_frame[1];
		pos_leg[1] = coord_frame[1] + 50.0f + leg_padding + pos_schede *  dim_frame[1];
	}

}

void statistics_run(void) {

	state_play_run();
	draw_frame();

	draw_histo_time();

	draw_histo_score();

	draw_bar();
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
	if (Input_KeyPressed(window, GLFW_KEY_DOWN) && bar_pos == 0.0f) {
		barStartX = bar_pos;
		bar_pos = 1.0f;
		barEndX = bar_pos;
		bar_moveElapsed = 0.0f;
		isMoving = 1;
		pos_schedeStartX = pos_schede;
		pos_schede = -1.0f;
		pos_schedeEndX = pos_schede;
	}
	if (Input_KeyPressed(window, GLFW_KEY_UP) && bar_pos == 1.0f) {
		barStartX = bar_pos;
		bar_pos = 0.0f;
		barEndX = bar_pos;
		bar_moveElapsed = 0.0f;
		isMoving = 1;
		pos_schedeStartX = pos_schede;
		pos_schede = 0.0f;
		pos_schedeEndX = pos_schede;
	}
}

void statistics_esc(void) {

}

void statistics_shutdown(void) {

}
