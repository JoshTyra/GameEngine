#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "GameState.h"

class MenuState : public GameState {
public:
    // Remove the explicit constructor that accepts GLFWwindow*

    virtual ~MenuState();

    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render() override;

    // No need for setWindowContext if not doing anything special with it
};

#endif // MENU_STATE_H
