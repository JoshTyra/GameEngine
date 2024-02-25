#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "GameState.h"
#include <GLFW/glfw3.h>

class MenuState : public GameState {
public:
    explicit MenuState(GLFWwindow* window);
    virtual ~MenuState();

    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render() override;
    void setWindowContext(GLFWwindow* window) override;

private:
    GLFWwindow* window;
};

#endif // MENU_STATE_H
