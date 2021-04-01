#ifndef BUTTON_RENDERER_H
#define BUTTON_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"
#include "player_renderer.h"
#include "bomb_renderer.h"
#include "coin_renderer.h"

using namespace std;

class ButtonRenderer
{
public:
    // Constructor (inits shaders/shapes)
    ButtonRenderer(Shader &shader, float startX, float startY, float width, float height);
    // Destructor
    ~ButtonRenderer();
    // Renders
    void DrawButton(Texture2D &texture);
    // Coin Pos
    pair<float, float> GetPos();
    // Coin Size
    pair<float, float> GetSize();
    // detect collision with player.
    bool DetectCollision(PlayerRenderer *playerRenderer, CoinRenderer **coinRenderers, BombRenderer **bombRenderers, int nCoins, int nBombs);
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;

    float buttonX, buttonY, height, width;
    int nV, mV;
    bool done;

    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates button
    float* generateButton();
    // checks x overlap
    bool xOverlap(pair<float, float> point, pair<float, float> size);
    // checks y overlap
    bool yOverlap(pair<float, float> point, pair<float, float> size);
};

#endif