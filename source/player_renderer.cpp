#include "player_renderer.h"

PlayerRenderer::PlayerRenderer(Shader &shader, float startX, float startY, float speed, float height, float width)
{
    this->shader = shader;
    this->playerX = startX;
    this->playerY = startY;
    this->speed = speed;
    this->height = height;
    this->width = width;
    this->score = 0;
    this->health = 100;
    this->img_no = 0;
    this->img_gap = 5;
    this->gap_cnt = this->img_gap;
    this->left = false;
    for(int i = 0; i < 5; i++){
        this->images.push_back("amongUs_right" + to_string(i) + ".png");
    }
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

int PlayerRenderer::ISize(){
    return this->images.size();
}

string PlayerRenderer::GetImage(){
    return this->images[this->img_no];
}

string PlayerRenderer::GetImage(int i){
    return this->images[i];
}

void PlayerRenderer::updateScore(int val){
    this->score += val;
}

int PlayerRenderer::getScore(){
    return this->score;
}

void PlayerRenderer::updateHealth(int val){
    this->health = max(0, this->health + val);
}

int PlayerRenderer::getHealth(){
    return this->health;
}

void PlayerRenderer::MoveLeft(float dt, MazeRenderer *mazeRenderer){
    // float deltaX = - dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->playerX + deltaX, this->playerY}, this->GetSize())){
    //     this->playerX += deltaX;
    // }
    float dis = mazeRenderer->get_left_gate_x(this->GetPos(), this->GetSize());
    this->playerX = max(dis, this->playerX - dt * this->speed);
    this->left = true;
    this->gap_cnt--;
    if(!this->gap_cnt){
        this->img_no = (this->img_no - 1 + this->ISize()) % this->ISize();  
        this->gap_cnt = this->img_gap;
    }
}

void PlayerRenderer::MoveRight(float dt, MazeRenderer *mazeRenderer){
    // float deltaX = dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->playerX + deltaX, this->playerY}, this->GetSize())){
    //     this->playerX += deltaX;
    // }
    float dis = mazeRenderer->get_right_gate_x(this->GetPos(), this->GetSize());
    this->playerX = min(dis - this->width, this->playerX + dt * this->speed);
    this->left = false;
    this->gap_cnt--;
    if(!this->gap_cnt){
        this->img_no = (this->img_no + 1) % this->ISize();
        this->gap_cnt = this->img_gap;
    }
}

void PlayerRenderer::MoveUp(float dt, MazeRenderer *mazeRenderer){
    // float deltaY = - dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->playerX, this->playerY + deltaY}, this->GetSize())){
    //     this->playerY += deltaY;
    // }
    float dis = mazeRenderer->get_up_gate_y(this->GetPos(), this->GetSize());
    this->playerY = max(dis, this->playerY - dt * this->speed);
    this->gap_cnt--;
    if(!this->gap_cnt){
        this->img_no = (this->img_no - 1 + this->ISize()) % this->ISize();  
        this->gap_cnt = this->img_gap;
    }
}

void PlayerRenderer::MoveDown(float dt, MazeRenderer *mazeRenderer){
    // float deltaY = dt * this->speed;
    // if(!mazeRenderer->detect_collision({this->playerX, this->playerY + deltaY}, this->GetSize())){
    //     this->playerY += deltaY;
    // }
    float dis = mazeRenderer->get_down_gate_y(this->GetPos(), this->GetSize());
    this->playerY = min(dis - this->height, this->playerY + dt * this->speed);
    this->gap_cnt--;
    if(!this->gap_cnt){
        this->img_no = (this->img_no + 1) % this->ISize();
        this->gap_cnt = this->img_gap;
    }
}

void PlayerRenderer::DrawPlayer(Texture2D &texture)
{
    // prepare transformations
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(this->playerX, this->playerY, 0.0f));
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

float* PlayerRenderer::generatePlayer(){
    this->nV = 4, this->mV = 4;
    float *vertices = new float[this->nV * this->mV];

    vertices[0] = 0, vertices[1] = 0, vertices[2] = 0, vertices[3] = 0;
    vertices[4] = this->width, vertices[5] = 0, vertices[6] = 1, vertices[7] = 0;
    vertices[8] = this->width, vertices[9] = this->height, vertices[10] = 1, vertices[11] = 1;
    vertices[12] = 0, vertices[13] = this->height, vertices[14] = 0, vertices[15] = 1;

    return vertices;
}

void PlayerRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = this->generatePlayer(); 

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

bool PlayerRenderer::overlapY(pair<float, float> point, pair<float, float> size){
    if(this->playerY > point.second && this->playerY < point.second + size.second) return true;
    if(this->playerY + this->height > point.second && this->playerY + this->height < point.second + size.second) return true;
    if(this->playerY <= point.second && this->playerY + this->height >= point.second + size.second) return true;
    return false;
}

bool PlayerRenderer::overlapX(pair<float, float> point, pair<float, float> size){
    if(this->playerX > point.first && this->playerX < point.first + size.first) return true;
    if(this->playerX + this->width > point.first && this->playerX + this->width < point.first + size.first) return true;
    if(this->playerX <= point.first && this->playerX + this->width >= point.first + size.first) return true;
    return false;
}

bool PlayerRenderer::DetectCollisionWithImposter(pair<float, float> point, pair<float, float> size){
    return this->overlapX(point, size) & this->overlapY(point, size);
}