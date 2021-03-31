#ifndef PLAYER_RENDERER_H
#define PLAYER_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "maze_renderer.h"

class PlayerRenderer
{
public:
    // Constructor (inits shaders/shapes)
    PlayerRenderer(Shader &shader, float startX, float startY, float speed, float height, float width);
    // Destructor
    ~PlayerRenderer();
    // Renders
    void DrawPlayer();
    // Moves left
    void MoveLeft(float dt, MazeRenderer *mazeRenderer);
    // Moves right
    void MoveRight(float dt, MazeRenderer *mazeRenderer);
    // Moves up
    void MoveUp(float dt, MazeRenderer *mazeRenderer);
    // Moves down
    void MoveDown(float dt, MazeRenderer *mazeRenderer);
    // Player Pos
    pair<float, float> GetPos();
    // Player Size
    pair<float, float> GetSize();
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;

    float playerX, playerY, speed, height, width;
    int nV, mV;

    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates player
    float* generatePlayer();
};

#endif