#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gamestate.h"
#include "state_play.h"
#include "renderer.h"
#include "input.h"
#include "paths.h"
#include "game_fun.h"
#include "config.h"
#include "history.h"
#include <string.h>

static GLuint idle_right[9];
static GLuint idle_left[9];
static GLuint jump_right;
static GLuint jump_left;
static double game_time;
static double time_in_state;
static double time_stop;
static int plat_state[3][3];
static int new_plat[3];
static int player_pos;
static int is_moving_right;
static int score;

static double max_time;

static vec4 null_color;

static vec4 plat_color;
static vec4 plat_border_color;
static vec4 plat_shadow_color;
static float border_widht;

//------animation
static float playerX;
static float startX;
static float targetX;
static float moveElapsed;
static int isMoving;
static int isPaused;
static double time_post_paused;
static float platX;

static float PstartX;
static float PendX;
static float PposX;
static vec4 icon_color;
static float rect_w;
static float rect_h;
static float border_radius_icon;
static vec4 border_color;
static float border_widht;


static vec4 color_dash_line;
static vec4 topColor_start;
static vec4 topColor_end;
static vec4 bottColor_start;
static vec4 bottColor_end;
static vec4 topColor;
static vec4 bottColor;

static vec4 timer_load_color_bg;
static vec4 timer_load_color;
static float time_load_lenght;

static vec4 text_color;
static vec4 text_border_color;
static float text_border_widht;

static float noise_gap[2][3][3];

#define max_history_load 3000
static ScoreEntry history[max_history_load];
static size_t count_history_load;
static int record;




static void draw_time_load_bar(void)
{
	DrawLine((float)(WIN_W-165),60.0f,(float)(WIN_W-165)+time_load_lenght, 60.0f, 15.0f,timer_load_color_bg);
	DrawLine((float)(WIN_W-165),60.0f,(float)(WIN_W-165) + (float)game_time*time_load_lenght/20.0f, 60.0f, 10.0f,timer_load_color);
}

static void update_noise_gap(void)
{
	float noise_increase = -2.1f*expf(-0.0076f*(float)score)+3.0f;
	for(int k=0;k<2;k++) {
		for(int i=0;i<3;i++) {
			for(int j=0;j<3;j++) {
				noise_gap[k][i][j] = 1.5f*(cosf(0.2f*(noise_increase)*(float)time_in_state*(1.0f + (float)(10*i)+(float)(20*j) + (float)(15*k)))+1.0f);
			}
		}
	}
}


static float PlayerGridToPixelX(int gridPos) {
    return (float)(gridPos * WIN_W / 3 + WIN_W / 6 - 48);
}

static void DrawPlat(void) {
	int pos_grid[2];
	float pos_pixel[2];
	for(int i=0;i<3;i++) {
		for(int j=0;j<3;j++) {
			if(plat_state[i][j] == 1) {
				pos_grid[0] = j;
				pos_grid[1] = i;

				grid_to_pixel(pos_grid, pos_pixel);
				if (isMoving == 1) {
					DrawRoundedRect(pos_pixel[0] + 5.0f,pos_pixel[1]+platX + 5.0f,(float)RECT_W,(float)RECT_H,plat_shadow_color,(float)BORDER_RADIUS, null_color, 0.0f);
					DrawRoundedRect(pos_pixel[0],pos_pixel[1]+platX,(float)RECT_W,(float)RECT_H,plat_color,(float)BORDER_RADIUS, plat_border_color, border_widht);
				}
				else {
					DrawRoundedRect(pos_pixel[0] + noise_gap[0][i][j] + 5.0f ,pos_pixel[1] + noise_gap[1][i][j] + 5.0f,(float)RECT_W,(float)RECT_H,plat_shadow_color, (float)BORDER_RADIUS, null_color, 0.0f);
					DrawRoundedRect(pos_pixel[0] + noise_gap[0][i][j],pos_pixel[1] + noise_gap[1][i][j],(float)RECT_W,(float)RECT_H,plat_color,(float)BORDER_RADIUS, plat_border_color, border_widht);
				}
			}
		}
		if (new_plat[i] == 1) {
			pos_grid[0] = i;
			pos_grid[1] = -1;

			grid_to_pixel(pos_grid, pos_pixel);
			if (isMoving == 1) {
				DrawRoundedRect(pos_pixel[0] + 5.0f,pos_pixel[1]+platX + 5.0f,(float)RECT_W,(float)RECT_H,plat_shadow_color,(float)BORDER_RADIUS, null_color, 0.0f);
				DrawRoundedRect(pos_pixel[0],pos_pixel[1]+platX,(float)RECT_W,(float)RECT_H,plat_color,(float)BORDER_RADIUS, plat_border_color, border_widht);
			}
		}
	}
}

static void end_animation(void)
{
	for(int j=0;j<3;j++) {
		plat_state[2][j]=plat_state[1][j];
	}
	for(int i=0;i<3;i++) {
		plat_state[1][i]=plat_state[0][i];
	}
	for(int k=0;k<3;k++) {
		plat_state[0][k]=new_plat[k];
	}
	platX = 0;
}

static int check_death(void)
{
	if(plat_state[1][player_pos]==0) {return 1;}
	else {return 0;}
}

static void update_background_colors(int score, vec4 topColor, vec4 bottColor)
{
    float t = (float)score / ((float)max_time*4.5f);

    if (t > 1.0f) {t = 1.0f;}

    for (int i = 0; i < 3; i++) {
        topColor[i]  = topColor_start[i]  + (topColor_end[i]  - topColor_start[i])  * t;
        bottColor[i] = bottColor_start[i] + (bottColor_end[i] - bottColor_start[i]) * t;
    }
}

static void update_after_press()
{
	targetX = PlayerGridToPixelX(player_pos);
    moveElapsed = 0.0f;
	if (isMoving == 1) {end_animation();}
    isMoving = 1;
	if (check_death() == 1) {
		if (score != 0)
			History_AddScore(score, game_time);
		GameState newstate = STATE_GAMEOVER;
		Game_SetState(newstate);
	}
	else {score++; update_background_colors(score,topColor,bottColor);}
	add_row(new_plat,player_pos);
}

static void draw_play_text()
{
	char text[32];

	snprintf(text, sizeof(text),"SCORE: %d", score);
	DrawText(text, 20.0f, 40.0f, 0.7f, text_color, text_border_color, text_border_widht);

	snprintf(text, sizeof(text),"TIME: %.1f", game_time);
	DrawText(text, (float)WIN_W-175.0f, 40.0f, 0.7f, text_color, text_border_color, text_border_widht);

	if (score < record) {
		snprintf(text, sizeof(text),"RECORD: %d", record);
	}
	else {
		snprintf(text, sizeof(text),"RECORD: %d", score);
	}
	DrawText(text, 20.0f, 70.0f, 0.4f, text_color, text_border_color, text_border_widht);

}

static void draw_record_line() {
	static int temp_score;
	if (isMoving == 0) {temp_score = score;}
	float line_pos = (float)(-WIN_H*(record-temp_score)/3 + WIN_H*2/3 + 50);
	if (isMoving == 1) {
		DrawDashedLine(0.0f, line_pos + platX, (float)WIN_W, line_pos + platX, 3.5f, 50.0f, 15.0f, color_dash_line);
	}
	else {
		DrawDashedLine(0.0f, line_pos, (float)WIN_W, line_pos, 3.5f, 50.0f, 15.0f, color_dash_line);
	}
}

static void load_idle() {
	char sprite_Path[512];
	char temp_path[32];
	for (int i=1; i<10;i++) {
		snprintf(temp_path, sizeof(temp_path), "assets/idle/idle_right%d.png", i);
		GetResourcePath(temp_path, sprite_Path, sizeof(sprite_Path));
		idle_right[i-1] = LoadTexture(sprite_Path);

		snprintf(temp_path, sizeof(temp_path), "assets/idle/idle_left%d.png", i);
		GetResourcePath(temp_path, sprite_Path, sizeof(sprite_Path));
		idle_left[i-1] = LoadTexture(sprite_Path);
	}
}



static GLuint idle_choose() {

    double animDuration = (double)8 * IDLE_FRAME_TIME;
    double cycleLength = animDuration + IDLE_HOLD_TIME;

    if (time_stop < 0.2) {
        return is_moving_right ? idle_right[0] : idle_left[0];
    }

    double t = fmod(time_stop - 0.2, cycleLength);

    if (t < animDuration) {
        int i = (int)(t / IDLE_FRAME_TIME) + 1;
        if (i > 8) i = 8;
        return is_moving_right ? idle_right[i] : idle_left[i];
    } else {
        return is_moving_right ? idle_right[0] : idle_left[0];
    }
}

static void updateTimerColor(void) {
    float t = (float)game_time / 20.0f;
    if (t > 1.0f) {t = 1.0f;}

    timer_load_color[1] = Lerp(1.0f, 0.0f, t);
}

static void draw_pause_icon(){
	if (isPaused && time_post_paused!=0) {
		DrawRoundedRect(((float)WIN_W/3.0f-rect_w/2.0f)+(float)WIN_W/12.0f+PposX + 5.0f,(float)WIN_H/3.0f + 5.0f,rect_w,rect_h,plat_shadow_color,border_radius_icon, null_color, 0.0f);
		DrawRoundedRect(((float)WIN_W*2.0f/3.0f-rect_w/2.0f)-(float)WIN_W/12.0f+PposX + 5.0f,(float)WIN_H/3.0f + 5.0f,rect_w,rect_h,plat_shadow_color,border_radius_icon, null_color, 0.0f);

		DrawRoundedRect(((float)WIN_W/3.0f-rect_w/2.0f)+(float)WIN_W/12.0f+PposX,(float)WIN_H/3.0f,rect_w,rect_h,icon_color,border_radius_icon, border_color, border_widht);
		DrawRoundedRect(((float)WIN_W*2.0f/3.0f-rect_w/2.0f)-(float)WIN_W/12.0f+PposX,(float)WIN_H/3.0f,rect_w,rect_h,icon_color,border_radius_icon, border_color, border_widht);
	}
}

void state_play_init(void) {
    char sprite_Path[512];
	load_idle();
	GetResourcePath("assets/jump_right.png", sprite_Path, sizeof(sprite_Path));
    jump_right = LoadTexture(sprite_Path);
	GetResourcePath("assets/jump_left.png", sprite_Path, sizeof(sprite_Path));
    jump_left = LoadTexture(sprite_Path);

	player_pos = 1;
	is_moving_right = 1;
	reset_plat_state(plat_state);
	init_plat_state(plat_state);

	playerX = PlayerGridToPixelX(player_pos);
    startX = playerX;
    targetX = playerX;
    isMoving = 0;
	isPaused = 0;
	time_post_paused = 0.0;
	platX = 0;
	score = 0;
	game_time = 0;
	time_in_state = 0;
	max_time = 20.0;
	time_stop = 0.0;
	PstartX = 0.0f;
	PendX = 0.0f;
	PposX = 0.0f;


	fill_color(color_dash_line, 255.0f, 0.1f, 0.1f, 0.5f);
	fill_color(topColor_start, 0.1f, 255.0f, 0.1f, 1.0f);
	fill_color(bottColor_start, 155.1f, 30.1f, 155.1f, 1.0f);
	fill_color(topColor_end, 255.0f, 0.1f, 0.1f, 1.0f);
	fill_color(bottColor_end, 0.1f, 0.1f, 255.0f, 1.0f);
	fill_color(null_color, 0.0f, 0.0f, 0.0f, 0.0f);

	//fill_color(timer_load_color_bg, 255.0f, 255.0f, 255.0f, 0.3f);
	fill_color(timer_load_color_bg, 0.1f, 0.1f, 0.1f, 0.35f);
	fill_color(timer_load_color, 255.0f, 255.0f, 20.0f, 1.0f);
	time_load_lenght = 147.0f;

	color_copy(topColor,topColor_start);
	color_copy(bottColor,bottColor_start);

	fill_color(text_color, 255.0f, 255.0f, 255.0f, 1.0f);
	fill_color(text_border_color, 0.1f, 0.1f, 0.1f, 1.0f);
	text_border_widht = 3.5f;

	fill_color(plat_color, 255.0f, 148.0f, 0.1f, 1.0f);
	fill_color(plat_border_color, 255.0f, 255.0f, 255.0f, 1.0f);
	fill_color(plat_shadow_color, 20.0f, 20.0f, 20.0f, 0.3f);
	border_widht = 3.0f;

	count_history_load = History_LoadAll(history, (size_t)max_history_load);
	record = max_score(history, (int)count_history_load);

	fill_color(icon_color, 255.0f, 148.0f, 0.1f, 1.0f);
	border_radius_icon = 20.0f;
	rect_w = (float)WIN_W/18.0f;
	rect_h = (float)WIN_H/3.0f;
	fill_color(border_color, 255.0f, 255.0f, 255.0f, 1.0f);
	border_widht = 3.0f;
}




void state_play_enter(void) {
	PstartX = 0.0f;
	PendX = (float)(WIN_W*3/5);
	time_post_paused = 0;
}

void state_play_update(double dt) {
    if (isMoving) {
        moveElapsed += (float)dt;
        float t = moveElapsed / (float)PLAYER_MOVE_DURATION;
        if (t >= 1.0f) {
            t = 1.0f;
            isMoving = 0;
			end_animation();
        }
        playerX = Lerp(startX, targetX, EaseOutQuad(t));
		platX = Lerp(0,(float)(WIN_H/3),EaseOutQuad(t));
		time_stop = 0;
    }
	else {time_stop += dt;}

	if (game_time>=max_time) {
		if (score != 0)
			History_AddScore(score, game_time);
		GameState newstate = STATE_GAMEOVER;
		Game_SetState(newstate);
	}


	if (score != 0) {
		game_time += dt;
	}
	time_in_state += dt;
	update_noise_gap();
	updateTimerColor();

	if (isPaused) {
		time_post_paused += (float)dt;
		float tp = (float)time_post_paused/ (float)PAUSE_MOVE_DURATION;
		if(tp>= 1.0f) {tp = 1.0f; isPaused = 0; time_post_paused = 0;}
		PposX = Lerp(PendX, PstartX, EaseOutBack(1.0f-tp));

	}

}

void state_play_run(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	DrawGradientBackground(topColor,bottColor, (4.0f-3.0f*expf(-0.0193f*(float)score))*(float)game_time);


	DrawPlat();

	GLuint currentSprite;

	if (isMoving == 1) {
		currentSprite = is_moving_right ? jump_right : jump_left;
	}
	else {
		currentSprite = idle_choose();
	}
	DrawSprite(currentSprite, playerX-20, (float)(WIN_H * 5 / 6 - 143), 140.0f, 140.0f);

	draw_record_line();

	draw_play_text();

	draw_time_load_bar();

	draw_pause_icon();


}


void state_play_handle_events(GLFWwindow* window) {


    if (Input_KeyPressed(window, GLFW_KEY_ESCAPE) || Input_KeyPressed(window, GLFW_KEY_Q)) {
		Game_Shutdown();
        glfwSetWindowShouldClose(window, 1);
	}
	if (Input_KeyPressed(window, GLFW_KEY_SPACE)) {
		GameState newstate = STATE_PAUSED;
		Game_SetState(newstate);
		isPaused = 1;
	}

    if ((Input_KeyPressed(window, GLFW_KEY_RIGHT) || Input_KeyPressed(window, GLFW_KEY_D)) && player_pos != 2) {
		startX = PlayerGridToPixelX(player_pos);
        player_pos += 1;
        is_moving_right = 1;
		update_after_press();

    }
    if ((Input_KeyPressed(window, GLFW_KEY_LEFT) || Input_KeyPressed(window, GLFW_KEY_A)) && player_pos != 0) {
		startX = PlayerGridToPixelX(player_pos);
        player_pos -= 1;
        is_moving_right = 0;
        update_after_press();
    }
}

void state_play_esc(void) {

}

void state_play_shutdown(void) {

}
