#include "bomb_renderer.h"

BombRenderer::BombRenderer(Shader &shader, float startX, float startY, float height, float width)
{
    this->shader = shader;
    this->bombX = startX;
    this->bombY = startY;
    this->height = height;
    this->width = width;
    this->done = false;
    this->initRenderData();
}

BombRenderer::~BombRenderer()
{
    glDeleteVertexArrays(1, &this->VAO);
}

pair<float, float> BombRenderer::GetPos(){
    return {this->bombX, this->bombY};
}

pair<float, float> BombRenderer::GetSize(){
    return {this->width, this->height};
}

void BombRenderer::DrawBomb(Texture2D &texture)
{
    if(this->done) return;

    // prepare transformations
    this->shader.Use();

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, this->nV);
    glBindVertexArray(0);
}

float* BombRenderer::generateBomb(){
    this->nV = 4, this->mV = 4;
    float *vertices = new float[this->nV * this->mV];

    vertices[0] = this->bombX, vertices[1] = this->bombY, vertices[2] = 0, vertices[3] = 0;
    vertices[4] = this->bombX + this->width, vertices[5] = this->bombY, vertices[6] = 1, vertices[7] = 0;
    vertices[8] = this->bombX + this->width, vertices[9] = this->bombY + this->height, vertices[10] = 1, vertices[11] = 1;
    vertices[12] = this->bombX, vertices[13] = this->bombY + this->height, vertices[14] = 0, vertices[15] = 1;

    return vertices;
}

void BombRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = this->generateBomb(); 

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

bool BombRenderer::yOverlap(pair<float, float> point, pair<float, float> size){
    if(point.first + size.first > this->bombX && point.first < this->bombX + this->width) return true;
    return false;
}

bool BombRenderer::xOverlap(pair<float, float> point, pair<float, float> size){
    if(point.second + size.second > this->bombY && point.second < this->bombY + this->height) return true;
    return false;
}

bool BombRenderer::DetectCollision(PlayerRenderer *playerRenderer){
    if(this->done) return false;
    bool col = this->xOverlap(playerRenderer->GetPos(), playerRenderer->GetSize()) & this->yOverlap(playerRenderer->GetPos(), playerRenderer->GetSize());
    if(col){
        playerRenderer->updateScore(-20);
        playerRenderer->updateHealth(-20);
        this->done = true;
    }
    return col;
}