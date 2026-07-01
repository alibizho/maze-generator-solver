#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "maze.h"


int main(int argc, char **argv){
    Maze m;

    if (argc > 2 && strcmp(argv[1], "--load") == 0){
        m = load_maze(argv[2]);
        if (m.rows == 0){
            return 1;
        }
    } else {
        int seed = (argc > 1) ? atoi(argv[1]) : (int)time(NULL);
        int rows = (argc > 2) ? atoi(argv[2]) : 10;
        int cols = (argc > 3) ? atoi(argv[3]) : rows;

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
    SolveResult result = solve_bfs(&m, start, end);

    save_maze_solved(&m, &result, start, end, "maze.txt");
    save_exploration(&result, "explore.txt");
    printf("Done");

    return 0;
}