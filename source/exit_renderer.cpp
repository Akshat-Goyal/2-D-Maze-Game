#include "exit_renderer.h"

ExitRenderer::ExitRenderer(Shader &shader, float startX, float startY, float width, float height)
{
    this->shader = shader;
    this->exitX = startX;
    this->exitY = startY;
    this->height = height;
    this->width = width;
    this->done = true;
    this->initRenderData();
}

ExitRenderer::~ExitRenderer()
{
    glDeleteVertexArrays(1, &this->VAO);
}

pair<float, float> ExitRenderer::GetPos(){
    return {this->exitX, this->exitY};
}

pair<float, float> ExitRenderer::GetSize(){
    return {this->width, this->height};
}

void ExitRenderer::DrawExit(Texture2D &texture)
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

float* ExitRenderer::generateExit(){
    this->nV = 4, this->mV = 4;
    float *vertices = new float[this->nV * this->mV];

    vertices[0] = this->exitX, vertices[1] = this->exitY, vertices[2] = 0, vertices[3] = 0;
    vertices[4] = this->exitX + this->width, vertices[5] = this->exitY, vertices[6] = 1, vertices[7] = 0;
    vertices[8] = this->exitX + this->width, vertices[9] = this->exitY + this->height, vertices[10] = 1, vertices[11] = 1;
    vertices[12] = this->exitX, vertices[13] = this->exitY + this->height, vertices[14] = 0, vertices[15] = 1;

    return vertices;
}

void ExitRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = this->generateExit(); 

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

bool ExitRenderer::yOverlap(pair<float, float> point, pair<float, float> size){
    if(point.first + size.first > this->exitX && point.first < this->exitX + this->width) return true;
    return false;
}

bool ExitRenderer::xOverlap(pair<float, float> point, pair<float, float> size){
    if(point.second + size.second > this->exitY && point.second < this->exitY + this->height) return true;
    return false;
}

bool ExitRenderer::DetectCollision(PlayerRenderer *playerRenderer){
    if(this->done) return false;
    bool col = this->xOverlap(playerRenderer->GetPos(), playerRenderer->GetSize()) & this->yOverlap(playerRenderer->GetPos(), playerRenderer->GetSize());
    if(col){
        // this->done = true;
    }
    return col;
}

void ExitRenderer::setDone(bool val){
    this->done = val;
}