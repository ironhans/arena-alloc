# Arena Allocator
Arena allocator created from [this implementation](https://github.com/tsoding/arena).
No significant modifications yet aside from mmap implementation and tests.
Implemented as an [stb-style header](https://github.com/nothings/stb).

# Usage

## Examples
```c
#include <stdio.h>

#define ARENA_IMPLEMENTATION
// if not defined, malloc implementation is default
#define ARENA_BACKEND ARENA_BACKEND_LINUX_MMAP
#include "arena-alloc.h"

int main(void)
{
	// can intialize arena with null values
	Arena a = {NULL, NULL};
	char *strings[10];
	for (size_t i = 0; i < 10; i++) {
		strings[i] = arena_alloc(&a , sizeof(char) * 5);
		for (size_t j = 0; j < 5; j++) {
			strings[i][j] = (char) (rand() % 26 + 97);
		}
	}
	// no need to free each string individually, just free the arena
	arena_free(&a);
}
```

# Testing
```shell
# Run tests
make tests

# Run tests with valgrind
make tests-valgrind
```
