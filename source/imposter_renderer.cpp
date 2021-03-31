#include "imposter_renderer.h"

ImposterRenderer::ImposterRenderer(Shader &shader, float startX, float startY, float speed, float height, float width)
{
    this->shader = shader;
    this->imposterX = startX;
    this->imposterY = startY;
    this->speed = speed;
    this->height = height;
    this->width = width;
    this->img_no = 0;
    this->img_gap = 5;
    this->gap_cnt = this->img_gap;
    this->left = false;
    for(int i = 0; i < 5; i++){
        this->images.push_back("imposter_right" + to_string(i) + ".png");
    }
    this->initRenderData();
}

ImposterRenderer::~ImposterRenderer()
{
    glDeleteVertexArrays(1, &this->VAO);
}

pair<float, float> ImposterRenderer::GetPos(){
    return {this->imposterX, this->imposterY};
}

pair<float, float> ImposterRenderer::GetSize(){
    return {this->width, this->height};
}

int ImposterRenderer::ISize(){
    return this->images.size();
}

string ImposterRenderer::GetImage(){
    return this->images[this->img_no];
}

string ImposterRenderer::GetImage(int i){
    return this->images[i];
}

void ImposterRenderer::MoveLeft(float dt, MazeRenderer *mazeRenderer){
    // float deltaX = - dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->imposterX + deltaX, this->imposterY}, this->GetSize())){
    //     this->imposterX += deltaX;
    // }
    float dis = mazeRenderer->get_left_gate_x(this->GetPos(), this->GetSize());
    this->imposterX = max(dis, this->imposterX - dt * this->speed);
    this->left = true;
    this->gap_cnt--;
    if(!this->gap_cnt){
        this->img_no = (this->img_no - 1 + this->ISize()) % this->ISize();  
        this->gap_cnt = this->img_gap;
    }
}

void ImposterRenderer::MoveRight(float dt, MazeRenderer *mazeRenderer){
    // float deltaX = dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->imposterX + deltaX, this->imposterY}, this->GetSize())){
    //     this->imposterX += deltaX;
    // }
    float dis = mazeRenderer->get_right_gate_x(this->GetPos(), this->GetSize());
    this->imposterX = min(dis - this->width, this->imposterX + dt * this->speed);
    this->left = false;
    this->gap_cnt--;
    if(!this->gap_cnt){
        this->img_no = (this->img_no + 1) % this->ISize();
        this->gap_cnt = this->img_gap;
    }
}

void ImposterRenderer::MoveUp(float dt, MazeRenderer *mazeRenderer){
    // float deltaY = - dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->imposterX, this->imposterY + deltaY}, this->GetSize())){
    //     this->imposterY += deltaY;
    // }
    float dis = mazeRenderer->get_up_gate_y(this->GetPos(), this->GetSize());
    this->imposterY = max(dis, this->imposterY - dt * this->speed);
    this->gap_cnt--;
    if(!this->gap_cnt){
        this->img_no = (this->img_no - 1 + this->ISize()) % this->ISize();  
        this->gap_cnt = this->img_gap;
    }
}

void ImposterRenderer::MoveDown(float dt, MazeRenderer *mazeRenderer){
    // float deltaY = dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->imposterX, this->imposterY + deltaY}, this->GetSize())){
    //     this->imposterY += deltaY;
    // }
    float dis = mazeRenderer->get_down_gate_y(this->GetPos(), this->GetSize());
    this->imposterY = min(dis - this->height, this->imposterY + dt * this->speed);
    this->gap_cnt--;
    if(!this->gap_cnt){
        this->img_no = (this->img_no + 1) % this->ISize();
        this->gap_cnt = this->img_gap;
    }
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

void ImposterRenderer::DrawImposter(Texture2D &texture)
{
    // prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(this->imposterX, this->imposterY, 0.0f));
    if(this->left){
        model = glm::scale(glm::translate(model, glm::vec3(this->width, 0.0f, 0.0f)), glm::vec3(-1.0f, 1.0f, 1.0f));
    }
    this->shader.SetMatrix4("model", model);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, this->nV);
    glBindVertexArray(0);
}

float* ImposterRenderer::generateImposter(){
    this->nV = 4, this->mV = 4;
    float *vertices = new float[this->nV * this->mV];

    vertices[0] = 0, vertices[1] = 0, vertices[2] = 0, vertices[3] = 0;
    vertices[4] = this->width, vertices[5] = 0, vertices[6] = 1, vertices[7] = 0;
    vertices[8] = this->width, vertices[9] = this->height, vertices[10] = 1, vertices[11] = 1;
    vertices[12] = 0, vertices[13] = this->height, vertices[14] = 0, vertices[15] = 1;

    return vertices;    
}

void ImposterRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = this->generateImposter(); 

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

bool ImposterRenderer::overlapY(pair<float, float> point, pair<float, float> size){
    if(this->imposterY > point.second && this->imposterY < point.second + size.second) return true;
    if(this->imposterY + this->height > point.second && this->imposterY + this->height < point.second + size.second) return true;
    if(this->imposterY <= point.second && this->imposterY + this->height >= point.second + size.second) return true;
    return false;
}

bool ImposterRenderer::overlapX(pair<float, float> point, pair<float, float> size){
    if(this->imposterX > point.first && this->imposterX < point.first + size.first) return true;
    if(this->imposterX + this->width > point.first && this->imposterX + this->width < point.first + size.first) return true;
    if(this->imposterX <= point.first && this->imposterX + this->width >= point.first + size.first) return true;
    return false;
}

bool ImposterRenderer::DetectCollisionWithPlayer(pair<float, float> point, pair<float, float> size){
    return this->overlapX(point, size) & this->overlapY(point, size);
}