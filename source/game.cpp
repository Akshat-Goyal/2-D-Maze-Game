#include "game.h"

// Game-related State data
MazeRenderer   *mazeRenderer;
PlayerRenderer *playerRenderer;
ImposterRenderer *imposterRenderer;
TextRenderer *textRenderer;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_PAUSE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete mazeRenderer;
    delete playerRenderer;
    delete imposterRenderer;
    delete textRenderer;
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
    ResourceManager::GetShader("player").Use().SetInteger("image", 0);
    ResourceManager::GetShader("player").SetMatrix4("projection", projection);
    // set render-specific controls
    playerRenderer = new PlayerRenderer(ResourceManager::GetShader("player"), playerX, playerY, 200, 40, 40);
    // load textures
    for(int i = 0; i < playerRenderer->ISize(); i++){
        ResourceManager::LoadTexture(("../source/images/" + playerRenderer->GetImage(i)).c_str(), true, playerRenderer->GetImage(i));
    }

    // load shaders of imposter
    ResourceManager::LoadShader("../source/shaders/imposter.vs", "../source/shaders/imposter.fs", nullptr, "imposter");
    // configure shaders
    ResourceManager::GetShader("imposter").Use().SetInteger("image", 0);
    ResourceManager::GetShader("imposter").SetMatrix4("projection", projection);
    // set render-specific controls
    imposterRenderer = new ImposterRenderer(ResourceManager::GetShader("imposter"), imposterX, imposterY, 2, 40, 40);
    // load textures
    for(int i = 0; i < imposterRenderer->ISize(); i++){
        ResourceManager::LoadTexture(("../source/images/" + imposterRenderer->GetImage(i)).c_str(), true, imposterRenderer->GetImage(i));
    }

    textRenderer = new TextRenderer(this->Width, this->Height);
    textRenderer->Load("../source/fonts/OCRAEXT.TTF", 24);

    this->health = 100;
    this->tasks = 0;
    this->total_tasks = 2;
    this->light = "On";
    this->max_time = 100;
    this->time_left = this->max_time;
    time(&this->startTime);

    this->State = GAME_ACTIVE;
}

void Game::Update(float dt)
{
    this->UpdateTime();   
}

void Game::Exit(){
    // exit(0);
}

void Game::ProcessInput(float dt)
{
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_W]){
        playerRenderer->MoveUp(dt, mazeRenderer);
        bool status = playerRenderer->DetectCollisionWithImposter(imposterRenderer->GetPos(), imposterRenderer->GetSize());
        // if(status){
        //     this->State = GAME_LOST;
        //     this->Exit(); return;
        // }
        imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
        status = imposterRenderer->DetectCollisionWithPlayer(playerRenderer->GetPos(), playerRenderer->GetSize());
        // if(status){
        //     this->State = GAME_LOST;
        //     this->Exit(); return;
        // }
    }
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_S]){
        playerRenderer->MoveDown(dt, mazeRenderer);
        bool status = playerRenderer->DetectCollisionWithImposter(imposterRenderer->GetPos(), imposterRenderer->GetSize());
        // if(status){
        //     this->State = GAME_LOST;
        //     this->Exit(); return;
        // }
        imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
        status = imposterRenderer->DetectCollisionWithPlayer(playerRenderer->GetPos(), playerRenderer->GetSize());
        // if(status){
        //     this->State = GAME_LOST;
        //     this->Exit(); return;
        // }
    }
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_A]){
        playerRenderer->MoveLeft(dt, mazeRenderer);
        bool status = playerRenderer->DetectCollisionWithImposter(imposterRenderer->GetPos(), imposterRenderer->GetSize());
        // if(status){
        //     this->State = GAME_LOST;
        //     this->Exit(); return;
        // }
        imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
        status = imposterRenderer->DetectCollisionWithPlayer(playerRenderer->GetPos(), playerRenderer->GetSize());
        // if(status){
        //     this->State = GAME_LOST;
        //     this->Exit(); return;
        // }
    }
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_D]){
        playerRenderer->MoveRight(dt, mazeRenderer);
        bool status = playerRenderer->DetectCollisionWithImposter(imposterRenderer->GetPos(), imposterRenderer->GetSize());
        // if(status){
        //     this->State = GAME_LOST;
        //     this->Exit(); return;
        // }
        imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
        status = imposterRenderer->DetectCollisionWithPlayer(playerRenderer->GetPos(), playerRenderer->GetSize());
        // if(status){
        //     this->State = GAME_LOST;
        //     this->Exit(); return;
        // }
    }
}

void Game::Render()
{
    mazeRenderer->DrawMaze();
    playerRenderer->DrawPlayer(ResourceManager::GetTexture(playerRenderer->GetImage()));
    imposterRenderer->DrawImposter(ResourceManager::GetTexture(imposterRenderer->GetImage()));
    textRenderer->RenderText("Health: " + to_string(this->health), 50, 0, 1.0f);
    textRenderer->RenderText("Tasks: " + to_string(this->tasks) + "/" + to_string(this->total_tasks), 50, 24, 1.0f);
    textRenderer->RenderText("Light: " + this->light, 50 + this->Width / 2, 0, 1.0f);
    textRenderer->RenderText("Time: " + to_string(this->time_left), 50 + this->Width / 2, 24, 1.0f);
    if(this->State == GAME_OVER){
        textRenderer->RenderText("Game Over", this->Width / 2 - 100, this->Height / 2, 2.0f);
    }
}

void Game::UpdateTime(){
    time_t current_time ;
    time(&current_time);
    int delta = int(current_time - this->startTime);
    this->time_left = max(this->max_time - delta, 0);
    if(this->time_left <= 0){
        this->State = GAME_OVER;
    }
}