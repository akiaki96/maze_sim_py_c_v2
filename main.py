import os
import json
import pygame
import numpy as np

from parse_maze_image import Maze, parse_maze_image

def main():
    TRUE_MAZE = parse_maze_image("maze_image\\gakusei_2023.png")
    N = TRUE_MAZE.size

    known_maze = Maze(N, None, None).empty()
    visited = np.zeros((N, N), dtype=bool)

if __name__ == "__main__":
    main()