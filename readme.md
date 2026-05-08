# Maze Simulator Python C v2

This project is a maze simulator implemented in Python with C extensions for solving algorithms.

## Requirements

- Python 3.9+
- Virtual environment (myenv)

## Installation

1. Activate the virtual environment:
   ```
   source myenv/bin/activate
   ```

2. Install dependencies:
   ```
   pip install -r requirements.txt
   ```

## Dependencies

- pygame
- numpy
- opencv-python

## Usage

Run the main script:
```
python main.py
```

Or run the maze logic:
```
python maze_logic.py
```

## Project Structure

- `main.py`: Main entry point
- `maze_logic.py`: Maze simulation logic
- `parse_maze_image.py`: Image parsing for maze
- `solver/`: C code for solver algorithms
- `maze_image/`: Maze images