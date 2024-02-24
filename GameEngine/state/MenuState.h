#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "GameState.h"

class MenuState : public GameState {
public:
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render() override;
};

#endif // MENU_STATE_H
