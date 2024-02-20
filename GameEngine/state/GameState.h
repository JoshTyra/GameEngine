// GameState.h
#ifndef GAME_STATE_H
#define GAME_STATE_H

// Define game states
enum class GameState {
    MENU,
    GAME,
    // Add more states as needed
};

class GameStateManager {
public:
    GameStateManager(); // Constructor
    ~GameStateManager(); // Destructor

    void changeState(GameState newState); // Method to change the current game state
    GameState getCurrentState() const; // Method to get the current game state

private:
    GameState currentState;
};

#endif // GAME_STATE_H

