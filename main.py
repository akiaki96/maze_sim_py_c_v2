from maze_logic import MazeSimulation


def main() -> None:
    simulation = MazeSimulation("maze_image/gakusei_2023.png", fps=5)
    simulation.run()


if __name__ == "__main__":
    main()
