#ifndef PLAYER_RENDERER_H
#define PLAYER_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"
#include "maze_renderer.h"
#include <vector>
#include <string>

using namespace std;

class PlayerRenderer
{
public:
    // Constructor (inits shaders/shapes)
    PlayerRenderer(Shader &shader, float startX, float startY, float speed, float width, float height);
    // Destructor
    ~PlayerRenderer();
    // Renders
    void DrawPlayer(Texture2D &texture);
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
    // Detects collision with imposter
    bool DetectCollisionWithImposter(pair<float, float> point, pair<float, float> size);
    // Returns image loc
    string GetImage();
    // Returns image loc
    string GetImage(int i);
    // no. of images
    int ISize();
    // updates score
    void updateScore(int val);
    // returns score
    int getScore();
    // updates health
    void updateHealth(int val);
    // returns health
    int getHealth();
    // set light
    void setLight(string val);
    // returns light
    string getLight();
    // toggles light
    void toggleLight();
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;

    float playerX, playerY, speed, height, width;
    int nV, mV, img_no, img_gap, gap_cnt, score, health;
    bool left;
    string light;
    vector<string> images;

    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates player
    float* generatePlayer();
    // Returns true if overlap in x direction
    bool overlapX(pair<float, float> point, pair<float, float> size);
    // Returns true if overlap in y direction
    bool overlapY(pair<float, float> point, pair<float, float> size);
};

#endif