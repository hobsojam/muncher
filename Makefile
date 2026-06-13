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

TESTCFLAGS  = -Wall -Isrc -Itests/stubs --coverage
TEST_MAP    = tests/test_map
TEST_PLAYER = tests/test_player
TEST_GHOST  = tests/test_ghost
TEST_BINS   = $(TEST_MAP) $(TEST_PLAYER) $(TEST_GHOST)

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

test: $(TEST_BINS)
	./$(TEST_MAP)
	./$(TEST_PLAYER)
	./$(TEST_GHOST)

$(TEST_MAP): tests/test_map.c tests/test_framework.h tests/stubs/raylib.h src/map.c src/map.h
	$(CC) tests/test_map.c -o $(TEST_MAP) $(TESTCFLAGS)

$(TEST_PLAYER): tests/test_player.c tests/test_framework.h tests/stubs/raylib.h src/player.c src/player.h src/map.c src/map.h
	$(CC) tests/test_player.c -o $(TEST_PLAYER) $(TESTCFLAGS)

$(TEST_GHOST): tests/test_ghost.c tests/test_framework.h tests/stubs/raylib.h src/ghost.c src/ghost.h src/map.c src/map.h src/player.h
	$(CC) tests/test_ghost.c -o $(TEST_GHOST) $(TESTCFLAGS)

lint:
	cppcheck --enable=all --error-exitcode=1 \
		--suppress=missingInclude \
		--suppress=missingIncludeSystem \
		--suppress=unmatchedSuppression \
		--suppress=normalCheckLevelMaxBranches \
		src/
	flawfinder src/

clean:
	$(RM) $(OUT)
	$(RM) $(TEST_MAP) $(TEST_PLAYER) $(TEST_GHOST)
