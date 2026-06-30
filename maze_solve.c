#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"



SolveResult solve_bfs(Maze *m, Point start, Point end){
    SolveResult result;
    result.length = 0;
    result.cells_visited = 0;


    int visited[MAX_SIZE][MAX_SIZE] = {0};
    Point came_from[MAX_SIZE][MAX_SIZE];

    Point queue[MAX_SIZE * MAX_SIZE];
    int front = 0, back = 0;

    Point directions[4] = {
        {-1, 0}, // up
        {1, 0},  // down
        {0, -1}, // left
        {0, 1} // right
    };

    if (start.row < 0 || start.row >= m->rows || start.col < 0 || start.col >= m->cols){
        printf("Start point is out of bounds");
        return result;
    }

    queue[back] = start;
    back++;
    visited[start.row][start.col] = 1;


    while (front < back){
        Point current = queue[front];
        front++;

        result.exploration_order[result.cells_visited] = current;

        result.cells_visited++;


        if (current.row == end.row && current.col == end.col){
            break;
        }

        for (int d = 0; d < 4; d++){
            int neighbour_row = current.row + directions[d].row;
            int neighbour_col = current.col + directions[d].col;



            if (neighbour_row < 0 || neighbour_row >= m->rows || neighbour_col < 0 || neighbour_col >= m->cols){
                continue;
            }

            int wall_row = 2 * current.row + directions[d].row + 1;
            int wall_col = 2 * current.col + directions[d].col + 1;

            if(m->grid[wall_row][wall_col] == 1){
                continue;
            }
            if(visited[neighbour_row][neighbour_col]){
                continue;
            }

            visited[neighbour_row][neighbour_col] = 1;
            came_from[neighbour_row][neighbour_col] = current;
            Point neighbour = {neighbour_row, neighbour_col};
            queue[back] = neighbour;
            back++;
        }
    }

    if (!visited[end.row][end.col]){
        printf("No End found");
        result.length = 0;
        return result;
    }

    Point temp[MAX_SIZE * MAX_SIZE];
    int len = 0;
    Point cur = end;

    while (cur.row != start.row || cur.col != start.col){
        temp[len] = cur;
        len++;
        cur = came_from[cur.row][cur.col];
    }

    temp[len] = start;
    len++;

    

    for (int i = 0; i<len; i++){
        result.path[i] = temp[len - 1 - i];
    }
    result.length = len;

    return result;
}