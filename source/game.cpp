/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game.h"
#include "resource_manager.h"
#include "maze_renderer.h"


// Game-related State data
MazeRenderer  *Renderer;


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    // delete Renderer;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("../source/shaders/maze.vs", "../source/shaders/maze.fs", nullptr, "maze");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("maze").Use().SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new MazeRenderer(ResourceManager::GetShader("maze"), 100, 100, 400, 400, 5, 5);
}

void Game::Update(float dt)
{
    
}

void Game::ProcessInput(float dt)
{
   
}

void Game::Render()
{
    Renderer->DrawMaze();
}