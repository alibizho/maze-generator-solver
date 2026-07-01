import os
import random
import subprocess
import tkinter as tk
from tkinter import filedialog

HERE = os.path.dirname(os.path.abspath(__file__))
MAZE_FILE = os.path.join(HERE, "maze.txt")
EXPLORE_FILE = os.path.join(HERE, "explore.txt")
BINARY = os.path.join(HERE, "maze")  

CELL = 22
STEP_MS = 100

MIN_CELLS = 2  
MAX_CELLS = 25    
DEFAULT_CELLS = 10

WALL = "black"
OPEN_BG = "white"
VISITED = "blue" 
PATH = "red"     
START = "green"
END = "blue"

DIRS = [(-1, 0), (1, 0), (0, -1), (0, 1)]


def cell_to_grid(row, col):
    return 2 * row + 1, 2 * col + 1


class MazeApp:
    def __init__(self, root):
        self.root = root
        self.chars = [] 
        self.explore = []  
        self.visited = set()
        self.step_index = 0
        self.after_id = None
        self.current_source = None  # how the current maze was made, so we can re-solve it

        controls = tk.Frame(root)
        controls.pack(fill="x", padx=6, pady=6)

        self.status = tk.StringVar()
        self.rows_var = tk.IntVar(value=DEFAULT_CELLS)
        self.cols_var = tk.IntVar(value=DEFAULT_CELLS)

        tk.Button(controls, text="New maze", command=self.regenerate).pack(side="left")
        tk.Button(controls, text="Load", command=self.load_from_file).pack(side="left", padx=(6, 0))

        tk.Label(controls, text="Rows").pack(side="left", padx=(12, 2))
        tk.Spinbox(controls, from_=MIN_CELLS, to=MAX_CELLS, width=4,
                   textvariable=self.rows_var).pack(side="left")
        tk.Label(controls, text="Cols").pack(side="left", padx=(8, 2))
        tk.Spinbox(controls, from_=MIN_CELLS, to=MAX_CELLS, width=4,
                   textvariable=self.cols_var).pack(side="left")

        tk.Button(controls, text="Solve BFS",
                  command=lambda: self.resolve("bfs")).pack(side="left", padx=(12, 0))
        tk.Button(controls, text="Solve DFS",
                  command=lambda: self.resolve("dfs")).pack(side="left", padx=(6, 0))

        tk.Label(controls, textvariable=self.status).pack(side="left", padx=10)

        self.canvas = tk.Canvas(root, highlightthickness=0, bg=OPEN_BG)
        self.canvas.pack(padx=6, pady=(0, 6))

        if os.path.exists(MAZE_FILE) and os.path.exists(EXPLORE_FILE):
            self.start_animation()
        else:
            self.regenerate()

    def compile_binary(self):
        sources = ["main.c", "maze_generate.c", "maze_solve.c"]
        try:
            subprocess.run(["cc", *sources, "-o", "maze"], cwd=HERE, check=True, capture_output=True)
            return True
        except subprocess.CalledProcessError as e:
            self.status.set("compile failed - see terminal")
            print(e.stderr.decode())
            return False

    def size_from(self, var):
        try:
            value = int(var.get())
        except (tk.TclError, ValueError):
            value = DEFAULT_CELLS
        value = max(MIN_CELLS, min(MAX_CELLS, value))
        var.set(value)
        return value

    def regenerate(self):
        if not self.compile_binary():
            return

        rows = self.size_from(self.rows_var)
        cols = self.size_from(self.cols_var)
        seed = random.randint(0, 1_000_000)
        self.current_source = ("gen", seed, rows, cols)
        self.run_solver("bfs")

    def resolve(self, algo):
        if self.current_source is None:
            self.status.set("generate or load a maze first")
            return
        if not self.compile_binary():
            return
        self.run_solver(algo)

    def run_solver(self, algo):
        if self.current_source[0] == "gen":
            _, seed, rows, cols = self.current_source
            args = [BINARY, str(seed), str(rows), str(cols), "--algo", algo]
            label = f"seed {seed}  ({rows}x{cols})  {algo.upper()}"
        else:
            _, path = self.current_source
            args = [BINARY, "--load", path, "--algo", algo]
            label = f"{os.path.basename(path)}  {algo.upper()}"
        try:
            subprocess.run(args, cwd=HERE, check=True, capture_output=True)
        except subprocess.CalledProcessError:
            self.status.set("solve failed")
            return
        self.status.set(label)
        self.start_animation()

    def read_maze_file(self, path):
        with open(path) as f:
            f.readline()
            return [line.rstrip("\n") for line in f if line.rstrip("\n") != ""]

    def load(self):
        self.chars = self.read_maze_file(MAZE_FILE)
        with open(EXPLORE_FILE) as f:
            count = int(f.readline())
            self.explore = [tuple(map(int, f.readline().split())) for _ in range(count)]

    def load_from_file(self):
        path = filedialog.askopenfilename(
            initialdir=HERE,
            title="Open an unsolved maze",
            filetypes=[("Maze files", "*.txt"), ("All files", "*.*")],
        )
        if not path:
            return
        if not self.compile_binary():
            return
        self.current_source = ("load", path)
        self.run_solver("bfs")

    def fill_cell(self, gr, gc, color):
        x, y = gc * CELL, gr * CELL
        self.canvas.create_rectangle(x, y, x + CELL, y + CELL, fill=color, outline="")

    def draw_base(self):
        self.canvas.delete("all")
        rows = len(self.chars)
        cols = max(len(row) for row in self.chars)
        self.canvas.config(width=cols * CELL, height=rows * CELL)

        for gr, line in enumerate(self.chars):
            for gc, ch in enumerate(line):
                if ch == "#":
                    self.fill_cell(gr, gc, WALL)
                elif ch == "S":
                    self.fill_cell(gr, gc, START)
                elif ch == "E":
                    self.fill_cell(gr, gc, END)

    def start_animation(self):
        if self.after_id is not None: 
            self.canvas.after_cancel(self.after_id)
            self.after_id = None
        self.load()
        self.draw_base()
        self.visited = set()
        self.step_index = 0
        self.step()

    def char_at(self, gr, gc):
        if 0 <= gr < len(self.chars) and 0 <= gc < len(self.chars[gr]):
            return self.chars[gr][gc]
        return "#"

    def step(self):
        if self.step_index >= len(self.explore):
            self.reveal_path()
            self.after_id = None
            return

        row, col = self.explore[self.step_index]
        gr, gc = cell_to_grid(row, col)
        self.visited.add((gr, gc))

        if self.char_at(gr, gc) not in ("S", "E"):
            self.fill_cell(gr, gc, VISITED)

        for dr, dc in DIRS:
            ngr, ngc = gr + 2 * dr, gc + 2 * dc    
            cgr, cgc = gr + dr, gc + dc                
            if (ngr, ngc) in self.visited and self.char_at(cgr, cgc) != "#":
                self.fill_cell(cgr, cgc, VISITED)

        self.step_index += 1
        self.after_id = self.canvas.after(STEP_MS, self.step)

    def reveal_path(self):
        for gr, line in enumerate(self.chars):
            for gc, ch in enumerate(line):
                if ch == ".":
                    self.fill_cell(gr, gc, PATH)
        for gr, line in enumerate(self.chars):
            for gc, ch in enumerate(line):
                if ch == "S":
                    self.fill_cell(gr, gc, START)
                elif ch == "E":
                    self.fill_cell(gr, gc, END)


def main():
    root = tk.Tk()
    root.title("Maze viewer")
    MazeApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
