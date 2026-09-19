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

#define max_history_load 3000
static ScoreEntry history[max_history_load];
static size_t count_history_load;
static int records[5];
static int n_records;

static vec4 color_leaderboard_border;
static vec4 color_leaderboard;
static vec4 color_line;
static vec4 color_line2;
static float line_width;
static float border_radius_lb;
static float border_width_lb;

static vec4 text_color;
static vec4 text_border_color;
static float text_border_width;

static void draw_leaderboard() {

	float coord_lb[2] = {(float)(WIN_W/6), (float)(WIN_H/6)};
	float dim_lb[2] = {(float)WIN_W-2.0f*coord_lb[0], (float)WIN_H-2.0f*coord_lb[1]};
	DrawRoundedRect((coord_lb[0]),(coord_lb[1]),dim_lb[0],dim_lb[1],color_leaderboard,border_radius_lb,color_leaderboard_border, border_width_lb);

	char text[32];

	snprintf(text, sizeof(text), "Leaderboard");
	DrawText(text, coord_lb[0]+dim_lb[0]/2.0f-150.0f,coord_lb[1]+70.0f,1.0f, text_color,text_border_color,text_border_width);

	DrawLine(coord_lb[0]+4.0f,coord_lb[1]+95.0f,coord_lb[0]+dim_lb[0]-4.0f, coord_lb[1]+95.0f,line_width,color_line);

	float text_scale = 1.0f;
	float offset = 0.0f;

	for(int i=0;i<n_records;i++) {
		snprintf(text, sizeof(text), "%d)        %d", i+1, records[i]);
		DrawText(text, (coord_lb[0]+130.0f+offset),(coord_lb[1]+150.0f+(float)(i*50)), text_scale, text_color, text_border_color, text_border_width);
		text_scale = 0.7f;
		offset = 50.0f;
	}
	float lineH;
	for(int i=0;i<n_records;i++) {
		lineH = 95.0f + (float)(i)*49.0f + 73.0f;
		if (i<n_records-1) {
			DrawLine(coord_lb[0]+4.0f,coord_lb[1]+lineH,coord_lb[0]+dim_lb[0]-4.0f, coord_lb[1]+lineH,line_width,color_line2);
		}
		else {
			DrawLine(coord_lb[0]+4.0f,coord_lb[1]+lineH,coord_lb[0]+dim_lb[0]-4.0f, coord_lb[1]+lineH,line_width,color_line);
		}
	}
}


void state_gameover_init(void) {
	char sprite_Path[512];
	GetResourcePath("assets/pixelart_skull.png", sprite_Path, sizeof(sprite_Path));
	skull = LoadTexture(sprite_Path);

	count_history_load = History_LoadAll(history, (size_t)max_history_load);
	n_records = top_five_scores(history, (int)count_history_load, records);

	fill_color(color_leaderboard_border, 255.0f, 148.0f, 0.1f, 1.0f);
	fill_color(color_leaderboard, 255.0f, 148.0f, 0.1f, 0.4f);
	fill_color(color_line, 255.0f, 148.0f, 0.1f, 1.0f);
	fill_color(color_line2, 255.0f, 148.0f, 0.1f, 0.42f);

	border_radius_lb = 10.0f;
	border_width_lb = 5.0f;
	line_width = 5.0f;


	fill_color(text_color, 255.0f, 148.0f, 0.1f, 1.0f);
	fill_color(text_border_color, 20.0f, 20.0f, 20.0f, 1.0f);
	text_border_width = 3.5f;

}

void state_gameover_enter(void) {
	count_history_load = History_LoadAll(history, (size_t)max_history_load);
	n_records = top_five_scores(history, (int)count_history_load, records);
}

void state_gameover_update(double dt) {
	dt=dt;
}

void state_gameover_run(void) {

	state_play_run();

	DrawSprite(skull, 0, 0, WIN_W, WIN_H);

	draw_leaderboard();
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
