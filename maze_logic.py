from action_loader import expand_action, load_action_json
from maze import Maze
from render import MazeApp
from solver_api import SolverAPI
from dir import Dir, LookAt
from action_enum import load_action_enum
from typing import Any


class MousePos:
    def __init__(self, x: int, y: int, dir: Dir):
        self.x = x
        self.y = y
        self.dir = dir

    def __repr__(self):
        return f"x:{self.x}, y:{self.y}, dir:{self.dir}"

    def move(self, step: int):
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

    def rotate(self, deg: int):
        self.dir = Dir((self.dir + deg // 45) % 8)


class MazeSimulation:
    def __init__(self, maze_image_path: str = "maze_image/zennihon_2025.png", fps: int = 5, action_json_path: str = "action.json", solver_type: str | None = None):
        self.maze_image_path = maze_image_path
        self.fps = fps
        self.action_json_path = action_json_path

        self.app = MazeApp(self.maze_image_path, fps=self.fps)
        self.true_maze = self.app.true_maze
        self.known_maze = Maze(self.true_maze.size, None, None).empty()
        self.app.known_maze = self.known_maze

        self.api = SolverAPI()
        
        # Use default solver if not specified
        if solver_type is None:
            self.solver_type = self.api.default_solver
        else:
            self.solver_type = solver_type
        
        # Validate solver type
        available = self.api.get_solver_list()
        if self.solver_type not in available:
            raise ValueError(f"Unknown solver type: '{self.solver_type}'. Available solvers: {available}")
        
        self._print_solver_info()
        
        self.act2num, self.num2act = load_action_enum()
        self.action_dict = load_action_json(self.action_json_path)

        self.m_pos = MousePos(0, 0, Dir.NORTH)
        self.rend_pos = MousePos(1, 1, Dir.NORTH)
        self.read_wall = False
        self.result: list[Any] = []
    
    def _print_solver_info(self) -> None:
        """Print information about the current solver and available solvers."""
        solver_info = self.api.get_solver_info(self.solver_type)
        print(f"\n=== Solver Configuration ===")
        print(f"Current solver: {solver_info['display_name']}")
        print(f"Description: {solver_info['description']}")
        print(f"\nAvailable solvers:")
        for name in self.api.get_solver_list():
            info = self.api.get_solver_info(name)
            print(f"  - {name}: {info['display_name']}")
        print(f"==========================\n")

    def initialize(self) -> None:
        res = self.api.solver_init(self.solver_type)
        self._update_result(res)
        self._log_result(res, "Initial result")

    def _update_result(self, res: Any) -> None:
        self.result = list(reversed(list(res.value)[:res.size]))

    def _log_result(self, res: Any, label: str) -> None:
        print(f"{label} size:", res.size)
        print(f"{label} values:", list(res.value)[:res.size])
        print([self.num2act.get(x, f"?({x})") for x in self.result.__reversed__()])

    def run(self) -> None:
        self.app.init_pygame()
        self.app.running = True
        self.initialize()

        while self.app.running:
            self.app.handle_events()

            if not self.app.paused:
                self._process_next_step()

                if self.read_wall and not self.result:
                    self._read_and_update_walls()

            self.app.draw()
            self.app.draw_mouse(self.rend_pos.x, self.rend_pos.y, self.rend_pos.dir)
            self.app.draw_pause_overlay()

            self.app.end_loop()

    def _process_next_step(self) -> None:
        if self.result:
            now = self.result.pop()
        else:
            now = self.act2num["ACT_NONE"]

        if isinstance(now, dict):
            if "MOVE" in now:
                self.rend_pos.move(now["MOVE"])
            elif "ROTATE" in now:
                self.rend_pos.rotate(now["ROTATE"])
            else:
                raise KeyError(f"Unknown atomic action step: {now}")
        elif now == self.act2num["ACT_NONE"]:
            pass
        elif now == self.act2num["SET_MOUSE_INFO"]:
            self.m_pos.x = self.result.pop()
            self.m_pos.y = self.result.pop()
            self.m_pos.dir = Dir(self.result.pop())
        elif now == self.act2num["SET_VISITED"]:
            vx = self.result.pop()
            vy = self.result.pop()
            self.app.visited[vy][vx] = True
        elif now == self.act2num["SET_WALL"]:
            vx = self.result.pop()
            vy = self.result.pop()
            vdir = self.result.pop()
            vwall = self.result.pop()
            self.known_maze.set_wall(vx, vy, vdir, vwall)
        elif now == self.act2num["READ_WALL"]:
            self.read_wall = True
        else:
            action_name = self.num2act[now]
            print(f"Action: {action_name} ({now})")
            expanded_steps = expand_action(action_name, self.action_dict)
            for step in reversed(expanded_steps):
                self.result.append(step)

        print(
            f"Mouse position: ({self.m_pos.x}, {self.m_pos.y}, {self.m_pos.dir.dir_to_str()}), "
            f"Render position: ({self.rend_pos.x}, {self.rend_pos.y}, {self.rend_pos.dir.dir_to_str()})"
        )

    def _read_and_update_walls(self) -> None:
        self.known_maze.print_maze_ascii(visited=self.app.visited, mouse_pos=self.m_pos)

        left = self.true_maze.look_wall(self.m_pos.x, self.m_pos.y, self.m_pos.dir.look_dir(LookAt.LEFT))
        front = self.true_maze.look_wall(self.m_pos.x, self.m_pos.y, self.m_pos.dir.look_dir(LookAt.FRONT))
        right = self.true_maze.look_wall(self.m_pos.x, self.m_pos.y, self.m_pos.dir.look_dir(LookAt.RIGHT))

        print(f"Mouse info: x={self.m_pos.x}, y={self.m_pos.y}, dir={self.m_pos.dir.dir_to_str()}")
        print("walls: left={}, front={}, right={}".format(left, front, right))

        res = self.api.solver_step(self.solver_type, left, front, right)
        
        self.known_maze.set_wall(self.m_pos.x, self.m_pos.y, self.m_pos.dir.look_dir(LookAt.LEFT), left)
        self.known_maze.set_wall(self.m_pos.x, self.m_pos.y, self.m_pos.dir.look_dir(LookAt.FRONT), front)
        self.known_maze.set_wall(self.m_pos.x, self.m_pos.y, self.m_pos.dir.look_dir(LookAt.RIGHT), right)

        self.read_wall = False
        self._update_result(res)
        self._log_result(res, "Next result")


def main() -> None:
    # solver_typeを指定しない場合、solver_config.jsonのdefault_solverが使用される
    # solver_typeを指定する場合: "left_wall" または "adachi"
    simulation = MazeSimulation("maze_image/zennihon_2025.png", fps=5)
    simulation.run()


if __name__ == "__main__":
    main()
