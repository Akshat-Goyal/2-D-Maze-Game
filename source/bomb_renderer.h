#ifndef BOMB_RENDERER_H
#define BOMB_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"
#include "player_renderer.h"

using namespace std;

class BombRenderer
{
public:
    // Constructor (inits shaders/shapes)
    BombRenderer(Shader &shader, float startX, float startY, float height, float width);
    // Destructor
    ~BombRenderer();
    // Renders
    void DrawBomb(Texture2D &texture);
    // Coin Pos
    pair<float, float> GetPos();
    // Coin Size
    pair<float, float> GetSize();
    // detect collision with player.
    bool DetectCollision(PlayerRenderer *playerRenderer);
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;

    float bombX, bombY, height, width;
    int nV, mV;
    bool done;

    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates coin
    float* generateBomb();
    // checks x overlap
    bool xOverlap(pair<float, float> point, pair<float, float> size);
    // checks y overlap
    bool yOverlap(pair<float, float> point, pair<float, float> size);
};

#endif