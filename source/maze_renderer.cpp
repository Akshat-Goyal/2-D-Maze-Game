#include "maze_renderer.h"

MazeRenderer::MazeRenderer(Shader &shader, float startX, float startY, float height, float width, int nX, int nY)
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
    this->initRenderData();
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

int MazeRenderer::side_rooms(pair<int, int> p){
    int rooms = 0;
    if(p.first == 0) rooms |= (1 << 0);
    if(p.first == this->nX - 1) rooms |= (1 << 1);
    if(p.second == 0) rooms |= (1 << 2);
    if(p.second == this->nY - 1) rooms |= (1 << 3);
    return rooms;
}

bool MazeRenderer::is_visited(pair<int, int> p, bool **visit){
    if(p.first < 0 || p.first >= this->nX || p.second < 0 || p.second >= this->nY || visit[p.second][p.first] == false) return false;
    return true;
}

pair<int, int> MazeRenderer::move_to(pair<int, int> p, int gn){
    pair<int, int> nxt;
    switch (gn){
        case 0: nxt = {p.first - 1, p.second}; break;
        case 1: nxt = {p.first + 1, p.second}; break;
        case 2: nxt = {p.first, p.second - 1}; break;
        case 3: nxt = {p.first, p.second + 1}; break;
    }
    return nxt;
}

int MazeRenderer::visited_rooms(pair<int, int> p, bool **visit){
    int rooms = 0;
    for(int i = 0; i < 4; i++){
        if(this->is_visited(this->move_to(p, i), visit)) rooms |= 1 << i;
    }
    return rooms;
}

int MazeRenderer::unvisited_rooms(pair<int, int> p, bool **visit){
    return ((1 << 4) - 1) ^ (this->visited_rooms(p, visit) | this->side_rooms(p));
}

bool MazeRenderer::has_unvisited_neigh(pair<int, int> p, bool **visit){
    if(this->unvisited_rooms(p, visit)) return true;
    return false;
}

pair<int, int> MazeRenderer::random_unvisited_neigh(pair<int, int> p, bool **visit){
    int unvisited_neigh = this->unvisited_rooms(p, visit);
    if(!unvisited_neigh) return {};
    
    int rr = rand() % __builtin_popcount(unvisited_neigh);   
    for(int i = 0; i < 4; i++){
        if(rr == 0 && (unvisited_neigh & (1 << i)) != 0) return this->move_to(p, i);
        if(unvisited_neigh & (1 << i)) rr--;
    }
    return {};
}

void MazeRenderer::remove_wall_bw(pair<int, int> p, pair<int, int> neigh){
    if(neigh.first == p.first - 1){
        this->get_room(p).open(0);
        this->get_room(neigh).open_opp(0);
        return;
    }
    if(neigh.first == p.first + 1){
        this->get_room(p).open(1);
        this->get_room(neigh).open_opp(1);
        return;
    }
    if(neigh.second == p.second - 1){
        this->get_room(p).open(2);
        this->get_room(neigh).open_opp(2);
        return;
    }
    if(neigh.second == p.second + 1){
        this->get_room(p).open(3);
        this->get_room(neigh).open_opp(3);
        return;
    }
}

bool MazeRenderer::is_valid(pair<int, int> p){
    if(p.first < 0 || p.first >= this->nX || p.second < 0 || p.second >= this->nY) return false;
    return true;
}

void MazeRenderer::mazeGenerator(pair<int, int> p){
    if(!this->is_valid(p)) return;

    bool **visit = new bool* [this->nY];
    for(int j = 0; j < this->nY; j++){
        visit[j] = new bool[this->nX]();
    }

    stack<pair<int, int>> st;
    visit[p.second][p.first] = true;
    st.push(p);

    while(!st.empty()){
        pair<int, int> cur = st.top(); st.pop();
        if(!this->has_unvisited_neigh(cur, visit)) continue;
        st.push(cur);
        pair<int, int> neigh = this->random_unvisited_neigh(cur, visit);
        this->remove_wall_bw(cur, neigh);
        visit[neigh.second][neigh.first] = true;
        st.push(neigh);
    }

    for(int j = 0; j < this->nY; j++){
        delete[] visit[j];
    }
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

void MazeRenderer::createExit(){
    int gn = rand() % 4;
    switch (gn){
        case 0: this->get_room({0, rand() % this->nY}).open(0); break;
        case 1: this->get_room({this->nX - 1, rand() % this->nY}).open(1); break;
        case 2: this->get_room({rand() % this->nX, 0}).open(2); break;
        case 3: this->get_room({rand() % this->nX, this->nY - 1}).open(3); break;
    }
}

float* MazeRenderer::generateMaze(){
    this->maze = new Room* [this->nY];
    for(int j = 0; j < this->nY; j++){
        this->maze[j] = new Room[this->nX]();
        for(int i = 0; i < this->nX; i++) 
            this->get_room(i, j).set_gate_coords({i, j}, {this->startX, this->startY}, {this->edgeX, this->edgeY});
    }

    this->mazeGenerator({rand() % this->nX, rand() % this->nY});

    this->createExit();

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

void MazeRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = generateMaze(); 

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