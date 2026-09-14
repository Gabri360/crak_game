#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <glad/glad.h>
#include <cglm/cglm.h>

#define TEXT_CHARACTER_COUNT 128

typedef struct {
    GLuint texture;
    int width;
    int height;
    int bearingX;
    int bearingY;
    unsigned int advance;
} Character;

void TextRenderer_Init(const char *fontPath, int fontSize, int screenWidth, int screenHeight);

void TextRenderer_Draw(const char *text, float x, float y, float scale, vec4 color, vec4 outlineColor, float outlineWidth);

void TextRenderer_Shutdown(void);

#endif
