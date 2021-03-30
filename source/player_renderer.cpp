/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "maze_renderer.h"

MazeRenderer::MazeRenderer(Shader &shader, float startX, float startY, float height, float width, int nX, int nY)
{
    this->shader = shader;
    this->initRenderData(startX, startY, height, width, nX, nY);
}

MazeRenderer::~MazeRenderer()
{
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

float* MazeRenderer::generateMaze(float startX, float startY, float width, float height, int nX, int nY){
    int maze_grid[nY][nX][4] = {0};
    {
        bool visit[nY][nX] = {false};
        int cnt_edges = 4 * nY * nX;

        maze_grid[0][0][0] = 1;
        cnt_edges--;
        maze_grid[0][0][2] = -1;
        visit[0][0] = true;

        int pX = 0, pY = 0;
        srand(time(NULL));
        while(true){
            while(true){
                int door = rand() % 4;
                while(maze_grid[pY][pX][door] != 0) door = rand() % 4;
                int cX = pX, cY = pY;
                switch (door){
                    case 0: cX -=1; break;
                    case 1: cX += 1; break;
                    case 2: cY -= 1; break;
                    case 3: cY += 1; break;
                }
                if(cX < 0 || cX >= nX || cY < 0 || cY >= nY ){
                    maze_grid[pY][pX][door] = 1; cnt_edges--;
                    pX = cX, pY = cY;
                    break;
                }
                if(visit[cY][cX]) continue;
                switch (door){
                    case 0: maze_grid[cY][cX][1] = 1; cnt_edges--; break;
                    case 1: maze_grid[cY][cX][0] = 1; cnt_edges--; break;
                    case 2: maze_grid[cY][cX][3] = 1; cnt_edges--; break;
                    case 3: maze_grid[cY][cX][2] = 1; cnt_edges--; break;
                }
                maze_grid[pY][pX][door] = 1; cnt_edges--;
                visit[cY][cX] = true;
                pX = cX, pY = cY;
                break;
            }
            if(pX < 0 || pX >= nX || pY < 0 || pY >= nY ) break;
        }
        this->nV = 2 * cnt_edges, this->mV = 2;
    }
    
    float *vertices = new float[this->nV * this->mV]();
    float edgeY = height / nY, edgeX = width / nX;
    for(int j = 0, cnt = 0; j < nY; j++){
        for(int i = 0; i < nX; i++){
            if(maze_grid[j][i][0] != 1){
                vertices[cnt++] = startY + j * edgeY;
                vertices[cnt++] = startX + i * edgeX;
                vertices[cnt++] = startY + (j + 1) * edgeY;
                vertices[cnt++] = startX + i * edgeX;
            }
            if(maze_grid[j][i][1] != 1){
                vertices[cnt++] = startY + j * edgeY;
                vertices[cnt++] = startX + (i + 1) * edgeX;
                vertices[cnt++] = startY + (j + 1) * edgeY;
                vertices[cnt++] = startX + (i + 1) * edgeX;
            }
            if(maze_grid[j][i][2] != 1){
                vertices[cnt++] = startY + j * edgeY;
                vertices[cnt++] = startX + i * edgeX;
                vertices[cnt++] = startY + j * edgeY;
                vertices[cnt++] = startX + (i + 1) * edgeX;
            }
            if(maze_grid[j][i][3] != 1){
                vertices[cnt++] = startY + (j + 1) * edgeY;
                vertices[cnt++] = startX + i * edgeX;
                vertices[cnt++] = startY + (j + 1) * edgeY;
                vertices[cnt++] = startX + (i + 1) * edgeX;
            }
        }
    }
    return vertices;
}

void MazeRenderer::initRenderData(float startX, float startY, float height, float width, int nX, int nY)
{
    // configure VAO/VBO
    unsigned int VBO;
    float *vertices = generateMaze(startX, startY, height, width, nX, nY); 

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