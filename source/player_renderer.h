/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef PLAYER_RENDERER_H
#define PLAYER_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

class PlayerRenderer
{
public:
    // Constructor (inits shaders/shapes)
    PlayerRenderer(Shader &shader);
    // Destructor
    ~PlayerRenderer();
    // Renders
    void DrawPlayer();
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;
    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates grid
    float* generatePlayer();
};

#endif