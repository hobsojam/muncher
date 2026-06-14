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

SRC = src/main.c src/map.c src/player.c src/ghost.c src/lives.c src/collision.c src/audio.c

TESTCFLAGS  = -Wall -Itests/stubs -Isrc --coverage -fprofile-abs-path
TEST_MAP    = tests/test_map
TEST_PLAYER = tests/test_player
TEST_GHOST  = tests/test_ghost
TEST_WIN    = tests/test_win
TEST_LIVES      = tests/test_lives
TEST_COLLISION  = tests/test_collision
TEST_BINS   = $(TEST_MAP) $(TEST_PLAYER) $(TEST_GHOST) $(TEST_WIN) $(TEST_LIVES) $(TEST_COLLISION)

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

test: $(TEST_BINS)
	./$(TEST_MAP)
	./$(TEST_PLAYER)
	./$(TEST_GHOST)
	./$(TEST_WIN)
	./$(TEST_LIVES)
	./$(TEST_COLLISION)

$(TEST_MAP): tests/test_map.c tests/test_framework.h tests/stubs/raylib.h src/map.c src/map.h
	$(CC) tests/test_map.c -o $(TEST_MAP) $(TESTCFLAGS)

$(TEST_PLAYER): tests/test_player.c tests/test_framework.h tests/stubs/raylib.h src/player.c src/player.h src/map.c src/map.h src/audio.c src/audio.h
	$(CC) tests/test_player.c src/audio.c -o $(TEST_PLAYER) $(TESTCFLAGS)

$(TEST_GHOST): tests/test_ghost.c tests/test_framework.h tests/stubs/raylib.h src/ghost.c src/ghost.h src/map.c src/map.h src/player.h src/audio.c src/audio.h
	$(CC) tests/test_ghost.c src/audio.c -o $(TEST_GHOST) $(TESTCFLAGS)

$(TEST_WIN): tests/test_win.c tests/test_framework.h src/map.c src/map.h
	$(CC) tests/test_win.c -o $(TEST_WIN) $(TESTCFLAGS)

$(TEST_LIVES): tests/test_lives.c tests/test_framework.h tests/stubs/raylib.h src/lives.c src/lives.h src/player.c src/player.h src/map.c src/map.h src/ghost.c src/ghost.h src/audio.c src/audio.h
	$(CC) tests/test_lives.c src/lives.c src/player.c src/ghost.c src/map.c src/audio.c -o $(TEST_LIVES) $(TESTCFLAGS)

$(TEST_COLLISION): tests/test_collision.c tests/test_framework.h tests/stubs/raylib.h src/collision.c src/collision.h src/player.c src/player.h src/ghost.c src/ghost.h src/map.c src/map.h src/audio.c src/audio.h
	$(CC) tests/test_collision.c src/collision.c src/player.c src/ghost.c src/map.c src/audio.c -o $(TEST_COLLISION) $(TESTCFLAGS)

lint:
	cppcheck --enable=all --error-exitcode=1 \
		--suppress=missingInclude \
		--suppress=missingIncludeSystem \
		--suppress=unmatchedSuppression \
		--suppress=normalCheckLevelMaxBranches \
		--suppress=staticFunction \
		--suppress=unusedFunction:src/map.c \
		src/
	flawfinder src/

clean:
	$(RM) $(OUT)
	$(RM) $(TEST_MAP) $(TEST_PLAYER) $(TEST_GHOST) $(TEST_WIN) $(TEST_LIVES) $(TEST_COLLISION)
