#include "vaporiseButton_renderer.h"

VaporiseButtonRenderer::VaporiseButtonRenderer(Shader &shader, float startX, float startY, float width, float height)
{
    this->shader = shader;
    this->vaporiseButtonX = startX;
    this->vaporiseButtonY = startY;
    this->height = height;
    this->width = width;
    this->done = false;
    this->initRenderData();
}

VaporiseButtonRenderer::~VaporiseButtonRenderer()
{
    glDeleteVertexArrays(1, &this->VAO);
}

pair<float, float> VaporiseButtonRenderer::GetPos(){
    return {this->vaporiseButtonX, this->vaporiseButtonY};
}

pair<float, float> VaporiseButtonRenderer::GetSize(){
    return {this->width, this->height};
}

void VaporiseButtonRenderer::DrawVaporiseButton(Texture2D &texture)
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

float* VaporiseButtonRenderer::generateVaporiseButton(){
    this->nV = 4, this->mV = 4;
    float *vertices = new float[this->nV * this->mV];

    vertices[0] = this->vaporiseButtonX, vertices[1] = this->vaporiseButtonY, vertices[2] = 0, vertices[3] = 0;
    vertices[4] = this->vaporiseButtonX + this->width, vertices[5] = this->vaporiseButtonY, vertices[6] = 1, vertices[7] = 0;
    vertices[8] = this->vaporiseButtonX + this->width, vertices[9] = this->vaporiseButtonY + this->height, vertices[10] = 1, vertices[11] = 1;
    vertices[12] = this->vaporiseButtonX, vertices[13] = this->vaporiseButtonY + this->height, vertices[14] = 0, vertices[15] = 1;

    return vertices;
}

void VaporiseButtonRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = this->generateVaporiseButton(); 

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

bool VaporiseButtonRenderer::yOverlap(pair<float, float> point, pair<float, float> size){
    if(point.first + size.first > this->vaporiseButtonX && point.first < this->vaporiseButtonX + this->width) return true;
    return false;
}

bool VaporiseButtonRenderer::xOverlap(pair<float, float> point, pair<float, float> size){
    if(point.second + size.second > this->vaporiseButtonY && point.second < this->vaporiseButtonY + this->height) return true;
    return false;
}

bool VaporiseButtonRenderer::DetectCollision(PlayerRenderer *playerRenderer, ImposterRenderer *imposterRenderer){
    if(this->done) return false;
    bool col = this->xOverlap(playerRenderer->GetPos(), playerRenderer->GetSize()) & this->yOverlap(playerRenderer->GetPos(), playerRenderer->GetSize());
    if(col){
        imposterRenderer->setVaporised(true);
        this->done = true;
    }
    return col;
}