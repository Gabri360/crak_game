#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

static char* ReadFile(const char* path) {
    FILE* f = fopen(path, "rb");
	if (!f) {
        fprintf(stderr, "[Error] impossible open file: %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc((size_t)(size + 1));
    fread(buf, 1, (size_t)size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

static GLuint CompileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        fprintf(stderr, "Shader compile error: %s\n", log);
    }
    return shader;
}

GLuint LoadShaderProgram(const char* vertPath, const char* fragPath) {
    char* vertSrc = ReadFile(vertPath);
    char* fragSrc = ReadFile(fragPath);

    GLuint vert = CompileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        fprintf(stderr, "Shader link error: %s\n", log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    free(vertSrc);
    free(fragSrc);
    return program;
}
