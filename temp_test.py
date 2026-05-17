from maze import Maze
from parse_maze_image import parse_maze_image
from solver_api import SolverAPI
from dir import Dir, LookAt
from action_enum import load_action_enum
from maze_logic import MousePos
import pygame
from typing import Tuple
from render import MazeRenderer, MazeApp

def conv_dir_to_render_dir(x:int, y:int, dir: Dir) -> (int, int, Dir):
    """
    迷路座標系の向き(dir)を、描画用の向きに変換する
    迷路座標系: (0,0)が左下、x右方向、y上方向
    描画座標系: (0,0)が左上、x右方向、y下方向
    """
    x = x*2 + 1
    y = y*2 + 1
    if dir == Dir.NORTH:
        return x, y-1, Dir.NORTH
    elif dir == Dir.SOUTH:
        return x, y+1, Dir.SOUTH
    elif dir == Dir.EAST:
        return x-1, y, Dir.EAST
    elif dir == Dir.WEST:
        return x+1, y, Dir.WEST
    else:
        raise ValueError(f"Invalid direction: {dir}")

def main():
    maze_image_path = "maze_image/gakusei_2023.png"
    
    app = MazeApp(maze_image_path, fps=5)

    TRUE_MAZE = parse_maze_image(maze_image_path)
    N = TRUE_MAZE.size

    known_maze = Maze(N, None, None).empty()

    api = SolverAPI()
    res = api.init_all()
    m_pos = MousePos(0,0,Dir.NORTH)
    act2num, num2act = load_action_enum()

    app.init_pygame()

    app.running = True
    while app.running:
        app.handle_events()
        
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
                vx = result.pop()
                vy = result.pop()
                app.visited[vy][vx] = True
            else:
                print(f"Action: {num2act[now]} ({now})")

        print(m_pos.x, m_pos.y, m_pos.dir)
        app.draw()
        rend_x, rend_y, _ = conv_dir_to_render_dir(m_pos.x, m_pos.y, m_pos.dir)
        app.draw_mouse(rend_x, rend_y, m_pos.dir)

        known_maze.print_maze_ascii(visited=app.visited, mouse_pos=m_pos)
        left = TRUE_MAZE.look_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.LEFT))
        front = TRUE_MAZE.look_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.FRONT))
        right = TRUE_MAZE.look_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.RIGHT))
        print(f"Mouse info: x={m_pos.x}, y={m_pos.y}, dir={m_pos.dir.dir_to_str()}")
        print("walls: left={}, front={}, right={}".format(left, front, right))
        res = api.solver_left_wall(left, front, right)
        known_maze.set_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.LEFT), left)
        known_maze.set_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.FRONT), front)
        known_maze.set_wall(m_pos.x, m_pos.y, m_pos.dir.look_dir(LookAt.RIGHT), right)

        
        app.end_loop()

if __name__ == "__main__":
    main()