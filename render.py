import sys

import pygame
from typing import Tuple
from maze import Maze
from parse_maze_image import parse_maze_image
from dir import Dir

class MazeRenderer:
    def __init__(self, screen, cell_size :int = 40, wall_thickness : int = 4, margin: int = 50):
        self.screen = screen
        self.cell_size = cell_size
        self.wall_thickness = wall_thickness
        self.margin = margin

        # define colors
        self.bg_color = (30, 30, 30)
        self.grid_color = (60, 60, 60)

        self.wall_color = 0xFF1414                 # true_mazeの壁
        self.known_wall_color = (240, 240, 240)    # known_mazeの確定壁
        self.unknown_wall_color = (100, 100, 100)  # unknown壁(薄い)

        self.visited_color = (50, 80, 50)          # 探索済みセル背景

        self.mouse_color = (0, 200, 255)
        self.mouse_dir_color = (255, 50, 50)

    def draw_dashed_line(self, color, start_pos, end_pos, width=1, dash_len=6, gap_len=6):
        x1, y1 = start_pos
        x2, y2 = end_pos

        dx = x2 - x1
        dy = y2 - y1
        dist = (dx * dx + dy * dy) ** 0.5

        if dist == 0:
            return

        vx = dx / dist
        vy = dy / dist

        step = dash_len + gap_len
        n = int(dist // step) + 1

        for i in range(n):
            sx = x1 + vx * (i * step)
            sy = y1 + vy * (i * step)

            ex = x1 + vx * (i * step + dash_len)
            ey = y1 + vy * (i * step + dash_len)

            # 線分が終点を超えないようにする
            if (ex - x1) * vx + (ey - y1) * vy > dist:
                ex, ey = x2, y2

            pygame.draw.line(self.screen, color, (sx, sy), (ex, ey), width)

    def cell_to_screen(self, maze_size: int, x: int, y: int) -> Tuple[int, int]:
        """
        迷路座標(x,y) -> pygame画面座標(px,py)
        (x=0,y=0が左下になるように変換)
        """
        px = self.margin + x * self.cell_size
        py = self.margin + (maze_size - 1 - y) * self.cell_size
        return px, py
    
    def grid_to_screen(self, maze_size: int, gx: int, gy: int):
        """
        gx,gy: 0..2N のグリッド座標
        - (odd,odd) : セル中心
        - (even,odd)/(odd,even) : 壁中心
        - (even,even) : 柱中心

        描画は外周柱(0,0)〜(2N,2N) を
        margin〜margin+N*cell_size に線形マッピングする。
        """
        cs = self.cell_size
        N = maze_size

        # 1グリッド = セルサイズ/2
        unit = cs / 2.0

        px = self.margin + gx * unit
        py = self.margin + (2 * N - gy) * unit  # y反転

        return int(px), int(py)

    def draw_background(self):
        self.screen.fill(self.bg_color)

    def draw_grid(self, maze_size: int):
        """薄いグリッド線"""
        w = maze_size * self.cell_size
        h = maze_size * self.cell_size

        for x in range(maze_size + 1):
            pygame.draw.line(
                self.screen, self.grid_color,
                (self.margin + x * self.cell_size, self.margin),
                (self.margin + x * self.cell_size, self.margin + h),
                1
            )

        for y in range(maze_size + 1):
            pygame.draw.line(
                self.screen, self.grid_color,
                (self.margin, self.margin + y * self.cell_size),
                (self.margin + w, self.margin + y * self.cell_size),
                1
            )


    def draw_walls(self, maze : Maze, known_maze : Maze = None, debug_mode=False):
        N = maze.size
        cs = self.cell_size
        t = self.wall_thickness

        # --------------------------
        # vwall: 縦線
        # --------------------------
        for y in range(N):
            for xb in range(N + 1):
                px = self.margin + xb * cs
                py = self.margin + (N - 1 - y) * cs

                start = (px, py)
                end   = (px, py + cs)
                if known_maze is not None and known_maze.vwall[y, xb]:
                    if maze.vwall[y, xb]:
                        pygame.draw.line(self.screen, self.wall_color, start, end, t)
                    else:
                        pygame.draw.line(self.screen, self.bg_color, start, end, t)

        # --------------------------
        # hwall: 横線
        # --------------------------
        for yb in range(N + 1):
            for x in range(N):
                px = self.margin + x * cs
                py = self.margin + (N - yb) * cs

                start = (px, py)
                end   = (px + cs, py)
                if known_maze is not None and known_maze.hwall[yb, x]:
                    if maze.hwall[yb, x]:
                        pygame.draw.line(self.screen, self.wall_color, start, end, t)
                    else:
                        pygame.draw.line(self.screen, self.bg_color, start, end, t)

    # ============================================================
    # visitedセル描画
    # ============================================================
    def draw_visited_cells(self, maze_size: int, visited):
        """
        visited: shape (N,N) bool
        """
        if visited is None:
            return

        N = maze_size
        cs = self.cell_size

        for y in range(N):
            for x in range(N):
                if visited[y][x]:
                    px, py = self.cell_to_screen(N, x, y)

                    # 塗りつぶし矩形（壁より少し内側に）
                    rect = pygame.Rect(
                        px + 2,
                        py + 2,
                        cs - 4,
                        cs - 4
                    )
                    pygame.draw.rect(self.screen, self.visited_color, rect)

    def draw_mouse(self, screen, maze_size: int, gx: int, gy: int, direction: Dir):
        cx, cy = self.grid_to_screen(maze_size, gx, gy)


        r = int(self.cell_size * 0.22)
        pygame.draw.circle(screen, self.mouse_color, (cx, cy), r)

        # 方向線
        if direction == Dir.NORTH:
            tip = (cx, cy - r)
        elif direction == Dir.SOUTH:
            tip = (cx, cy + r)
        elif direction == Dir.EAST:
            tip = (cx + r, cy)
        elif direction == Dir.WEST:
            tip = (cx - r, cy)
        else:
            tip = (cx, cy)  # 斜めはとりあえず描かない

        pygame.draw.line(screen, self.mouse_dir_color, (cx, cy), tip, 3)

class MazeApp:
    def __init__(self, maze_image_path: str, fps: int = 30):
        self.maze_image_path = maze_image_path
        self.fps = fps


        self.true_maze = parse_maze_image(maze_image_path)
        self.N = self.true_maze.size

        self.known_maze = Maze(self.N, vwall=None, hwall=None).fill()
        self.visited = [[False for _ in range(self.N)] for _ in range(self.N)]

        self.screen = None
        self.clock = None
        self.running = False

    def init_pygame(self):
        pygame.init()
        renderer_margin = 50
        renderer_cell_size = 40
        self.screen_size = 2 * renderer_margin + self.N * renderer_cell_size
        self.screen = pygame.display.set_mode((self.screen_size, self.screen_size))
        self.renderer = MazeRenderer(self.screen, cell_size=renderer_cell_size, margin=renderer_margin)
        pygame.display.set_caption("Maze Renderer")
        self.clock = pygame.time.Clock()
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
                pygame.quit()
                sys.exit()

    def update(self):
        # 今は何もしないが、探索を入れるならここ
        pass

    def draw(self):
        self.renderer.draw_background()
        self.renderer.draw_grid(self.N)
        self.renderer.draw_walls(self.true_maze, known_maze=self.known_maze)
        self.renderer.draw_visited_cells(self.N, self.visited)




    def draw_mouse(self, grid_x :int, grid_y :int, grid_dir :Dir):
        self.renderer.draw_mouse(self.screen, self.N, grid_x, grid_y, grid_dir)

    def end_loop(self):
        pygame.display.update()
        self.clock.tick(self.fps)


def main():
    app = MazeApp("maze_image/zennihon_2025.png")
    app.init_pygame()
    while True:
        app.handle_events()
        app.draw()

        app.draw_mouse(1,1,Dir.NORTH)
        
        pygame.display.flip()
        app.clock.tick(app.fps)


if __name__ == "__main__":
    main()