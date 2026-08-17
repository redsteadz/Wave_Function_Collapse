# Wave Function Collapse

An interactive, WFC-inspired road generator written in C++ with [raylib](https://www.raylib.com/). The demo repeatedly collapses the lowest-entropy cell and propagates edge constraints across a 20 × 20 grid.

## How it works

- Loads five road-tile states: blank, up, right, down, and left
- Derives edge compatibility masks from the border pixels of each tile image
- Tracks the remaining valid options for every grid cell
- Chooses randomly among cells with the fewest available options
- Propagates the selected tile's constraints to adjacent cells
- Displays each unresolved cell's current option count

## Controls

| Input | Action |
| --- | --- |
| Left click | Force a random collapse at the selected cell |
| `Space` | Run an extra collapse iteration |
| `R` | Reset the grid |

The generator also advances automatically once per frame.

## Build and run

You need a C++ compiler, CMake, and a raylib installation discoverable by CMake.

~~~bash
git clone https://github.com/redsteadz/Wave_Function_Collapse.git
cd Wave_Function_Collapse
cmake -S . -B build
cmake --build build
cd build
./WFC
~~~

CMake copies the tile assets into the build directory.

## Scope

This repository is a focused visualization of entropy selection and local constraint propagation. It does not currently implement contradiction recovery or backtracking.
