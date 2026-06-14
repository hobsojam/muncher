# Muncher

A Pac-Man clone built in C with [raylib](https://www.raylib.com/).

## Gameplay

- Eat all dots to clear the maze; levels are procedurally generated
- Power pellets (large dots) turn ghosts blue — eat them for bonus points
- A fruit bonus appears once per level when half the dots are eaten
- Collect enough points to earn extra lives (at 1,500 and 5,000)
- Avoid ghosts in normal mode; you start with 3 lives before game over
- Wall colour changes every 5 levels; ghosts get harder as levels increase

### Controls

| Key | Action |
|-----|--------|
| Arrow keys | Move |
| `R` | Next level (on level clear) / Restart (on game over) |
| `P` | Pause / unpause |
| `F11` | Toggle fullscreen |
| `M` | Toggle music mute |
| `N` | Toggle SFX mute |
| `[` / `]` | Music volume down / up |
| `,` / `.` | SFX volume down / up |

### Scoring

| Item | Points |
|------|--------|
| Dot | 1 |
| Power pellet | 10 |
| Fruit bonus | 100 |
| 1st ghost (per frightened session) | 200 |
| 2nd ghost | 400 |
| 3rd ghost | 800 |
| 4th ghost | 1,600 |

Extra lives are awarded at **1,500** and **5,000** points (maximum 5 lives).

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

### Test

```sh
make test
```

### Lint

```sh
make lint
```

## Project structure

```text
src/
  main.c          # entry point and game loop
  map.h/map.c     # procedural tile grid, generation, rendering
  player.h/.c     # player movement, input, extra-life tracking
  ghost.h/.c      # ghost AI (scatter, chase, frightened modes)
  lives.h/.c      # life decrement, respawn, game over
  collision.h/.c  # ghost collision detection and scoring
  fruit.h/.c      # fruit bonus spawn, collection, popup
  audio.h/.c      # music and sound effects
  hiscore.h/.c    # persistent high score (binary file)
assets/
  music/          # theme.ogg (looping background music)
  sounds/         # chomp, power, ghost_eat, death, fruit_eat, extra_life WAVs
tests/
  test_*.c        # unit tests (custom framework, no dependencies)
  stubs/          # no-op stubs for raylib and audio (headless builds)
Makefile
```

## Ghost behaviour

Each ghost has a distinct personality:

- **Blinky (red)** — directly chases the player
- **Pinky (pink)** — targets 4 tiles ahead of the player
- **Inky (cyan)** — uses Blinky's position to create a pincer attack
- **Clyde (orange)** — chases when far away, retreats to his corner when close; his shy radius shrinks by one tile per level, making him progressively more aggressive

Ghosts alternate between scatter mode (retreating to corners) and chase mode on a fixed timer. Eating a power pellet triggers frightened mode for 8 seconds.

## CI

GitHub Actions runs on every push and pull request:
- Compiles the game on Ubuntu
- Runs all unit tests
- Runs cppcheck and flawfinder
- Runs SonarCloud static analysis with coverage reporting
