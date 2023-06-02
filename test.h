#include <assert.h>

#define ARENA_DEBUG
#define ARENA_IMPLEMENTATION
// #define ARENA_BACKEND ARENA_BACKEND_LINUX_MMAP
#include "arena-alloc.h"

typedef struct Node Node;
struct Node {
	int value;
	Node *next;
};

Arena arena_setup(void);

void big_mem_allocs(void);
void variable_sized_allocs(void);
void large_linked_list_alloc(void);
