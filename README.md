# Desire Match Simulator (ncurses, C++20)

Text-mode simulator illustrating multithreading in C++20: two threads (Marina and Roman) randomly generate desires until 10 matches are found. UI is fixed to 80x24 and rendered with ncurses.

## Project layout
- `src/app`: simulation orchestrator (`DesireSimulator`).
- `src/core`: shared types, desire list, and history tracking.
- `src/ui`: ncurses-based terminal helpers plus HUD/table renderers.

## What it does
- Two `std::jthread` workers generate random desires from a 150-item list.
- Main loop tracks attempts, dynamic tick (faster on no-match, reset on match), shows a colored HUD, table of recent attempts, and a progress bar.
- HUD now displays Fastest/Slowest/Avg time between matches (seconds, 1 decimal) to give pacing context.
- On 10 matches, shows a summary with top repeated desires.

## Build & Run (Linux)
```
cmake -S . -B build
cmake --build build
./build/desire_simulator
```

## Windows notes
- The code uses `ncurses`. On Windows, build with a compatible curses library (e.g., PDCurses) and point CMake to it (`-DCURSES_LIBRARY` / `-DCURSES_INCLUDE_PATH`), or use WSL where ncurses is available by default.
- Visual C++ is not supported in the current CMake setup; prefer MinGW or WSL for a smoother build.

## Key dependencies
- C++20 compiler, CMake ≥3.20
- ncurses (or compatible curses implementation)
