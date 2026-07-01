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





void print_maze(Maze *m){

    int grid_h = 2 * m->rows + 1;
    int grid_w = 2 * m->cols + 1;

    for (int r = 0; r < grid_h; r++){
        for (int c= 0; c< grid_w; c++){
            if (m->grid[r][c] == 1){
                printf("#");
            } else{
                printf(" ");
            }
        }
        printf("\n");
    }
}


void print_path_overlay(Maze *m, SolveResult *result){
    int grid_h = 2 * m->rows + 1;
    int grid_w = 2 * m->cols + 1;

    for (int r = 0; r < grid_h; r++){
        for (int c= 0; c< grid_w; c++){

            int is_path = 0;

            for (int i = 0; i<result->length;i++){
                if ((r == (2 * result->path[i].row + 1)) && c == (2*result->path[i].col + 1)){
                    is_path = 1;
                    break;
                }
            }
            if (m->grid[r][c] == 1){
                printf("#");
            } else if (is_path){
                printf("O");
            }else{
                printf(" ");
            }
        }
        printf("\n");
    }
}



int save_maze_solved(Maze *m, SolveResult *result, Point start, Point end, const char *filename){
    FILE *fp = fopen(filename, "w");

    if (fp == NULL)
        return 0;

    int grid_h = 2* m->rows + 1;
    int grid_w = 2 * m->cols + 1;

    fprintf(fp, "%d %d\n", m->rows, m->cols);

    for (int r = 0; r< grid_h; r++){
        for (int c = 0; c< grid_w; c++){
            char ch;
            if (m->grid[r][c] == 1){
                ch = '#';
            } else{
                ch = ' ';
                for (int i = 0; i< result->length; i++){
                    int current_row = 2 * result->path[i].row + 1;
                    int current_col = 2 * result->path[i].col + 1;

                    if (r == current_row && c == current_col){
                        ch = '.';
                        break;
                    }
                    if (i > 0){
                        int qr = 2 * result->path[i - 1].row + 1;
                        int qc = 2 * result->path[i - 1].col + 1;

                        if (r == (current_row + qr) / 2 && c == (current_col + qc) / 2){
                            ch = '.';
                            break;
                        }

                    }
                }
            }
            if (r == 2 * start.row + 1 && c == 2* start.col + 1)
                ch = 'S';
            if (r == 2 * end.row + 1 && c == 2* end.col + 1)
                ch = 'E';

            fputc(ch, fp);
        }

        fputc('\n', fp);
    }

    fclose(fp);

    return 1;
}


int save_exploration(SolveResult *result, const char *filename){
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
        return 0;


    fprintf(fp, "%d\n", result->cells_visited);

    for (int i = 0; i< result->cells_visited; i++){
        fprintf(fp, "%d %d\n", result->exploration_order[i].row, result->exploration_order[i].col);
    }

    fclose(fp);

    return 1;
}