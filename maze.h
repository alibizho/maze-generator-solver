// maze.h
#ifndef MAZE_H
#define MAZE_H

#define MAX_SIZE 51

typedef struct {
    int rows, cols;
    int grid[MAX_SIZE][MAX_SIZE];
} Maze;

typedef struct {
    int row, col;
} Point;

typedef struct {
    Point path[MAX_SIZE * MAX_SIZE];
    int length;
    int cells_visited;
} SolveResult;

// Person A's functions (defined in maze_generate.c)
Maze create_maze(int rows, int cols);
void generate_maze(Maze *m, int seed);
int save_maze(Maze *m, const char *filename);
Maze load_maze(const char *filename);

// Person B's functions (defined in maze_solve.c)
SolveResult solve_bfs(Maze *m, Point start, Point end);
void print_maze(Maze *m);
void print_path_overlay(Maze *m, SolveResult *r);

#endif