#ifndef GAMEFUN_H
#define GAMEFUN_H

#include "history.h"


float Lerp(float a, float b, float t);
float EaseOutQuad(float t);
void grid_to_pixel(int pos_grid[], float pos_pixel[]);
void init_plat_state(int plat_state[3][3]);
void add_row(int new_plat[], int start);
void reset_plat_state(int plat_state[3][3]);
void fill_color(vec4 color, float r, float g, float b, float a);
void color_copy(vec4 color1, vec4 color2);
int max_score(ScoreEntry history[], int max_count);

#endif
