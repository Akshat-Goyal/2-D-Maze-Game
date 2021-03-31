#include "game.h"

// Game-related State data
MazeRenderer   *mazeRenderer;
PlayerRenderer *playerRenderer;
ImposterRenderer *imposterRenderer;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete mazeRenderer;
    delete playerRenderer;
    delete imposterRenderer;
}

void Game::Init()
{
    // normalization matrix
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    int nX = 5, nY = 5;
    float startX = 100, startY = 100;
    float mazeW = 400, mazeH = 400;
    int pX = rand() % nX, pY = rand() % nY;
    int iX = rand() % nX, iY = rand() % nY;
    while(pX == iX && pY == iY){
        iX = rand() % nX, iY = rand() % nY;
    }
    float playerX = (mazeW / nX) * pX + startX;
    float playerY = (mazeH / nY) * pY + startY;
    float imposterX = (mazeW / nX) * iX + startX;
    float imposterY = (mazeH / nY) * iY + startY;

    // load shaders of maze
    ResourceManager::LoadShader("../source/shaders/maze.vs", "../source/shaders/maze.fs", nullptr, "maze");
    // configure shaders
    ResourceManager::GetShader("maze").Use().SetMatrix4("projection", projection);
    // set render-specific controls
    mazeRenderer = new MazeRenderer(ResourceManager::GetShader("maze"), startX, startY, mazeW, mazeH, nX, nY, {pX, pY}, {iX, iY});

    // load shaders of player
    ResourceManager::LoadShader("../source/shaders/player.vs", "../source/shaders/player.fs", nullptr, "player");
    // configure shaders
    ResourceManager::GetShader("player").Use().SetMatrix4("projection", projection);
    // set render-specific controls
    playerRenderer = new PlayerRenderer(ResourceManager::GetShader("player"), playerX, playerY, 200, 20, 20);

    // load shaders of imposter
    ResourceManager::LoadShader("../source/shaders/imposter.vs", "../source/shaders/imposter.fs", nullptr, "imposter");
    // configure shaders
    ResourceManager::GetShader("imposter").Use().SetMatrix4("projection", projection);
    // set render-specific controls
    imposterRenderer = new ImposterRenderer(ResourceManager::GetShader("imposter"), imposterX, imposterY, 2, 20, 20);
}

void Game::Update(float dt)
{
    
}

void Game::ProcessInput(float dt)
{
    if(this->Keys[GLFW_KEY_W]){
        playerRenderer->MoveUp(dt, mazeRenderer);
        imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
    }
    if(this->Keys[GLFW_KEY_S]){
        playerRenderer->MoveDown(dt, mazeRenderer);
        imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
    }
    if(this->Keys[GLFW_KEY_A]){
        playerRenderer->MoveLeft(dt, mazeRenderer);
        imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
    }
    if(this->Keys[GLFW_KEY_D]){
        playerRenderer->MoveRight(dt, mazeRenderer);
        imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
    }
}

void Game::Render()
{
    mazeRenderer->DrawMaze();
    playerRenderer->DrawPlayer();
    imposterRenderer->DrawImposter();
}