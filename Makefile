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

SRC = src/main.c src/map.c src/player.c src/ghost.c src/lives.c src/collision.c src/audio.c src/fruit.c

TESTCFLAGS  = -Wall -Itests/stubs -Isrc -DMUNCHER_TEST --coverage -fprofile-abs-path
TEST_MAP    = tests/test_map
TEST_PLAYER = tests/test_player
TEST_GHOST  = tests/test_ghost
TEST_WIN    = tests/test_win
TEST_LIVES      = tests/test_lives
TEST_COLLISION  = tests/test_collision
TEST_AUDIO      = tests/test_audio
TEST_FRUIT      = tests/test_fruit
TEST_BINS   = $(TEST_MAP) $(TEST_PLAYER) $(TEST_GHOST) $(TEST_WIN) $(TEST_LIVES) $(TEST_COLLISION) $(TEST_AUDIO) $(TEST_FRUIT)
TEST_EXES   = $(addsuffix .exe,$(TEST_BINS))
TEST_OUTPUTS = $(TEST_BINS) $(TEST_EXES)
TEST_CLEAN_TARGETS = $(OUT) $(TEST_OUTPUTS)
TEST_CLEAN_SCRIPT = $(foreach file,$(TEST_CLEAN_TARGETS),if (Test-Path -LiteralPath '$(file)') { Remove-Item -LiteralPath '$(file)' -Force };)

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
	./$(TEST_AUDIO)
	./$(TEST_FRUIT)

$(TEST_MAP): tests/test_map.c tests/test_framework.h tests/stubs/raylib.h src/map.c src/map.h src/map_internal.h
	$(CC) tests/test_map.c src/map.c -o $(TEST_MAP) $(TESTCFLAGS)

$(TEST_PLAYER): tests/test_player.c tests/test_framework.h tests/stubs/raylib.h src/player.c src/player.h src/player_internal.h src/map.c src/map.h src/audio.c src/audio.h
	$(CC) tests/test_player.c src/player.c src/map.c src/audio.c -o $(TEST_PLAYER) $(TESTCFLAGS)

$(TEST_GHOST): tests/test_ghost.c tests/test_framework.h tests/stubs/raylib.h src/ghost.c src/ghost.h src/ghost_internal.h src/map.c src/map.h src/player.h src/audio.c src/audio.h
	$(CC) tests/test_ghost.c src/ghost.c src/map.c src/audio.c -o $(TEST_GHOST) $(TESTCFLAGS)

$(TEST_WIN): tests/test_win.c tests/test_framework.h src/map.c src/map.h
	$(CC) tests/test_win.c src/map.c -o $(TEST_WIN) $(TESTCFLAGS)

$(TEST_LIVES): tests/test_lives.c tests/test_framework.h tests/stubs/raylib.h src/lives.c src/lives.h src/player.c src/player.h src/map.c src/map.h src/ghost.c src/ghost.h src/audio.c src/audio.h
	$(CC) tests/test_lives.c src/lives.c src/player.c src/ghost.c src/map.c src/audio.c -o $(TEST_LIVES) $(TESTCFLAGS)

$(TEST_COLLISION): tests/test_collision.c tests/test_framework.h tests/stubs/raylib.h src/collision.c src/collision.h src/player.c src/player.h src/ghost.c src/ghost.h src/map.c src/map.h src/audio.c src/audio.h
	$(CC) tests/test_collision.c src/collision.c src/player.c src/ghost.c src/map.c src/audio.c -o $(TEST_COLLISION) $(TESTCFLAGS)

$(TEST_AUDIO): tests/test_audio.c tests/test_framework.h tests/stubs/raylib.h src/audio.c src/audio.h src/audio_internal.h
	$(CC) tests/test_audio.c src/audio.c -o $(TEST_AUDIO) $(TESTCFLAGS)

$(TEST_FRUIT): tests/test_fruit.c tests/test_framework.h tests/stubs/raylib.h src/fruit.c src/fruit.h src/player.c src/player.h src/map.c src/map.h src/audio.c src/audio.h
	$(CC) tests/test_fruit.c src/fruit.c src/player.c src/map.c src/audio.c -o $(TEST_FRUIT) $(TESTCFLAGS)

lint:
	cppcheck --enable=all --error-exitcode=1 \
		--suppress=missingInclude \
		--suppress=missingIncludeSystem \
		--suppress=unmatchedSuppression \
		--suppress=normalCheckLevelMaxBranches \
		--suppress=staticFunction \
		--suppress=unusedFunction:src/map.c \
		--suppress=unusedFunction:src/audio.c \
		src/
	flawfinder src/

clean:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "$(TEST_CLEAN_SCRIPT)"
else
	$(RM) $(OUT)
	$(RM) $(TEST_OUTPUTS)
endif
