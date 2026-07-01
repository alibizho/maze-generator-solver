#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "maze.h"


#define GR(r) (2*(r)+1)
#define GC(c) (2*(c)+1)

static void shuffle(int a[], int n);
static void carve(Maze *m, int r, int c, int visited[MAX_SIZE][MAX_SIZE]);

Maze create_maze(int rows, int cols)
{
    Maze m;

    m.rows = rows;
    m.cols = cols;

    int height = 2 * rows + 1;
    int width = 2 * cols + 1;

    for (int i = 0; i < MAX_SIZE; i++)
        for (int j = 0; j < MAX_SIZE; j++)
            m.grid[i][j] = 1;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            m.grid[GR(r)][GC(c)] = 0;
        }
    }

    return m;
}

void generate_maze(Maze *m, int seed)
{
    srand(seed);

    int visited[MAX_SIZE][MAX_SIZE] = {0};

    carve(m, 0, 0, visited);
}

static void carve(Maze *m, int r, int c, int visited[MAX_SIZE][MAX_SIZE])
{
    visited[r][c] = 1;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    int order[4] = {0, 1, 2, 3};
    shuffle(order, 4);

    for (int i = 0; i < 4; i++)
    {
        int d = order[i];

        int nr = r + dr[d];
        int nc = c + dc[d];

        if (nr < 0 || nr >= m->rows)
            continue;

        if (nc < 0 || nc >= m->cols)
            continue;

        if (visited[nr][nc])
            continue;

        int wr = GR(r) + dr[d];
        int wc = GC(c) + dc[d];

        m->grid[wr][wc] = 0;

        carve(m, nr, nc, visited);
    }
}

static void shuffle(int a[], int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        int t = a[i];
        a[i] = a[j];
        a[j] = t;
    }
}

int save_maze(Maze *m, const char *filename)
{
    FILE *fp = fopen(filename, "w");

    if (fp == NULL)
        return 0;

    fprintf(fp, "%d %d\n", m->rows, m->cols);

    int height = 2 * m->rows + 1;
    int width = 2 * m->cols + 1;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (m->grid[i][j])
                fputc('#', fp);
            else
                fputc(' ', fp);
        }

        fputc('\n', fp);
    }

    fclose(fp);

    return 1;
}

Maze load_maze(const char *filename)
{
    Maze m;

    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        m.rows = 0;
        m.cols = 0;
        return m;
    }

    if (fscanf(fp, "%d %d\n", &m.rows, &m.cols) != 2)
    {
        fclose(fp);
        m.rows = 0;
        m.cols = 0;
        return m;
    }

    // Reject sizes that would overflow the fixed grid.
    int max_cells = (MAX_SIZE - 1) / 2;
    if (m.rows < 1 || m.cols < 1 || m.rows > max_cells || m.cols > max_cells)
    {
        fclose(fp);
        m.rows = 0;
        m.cols = 0;
        return m;
    }

    int height = 2 * m.rows + 1;
    int width = 2 * m.cols + 1;

    char ch;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            ch = fgetc(fp);

            if (ch == '#')
                m.grid[i][j] = 1;
            else
                m.grid[i][j] = 0;
        }

        fgetc(fp);
    }

    fclose(fp);

    return m;
}