# Muncher

## Project Overview

A Pac-Man clone built in C with raylib. Single-player, Windows native desktop game.
See [CLAUDE_SECURITY.md](CLAUDE_SECURITY.md) for security constraints that **MUST** be followed.

## Development Commands

```sh
make              # compile muncher.exe
make test         # build and run all unit tests — REQUIRED before commit
make lint         # run cppcheck + flawfinder static analysis — REQUIRED before commit
make clean        # remove muncher.exe and test binaries
./muncher.exe     # run the game
```

`make lint` requires cppcheck (installed to `C:\Program Files\Cppcheck\`) and flawfinder (`pip install flawfinder`).

## Testing

Use TDD for all new logic: write the failing test first, then implement until it passes.

Unit tests live in `tests/`. Each test file includes only `.h` headers (via `-Isrc`) and
lists the `.c` source files it needs as separate compilation units in the Makefile rule —
this ensures gcov records absolute paths so lcov coverage reporting works correctly.

Stub headers for raylib live in `tests/stubs/raylib.h` (no-op draw functions, no window).

```sh
make test         # runs all test binaries; each prints N/N passed
```

Verify new features manually with `./muncher.exe` after the test suite passes.

## Architecture

```text
muncher/
  src/
    main.c              # entry point, game loop, window setup
    map.h/.c            # tile grid, procedural generation, rendering
    map_internal.h      # internal map helpers (test-visible)
    player.h/.c         # player state, movement, input, extra-life tracking
    player_internal.h   # internal player helpers (test-visible)
    ghost.h/.c          # ghost AI (scatter/chase/frightened/house/exiting modes)
    ghost_internal.h    # internal ghost helpers and runtime state accessors
    lives.h/.c          # life decrement, respawn sequencing, game-over flag
    collision.h/.c      # ghost–player collision detection and scoring
    fruit.h/.c          # level-based fruit spawn, collection, score popup
    audio.h/.c          # music and sound-effect playback (raylib wrappers)
    audio_internal.h    # internal audio helpers (test-visible)
    hiscore.h/.c        # persistent high score (binary file on disk)
  tests/
    test_*.c            # unit tests; each binary is self-contained
    stubs/raylib.h      # no-op raylib stubs for headless test builds
  Makefile
```

Raylib 6.0 is built from source and linked statically. The raylib source lives outside
this repository at an absolute path on the developer machine — see the Makefile for the
exact location. Toolchain: w64devkit MinGW-w64 GCC.

## Conventions

- C99, no C++ features
- All game state passed explicitly — no globals except compile-time constants
- Grid coordinates are integer tile indices; pixel coordinates are floats
- **TDD:** Write the test first, make it fail, then implement. New logic without a test is not done.
- **IMPORTANT:** Run `make test && make lint` before every commit
