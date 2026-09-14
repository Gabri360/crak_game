#include "text_renderer.h"
#include "shader.h"
#include "paths.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT_OUTLINE_PADDING 6

#define TEXT_CHARACTER_COUNT 128

static Character characters[TEXT_CHARACTER_COUNT];

static GLuint textVAO;
static GLuint textVBO;
static GLuint textShaderProgram;

static GLint locTextProj, locTextColor, locTextOutlineColor, locTextOutlineWidth;

void TextRenderer_Init(const char *fontPath, int fontSize, int screenWidth, int screenHeight)
{
    char vertPath[512];
    char fragPath[512];

    GetResourcePath("shaders/text.vert", vertPath, sizeof(vertPath));
    GetResourcePath("shaders/text.frag", fragPath, sizeof(fragPath));

    textShaderProgram = LoadShaderProgram(vertPath, fragPath);

    locTextProj = glGetUniformLocation(textShaderProgram, "uProj");
    locTextColor = glGetUniformLocation(textShaderProgram, "uColor");
	locTextOutlineColor = glGetUniformLocation(textShaderProgram, "uOutlineColor");
	locTextOutlineWidth = glGetUniformLocation(textShaderProgram, "uOutlineWidth");

    FT_Library ft;

    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "[Error] FreeType initialization failed\n");
        return;
    }

    FT_Face face;

    if (FT_New_Face(ft, fontPath, 0, &face)) {
        fprintf(stderr, "[Error] Failed to load font: %s\n", fontPath);
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, (FT_UInt)fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);



	for (unsigned int c = 0; c < TEXT_CHARACTER_COUNT; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			fprintf(stderr, "[Warning] Failed to load character %u\n", c);
			continue;
		}

		unsigned int srcW = face->glyph->bitmap.width;
		unsigned int srcH = face->glyph->bitmap.rows;
		unsigned int dstW = srcW + 2 * TEXT_OUTLINE_PADDING;
		unsigned int dstH = srcH + 2 * TEXT_OUTLINE_PADDING;

		unsigned char *padded = calloc((size_t)dstW * dstH, 1);
		if (!padded) {
			fprintf(stderr, "[Error] Allocazione buffer glifo fallita: %u\n", c);
			continue;
		}

		for (unsigned int row = 0; row < srcH; row++) {
			memcpy(
				   padded + (size_t)(row + TEXT_OUTLINE_PADDING) * dstW + TEXT_OUTLINE_PADDING,
				   face->glyph->bitmap.buffer + (size_t)row * srcW,
				   srcW
				  );
		}

		glGenTextures(1, &characters[c].texture);
		glBindTexture(GL_TEXTURE_2D, characters[c].texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (GLsizei)dstW, (GLsizei)dstH, 0, GL_RED, GL_UNSIGNED_BYTE, padded);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		free(padded);

		characters[c].width  = (int)dstW;
		characters[c].height = (int)dstH;

		characters[c].bearingX = face->glyph->bitmap_left - TEXT_OUTLINE_PADDING;
		characters[c].bearingY = face->glyph->bitmap_top + TEXT_OUTLINE_PADDING;
		characters[c].advance = (unsigned int)face->glyph->advance.x;
	}

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    mat4 projection;

    glm_ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f, projection);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(textShaderProgram);
    glUniformMatrix4fv(locTextProj, 1, GL_FALSE, (float *)projection);
}

void TextRenderer_Draw(const char *text, float x, float y, float scale, vec4 color, vec4 outlineColor, float outlineWidth)
{
    glUseProgram(textShaderProgram);
    glUniform4fv(locTextColor, 1, color);
    glUniform4fv(locTextOutlineColor, 1, outlineColor);
    glUniform1f(locTextOutlineWidth, outlineWidth);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    for (const char *p = text; *p != '\0'; p++) {
        unsigned char c = (unsigned char)*p;

        if (c >= TEXT_CHARACTER_COUNT)
            continue;

        Character ch = characters[c];

        float xpos = x + (float)ch.bearingX * scale;
        float ypos = y - (float)ch.bearingY * scale;
        float w = (float)ch.width * scale;
        float h = (float)ch.height * scale;

        float vertices[6][4] = {
			{ xpos,     ypos + h, 0.0f, 1.0f },
			{ xpos,     ypos,     0.0f, 0.0f },
			{ xpos + w, ypos,     1.0f, 0.0f },

			{ xpos,     ypos + h, 0.0f, 1.0f },
			{ xpos + w, ypos,     1.0f, 0.0f },
			{ xpos + w, ypos + h, 1.0f, 1.0f }
		};

        glBindTexture(GL_TEXTURE_2D, ch.texture);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (float)(ch.advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer_Shutdown(void)
{
    for (int i = 0; i < TEXT_CHARACTER_COUNT; i++) {
        if (characters[i].texture != 0)
            glDeleteTextures(1, &characters[i].texture);
    }

    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);
    glDeleteProgram(textShaderProgram);
}
