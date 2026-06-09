from maze import Maze
from parse_maze_image import parse_maze_image
from solver_api import SolverAPI
from dir import Dir, LookAt
from action_enum import load_action_enum

class MousePos:
    def __init__(self, x, y, dir):
        self.x = x
        self.y = y
        self.dir = dir

    def __repr__(self):
        return f"x:{self.x}, y:{self.y}, dir:{self.dir}"

    def move(self, step):
        if self.dir == Dir.EAST:
            self.x += step
        elif self.dir == Dir.NORTH:
            self.y += step
        elif self.dir == Dir.WEST:
            self.x -= step
        elif self.dir == Dir.SOUTH:
            self.y -= step
        elif self.dir == Dir.NORTHEAST:
            self.x += step
            self.y += step
        elif self.dir == Dir.NORTHWEST:
            self.x -= step
            self.y += step
        elif self.dir == Dir.SOUTHWEST:
            self.x -= step
            self.y -= step
        elif self.dir == Dir.SOUTHEAST:
            self.x += step
            self.y -= step
        else:
            raise ValueError(f"Invalid direction: {self.dir}")

    def rotate(self, deg):
        self.dir = Dir((self.dir + deg//45) % 8)


def main():
    TRUE_MAZE = parse_maze_image("maze_image/gakusei_2023.png")
    N = TRUE_MAZE.size

    known_maze = Maze(N, None, None).empty()
    visited = [[False] * N for _ in range(N)]

    api = SolverAPI()
    api.init_all()
    act2num, num2act = load_action_enum()
    m_pos = MousePos(0, 0, Dir.NORTH)  # 初期位置と向き

    while True:
        left = TRUE_MAZE.look_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.LEFT))
        front = TRUE_MAZE.look_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.FRONT))
        right = TRUE_MAZE.look_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.RIGHT))
        print(f"Mouse info: x={m_pos.x}, y={m_pos.y}, dir={m_pos.dir.dir_to_str()}")
        print("walls: left={}, front={}, right={}".format(left, front, right))
        res = api.solver_left_wall(left, front, right)
        known_maze.set_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.LEFT), left)
        known_maze.set_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.FRONT), front)
        known_maze.set_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.RIGHT), right)
        visited[m_pos.y][m_pos.x] = True

        known_maze.print_maze_ascii(visited=visited, mouse_pos=m_pos)

        print("Result size:", res.size)
        print("Result values:", list(res.value)[:res.size])

        result = list(reversed(list(res.value)[:res.size]))

        print([num2act.get(x, f"?({x})") for x in result.__reversed__()])
        while result:
            now = result.pop()
            if now == act2num["ACT_NONE"]:
                continue
            elif now == act2num["SET_MOUSE_INFO"]:
                m_pos.x = result.pop()
                m_pos.y = result.pop()
                m_pos.dir = Dir(result.pop())
            elif now == act2num["SET_VISITED"]:
                result.pop()
                result.pop()
            else:
                try:
                    print(f"Action: {num2act[now]} ({now})")
                except KeyError:
                    print(f"now : {now}")
                    raise KeyError
        input()

if __name__ == "__main__":
    main()