#include "renderer.h"
#include <stdlib.h>
#include <time.h>
#include "game_fun.h"
#include "config.h"
#include <stdio.h>

static int myRand(int x, int y)
{
    return x + rand() % (y - x + 1);
}

static int plat_init_valid(int plat_state[3][3], int start)
{
	if (((start == 0 || start ==2) && plat_state[1][1] == 1) && (plat_state[0][0] == 1 || plat_state[0][2] == 1)) {
		return 1;
	}
	if (start == 1 && (plat_state[1][0] == 1 || plat_state[1][2] == 1) && plat_state[0][1] == 1) {
		return 1;
	}
	return 0;
}

static int row_valid(int row[], int start)
{
	if ((start == 0 || start == 2) && (row[0] == 1 || row[2] == 1)) {
		return 1;
	}
	if (start == 1 && row[1] == 1) {
		return 1;
	}
	return 0;
}


float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float EaseOutQuad(float t) {
    return 1.0f - (1.0f - t) * (1.0f - t);
}


void grid_to_pixel(int pos_grid[], float pos_pixel[])
{
	int cell_width = WIN_W / 3;
    int cell_height = WIN_H / 3;

	pos_pixel[0] = (float)(pos_grid[0] * cell_width + cell_width / 2 - RECT_W / 2);
	pos_pixel[1] = (float)(pos_grid[1] * cell_height + cell_height/2 - RECT_H / 2);
}

void init_plat_state(int plat_state[3][3])
{
	int rand_val;
	while(1) {
		for(int n=0;n<3;n++) {
			plat_state[2][n] = n % 2;
		}
		for(int i=0;i<2;i++) {
			for(int j=0;j<3;j++) {
				rand_val = myRand(0, RAND_VALUE);
				if(rand_val == 0) {
					plat_state[i][j] = 1;
				}
			}
		}
		if (plat_init_valid(plat_state, 1) == 1) {break;}
		else {
			for(int i=0;i<2;i++) {
				for(int j=0;j<3;j++) {
					plat_state[i][j] = 0;
				}
			}
		}
	}
}

void add_row(int new_plat[], int start)
{
	int rand_val;
	while(1) {
		for(int i=0;i<3;i++) {new_plat[i] = 0;}
		for(int j=0;j<3;j++) {
			rand_val = myRand(0, RAND_VALUE);
			if (rand_val == 0) {new_plat[j] = 1;}
		}
		if (row_valid(new_plat,start)==1) {break;}
	}
}

void reset_plat_state(int plat_state[3][3])
{
	for(int i=0;i<3;i++) {
		for(int j=0;j<3;j++) {
			plat_state[i][j] = 0;
		}
	}
}

void fill_color(vec4 color, float r, float g, float b, float a)
{
	color[0] = r/255;
	color[1] = g/255;
	color[2] = b/255;
	color[3] = a;
}

void color_copy(vec4 color1, vec4 color2)
{
	for(int i=0;i<4;i++) {
		color1[i] = color2[i];
	}
}

int max_score(ScoreEntry history[], int max_count)
{
	int max = history[0].score;
	for(int i=1;i<max_count;i++) {
		if (history[i].score > max) {
			max = history[i].score;
		}
	}
	return max;
}

int top_five_scores(ScoreEntry history[], int max_count, int result[])
{
    int count = 0;

    for (int i = 0; i < max_count; i++) {
        int x = history[i].score;

        int is_present = 0;
        for (int j = 0; j < count; j++) {
            if (result[j] == x) {
                is_present = 1;
                break;
            }
        }

        if (is_present) {continue;}

        int pos = 0;
        while (pos < count && result[pos] > x) {
            pos++;
        }

        if (pos >= 5 && count == 5) {continue;}

        int limit;

		if (count <= 4) {
			limit = count;
		}
		else {
			limit = 4;
		}

        for (int j = limit; j > pos; j--) {
            result[j] = result[j - 1];
        }

        result[pos] = x;

        if (count < 5) {
            count++;
        }
    }

    return count;
}
