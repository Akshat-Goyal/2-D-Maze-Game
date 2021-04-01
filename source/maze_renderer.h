#ifndef MAZE_RENDERER_H
#define MAZE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include <stack>

using namespace std;

struct Room{
    int left, right, up, down;

    pair<pair<float, float>, pair<float, float>> left_gate, right_gate, up_gate, down_gate;

    Room():left(0), right(0), up(0), down(0){}

    Room(int left, int right, int up, int down):left(left), right(right), up(up), down(down){}

    int gate(int gn){
        int val;
        switch (gn){
            case 0: val = left; break;
            case 1: val = right; break;
            case 2: val = up; break;
            case 3: val = down; break;
        }
        return val;
    }

    void open(int gn){
        switch (gn){
            case 0: left = 1; break;
            case 1: right = 1; break;
            case 2: up = 1; break;
            case 3: down = 1; break;
        }
    }

    void open_opp(int gn){
        switch (gn){
            case 0: right = 1; break;
            case 1: left = 1; break;
            case 2: down = 1; break;
            case 3: up = 1; break;
        }
    }

    void set(int gn, int val){
        switch (gn){
            case 0: left = val; break;
            case 1: right = val; break;
            case 2: up = val; break;
            case 3: down = val; break;
        }
    }

    void set_left_gate_coord(pair<int, int> p, pair<float, float> start, pair<float, float> edge){
        this->left_gate.first = {start.first + p.first * edge.first, start.second + p.second * edge.second};
        this->left_gate.second = {start.first + p.first * edge.first, start.second + (p.second + 1) * edge.second};
    }

    void set_right_gate_coord(pair<int, int> p, pair<float, float> start, pair<float, float> edge){
        this->right_gate.first = {start.first + (p.first + 1) * edge.first, start.second + p.second * edge.second};
        this->right_gate.second = {start.first + (p.first + 1) * edge.first, start.second + (p.second + 1) * edge.second};
    }

    void set_up_gate_coord(pair<int, int> p, pair<float, float> start, pair<float, float> edge){
        this->up_gate.first = {start.first + p.first * edge.first, start.second + p.second * edge.second};
        this->up_gate.second = {start.first + (p.first + 1) * edge.first, start.second + p.second * edge.second};
    }

    void set_down_gate_coord(pair<int, int> p, pair<float, float> start, pair<float, float> edge){
        this->down_gate.first = {start.first + p.first * edge.first, start.second + (p.second + 1) * edge.second};
        this->down_gate.second = {start.first + (p.first + 1) * edge.first, start.second + (p.second + 1) * edge.second};
    }

    void set_gate_coords(pair<int, int> p, pair<float, float> start, pair<float, float> edge){
        this->set_left_gate_coord(p, start, edge);
        this->set_right_gate_coord(p, start, edge);
        this->set_up_gate_coord(p, start, edge);
        this->set_down_gate_coord(p, start, edge);
    }

    void fill_left_gate_coord(float *vertices){
        vertices[0] = this->left_gate.first.first;
        vertices[1] = this->left_gate.first.second;
        vertices[2] = this->left_gate.second.first;
        vertices[3] = this->left_gate.second.second;
    }

    void fill_right_gate_coord(float *vertices){
        vertices[0] = this->right_gate.first.first;
        vertices[1] = this->right_gate.first.second;
        vertices[2] = this->right_gate.second.first;
        vertices[3] = this->right_gate.second.second;
    }

    void fill_up_gate_coord(float *vertices){
        vertices[0] = this->up_gate.first.first;
        vertices[1] = this->up_gate.first.second;
        vertices[2] = this->up_gate.second.first;
        vertices[3] = this->up_gate.second.second;
    }

    void fill_down_gate_coord(float *vertices){
        vertices[0] = this->down_gate.first.first;
        vertices[1] = this->down_gate.first.second;
        vertices[2] = this->down_gate.second.first;
        vertices[3] = this->down_gate.second.second;
    }

    void fill_gate_coord(float *vertices, int gn){
        switch (gn){
            case 0: this->fill_left_gate_coord(vertices); break;
            case 1: this->fill_right_gate_coord(vertices); break;
            case 2: this->fill_up_gate_coord(vertices); break;
            case 3: this->fill_down_gate_coord(vertices); break;
        }
    }

    pair<pair<float, float>, pair<float, float>> get_gate_coord(int gn){
        pair<pair<float, float>, pair<float, float>> gate;
        switch (gn){
            case 0: gate = this->left_gate; break;
            case 1: gate = this->right_gate; break;
            case 2: gate = this->up_gate; break;
            case 3: gate = this->down_gate; break;
        }
        return gate;
    }
};

class MazeRenderer
{
public:
    // Constructor (inits shaders/shapes)
    MazeRenderer(Shader &shader, float startX, float startY, float height, float width, int nX, int nY);
    // Destructor
    ~MazeRenderer();
    // Renders
    void DrawMaze();

    // returns distance from nearest left gate
    float get_left_gate_x(pair<float, float> point, pair<float, float> size);
    // returns distance from nearest right gate
    float get_right_gate_x(pair<float, float> point, pair<float, float> size);
    // returns distance from nearest top gate
    float get_up_gate_y(pair<float, float> point, pair<float, float> size);
    // returns distance from nearest bottom gate
    float get_down_gate_y(pair<float, float> point, pair<float, float> size);
    // detects collision of rectange with gates
    bool detect_collision(pair<float, float> point, pair<float, float> size);
    // returns direction to which imposter moves to reach the player
    pair<float, float> find_path(pair<float, float> player, pair<float, float> imposter);
    // returns true if rectangle is occupied in single room
    bool is_in_room(pair<float, float> point, pair<float, float> size);
private:
    // Render state
    Shader       shader; 
    unsigned int VAO;

    Room **maze = NULL;
    int **graph = NULL;
    float startX, startY, height, width, edgeX, edgeY;
    int nX, nY, nV, mV;

    // Initializes and configures the buffer and vertex attributes
    void initRenderData();
    // Generates Maze and its coordinates
    float* generateMaze();
    // Generates maze
    void mazeGenerator(pair<int, int> p);
    // create exit gate
    void createExit();
    // Forms graph which contains shortest distance b/w ith and jth room of maze
    void distance_graph();
    // Moves to next room to p through gate number: gn
    pair<int, int> move_to(pair<int, int> p, int gn);
    // Returns true if p is visited
    bool is_visited(pair<int, int> p, bool **visit);
    // Returns bitmask of visited neighbouring rooms of p
    int visited_rooms(pair<int, int> p, bool **visit);
    // Returns false is p is outside Maze
    bool is_valid(pair<int, int> p);
    // Counts closed gates
    void count_edges();
    // Returns reference to room p of Maze
    Room &get_room(pair<int, int> p);
    // Returns reference to room {x, y} of Maze
    Room &get_room(int x, int y);
    // Returns room no of point in maze
    pair<int, int> get_room_no(pair<float, float> point);
    // Returns true if there are unvisited neighbours of p
    bool has_unvisited_neigh(pair<int, int> p, bool **visit);
    // Returns random unvisited neighbour of p
    pair<int, int> random_unvisited_neigh(pair<int, int> p, bool **visit);
    // Returns bitmask of unvisited rooms
    int unvisited_rooms(pair<int, int> p, bool **visit);
    // Returns bitmask of side (outside maze) rooms
    int side_rooms(pair<int, int> p);
    // Opens wall bw p and its neighbour
    void remove_wall_bw(pair<int, int> p, pair<int, int> neigh);
    // Checks y overlap
    bool is_Y_overlapping(pair<float, float> point, pair<float, float> size, pair<pair<float, float>, pair<float, float>> gate);
    // Checks x overlap
    bool is_X_overlapping(pair<float, float> point, pair<float, float> size, pair<pair<float, float>, pair<float, float>> gate);
    // Checks collision of point of size: size with gate
    bool check_collision(pair<float, float> point, pair<float, float> size, pair<pair<float, float>, pair<float, float>> gate);
};

#endif