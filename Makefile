CC = gcc
RAYLIB = C:/Users/ch8jw/dev/lib/raylib-6.0

CFLAGS = -Wall -I$(RAYLIB)/src
LDFLAGS = -L$(RAYLIB)/src -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = src/main.c src/map.c src/player.c
OUT = muncher.exe

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

lint:
	cppcheck --enable=all --error-exitcode=1 --suppress=missingInclude --suppress=unmatchedSuppression src/
	flawfinder src/

clean:
	del /Q $(OUT)
