CC := gcc
CFLAGS := -Wall -pedantic -Wextra -Werror


SRC := $(wildcard ./*.c)
INC := $(wildcard ./*.h)
TEST_BIN := tests

.PHONY: tests tests-valgrind clean

run-tests: $(TEST_BIN)
	."/$(TEST_BIN)"

run-valgrind: $(TEST_BIN)
	valgrind --leak-check=full \
    	--show-leak-kinds=all \
    	--track-origins=yes \
    	--verbose \
    	--log-file="valgrind-out.txt" \
    	."/$(TEST_BIN)"
	cat "valgrind-out.txt"

$(TEST_BIN):
	$(CC) $(CFLAGS) -ggdb3 $(SRC) $(TEST) -o "tests"

clean:
	rm -rv $(TEST_BIN)
	rm -rv valgrind-out.txt
