# Muncher

## Project Overview

A Pac-Man clone built in C with raylib. Single-player, Windows native desktop game.
See [CLAUDE_SECURITY.md](CLAUDE_SECURITY.md) for security constraints that **MUST** be followed.

## Development Commands

```sh
make              # compile muncher.exe
make lint         # run cppcheck + flawfinder static analysis — REQUIRED before commit
make clean        # remove muncher.exe
./muncher.exe     # run the game
```

`make lint` requires cppcheck (installed to `C:\Program Files\Cppcheck\`) and flawfinder (`pip install flawfinder`).

## Testing

There is no automated test suite. Verify changes by running `./muncher.exe` and manually testing:
- Player moves in all four directions with arrow keys
- Player stops at walls and eats dots on contact
- `make lint` passes with no errors

## Architecture

```text
muncher/
  src/
    main.c        # entry point, game loop, window setup
    map.h/map.c   # tile grid, map data, rendering
    player.h/.c   # player state, movement, input
  Makefile
```

Raylib 6.0 is built from source and linked statically. The raylib source lives outside
this repository at an absolute path on the developer machine — see the Makefile for the
exact location. Toolchain: w64devkit MinGW-w64 GCC.

## Conventions

- C99, no C++ features
- All game state passed explicitly — no globals except compile-time constants
- Grid coordinates are integer tile indices; pixel coordinates are floats
- **IMPORTANT:** Run `make lint` before every commit
