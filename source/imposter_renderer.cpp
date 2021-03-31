#include "imposter_renderer.h"

ImposterRenderer::ImposterRenderer(Shader &shader, float startX, float startY, float speed, float height, float width)
{
    this->shader = shader;
    this->imposterX = startX;
    this->imposterY = startY;
    this->speed = speed;
    this->height = height;
    this->width = width;
    this->initRenderData();
}

ImposterRenderer::~ImposterRenderer()
{
    glDeleteVertexArrays(1, &this->VAO);
}

void ImposterRenderer::DrawImposter()
{
    // prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(this->imposterX, this->imposterY, 0.0f));
    this->shader.SetMatrix4("model", model);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_LINES, 0, this->nV);
    glBindVertexArray(0);
}

pair<float, float> ImposterRenderer::GetPos(){
    return {this->imposterX, this->imposterY};
}

pair<float, float> ImposterRenderer::GetSize(){
    return {this->width, this->height};
}

void ImposterRenderer::MoveLeft(float dt, MazeRenderer *mazeRenderer){
    // float deltaX = - dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->imposterX + deltaX, this->imposterY}, this->GetSize())){
    //     this->imposterX += deltaX;
    // }
    float dis = mazeRenderer->get_left_gate_x(this->GetPos(), this->GetSize());
    this->imposterX = max(dis, this->imposterX - dt * this->speed);
}

void ImposterRenderer::MoveRight(float dt, MazeRenderer *mazeRenderer){
    // float deltaX = dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->imposterX + deltaX, this->imposterY}, this->GetSize())){
    //     this->imposterX += deltaX;
    // }
    float dis = mazeRenderer->get_right_gate_x(this->GetPos(), this->GetSize());
    this->imposterX = min(dis - this->width, this->imposterX + dt * this->speed);
}

void ImposterRenderer::MoveUp(float dt, MazeRenderer *mazeRenderer){
    // float deltaY = - dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->imposterX, this->imposterY + deltaY}, this->GetSize())){
    //     this->imposterY += deltaY;
    // }
    float dis = mazeRenderer->get_up_gate_y(this->GetPos(), this->GetSize());
    this->imposterY = max(dis, this->imposterY - dt * this->speed);
}

void ImposterRenderer::MoveDown(float dt, MazeRenderer *mazeRenderer){
    // float deltaY = dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->imposterX, this->imposterY + deltaY}, this->GetSize())){
    //     this->imposterY += deltaY;
    // }
    float dis = mazeRenderer->get_down_gate_y(this->GetPos(), this->GetSize());
    this->imposterY = min(dis - this->height, this->imposterY + dt * this->speed);
}

void ImposterRenderer::MoveImposter(float dt, MazeRenderer *mazeRenderer, PlayerRenderer *playerRenderer){
    pair<float, float> vec;
    if(!mazeRenderer->is_in_room(this->GetPos(), this->GetSize())){
        vec = this->prevMove;
    }
    else{
        vec = mazeRenderer->find_path(playerRenderer->GetPos(), this->GetPos());
        this->prevMove = vec;
    }
    if(vec.first < 0) this->MoveLeft(-vec.first * dt, mazeRenderer);
    else this->MoveRight(vec.first * dt, mazeRenderer);
    if(vec.second < 0) this->MoveUp(-vec.second * dt, mazeRenderer);
    else this->MoveDown(vec.second * dt, mazeRenderer);
}

float* ImposterRenderer::generateImposter(){
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

void ImposterRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = generateImposter(); 

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