#ifndef VAPORISE_BUTTON_RENDERER_H
#define VAPORISE_BUTTON_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"
#include "player_renderer.h"
#include "imposter_renderer.h"

using namespace std;

class VaporiseButtonRenderer
{
public:
    // Constructor (inits shaders/shapes)
    VaporiseButtonRenderer(Shader &shader, float startX, float startY, float width, float height);
    // Destructor
    ~VaporiseButtonRenderer();
    // Renders
    void DrawVaporiseButton(Texture2D &texture);
    // Coin Pos
    pair<float, float> GetPos();
    // Coin Size
    pair<float, float> GetSize();
    // detect collision with player.
    bool DetectCollision(PlayerRenderer *playerRenderer, ImposterRenderer *imposterRenderer);
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;

    float vaporiseButtonX, vaporiseButtonY, height, width;
    int nV, mV;
    bool done;

    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates button
    float* generateVaporiseButton();
    // checks x overlap
    bool xOverlap(pair<float, float> point, pair<float, float> size);
    // checks y overlap
    bool yOverlap(pair<float, float> point, pair<float, float> size);
};

#endif