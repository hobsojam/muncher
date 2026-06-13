CC     = gcc
RAYLIB = C:/Users/ch8jw/dev/lib/raylib-6.0

CFLAGS = -Wall -I$(RAYLIB)/src

ifeq ($(OS),Windows_NT)
    LDFLAGS = -L$(RAYLIB)/src -lraylib -lopengl32 -lgdi32 -lwinmm
    OUT     = muncher.exe
    RM      = del /Q
else
    LDFLAGS = -L$(RAYLIB)/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    OUT     = muncher
    RM      = rm -f
endif

SRC = src/main.c src/map.c src/player.c src/ghost.c

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

lint:
	cppcheck --enable=all --error-exitcode=1 --suppress=missingInclude --suppress=unmatchedSuppression src/
	flawfinder src/

clean:
	$(RM) $(OUT)
