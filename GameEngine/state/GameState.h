#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <GLFW/glfw3.h> // Ensure GLFW is included for GLFWwindow*

class GameState {
public:
    virtual ~GameState() {}

    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    // Provide a default implementation for setting window context
    virtual void setWindowContext(GLFWwindow* window) { this->window = window; }

protected:
    GLFWwindow* window = nullptr; // Holds the GLFW window context
};

#endif // GAME_STATE_H
