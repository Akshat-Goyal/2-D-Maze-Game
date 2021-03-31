#include "player_renderer.h"

PlayerRenderer::PlayerRenderer(Shader &shader, float startX, float startY, float speed, float height, float width)
{
    this->shader = shader;
    this->playerX = startX;
    this->playerY = startY;
    this->speed = speed;
    this->height = height;
    this->width = width;
    this->initRenderData();
}

PlayerRenderer::~PlayerRenderer()
{
    glDeleteVertexArrays(1, &this->VAO);
}

pair<float, float> PlayerRenderer::GetPos(){
    return {this->playerX, this->playerY};
}

pair<float, float> PlayerRenderer::GetSize(){
    return {this->width, this->height};
}

void PlayerRenderer::MoveLeft(float dt, MazeRenderer *mazeRenderer){
    // float deltaX = - dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->playerX + deltaX, this->playerY}, this->GetSize())){
    //     this->playerX += deltaX;
    // }
    float dis = mazeRenderer->get_left_gate_x(this->GetPos(), this->GetSize());
    this->playerX = max(dis, this->playerX - dt * this->speed);
}

void PlayerRenderer::MoveRight(float dt, MazeRenderer *mazeRenderer){
    // float deltaX = dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->playerX + deltaX, this->playerY}, this->GetSize())){
    //     this->playerX += deltaX;
    // }
    float dis = mazeRenderer->get_right_gate_x(this->GetPos(), this->GetSize());
    this->playerX = min(dis - this->width, this->playerX + dt * this->speed);
}

void PlayerRenderer::MoveUp(float dt, MazeRenderer *mazeRenderer){
    // float deltaY = - dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->playerX, this->playerY + deltaY}, this->GetSize())){
    //     this->playerY += deltaY;
    // }
    float dis = mazeRenderer->get_up_gate_y(this->GetPos(), this->GetSize());
    this->playerY = max(dis, this->playerY - dt * this->speed);
}

void PlayerRenderer::MoveDown(float dt, MazeRenderer *mazeRenderer){
    // float deltaY = dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->playerX, this->playerY + deltaY}, this->GetSize())){
    //     this->playerY += deltaY;
    // }
    float dis = mazeRenderer->get_down_gate_y(this->GetPos(), this->GetSize());
    this->playerY = min(dis - this->height, this->playerY + dt * this->speed);
}

void PlayerRenderer::DrawPlayer()
{
    // prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(this->playerX, this->playerY, 0.0f));
    this->shader.SetMatrix4("model", model);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_LINES, 0, this->nV);
    glBindVertexArray(0);
}

float* PlayerRenderer::generatePlayer(){
    this->nV = 8, this->mV = 2;
    float *vertices = new float[this->nV * this->mV];
    vertices[0] = 0, vertices[1] = 0;
    vertices[2] = this->height, vertices[3] = 0;

    vertices[4] = 0, vertices[5] = 0;
    vertices[6] = 0, vertices[7] = this->width;

    vertices[8] = 0, vertices[9] = this->width;
    vertices[10] = this->height, vertices[11] = this->width;

    vertices[12] = this->height, vertices[13] = 0;
    vertices[14] = this->height, vertices[15] = this->width;    
    return vertices;
}

void PlayerRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = generatePlayer(); 

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->nV * this->mV * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, this->mV, GL_FLOAT, GL_FALSE, this->mV * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete [] vertices;
}