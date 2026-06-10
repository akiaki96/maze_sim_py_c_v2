from maze_logic import MazeSimulation


def main() -> None:
    simulation = MazeSimulation("maze_image/zennihon_2025.png", fps=20)
    simulation.run()


if __name__ == "__main__":
    main()
