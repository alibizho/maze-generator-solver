#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "maze.h"


int main(int argc, char **argv){
    Maze m;


    const char *algo = "bfs";
    char *rest[16];
    int rest_count = 0;
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--algo") == 0 && i + 1 < argc){
            algo = argv[++i];
        } else if (rest_count < 16){
            rest[rest_count++] = argv[i];
        }
    }
    // load a maze from the file
    if (rest_count >= 2 && strcmp(rest[0], "--load") == 0){
        m = load_maze(rest[1]);
        if (m.rows == 0){
            return 1;
        }
        // generate a random maze
    } else {
        int seed = (rest_count > 0) ? atoi(rest[0]) : (int)time(NULL);
        int rows = (rest_count > 1) ? atoi(rest[1]) : 10;
        int cols = (rest_count > 2) ? atoi(rest[2]) : rows;

        int max_cells = (MAX_SIZE - 1) / 2;
        if (rows < 1) rows = 1;
        if (cols < 1) cols = 1;
        if (rows > max_cells) rows = max_cells;
        if (cols > max_cells) cols = max_cells;

        m = create_maze(rows, cols);
        generate_maze(&m, seed);
    }

    Point start = {0, 0};
    Point end = {m.rows - 1, m.cols - 1};
    SolveResult result = (strcmp(algo, "dfs") == 0)
        ? solve_dfs(&m, start, end)
        : solve_bfs(&m, start, end);

    save_maze_solved(&m, &result, start, end, "maze.txt");
    save_exploration(&result, "explore.txt");
    printf("Done");

    return 0;
}