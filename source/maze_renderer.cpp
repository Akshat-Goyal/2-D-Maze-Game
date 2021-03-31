#include "maze_renderer.h"
#include <iostream>
MazeRenderer::MazeRenderer(Shader &shader, float startX, float startY, float height, float width, int nX, int nY, pair<int, int> player, pair<int, int> imposter)
{
    this->shader = shader;
    this->startX = startX;
    this->startY = startY;
    this->height = height;
    this->width = width;
    this->nX = nX;
    this->nY = nY;
    this->edgeX = width / nX;
    this->edgeY = height / nY;
    this->initRenderData(player, imposter);
}

MazeRenderer::~MazeRenderer()
{
    {
        for(int j = 0; j < this->nY; j++) delete [] this->maze[j];
        delete[] this->maze;
    }
    {
        int n = this->nX * this->nY;
        for(int i = 0; i < n; i++) delete [] this->graph[i];
        delete[] this->graph;
    }
    glDeleteVertexArrays(1, &this->VAO);
}

void MazeRenderer::DrawMaze()
{
    // prepare transformations
    this->shader.Use();

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_LINES, 0, this->nV);
    glBindVertexArray(0);
}

bool MazeRenderer::is_in_room(pair<float, float> point, pair<float, float> size){
    if(this->get_room_no(point) != this->get_room_no({point.first + size.first, point.second})) return false;
    if(this->get_room_no(point) != this->get_room_no({point.first, point.second + size.second})) return false;
    return true;
}

pair<int, int> MazeRenderer::get_room_no(pair<float, float> point){
    int pX = (int)((point.first - this->startX) / this->edgeX);
    int pY = (int)((point.second - this->startY) / this->edgeY);
    return {pX, pY};
}

bool MazeRenderer::is_Y_overlapping(pair<float, float> point, pair<float, float> size, pair<pair<float, float>, pair<float, float>> gate){
    if(gate.first.second > point.second && gate.first.second < point.second + size.second) return true;
    if(gate.second.second > point.second && gate.second.second < point.second + size.second) return true;
    if(gate.first.second <= point.second && gate.second.second >= point.second + size.second) return true;
    return false;
}

bool MazeRenderer::is_X_overlapping(pair<float, float> point, pair<float, float> size, pair<pair<float, float>, pair<float, float>> gate){
    if(gate.first.first > point.first && gate.first.first < point.first + size.first) return true;
    if(gate.second.first > point.first && gate.second.first < point.first + size.first) return true;
    if(gate.first.first <= point.first && gate.second.first >= point.first + size.first) return true;
    return false;
}

float MazeRenderer::get_left_gate_x(pair<float, float> point, pair<float, float> size){
    float mx = this->startX - this->edgeX;
    for(int j = 0; j < this->nY; j++){
        for(int i = 0; i < this->nX; i++){
            for(int k = 0; k < 4; k++){
                if(this->get_room(i, j).gate(k) <= 0 && this->is_Y_overlapping(point, size, this->get_room(i, j).get_gate_coord(k))){
                    if(point.first >= this->get_room(i, j).get_gate_coord(k).second.first)
                        mx = max(mx, this->get_room(i, j).get_gate_coord(k).second.first);
                }
            }
        }
    }
    return mx;
}

float MazeRenderer::get_right_gate_x(pair<float, float> point, pair<float, float> size){
    point = {point.first + size.first, point.second};
    float mn = this->startX + this->width + this->edgeX;
    for(int j = 0; j < this->nY; j++){
        for(int i = 0; i < this->nX; i++){
            for(int k = 0; k < 4; k++){
                if(this->get_room(i, j).gate(k) <= 0 && this->is_Y_overlapping(point, size, this->get_room(i, j).get_gate_coord(k))){
                    if(point.first <= this->get_room(i, j).get_gate_coord(k).first.first)
                        mn = min(mn, this->get_room(i, j).get_gate_coord(k).first.first);
                }
            }
        }
    }
    return mn;
}

float MazeRenderer::get_up_gate_y(pair<float, float> point, pair<float, float> size){
    float mx = this->startY - this->edgeY;
    for(int j = 0; j < this->nY; j++){
        for(int i = 0; i < this->nX; i++){
            for(int k = 0; k < 4; k++){
                if(this->get_room(i, j).gate(k) <= 0 && this->is_X_overlapping(point, size, this->get_room(i, j).get_gate_coord(k))){
                    if(point.second >= this->get_room(i, j).get_gate_coord(k).second.second)
                        mx = max(mx, this->get_room(i, j).get_gate_coord(k).second.second);
                }
            }
        }
    }
    return mx;
}

float MazeRenderer::get_down_gate_y(pair<float, float> point, pair<float, float> size){
    point = {point.first, point.second + size.second};
    float mn = this->startY + this->height + this->edgeY;
    for(int j = 0; j < this->nY; j++){
        for(int i = 0; i < this->nX; i++){
            for(int k = 0; k < 4; k++){
                if(this->get_room(i, j).gate(k) <= 0 && this->is_X_overlapping(point, size, this->get_room(i, j).get_gate_coord(k))){
                    if(point.second <= this->get_room(i, j).get_gate_coord(k).first.second)
                        mn = min(mn, this->get_room(i, j).get_gate_coord(k).first.second);
                }
            }
        }
    }
    return mn;
}

bool MazeRenderer::check_collision(pair<float, float> point, pair<float, float> size, pair<pair<float, float>, pair<float, float>> gate){
    return this->is_X_overlapping(point, size, gate) & this->is_Y_overlapping(point, size, gate);
}

bool MazeRenderer::detect_collision(pair<float, float> point, pair<float, float> size){
    for(int j = 0; j < this->nY; j++){
        for(int i = 0; i < this->nX; i++){
            for(int k = 0; k < 4; k++){
                if(this->get_room(i, j).gate(k) > 0) continue;
                bool col = check_collision(point, size, this->get_room(i, j).get_gate_coord(k));
                if(col) return col;
            }
        }
    }
    return false;
}

Room& MazeRenderer::get_room(pair<int, int> p){
    return this->maze[p.second][p.first];
}

Room& MazeRenderer::get_room(int x, int y){
    return this->maze[y][x];
}

bool MazeRenderer::is_blocked(pair<int, int> p){
    for(int i = 0; i < 4; i++){
        if(this->get_room(p).gate(i) >= 0) return false;
    }
    return true;
}

pair<int, int> MazeRenderer::move_to(pair<int, int> p, int gn){
    pair<int, int> next;
    switch (gn){
        case 0: next = {p.first - 1, p.second}; break;
        case 1: next = {p.first + 1, p.second}; break;
        case 2: next = {p.first, p.second - 1}; break;
        case 3: next = {p.first, p.second + 1}; break;
    }
    return next;
}

int MazeRenderer::blocked_gates(pair<int, int> p){
    int gates = 0;
    for(int i = 0; i < 4; i++){
        if(this->get_room(p).gate(i) < 0) gates |= 1 << i;
    }
    return gates;
}

bool MazeRenderer::is_visited(pair<int, int> p, bool **visit){
    if(p.first < 0 || p.first >= this->nX || p.second < 0 || p.second >= this->nY || visit[p.second][p.first] == 0) return false;
    return true;
}

int MazeRenderer::visited_rooms(pair<int, int> p, bool **visit){
    int rooms = 0;
    for(int i = 0; i < 4; i++){
        if(this->is_visited(this->move_to(p, i), visit)) rooms |= 1 << i;
    }
    return rooms;
}

int MazeRenderer::side_gates(pair<int, int> p){
    int gates = 0;
    if(p.first == 0) gates |= (1 << 0);
    if(p.second == 0) gates |= (1 << 2);
    if(p.first == this->nX - 1) gates |= (1 << 1);
    if(p.second == this->nY - 1) gates |= (1 << 3);
    return gates;
}

int MazeRenderer::random_gate(pair<int, int> p, bool **visit, bool exit){
    int closed_gates = this->blocked_gates(p) | this->visited_rooms(p, visit);
    if(!exit) closed_gates |= this->side_gates(p);
    int gates = ((1 << 4) - 1) - closed_gates;
    if(!gates) return -1;
    int rg = rand() % __builtin_popcount(gates);
    for(int i = 0; i < 4; i++){
        if(rg == 0 && (gates & (1 << i)) != 0) return i;
        if(gates & (1 << i)) rg--;
    }
    return -1;
}

bool MazeRenderer::is_valid(pair<int, int> p){
    if(p.first < 0 || p.first >= this->nX || p.second < 0 || p.second >= this->nY) return false;
    return true;
}

void MazeRenderer::side_closed_room(pair<int, int> p, Room &room){
    if(p.first == 0 && room.gate(0) == 0) room.set(0, -1);
    if(p.second == 0 && room.gate(2) == 0) room.set(2, -1);
    if(p.first == this->nX - 1 && room.gate(1) == 0) room.set(1, -1);
    if(p.second == this->nY - 1 && room.gate(3) == 0) room.set(3, -1);
}

void MazeRenderer::openGates(pair<int, int> p, bool exit){
    bool **visit = new bool* [this->nY];
    for(int j = 0; j < this->nY; j++) visit[j] = new bool[this->nX]();

    this->side_closed_room(p, this->get_room(p));
    visit[p.second][p.first] = true;

    srand(time(NULL));
    while(this->is_valid(p)){
        int gn = this->random_gate(p, visit, exit);
        if(gn == -1) break;
        this->get_room(p).open(gn);
        p = this->move_to(p, gn);
        if(this->is_valid(p)){
            this->get_room(p).open_opp(gn);
            visit[p.second][p.first] = true;
        }
    }

    for(int j = 0; j < this->nY; j++) delete [] visit[j];
    delete[] visit;
}

void MazeRenderer::count_edges(){
    int edges = 0;
    for(int j = 0; j < this->nY; j++){
        for(int i = 0; i < this->nX; i++){
            for(int k = 0; k < 4; k++){
                if(this->get_room(i, j).gate(k) <= 0) edges++;
            }
        }
    }
    this->nV = 2 * edges;
    this->mV = 2;
}

float* MazeRenderer::generateMaze(pair<int, int> player, pair<int, int> imposter){
    this->maze = new Room* [this->nY];
    for(int j = 0; j < this->nY; j++){
        this->maze[j] = new Room[this->nX]();
        for(int i = 0; i < this->nX; i++) this->get_room(i, j).set_gate_coords({i, j}, {this->startX, this->startY}, {this->edgeX, this->edgeY});
    }
    
    this->openGates(player, true);
    this->openGates(imposter, false);

    this->distance_graph();

    this->count_edges();

    float *vertices = new float[this->nV * this->mV];
    for(int j = 0, cnt = 0; j < nY; j++){
        for(int i = 0; i < nX; i++){
            for(int k = 0; k < 4; k++){
                if(this->get_room(i, j).gate(k) <= 0){
                    this->get_room(i, j).fill_gate_coord(vertices + cnt, k); cnt += 4;
                }
            }
        }
    }

    return vertices;
}

void MazeRenderer::initRenderData(pair<int, int> player, pair<int, int> imposter)
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = generateMaze(player, imposter); 

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

void MazeRenderer::distance_graph(){
    auto idx = [&] (pair<int, int> loc) {return loc.second * this->nX + loc.first;};

    int n = this->nX * this->nY, INF = 1e8;
    this->graph = new int* [n];
    for(int i = 0; i < n; i++){
        this->graph[i] = new int[n];
        for(int j = 0; j < n; j++) this->graph[i][j] = INF;
    }
    
    for(int j = 0; j < this->nY; j++){
        for(int i = 0; i < this->nX; i++){
            this->graph[idx({i, j})][idx({i, j})] = 0;
            for(int k = 0; k < 2; k++){
                pair<int, int> nxt = this->move_to({i, j}, k);
                if(this->is_valid(nxt) && this->get_room(i, j).gate(k) > 0) this->graph[idx({i, j})][idx(nxt)] = this->edgeX;
            }
            for(int k = 2; k < 4; k++){
                pair<int, int> nxt = this->move_to({i, j}, k);
                if(this->is_valid(nxt) && this->get_room(i, j).gate(k) > 0) this->graph[idx({i, j})][idx(nxt)] = this->edgeY;
            }
        }
    }

    for(int k = 0; k < n; k++){
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                if(this->graph[i][j] > this->graph[i][k] + this->graph[k][j]){
                    this->graph[i][j] = this->graph[i][k] + this->graph[k][j];
                }
            }
        }
    }
}

pair<float, float> MazeRenderer::find_path(pair<float, float> player, pair<float, float> imposter){
    auto idx = [&] (pair<int, int> loc) {return loc.second * this->nX + loc.first;};
    auto equate = [] (float a, float b, float eps = 1e-4) {return fabs(a - b) <= eps ? true: false;};

    pair<int, int> pn = this->get_room_no(player);
    pair<int, int> in = this->get_room_no(imposter);

    if(pn == in) 
        return {player.first - imposter.first, player.second - imposter.second};
    else{
        pair<int, int> nxt = this->move_to(in, 0);
        if(this->is_valid(nxt) && equate(this->graph[idx(in)][idx(nxt)] + this->graph[idx(nxt)][idx(pn)], this->graph[idx(in)][idx(pn)])){
            return {-this->edgeX, 0};
        }
        nxt = this->move_to(in, 1);
        if(this->is_valid(nxt) && equate(this->graph[idx(in)][idx(nxt)] + this->graph[idx(nxt)][idx(pn)], this->graph[idx(in)][idx(pn)])){
            return {this->edgeX, 0};
        }
        nxt = this->move_to(in, 2);
        if(this->is_valid(nxt) && equate(this->graph[idx(in)][idx(nxt)] + this->graph[idx(nxt)][idx(pn)], this->graph[idx(in)][idx(pn)])){
            return {0, -this->edgeY};
        }
        nxt = this->move_to(in, 3);
        if(this->is_valid(nxt) && equate(this->graph[idx(in)][idx(nxt)] + this->graph[idx(nxt)][idx(pn)], this->graph[idx(in)][idx(pn)])){
            return {0, this->edgeY};
        }
        return {0, 0};
    }
}