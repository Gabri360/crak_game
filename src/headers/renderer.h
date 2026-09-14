#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <cglm/cglm.h>

void Renderer_Init(int screenWidth, int screenHeight);
void Renderer_Shutdown(void);

void DrawRoundedRect(float x, float y, float w, float h, vec4 color, float radius, vec4 border_color, float border_widht);
void DrawRect(float x, float y, float w, float h, vec4 color);
void DrawSprite(GLuint texture, float x, float y, float w, float h);
void DrawGradientBackground(vec4 colorTop, vec4 colorBottom, float time);

GLuint LoadTexture(const char* path);

void DrawText(const char *text, float x, float y, float scale, vec4 color);

#endif
