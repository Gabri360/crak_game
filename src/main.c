#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "renderer.h"
#include "gamestate.h"
#include "config.h"

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "[Error] init GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "Crak", NULL, NULL);
    if (!window) {
        fprintf(stderr, "[Errore]  window generation\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "[Error] init GLAD\n");
        return -1;
    }

    glViewport(0, 0, WIN_W, WIN_H);
    Renderer_Init(WIN_W, WIN_H);

	double lastTime = glfwGetTime();
	srand((unsigned int)time(NULL));
	Game_Init();
	//------------------------------------------- MAIN LOOP
    while (!glfwWindowShouldClose(window)) {

		double currentTime = glfwGetTime();
        double dt = currentTime - lastTime;
        lastTime = currentTime;

		Game_Update(dt);
		Game_Run();



		glfwPollEvents();
		Game_handle_events(window);

        glfwSwapBuffers(window);

		// fps-limiter
		double frameTime = glfwGetTime() - currentTime;
        double sleepTime = TARGET_FRAMETIME - frameTime;
        if (sleepTime > 0) {
            struct timespec ts;
            ts.tv_sec = (time_t)sleepTime;
            ts.tv_nsec = (long)((sleepTime - (double)ts.tv_sec) * 1e9);
            nanosleep(&ts, NULL);
        }
	}
	//--------------------------------------------
    Renderer_Shutdown();
    glfwTerminate();
    return 0;
}
