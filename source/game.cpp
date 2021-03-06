#include "game.h"

// Game-related State data
MazeRenderer   *mazeRenderer;
PlayerRenderer *playerRenderer;
ImposterRenderer *imposterRenderer;
TextRenderer *textRenderer;
CoinRenderer **coinRenderers;
BombRenderer **bombRenderers;
ButtonRenderer *buttonRenderer;
VaporiseButtonRenderer *vaporiseButtonRenderer;
ExitRenderer *exitRenderer;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_STOP), Keys(), KeysProcessed(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete mazeRenderer;
    delete playerRenderer;
    delete imposterRenderer;
    delete textRenderer;
    {
        for(int i = 0; i < this->nCoins; i++) delete coinRenderers[i];
        delete[] coinRenderers;
    }
    {
        for(int i = 0; i < this->nBombs; i++) delete bombRenderers[i];
        delete[] bombRenderers;
    }
    delete buttonRenderer;
    delete vaporiseButtonRenderer;
    delete exitRenderer;
}

void Game::Init()
{
    srand(time(NULL));

    // normalization matrix
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    int nX = 12, nY = 12;
    float startX = 100, startY = 100;
    float mazeW = 1000, mazeH = 600;
    float edgeX = mazeW / nX, edgeY = mazeH / nY;

    // load shaders of maze
    ResourceManager::LoadShader("../source/shaders/maze.vs", "../source/shaders/maze.fs", nullptr, "maze");
    // configure shaders
    ResourceManager::GetShader("maze").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("maze").SetVector3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    // set render-specific controls
    mazeRenderer = new MazeRenderer(ResourceManager::GetShader("maze"), startX, startY, mazeW, mazeH, nX, nY);

    // load shaders of texture
    ResourceManager::LoadShader("../source/shaders/texture.vs", "../source/shaders/texture.fs", nullptr, "texture");
    // configure shaders
    ResourceManager::GetShader("texture").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("texture").SetVector3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    this->nCoins = rand() % 8 + 1;
    this->coinsTaken = 0;
    pair<float, float> cSize = {min(20.0f, edgeX), min(20.0f, edgeY)};
    coinRenderers = new CoinRenderer* [this->nCoins];
    for(int i = 0, deltaX = (int)(edgeX - cSize.first), deltaY = (int)(edgeY - cSize.second); i < this->nCoins; i++){
        pair<int, int> p = {rand() % nX, rand() % nY};
        float posX = startX + p.first * edgeX + (deltaX == 0 ? 0 : (float)(rand() % deltaX));
        float posY = startY + p.second * edgeY + (deltaY == 0 ? 0 : (float)(rand() % deltaY));
        coinRenderers[i] = new CoinRenderer(ResourceManager::GetShader("texture"), posX, posY, cSize.first, cSize.second);
    }
    // load textures
    ResourceManager::LoadTexture("../source/images/coin.png", true, "coin");

    this->nBombs = rand() % 3 + 1;
    this->bombsTaken = 0;
    pair<float, float> bSize = {min(20.0f, edgeY), min(20.0f, edgeY)};
    bombRenderers = new BombRenderer* [this->nBombs];
    for(int i = 0, deltaX = (int)(edgeX - bSize.first), deltaY = (int)(edgeY - bSize.second); i < this->nBombs; i++){
        pair<int, int> p = {rand() % nX, rand() % nY};
        float posX = startX + p.first * edgeX + (deltaX == 0 ? 0 : (float)(rand() % deltaX));
        float posY = startY + p.second * edgeY + (deltaY == 0 ? 0 : (float)(rand() % deltaY));
        bombRenderers[i] = new BombRenderer(ResourceManager::GetShader("texture"), posX, posY, cSize.first, cSize.second);
    }
    // load textures
    ResourceManager::LoadTexture("../source/images/bomb.png", true, "bomb");

    pair<float, float> btSize = {min(30.0f, edgeX), min(20.0f, edgeY)};
    {
        pair<int, int> p = {rand() % nX, rand() % nY};
        int deltaX = (int)(edgeX - btSize.first), deltaY = (int)(edgeY - btSize.second);
        float posX = startX + p.first * edgeX + (deltaX == 0 ? 0 : (float)(rand() % deltaX));
        float posY = startY + p.second * edgeY + (deltaY == 0 ? 0 : (float)(rand() % deltaY));
        buttonRenderer = new ButtonRenderer(ResourceManager::GetShader("texture"), posX, posY, btSize.first, btSize.second);
    }
    // load textures
    ResourceManager::LoadTexture("../source/images/button.png", true, "button");

    pair<float, float> vbtSize = {min(30.0f, edgeX), min(20.0f, edgeY)};
    {
        pair<int, int> p = {rand() % nX, rand() % nY};
        while(p.first == buttonRenderer->GetPos().first && p.second == buttonRenderer->GetPos().second){
            p = {rand() % nX, rand() % nY};        
        }
        int deltaX = (int)(edgeX - vbtSize.first), deltaY = (int)(edgeY - vbtSize.second);
        float posX = startX + p.first * edgeX + (deltaX == 0 ? 0 : (float)(rand() % deltaX));
        float posY = startY + p.second * edgeY + (deltaY == 0 ? 0 : (float)(rand() % deltaY));
        vaporiseButtonRenderer = new VaporiseButtonRenderer(ResourceManager::GetShader("texture"), posX, posY, vbtSize.first, vbtSize.second);
    }
    // load textures
    ResourceManager::LoadTexture("../source/images/vaporiseButton.png", true, "vaporiseButton");

    pair<float, float> eSize = {min(50.0f, edgeX), min(60.0f, edgeY)};
    {
        pair<int, int> p = {rand() % nX, rand() % nY};
        int deltaX = (int)(edgeX - eSize.first), deltaY = (int)(edgeY - eSize.second);
        float posX = startX + p.first * edgeX + (deltaX == 0 ? 0 : (float)(rand() % deltaX));
        float posY = startY + p.second * edgeY + (deltaY == 0 ? 0 : (float)(rand() % deltaY));
        exitRenderer = new ExitRenderer(ResourceManager::GetShader("texture"), posX, posY, eSize.first, eSize.second);
    }
    // load textures
    ResourceManager::LoadTexture("../source/images/exit.png", true, "exit");


    // load shaders of textureM
    ResourceManager::LoadShader("../source/shaders/textureM.vs", "../source/shaders/textureM.fs", nullptr, "textureM");
    // configure shaders
    ResourceManager::GetShader("textureM").Use().SetInteger("image", 0);
    ResourceManager::GetShader("textureM").SetMatrix4("projection", projection);
    ResourceManager::GetShader("textureM").SetVector3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    int pX = rand() % nX, pY = rand() % nY;
    int iX = rand() % nX, iY = rand() % nY;
    while(pX == iX && pY == iY){
        iX = rand() % nX, iY = rand() % nY;
    }
    float playerX = (mazeW / nX) * pX + startX;
    float playerY = (mazeH / nY) * pY + startY;
    float imposterX = (mazeW / nX) * iX + startX;
    float imposterY = (mazeH / nY) * iY + startY;

    // set render-specific controls
    playerRenderer = new PlayerRenderer(ResourceManager::GetShader("textureM"), playerX, playerY, 200, min(40.0f, edgeX), min(40.0f, edgeY));
    // load textures
    for(int i = 0; i < playerRenderer->ISize(); i++){
        ResourceManager::LoadTexture(("../source/images/" + playerRenderer->GetImage(i)).c_str(), true, playerRenderer->GetImage(i));
    }

    // set render-specific controls
    imposterRenderer = new ImposterRenderer(ResourceManager::GetShader("textureM"), imposterX, imposterY, 60, min(40.0f, edgeX), min(40.0f, edgeY));
    // load textures
    for(int i = 0; i < imposterRenderer->ISize(); i++){
        ResourceManager::LoadTexture(("../source/images/" + imposterRenderer->GetImage(i)).c_str(), true, imposterRenderer->GetImage(i));
    }

    textRenderer = new TextRenderer(this->Width, this->Height);
    textRenderer->Load("../source/fonts/OCRAEXT.TTF", 24);

    this->tasks = 0;
    this->total_tasks = 2;
    this->max_time = 100;
    this->time_left = this->max_time;
    time(&this->startTime);

    this->State = GAME_ACTIVE;
}

void Game::Update(float dt)
{
    imposterRenderer->MoveImposter(dt, mazeRenderer, playerRenderer);
    this->CheckButton();
    this->CheckVaporiseButton();
    this->CheckCoins();
    this->CheckBombs();
    this->CheckExit();
    this->CheckPlayer();
    this->UpdateShader();
    this->UpdateTime();  
}

void Game::ProcessInput(float dt)
{
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_W]){
        playerRenderer->MoveUp(dt, mazeRenderer);
    }
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_S]){
        playerRenderer->MoveDown(dt, mazeRenderer);
    }
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_A]){
        playerRenderer->MoveLeft(dt, mazeRenderer);
    }
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_D]){
        playerRenderer->MoveRight(dt, mazeRenderer);
    }
    if(this->State == GAME_ACTIVE && this->Keys[GLFW_KEY_L] && !this->KeysProcessed[GLFW_KEY_L]){
        this->KeysProcessed[GLFW_KEY_L] = true;
        playerRenderer->toggleLight();
    }
}

void Game::Render()
{
    mazeRenderer->DrawMaze();
    for(int i = 0; i < this->nCoins; i++){
        coinRenderers[i]->DrawCoin(ResourceManager::GetTexture("coin"));
    }
    for(int i = 0; i < this->nBombs; i++){
        bombRenderers[i]->DrawBomb(ResourceManager::GetTexture("bomb"));
    }
    buttonRenderer->DrawButton(ResourceManager::GetTexture("button"));
    vaporiseButtonRenderer->DrawVaporiseButton(ResourceManager::GetTexture("vaporiseButton"));
    exitRenderer->DrawExit(ResourceManager::GetTexture("exit"));
    playerRenderer->DrawPlayer(ResourceManager::GetTexture(playerRenderer->GetImage()));
    imposterRenderer->DrawImposter(ResourceManager::GetTexture(imposterRenderer->GetImage()));
    textRenderer->RenderText("Health: " + to_string(playerRenderer->getHealth()), 50, 0, 1.0f);
    textRenderer->RenderText("Tasks: " + to_string(this->tasks) + "/" + to_string(this->total_tasks), 50, 24, 1.0f);
    textRenderer->RenderText("Score: " + to_string(playerRenderer->getScore()), 50, 48, 1.0f);
    textRenderer->RenderText("Light: " + playerRenderer->getLight(), 50 + this->Width / 2, 0, 1.0f);
    textRenderer->RenderText("Time: " + to_string(this->time_left), 50 + this->Width / 2, 24, 1.0f);
    if(this->State == GAME_OVER){
        textRenderer->RenderText("Game Over", this->Width / 2 - 100, this->Height / 2, 2.0f);
    }
    if(this->State == GAME_LOST){
        textRenderer->RenderText("You Lost", this->Width / 2 - 100, this->Height / 2, 2.0f);
    }
    if(this->State == GAME_WON){
        textRenderer->RenderText("You Won", this->Width / 2 - 100, this->Height / 2, 2.0f);
    }
}

void Game::UpdateTime(){
    if(this->State != GAME_ACTIVE) return;
    
    time_t current_time ;
    time(&current_time);
    int delta = int(current_time - this->startTime);
    if(playerRenderer->getLight() == "OFF"){
        playerRenderer->updateScore(this->time_left - max(this->max_time - delta, 0));
    }
    this->time_left = max(this->max_time - delta, 0);
    if(this->time_left <= 0){
        this->State = GAME_OVER;
    }
}

void Game::UpdateTask(){
    this->tasks += 1;
    if(this->tasks == this->total_tasks){
        exitRenderer->setDone(false);
    }
}

void Game::UpdateShader(){
    ResourceManager::GetShader("maze").Use().SetVector3f("lightPos", glm::vec3(playerRenderer->GetPos().first, playerRenderer->GetPos().second, 0.0f));
    ResourceManager::GetShader("texture").Use().SetVector3f("lightPos", glm::vec3(playerRenderer->GetPos().first, playerRenderer->GetPos().second, 0.0f));
    ResourceManager::GetShader("textureM").Use().SetVector3f("lightPos", glm::vec3(playerRenderer->GetPos().first, playerRenderer->GetPos().second, 0.0f));

    // if(playerRenderer->getLight() == "OFF"){
    //     ResourceManager::GetShader("maze").Use().SetInteger("light", 0);
    //     ResourceManager::GetShader("texture").Use().SetInteger("light", 0);
    //     ResourceManager::GetShader("textureM").Use().SetInteger("light", 0);
    // }
    // else{
    //     ResourceManager::GetShader("maze").Use().SetInteger("light", 1);
    //     ResourceManager::GetShader("texture").Use().SetInteger("light", 1);
    //     ResourceManager::GetShader("textureM").Use().SetInteger("light", 1);
    // }

    if(playerRenderer->getLight() == "OFF"){
        ResourceManager::GetShader("maze").Use().SetFloat("lightCutOff", min(this->Height, this->Width) / 10);
        ResourceManager::GetShader("texture").Use().SetFloat("lightCutOff", min(this->Height, this->Width) / 10);
        ResourceManager::GetShader("textureM").Use().SetFloat("lightCutOff", min(this->Height, this->Width) / 10);
    }
    else{
        ResourceManager::GetShader("maze").Use().SetFloat("lightCutOff", max(this->Width, this->Height));
        ResourceManager::GetShader("texture").Use().SetFloat("lightCutOff", max(this->Width, this->Height));
        ResourceManager::GetShader("textureM").Use().SetFloat("lightCutOff", max(this->Width, this->Height));
    }
}

void Game::CheckCoins(){
    if(this->coinsTaken == this->nCoins) return;
    for(int i = 0; i < this->nCoins; i++){
        this->coinsTaken += coinRenderers[i]->DetectCollision(playerRenderer);
    }
    if(this->coinsTaken == this->nCoins){
        this->UpdateTask();
    }
}

void Game::CheckBombs(){
    if(this->bombsTaken == this->nBombs) return;
    for(int i = 0; i < this->nBombs; i++){
        this->bombsTaken += bombRenderers[i]->DetectCollision(playerRenderer);
    }
}

void Game::CheckButton(){
    buttonRenderer->DetectCollision(playerRenderer, coinRenderers, bombRenderers, this->getNCoins(), this->getNBombs());
}

void Game::CheckVaporiseButton(){
    if(vaporiseButtonRenderer->DetectCollision(playerRenderer, imposterRenderer)){
        this->UpdateTask();
    }
}

void Game::CheckExit(){
    if(exitRenderer->DetectCollision(playerRenderer)){
        this->State = GAME_WON;
    }
}

void Game::CheckPlayer(){
    if(!imposterRenderer->isVaporised() && playerRenderer->DetectCollisionWithImposter(imposterRenderer->GetPos(), imposterRenderer->GetSize())){
        this->State = GAME_LOST;
    }
    if(playerRenderer->getHealth() <= 0){
        this->State = GAME_LOST;
    }
}

int Game::getNCoins(){
    return this->nCoins;
}

int Game::getNBombs(){
    return this->nBombs;
}
