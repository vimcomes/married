# Desire Match Simulator (C++20)

Text-mode simulator illustrating multithreading in C++20: two threads (Marina and Roman) randomly generate desires until 10 matches are found. UI is fixed to 80x24 and rendered directly with ANSI/WinAPI terminal control (no ncurses dependency).

## What it does
- Two `std::jthread` workers generate random desires from a 150-item list.
- Main loop tracks attempts, dynamic tick (faster on no-match, reset on match), shows a colored HUD, table of recent attempts, and a progress bar.
- On 10 matches, shows a summary with top repeated desires.

## Build & Run (Linux)
```
cmake -S . -B build
cmake --build build
./build/desire_simulator
```

## Windows notes
- Uses ANSI escape sequences; on modern Windows terminals this works when virtual terminal processing is enabled (done automatically at startup).
- Prefer MinGW/WSL; Visual C++ is not targeted.

## Key dependencies
- C++20 compiler, CMake ≥3.20
