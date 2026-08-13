# Flappy Bird

A **Flappy Bird** clone built with C++17 and [raylib](https://www.raylib.com/).

## Features

- Endless procedurally-spawned pipes with randomized gap heights
- Smooth physics: gravity, jump impulse, and velocity-based bird rotation
- Infinite scrolling background
- Score tracking with a persistent in-session high score
- Menu, HUD, and game-over UI with rounded buttons
- Single codebase, two targets: **native desktop** and **browser (WASM)**

## Controls

| Action     | Key / Input                   |
|------------|-------------------------------|
| Jump       | `SPACE` or `Left Mouse Click` |
| Start      | `SPACE` / click `START`       |
| Restart    | `SPACE` / click `RESTART`     |

## Requirements

- **CMake** >= 3.16
- A C++17 compiler: `g++` / `clang++`
- **raylib** (`-lraylib`), either:
  - installed system-wide (found via `find_package`), or
  - present as a prebuilt local `raylib/` directory containing `libraylib.a` and headers
- For web: **Emscripten SDK** (`em++` / `emcmake`)

## Build & Run (Native)

```bash
cmake -B build
cmake --build build
./build/flappy
```

Or step by step:

```bash
mkdir build && cd build
cmake ..
make
./flappy
```

## Build for the Web (WASM)

With the Emscripten SDK active, point CMake at the Emscripten toolchain inside a dedicated build directory:

```bash
cmake -B build-web \
  -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
  -DRAYLIB_PATH="/path/to/raylib" \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
```

This produces `build-web/flappy.html` (with `flappy.js` and `flappy.wasm` next to it). Assets are preloaded straight into the WASM filesystem.

Serve it and play in any modern browser:

```bash
python3 -m http.server 8080 -d build-web
# open http://localhost:8080/flappy.html
```

## Project Layout

```
.
├── CMakeLists.txt          # native + web builds
├── main.cpp                # game loop, physics, pipes, collisions
├── include/
│   ├── constants.hpp       # tuning values, scaling, game states
│   └── ui.hpp              # menu / HUD / game-over UI
└── assets/
    ├── bird.png
    ├── pipe.png
    └── background.png
```

## Gameplay Details

- `bird.velocityY += GRAVITY * dt` — gravity is applied every frame
- A jump sets `velocityY = BIRD_JUMP_FORCE`, and the bird rotates based on its velocity
- Every `PIPE_SPAWN_DELAY` seconds a new pipe pair slides in from the right
- Score increases when the bird clears a pipe's right edge
- Game over on pipe collision, ceiling, or floor; best score is kept in `highScore`

Tune every value in `include/constants.hpp`: gravity, jump force, pipe speed, gap size, and the global `SCALE`.

## Tech Notes

- Pixel art is scaled with a single `SCALE` factor, keeping the game resolution-agnostic
- `UpdateDrawFrame()` doubles as the Emscripten main loop and the desktop game loop
- Physics use `GetFrameTime()` (delta time), so the game plays consistently at any frame rate
