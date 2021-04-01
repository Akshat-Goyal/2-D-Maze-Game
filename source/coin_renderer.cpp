#include "coin_renderer.h"

CoinRenderer::CoinRenderer(Shader &shader, float startX, float startY, float width, float height)
{
    this->shader = shader;
    this->coinX = startX;
    this->coinY = startY;
    this->height = height;
    this->width = width;
    this->done = true;
    this->initRenderData();
}

CoinRenderer::~CoinRenderer()
{
    glDeleteVertexArrays(1, &this->VAO);
}

pair<float, float> CoinRenderer::GetPos(){
    return {this->coinX, this->coinY};
}

pair<float, float> CoinRenderer::GetSize(){
    return {this->width, this->height};
}

void CoinRenderer::DrawCoin(Texture2D &texture)
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

float* CoinRenderer::generateCoin(){
    this->nV = 4, this->mV = 4;
    float *vertices = new float[this->nV * this->mV];

    vertices[0] = this->coinX, vertices[1] = this->coinY, vertices[2] = 0, vertices[3] = 0;
    vertices[4] = this->coinX + this->width, vertices[5] = this->coinY, vertices[6] = 1, vertices[7] = 0;
    vertices[8] = this->coinX + this->width, vertices[9] = this->coinY + this->height, vertices[10] = 1, vertices[11] = 1;
    vertices[12] = this->coinX, vertices[13] = this->coinY + this->height, vertices[14] = 0, vertices[15] = 1;

    return vertices;
}

void CoinRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = this->generateCoin(); 

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

bool CoinRenderer::yOverlap(pair<float, float> point, pair<float, float> size){
    if(point.first + size.first > this->coinX && point.first < this->coinX + this->width) return true;
    return false;
}

bool CoinRenderer::xOverlap(pair<float, float> point, pair<float, float> size){
    if(point.second + size.second > this->coinY && point.second < this->coinY + this->height) return true;
    return false;
}

bool CoinRenderer::DetectCollision(PlayerRenderer *playerRenderer){
    if(this->done) return false;
    bool col = this->xOverlap(playerRenderer->GetPos(), playerRenderer->GetSize()) & this->yOverlap(playerRenderer->GetPos(), playerRenderer->GetSize());
    if(col){
        playerRenderer->updateScore(20);
        this->done = true;
    }
    return col;
}

void CoinRenderer::setDone(bool val){
    this->done = val;
}