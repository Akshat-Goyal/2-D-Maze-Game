#ifndef COIN_RENDERER_H
#define COIN_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"
#include "player_renderer.h"

using namespace std;

class CoinRenderer
{
public:
    // Constructor (inits shaders/shapes)
    CoinRenderer(Shader &shader, float startX, float startY, float width, float height);
    // Destructor
    ~CoinRenderer();
    // Renders
    void DrawCoin(Texture2D &texture);
    // Coin Pos
    pair<float, float> GetPos();
    // Coin Size
    pair<float, float> GetSize();
    // detect collision with player.
    bool DetectCollision(PlayerRenderer *playerRenderer);
    // set done
    void setDone(bool val);
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;

    float coinX, coinY, height, width;
    int nV, mV;
    bool done;

    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates coin
    float* generateCoin();
    // checks x overlap
    bool xOverlap(pair<float, float> point, pair<float, float> size);
    // checks y overlap
    bool yOverlap(pair<float, float> point, pair<float, float> size);
};

#endif