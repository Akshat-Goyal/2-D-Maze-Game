#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "resource_manager.h"
#include "text_renderer.h"
#include "maze_renderer.h"
#include "player_renderer.h"
#include "imposter_renderer.h"
#include "coin_renderer.h"
#include "bomb_renderer.h"
#include <string>

using namespace std;

// Represents the current state of the game
enum GameState {
    GAME_PAUSE,
    GAME_ACTIVE,
    GAME_WIN,
    GAME_LOST,
    GAME_OVER
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;	
    bool                    Keys[1024];
    unsigned int            Width, Height;

    int tasks, total_tasks, max_time, time_left, nCoins, coinsTaken, nBombs, bombsTaken;
    time_t startTime;
    string light;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void Exit();
    void UpdateTime();
    void UpdateTask();
    void CheckCoins();
    void CheckBombs();
    void CheckPlayer();
};

#endif