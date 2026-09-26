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
#include "state_statistics.h"

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

static float coord_lb[2];
static float dim_lb[2];

static vec4 text_color;
static vec4 text_border_color;
static float text_border_width;

static float lb_anim_time;
static double game_time;

static vec4 color_keys;
static vec4 black;
static vec4 blank;
static vec4 color_selected_key;
static int select_key;
static float key_startX;
static float key_endX;
static float key_posX;
static int key_isMoving;
static float key_moveElapsed;

static float apkey_posX;
static float apkey_moveElapsed;

static void draw_leaderboard(void) {

	float revealW, revealH;

    if (lb_anim_time < LB_ANIM_PHASE1_DURATION) {
        float t = lb_anim_time / LB_ANIM_PHASE1_DURATION;
        float eased = EaseOutQuad(t);
        revealW = Lerp(0.0f, dim_lb[0], eased);
        revealH = LB_ANIM_LINE_THICKNESS;
    } else {
        revealW = dim_lb[0];
        float t2 = (lb_anim_time - LB_ANIM_PHASE1_DURATION) / LB_ANIM_PHASE2_DURATION;
        if (t2 > 1.0f) t2 = 1.0f;
        float eased2 = EaseOutQuad(t2);
        revealH = Lerp(LB_ANIM_LINE_THICKNESS, dim_lb[1], eased2);
    }

    if (revealW < 1.0f) revealW = 1.0f;
    if (revealH < 1.0f) revealH = 1.0f;

    float centerX = coord_lb[0] + dim_lb[0] / 2.0f;
    float centerY = coord_lb[1] + dim_lb[1] / 2.0f;
    float revealX = centerX - revealW / 2.0f;
    float revealY = centerY - revealH / 2.0f;

    glEnable(GL_SCISSOR_TEST);
    glScissor((int)revealX, (int)((float)WIN_H - (revealY + revealH)), (int)revealW, (int)revealH);
	//DrawRoundedRect((coord_lb[0]),(coord_lb[1]),dim_lb[0],dim_lb[1],color_leaderboard,border_radius_lb,color_leaderboard_border, border_width_lb);
	DrawRoundedRect(revealX,(int)((float)WIN_H - (revealY + revealH)),revealW,revealH,color_leaderboard,border_radius_lb,color_leaderboard_border, border_width_lb);

	if(game_time>LB_ANIM_PHASE1_DURATION) {
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
				DrawLine(coord_lb[0]+4.0f,coord_lb[1]+lineH,coord_lb[0]+dim_lb[0]-4.0f, coord_lb[1]+lineH,line_width/2.0f,color_line2);
			}
			else {
				DrawLine(coord_lb[0]+4.0f,coord_lb[1]+lineH,coord_lb[0]+dim_lb[0]-4.0f, coord_lb[1]+lineH,line_width/2.0f,color_line);
			}
		}
		DrawLine(coord_lb[0]+245.0f,coord_lb[1]+95.0f,coord_lb[0]+245.0f, coord_lb[1]+lineH,line_width/2.0f,color_line2);
		DrawLine(coord_lb[0]+245.0f + 100.0f,coord_lb[1]+95.0f,coord_lb[0]+245.0f + 100.0f, coord_lb[1]+lineH,line_width/2.0f,color_line2);
	}

    glDisable(GL_SCISSOR_TEST);
}
static void draw_keys(void) {
	float padding_keys = 25.0f;
	float coord_keys[2] = {coord_lb[0], coord_lb[1] + dim_lb[1] + 20.0f + apkey_posX};
	DrawRoundedRect(coord_keys[0],coord_keys[1], dim_lb[0]/2.0f-padding_keys, 50.0f, color_keys, border_radius_lb, color_leaderboard_border, border_width_lb);
	DrawRoundedRect(coord_keys[0]+ dim_lb[0]/2.0f+padding_keys,coord_keys[1], dim_lb[0]/2.0f-padding_keys, 50.0f, color_keys, border_radius_lb, color_leaderboard_border, border_width_lb);



	DrawRoundedRect(coord_keys[0] + ( dim_lb[0]/2.0f+padding_keys) * key_posX,coord_keys[1], dim_lb[0]/2.0f-padding_keys, 50.0f, color_selected_key, border_radius_lb, blank, border_width_lb);

	char text[32];
	snprintf(text, sizeof(text), "RESTART");
	DrawText(text, coord_keys[0] + 70.0f, coord_keys[1] + 35.0f, 0.6f, text_color,text_border_color,text_border_width);
	snprintf(text, sizeof(text), "STATISTICS");
	DrawText(text, coord_keys[0]+ dim_lb[0]/2.0f+padding_keys + 60.0f, coord_keys[1] + 35.0f, 0.6f, text_color,text_border_color,text_border_width);
}

void gameover_load(void) {
	/* fill_color(color_leaderboard_border, 255.0f, 148.0f, 0.1f, 1.0f); */
	/* fill_color(color_leaderboard, 255.0f, 148.0f, 0.1f, 0.4f); */
	/* fill_color(color_line, 255.0f, 148.0f, 0.1f, 1.0f); */
	/* fill_color(color_line2, 255.0f, 148.0f, 0.1f, 0.42f); */

	fill_color(color_leaderboard_border, 20.0f, 20.0f, 20.0f, 1.0f);
	fill_color(color_leaderboard, 20.0f, 20.0f, 20.0f, 0.4f);
	fill_color(color_line, 20.0f, 20.0f, 20.0f, 1.0f);
	fill_color(color_line2, 20.0f, 20.0f, 20.0f, 0.42f);
	fill_color(text_border_color, 0.1f, 0.1f, 0.1f, 1.0f);
	fill_color(text_color, 245.0f, 140.0f, 0.1f, 1.0f);
	fill_color(color_keys, 20.0f, 20.0f, 20.0f, 0.4f);
	fill_color(black, 0.0f, 0.0f, 0.0f, 1.0f);
	fill_color(blank, 255.0f, 255.0f, 255.0f, 1.0f);
	fill_color(color_selected_key, 255.0f, 146.0f, 0.1f, 0.4f);

	apkey_moveElapsed = 0.0f;
	key_isMoving = 0;
	select_key = 0;
	key_posX = 0.0f;
	border_radius_lb = 10.0f;
	border_width_lb = 3.0f;
	line_width = 5.0f;
	lb_anim_time = 0.0f;


	/* fill_color(text_color, 255.0f, 148.0f, 0.1f, 1.0f); */
	/* fill_color(text_border_color, 20.0f, 20.0f, 20.0f, 1.0f); */
	text_border_width = 3.5f;

	coord_lb[0] = (float)(WIN_W/6);
	coord_lb[1] = (float)(WIN_H/6);
	dim_lb[0] = (float)WIN_W-2.0f*coord_lb[0];
	dim_lb[1] = (float)WIN_H-2.0f*coord_lb[1];
}



void state_gameover_init(void) {

	count_history_load = History_LoadAll(history, (size_t)max_history_load);
	n_records = top_five_scores(history, (int)count_history_load, records);
}

void state_gameover_enter(void) {
	count_history_load = History_LoadAll(history, (size_t)max_history_load);
	n_records = top_five_scores(history, (int)count_history_load, records);

	game_time = 0.0f;
	lb_anim_time = 0.0f;
	apkey_moveElapsed = 0.0f;
	select_key = 0;
	key_posX = 0.0f;
}

void state_gameover_update(double dt) {
	game_time+=dt;
	lb_anim_time += (float)dt;

	state_play_update(dt);

	if (key_isMoving) {
		key_moveElapsed += (float)dt;
		float t = key_moveElapsed / (float) KEY_MOVE_DURATION;
		if (t >= 1.0f) {
			t = 1.0f;
			key_isMoving = 0;
		}
		key_posX = Lerp(key_startX, key_endX, EaseOutBack2(t, 1.3f));
	}
	if (game_time < APKEY_MOVE_DURATION) {
		apkey_moveElapsed += (float)dt;
		float t = apkey_moveElapsed / (float) APKEY_MOVE_DURATION;
		if (t >= 1.0f) {
			t = 1.0f;
		}
		apkey_posX = Lerp((float)WIN_H -coord_lb[1] - dim_lb[1] - 20.0f, 0.0f, EaseOutBack2(t, 1.3f));
	}
}

void state_gameover_run(void) {

	state_play_run();
	draw_leaderboard();
	draw_keys();
}

void state_gameover_handle_events(GLFWwindow* window) {

    if (Input_KeyPressed(window, GLFW_KEY_ESCAPE)) {
		Game_Shutdown();
        glfwSetWindowShouldClose(window, 1);
	}
	else if (Input_KeyPressed(window, GLFW_KEY_ENTER) || Input_KeyPressed(window, GLFW_KEY_SPACE)) {
		GameState newstate;
		if (select_key == 0) {
			newstate = STATE_PLAY;
			Game_Init();
		}
		else if (select_key ==1) {
			newstate = STATE_STATISTICS;
			set_prev_state(1);
		}
		Game_SetState(newstate);
	}
	else if (Input_KeyPressed(window, GLFW_KEY_S)) {
		GameState newstate = STATE_STATISTICS;
		Game_SetState(newstate);
		set_prev_state(1);
	}
	else if (Input_KeyPressed(window, GLFW_KEY_R)  || Input_KeyPressed(window, GLFW_KEY_Q)) {
		GameState newstate = STATE_PLAY;
		Game_Init();
		Game_SetState(newstate);
	}
	else if ((Input_KeyPressed(window, GLFW_KEY_RIGHT) || Input_KeyPressed(window, GLFW_KEY_D)) && select_key == 0) {
		key_startX = (float)select_key;
		select_key = 1;
		key_endX = (float)select_key;
		key_isMoving = 1;
		key_moveElapsed = 0.0f;
	}

	else if ((Input_KeyPressed(window, GLFW_KEY_LEFT) || Input_KeyPressed(window, GLFW_KEY_A))&& select_key == 1) {
		key_startX = (float)select_key;
		select_key = 0;
		key_endX = (float)select_key;
		key_isMoving = 1;
		key_moveElapsed = 0.0f;
	}

}

void state_gameover_esc(void) {

}

void state_gameover_shutdown(void) {

}
