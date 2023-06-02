#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>

#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT assert
#endif

#define ARENA_BACKEND_LIBC_MALLOC 0
#define ARENA_BACKEND_LINUX_MMAP 1

#ifndef ARENA_BACKEND
#define ARENA_BACKEND ARENA_BACKEND_LIBC_MALLOC
#endif // ARENA_BACKEND

typedef struct Region Region;

// try region implemented as a dynamic array instead of linked list
struct Region {
	Region *next;
    size_t count;
    size_t capacity;
    uintptr_t data[];
};

typedef struct {
	Region *begin, *end;
} Arena;

#define REGION_DEFAULT_CAPACITY (8 * 1024)

static Region *new_region(size_t capacity);
static void free_region(Region *r);

void *arena_alloc(Arena *a, size_t bytes);

void arena_reset(Arena *a);
void arena_free(Arena *a);

#ifdef ARENA_DEBUG
size_t new_region_calls = 0;
size_t times_region_skipped = 0;
size_t alloc_exceed_default = 0;
size_t internal_frag = 0;
#endif // ARENA_DEBUG statistics

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION

#if ARENA_BACKEND == ARENA_BACKEND_LIBC_MALLOC
#include <stdlib.h>

Region *new_region(size_t minimum_bytes)
{
	size_t capacity = REGION_DEFAULT_CAPACITY;
	if (minimum_bytes > capacity) {
		capacity = minimum_bytes;
		#ifdef ARENA_DEBUG
		alloc_exceed_default++;
		#endif
	}
	size_t size = sizeof(Region) + sizeof(uintptr_t) * capacity;
	Region *r = malloc(size);
	// Region *r = malloc(capacity);
	ARENA_ASSERT(r);
	r->next = NULL;
	r->count = 0;
	r->capacity = capacity;
	#ifdef ARENA_DEBUG
	new_region_calls++;
	#endif
	return r;
}

void free_region(Region *r)
{
	free(r);
}

#elif ARENA_BACKEND == ARENA_BACKEND_LINUX_MMAP
#include <sys/mman.h>

Region *new_region(size_t minimum_bytes)
{
	size_t capacity = REGION_DEFAULT_CAPACITY;
	if (minimum_bytes > capacity) {
		capacity = minimum_bytes;
		#ifdef ARENA_DEBUG
		alloc_exceed_default++;
		#endif
	}
	size_t size = sizeof(Region) + sizeof(uintptr_t) * capacity;
	Region *r = mmap (NULL, size,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
	ARENA_ASSERT(r);
	r->next = NULL;
	r->count = 0;
	r->capacity = capacity;
	#ifdef ARENA_DEBUG
	new_region_calls++;
	#endif
	return r;
}

void free_region(Region *r)
{
	munmap(r, sizeof(*r) + sizeof(*r->data) * r->capacity);
}
#else
#  error "Unknown Arena backend"
#endif

void *arena_alloc(Arena *a, size_t bytes)
{
	// byte-alignment
	size_t data_size = (bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

	// uninitialized arena
	if (a->end == NULL) {
		ARENA_ASSERT(a->begin == NULL);
		a->begin = new_region(data_size);
		a->end = a->begin;
	}

	while (a->end->count + data_size > a->end->capacity && a->end->next != NULL) {
		#ifdef ARENA_DEBUG
		times_region_skipped++;
		// internal fragmentation, want to avoid
		internal_frag += (a->end->capacity - a->end->count);
		#endif
		a->end = a->end->next;
	}

	if (a->end->count + data_size > a->end->capacity) {
		ARENA_ASSERT(a->end->next == NULL);
		#ifdef ARENA_DEBUG
		times_region_skipped++;
		// internal fragmentation, want to avoid
		internal_frag += (a->end->capacity - a->end->count);
		#endif
		a->end->next = new_region(data_size);
		a->end = a->end->next;
	}

	void *result = &(a->end->data[a->end->count]);
	a->end->count += data_size;
	return result;
}

void arena_reset(Arena *a)
{
	for (Region *r = a->begin; r != NULL; r = r->next) {
		r->count = 0;
	}
	a->end = a->begin;
}

void arena_free(Arena *a)
{
	Region *r = a->begin;
	while (r) {
		Region *temp = r;
		r = temp->next;
		free_region(temp);
	}
	a->begin = NULL;
	a->end = NULL;
}

#endif // ARENA_IMPLEMENTATION
