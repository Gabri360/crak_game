#include "input.h"

static bool prevState[GLFW_KEY_LAST + 1] = {0};

bool Input_KeyPressed(GLFWwindow* window, int key) {
    bool isDown = glfwGetKey(window, key) == GLFW_PRESS;
    bool justPressed = isDown && !prevState[key];
    prevState[key] = isDown;
    return justPressed;
}
