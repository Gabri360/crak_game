#include "renderer.h"
#include "shader.h"
#include "stb_image.h"
#include "paths.h"
#include "config.h"
#include "text_renderer.h"
#include <math.h>

static GLuint quadVAO, quadVBO;

static GLuint roundedShaderProgram;
static GLint locRRModel, locRRProj, locRRColor, locRRSize, locRRRadius;

static GLuint shaderProgram;
static GLint locModel, locProj, locColor, locUseTexture;

static GLuint backgroundShaderProgram;
static GLint locBGModel, locBGProj, locBGColorTop, locBGColorBottom, locBGt;

static mat4 projection;

void Renderer_Init(int screenWidth, int screenHeight) {
    // --- Shader ---
	char vertPath[512], fragPath[512], rrFragPath[512], bgFragPath[512];
    GetResourcePath("shaders/sprite.vert", vertPath, sizeof(vertPath));
    GetResourcePath("shaders/sprite.frag", fragPath, sizeof(fragPath));
	GetResourcePath("shaders/rounded_rect.frag", rrFragPath, sizeof(rrFragPath));
	GetResourcePath("shaders/background.frag", bgFragPath, sizeof(bgFragPath));

    shaderProgram = LoadShaderProgram(vertPath, fragPath);
    locModel = glGetUniformLocation(shaderProgram, "uModel");
    locProj  = glGetUniformLocation(shaderProgram, "uProj");
    locColor = glGetUniformLocation(shaderProgram, "uColor");
    locUseTexture = glGetUniformLocation(shaderProgram, "uUseTexture");


    roundedShaderProgram = LoadShaderProgram(vertPath, rrFragPath);
    locRRModel  = glGetUniformLocation(roundedShaderProgram, "uModel");
    locRRProj   = glGetUniformLocation(roundedShaderProgram, "uProj");
    locRRColor  = glGetUniformLocation(roundedShaderProgram, "uColor");
    locRRSize   = glGetUniformLocation(roundedShaderProgram, "uSize");
    locRRRadius = glGetUniformLocation(roundedShaderProgram, "uRadius");


	backgroundShaderProgram = LoadShaderProgram(vertPath, bgFragPath);
	locBGModel = glGetUniformLocation(backgroundShaderProgram, "uModel");
	locBGProj  = glGetUniformLocation(backgroundShaderProgram, "uProj");
	locBGColorTop    = glGetUniformLocation(backgroundShaderProgram, "uColorTop");
	locBGColorBottom = glGetUniformLocation(backgroundShaderProgram, "uColorBottom");
	locBGt = glGetUniformLocation(backgroundShaderProgram, "ut");



    float quadVertices[] = {
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 0.0f,  0.0f, 0.0f,

        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- Proiezione ortho ---
    glm_ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f, projection);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//----font-----
	char fontPath[512];

	GetResourcePath("assets/OpenSans-Bold.ttf", fontPath, sizeof(fontPath));

	TextRenderer_Init(fontPath, 48, WIN_W, WIN_H);
}

void Renderer_Shutdown(void) {
    TextRenderer_Shutdown();

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(roundedShaderProgram);
    glDeleteProgram(backgroundShaderProgram);

}

static void DrawQuad(GLuint texture, float x, float y, float w, float h, vec4 color, int useTexture) {
    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){x, y, 0.0f});
    glm_scale(model, (vec3){w, h, 1.0f});

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(locModel, 1, GL_FALSE, (float*)model);
    glUniformMatrix4fv(locProj, 1, GL_FALSE, (float*)projection);
    glUniform4fv(locColor, 1, color);
    glUniform1i(locUseTexture, useTexture);

    if (useTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void DrawRoundedRect(float x, float y, float w, float h, vec4 color, float radius) {

    float maxRadius = fminf(w, h) * 0.5f;
    if (radius > maxRadius) radius = maxRadius;

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){x, y, 0.0f});
    glm_scale(model, (vec3){w, h, 1.0f});

    glUseProgram(roundedShaderProgram);
    glUniformMatrix4fv(locRRModel, 1, GL_FALSE, (float*)model);
    glUniformMatrix4fv(locRRProj, 1, GL_FALSE, (float*)projection);
    glUniform4fv(locRRColor, 1, color);
    glUniform2f(locRRSize, w, h);
    glUniform1f(locRRRadius, radius);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawRect(float x, float y, float w, float h, vec4 color) {
    DrawQuad(0, x, y, w, h, color, GL_FALSE);
}

void DrawSprite(GLuint texture, float x, float y, float w, float h) {
    vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
    DrawQuad(texture, x, y, w, h, white, GL_TRUE);
}

GLuint LoadTexture(const char* path) {
    int w, h, ch;
    unsigned char* data = stbi_load(path, &w, &h, &ch, 4);
    if (!data) {
        fprintf(stderr, "[Error] load texture: %s\n", path);
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return tex;
}

void DrawGradientBackground(vec4 colorTop, vec4 colorBottom, float time) {
    mat4 model;
    glm_mat4_identity(model);
    glm_scale(model, (vec3){(float)WIN_W, (float)WIN_H, 1.0f});

    glUseProgram(backgroundShaderProgram);
    glUniformMatrix4fv(locBGModel, 1, GL_FALSE, (float*)model);
    glUniformMatrix4fv(locBGProj, 1, GL_FALSE, (float*)projection);
    glUniform4fv(locBGColorTop, 1, colorTop);
    glUniform4fv(locBGColorBottom, 1, colorBottom);
	glUniform1f(locBGt, time);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawText(const char *text, float x, float y, float scale, vec4 color)
{
    TextRenderer_Draw(text, x, y, scale, color);
}
