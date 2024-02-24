// GameState.h
#ifndef GAME_STATE_H
#define GAME_STATE_H

class GameState {
public:
    virtual ~GameState() {}
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
};

#endif // GAME_STATE_H
