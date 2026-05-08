import numpy as np
from dir import Dir

class Maze:
    def __init__(self, size: int, vwall: np.ndarray, hwall: np.ndarray):
        self.size = size
        self.vwall = vwall
        self.hwall = hwall

    def __repr__(self):
        return f"Maze(size={self.size}, vwall shape={self.vwall.shape}, hwall shape={self.hwall.shape})"

    def fill(self):
        return Maze(self.size, np.ones((self.size, self.size + 1)), np.ones((self.size + 1, self.size)))
    
    def empty(self):
        zero = Maze(self.size, np.zeros((self.size, self.size + 1)), np.zeros((self.size + 1, self.size)))
        [zero.set_wall(0, y, Dir.WEST, True) for y in range(self.size)]
        [zero.set_wall(self.size - 1, y, Dir.EAST, True) for y in range(self.size)]
        [zero.set_wall(x, 0, Dir.SOUTH, True) for x in range(self.size)]
        [zero.set_wall(x, self.size - 1, Dir.NORTH, True) for x in range(self.size)]
        return zero

    def look_wall(self, x:int, y:int, d: Dir) -> bool:
        """
        (x,y)セルの方向dに壁があるか
        d: 0=E, 1=N, 2=W, 3=S
        """
        if d == Dir.EAST:
            return self.vwall[y, x + 1]
        elif d == Dir.NORTH:
            return self.hwall[y + 1, x]
        elif d == Dir.WEST:
            return self.vwall[y, x]
        elif d == Dir.SOUTH:
            return self.hwall[y, x]
        else:
            raise ValueError("Invalid direction")

    def set_wall(self, x:int, y:int, d: Dir, has_wall: bool):
        if d == Dir.EAST:
            self.vwall[y, x + 1] = 1 if has_wall else 0
        elif d == Dir.NORTH:
            self.hwall[y + 1, x] = 1 if has_wall else 0
        elif d == Dir.WEST:
            self.vwall[y, x] = 1 if has_wall else 0
        elif d == Dir.SOUTH:
            self.hwall[y, x] = 1 if has_wall else 0
        else:
            raise ValueError("Invalid direction")

    def print_maze_ascii(self, visited=None, mouse_pos=None):
        N = self.size
        v = self.vwall
        h = self.hwall

        print("=== ASCII Maze View (y=N-1 at top) ===")

        for y in reversed(range(N)):
            # 上側境界（hwall[y+1]がNorth）
            line_top = ""
            for x in range(N):
                line_top += "+"
                line_top += "---" if self.look_wall(x, y, Dir.NORTH) else "   "
            line_top += "+"
            print(line_top)

            # 左右境界（vwall[y,x]がWest）
            line_mid = ""
            for x in range(N):
                line_mid += "|" if self.look_wall(x, y, Dir.WEST) else " "
                if mouse_pos is not None and mouse_pos.x == x and mouse_pos.y == y:
                    # red
                    line_mid += " \033[91m*\033[0m "
                elif visited is not None and visited[y][x]:
                    line_mid += " * "
                else:
                    line_mid += "   "
            line_mid += "|" if self.look_wall(N - 1, y, Dir.EAST) else " "
            print(line_mid)

        # 最下段境界
        line_bottom = ""
        for x in range(N):
            line_bottom += "+"
            line_bottom += "---" if self.look_wall(x, 0, Dir.SOUTH) else "   "
        line_bottom += "+"
        print(line_bottom)