# Muncher

A Pac-Man clone built in C with [raylib](https://www.raylib.com/).

## Gameplay

- Eat all dots to clear the maze
- Power pellets (large dots) turn ghosts blue — eat them for bonus points
- Avoid ghosts in normal mode or you die

### Controls

| Key | Action |
|-----|--------|
| Arrow keys | Move |

### Scoring

| Item | Points |
|------|--------|
| Dot | 1 |
| Power pellet | 10 |

## Building

### Prerequisites

- [w64devkit](https://github.com/skeeto/w64devkit/releases) (MinGW-w64 GCC for Windows)
- [raylib 6.0](https://github.com/raysan5/raylib) — built from source (see below)
- [cppcheck](https://cppcheck.sourceforge.io/) — for linting
- flawfinder — `pip install flawfinder`

### Build raylib from source

```sh
cd C:/Users/<you>/dev/lib/raylib-6.0/src
make PLATFORM=PLATFORM_DESKTOP
```

### Build and run

```sh
make
./muncher.exe
```

### Lint

```sh
make lint
```

## Project structure

```text
src/
  main.c       # entry point and game loop
  map.h/map.c  # tile grid, map data, rendering
  player.h/.c  # player movement and input
  ghost.h/.c   # ghost AI (scatter, chase, frightened modes)
Makefile
```

## Ghost behaviour

Each ghost has a distinct personality:

- **Blinky (red)** — directly chases the player
- **Pinky (pink)** — targets 4 tiles ahead of the player
- **Inky (cyan)** — uses Blinky's position to create a pincer attack
- **Clyde (orange)** — chases when far away, retreats to his corner when close

Ghosts alternate between scatter mode (retreating to corners) and chase mode on a fixed timer. Eating a power pellet triggers frightened mode for 8 seconds.

## CI

GitHub Actions runs on every push and pull request:
- Compiles the game on Ubuntu
- Runs cppcheck and flawfinder
- Runs SonarCloud static analysis
