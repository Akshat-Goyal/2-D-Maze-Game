#ifndef MAZE_RENDERER_H
#define MAZE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

class MazeRenderer
{
public:
    // Constructor (inits shaders/shapes)
    MazeRenderer(Shader &shader, float startX, float startY, float height, float width, int nX, int nY);
    // Destructor
    ~MazeRenderer();
    // Renders
    void DrawMaze();
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;
    int nV, mV;
    // Initializes and configures the buffer and vertex attributes
    void initRenderData(float startX, float startY, float height, float width, int nX, int nY);
    // Generates grid
    float* generateMaze(float startX, float startY, float height, float width, int nX, int nY);
};

#endif