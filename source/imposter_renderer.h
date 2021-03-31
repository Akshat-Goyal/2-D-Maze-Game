#ifndef IMPOSTER_RENDERER_H
#define IMPOSTER_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"
#include "maze_renderer.h"
#include "player_renderer.h"
#include <vector>
#include <string>

using namespace std;

class ImposterRenderer
{
public:
    // Constructor (inits shaders/shapes)
    ImposterRenderer(Shader &shader, float startX, float startY, float speed, float height, float width);
    // Destructor
    ~ImposterRenderer();
    // Renders
    void DrawImposter(Texture2D &texture);
    // Moves left
    void MoveLeft(float dt, MazeRenderer *mazeRenderer);
    // Moves right
    void MoveRight(float dt, MazeRenderer *mazeRenderer);
    // Moves up
    void MoveUp(float dt, MazeRenderer *mazeRenderer);
    // Moves down
    void MoveDown(float dt, MazeRenderer *mazeRenderer);
    // Imposter Pos
    pair<float, float> GetPos();
    // Imposter Size
    pair<float, float> GetSize();
    // Moves
    void MoveImposter(float dt, MazeRenderer *mazeRenderer, PlayerRenderer *playerRenderer);
    // Detects collision with player
    bool DetectCollisionWithPlayer(pair<float, float> point, pair<float, float> size);
    // Returns image loc
    string GetImage();
    // Returns image loc
    string GetImage(int i);
    // no. of images
    int ISize();
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;

    float imposterX, imposterY, speed, height, width;
    pair<float, float> prevMove = {0, 0};
    int nV, mV, img_no, img_gap, gap_cnt;
    bool left;
    vector<string> images;

    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates imposter
    float* generateImposter();
    // Returns true if overlap in x direction
    bool overlapX(pair<float, float> point, pair<float, float> size);
    // Returns true if overlap in y direction
    bool overlapY(pair<float, float> point, pair<float, float> size);
};

#endif