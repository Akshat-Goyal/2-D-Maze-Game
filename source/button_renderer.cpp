#include "button_renderer.h"

ButtonRenderer::ButtonRenderer(Shader &shader, float startX, float startY, float width, float height)
{
    this->shader = shader;
    this->buttonX = startX;
    this->buttonY = startY;
    this->height = height;
    this->width = width;
    this->done = false;
    this->initRenderData();
}

ButtonRenderer::~ButtonRenderer()
{
    glDeleteVertexArrays(1, &this->VAO);
}

pair<float, float> ButtonRenderer::GetPos(){
    return {this->buttonX, this->buttonY};
}

pair<float, float> ButtonRenderer::GetSize(){
    return {this->width, this->height};
}

void ButtonRenderer::DrawButton(Texture2D &texture)
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

float* ButtonRenderer::generateButton(){
    this->nV = 4, this->mV = 4;
    float *vertices = new float[this->nV * this->mV];

    vertices[0] = this->buttonX, vertices[1] = this->buttonY, vertices[2] = 0, vertices[3] = 0;
    vertices[4] = this->buttonX + this->width, vertices[5] = this->buttonY, vertices[6] = 1, vertices[7] = 0;
    vertices[8] = this->buttonX + this->width, vertices[9] = this->buttonY + this->height, vertices[10] = 1, vertices[11] = 1;
    vertices[12] = this->buttonX, vertices[13] = this->buttonY + this->height, vertices[14] = 0, vertices[15] = 1;

    return vertices;
}

void ButtonRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = this->generateButton(); 

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

bool ButtonRenderer::yOverlap(pair<float, float> point, pair<float, float> size){
    if(point.first + size.first > this->buttonX && point.first < this->buttonX + this->width) return true;
    return false;
}

bool ButtonRenderer::xOverlap(pair<float, float> point, pair<float, float> size){
    if(point.second + size.second > this->buttonY && point.second < this->buttonY + this->height) return true;
    return false;
}

bool ButtonRenderer::DetectCollision(PlayerRenderer *playerRenderer, CoinRenderer **coinRenderers, BombRenderer **bombRenderers, int nCoins, int nBombs){
    if(this->done) return false;
    bool col = this->xOverlap(playerRenderer->GetPos(), playerRenderer->GetSize()) & this->yOverlap(playerRenderer->GetPos(), playerRenderer->GetSize());
    if(col){
        for(int i = 0; i < nCoins; i++){
            coinRenderers[i]->setDone(false);
        }
        for(int i = 0; i < nBombs; i++){
            bombRenderers[i]->setDone(false);
        }
        this->done = true;
    }
    return col;
}