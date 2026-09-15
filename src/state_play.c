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

static GLuint idle_right;
static GLuint idle_left;
static GLuint jump_right;
static GLuint jump_left;
static double game_time;
static int plat_state[3][3];
static int new_plat[3];
static int player_pos;
static int is_moving_right;
static int score;

static double max_time;

static vec4 plat_color;
static vec4 plat_border_color;
static float border_widht;

//------animation
static float playerX;
static float startX;
static float targetX;
static float moveElapsed;
static int isMoving;
static float platX;


static vec4 topColor_start;
static vec4 topColor_end;
static vec4 bottColor_start;
static vec4 bottColor_end;
static vec4 topColor;
static vec4 bottColor;

static vec4 text_color;
static vec4 text_border_color;
static float text_border_widht;

static float noise_gap[2][3][3];



static void update_noise_gap()
{
	float noise_increase = -2.1f*expf(-0.0076f*(float)score)+3.0f;
	for(int k=0;k<2;k++) {
		for(int i=0;i<3;i++) {
			for(int j=0;j<3;j++) {
				noise_gap[k][i][j] = 1.5f*(cosf(0.2f*(noise_increase)*(float)game_time*(1.0f + (float)(10*i)+(float)(20*j) + (float)(15*k)))+1.0f);
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
					DrawRoundedRect(pos_pixel[0],pos_pixel[1]+platX,(float)RECT_W,(float)RECT_H,plat_color,(float)BORDER_RADIUS, plat_border_color, border_widht);
				}
				else {
					DrawRoundedRect(pos_pixel[0] + noise_gap[0][i][j],pos_pixel[1] + noise_gap[1][i][j],(float)RECT_W,(float)RECT_H,plat_color,(float)BORDER_RADIUS, plat_border_color, border_widht);
				}
			}
		}
		if (new_plat[i] == 1) {
			pos_grid[0] = i;
			pos_grid[1] = -1;

			grid_to_pixel(pos_grid, pos_pixel);
			if (isMoving == 1) {
				DrawRoundedRect(pos_pixel[0],pos_pixel[1]+platX,(float)RECT_W,(float)RECT_H,plat_color,(float)BORDER_RADIUS, plat_border_color, border_widht);
			}
			else {
				DrawRoundedRect(pos_pixel[0],pos_pixel[1],(float)RECT_W,(float)RECT_H,plat_color,(float)BORDER_RADIUS, plat_border_color, border_widht);
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

}

void state_play_init(void) {
    char sprite_Path[512];
    GetResourcePath("assets/idle_right.png", sprite_Path, sizeof(sprite_Path));
    idle_right = LoadTexture(sprite_Path);
	GetResourcePath("assets/idle_left.png", sprite_Path, sizeof(sprite_Path));
    idle_left = LoadTexture(sprite_Path);
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
	platX = 0;
	score = 0;
	game_time = 0;
	max_time = 20.0;


	fill_color(topColor_start, 0.1f, 255.0f, 0.1f, 1.0f);
	fill_color(bottColor_start, 155.1f, 30.1f, 155.1f, 1.0f);
	fill_color(topColor_end, 255.0f, 0.1f, 0.1f, 1.0f);
	fill_color(bottColor_end, 0.1f, 0.1f, 255.0f, 1.0f);

	color_copy(topColor,topColor_start);
	color_copy(bottColor,bottColor_start);

	fill_color(text_color, 255.0f, 255.0f, 255.0f, 1.0f);
	fill_color(text_border_color, 0.1f, 0.1f, 0.1f, 1.0f);
	text_border_widht = 3.5f;

	fill_color(plat_color, 255.0f, 148.0f, 0.1f, 1.0f);
	fill_color(plat_border_color, 255.0f, 255.0f, 255.0f, 1.0f);
	border_widht = 3.0f;
}




void state_play_enter(void) {
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
    }

	if (game_time>=max_time) {
		History_AddScore(score, game_time);
		GameState newstate = STATE_GAMEOVER;
		Game_SetState(newstate);
	}

	game_time += dt;
	update_noise_gap();

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
		currentSprite = is_moving_right ? idle_right : idle_left;
	}
	DrawSprite(currentSprite, playerX-20, (float)(WIN_H * 5 / 6 - 143), 140.0f, 140.0f);


	draw_play_text();
}


void state_play_handle_events(GLFWwindow* window) {


    if (Input_KeyPressed(window, GLFW_KEY_ESCAPE) || Input_KeyPressed(window, GLFW_KEY_Q)) {
		Game_Shutdown();
        glfwSetWindowShouldClose(window, 1);
	}
	if (Input_KeyPressed(window, GLFW_KEY_SPACE)) {
		GameState newstate = STATE_PAUSED;
		Game_SetState(newstate);
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
