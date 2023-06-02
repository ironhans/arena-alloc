#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "test.h"

Arena arena_setup(void)
{
	Arena arena = {NULL, NULL};
	return arena;
}

void variable_sized_allocs(void)
{
	Arena a = arena_setup();
	char *strings[1000];
	for (size_t i = 0; i < 1000; i++) {
		size_t variable_len = rand() % 990 + 10;
		strings[i] = arena_alloc(&a , sizeof(char) * variable_len);
		for (size_t j = 0; j < variable_len; j++) {
			strings[i][j] = (char) (rand() % 26 + 97);
		}
	}
	arena_free(&a);

}

void big_mem_allocs(void)
{
	Arena a = arena_setup();
	char *strings[1000];
	for (size_t i = 0; i < 100; i++) {
		strings[i] = arena_alloc(&a , sizeof(char) * 10000);
		for (size_t j = 0; j < 10000; j++) {
			strings[i][j] = (char) (rand() % 26 + 97);
		}
	}
	arena_free(&a);
}

void large_linked_list_alloc(void)
{
	Arena a = arena_setup();
	Node *list = arena_alloc(&a , sizeof(Node));
	list->value = 0;
	Node *head = list;
	for (size_t i = 1; i < 1000000; i++) {
		Node *n = arena_alloc(&a, sizeof(Node));
		n->value = i;
		head->next = n;
		head = n;
	}
	head = list;
	for (int i = 0; i < 1000000; i++) {
		assert(head->value == i);
		head = head->next;
	}
	arena_free(&a);
}

int main(void)
{
	srand(time(NULL)); 

	variable_sized_allocs();
	printf("Allocating strings length of variable length (10 - 1000), 1000 times\n");
	printf("Number of new_region calls: %ld\n", new_region_calls);
	printf("Number of times region skipped: %ld\n", times_region_skipped);
	printf("Number of times alloc exceeded default: %ld\n", alloc_exceed_default);
	printf("Bytes lost to internal fragmentation: %ld\n", internal_frag);
	new_region_calls = 0;
	times_region_skipped = 0;
	alloc_exceed_default = 0;
	internal_frag = 0;

	printf("\n---------\n\n");

	// big_mem_allocs();
	// printf("Allocating strings length 10,000, 100 times\n");
	// printf("Number of new_region calls: %ld\n", new_region_calls);
	// printf("Number of times region skipped: %ld\n", times_region_skipped);
	// printf("Number of times alloc exceeded default: %ld\n", alloc_exceed_default);
	// printf("Bytes lost to internal fragmentation: %ld\n", internal_frag);
	// new_region_calls = 0;
	// times_region_skipped = 0;
	// alloc_exceed_default = 0;
	// internal_frag = 0;

	// printf("\n---------\n\n");

	large_linked_list_alloc();
	printf("Creating and destroying large linked list\n");
	printf("Number of new_region calls: %ld\n", new_region_calls);
	printf("Number of times region skipped: %ld\n", times_region_skipped);
	printf("Number of times alloc exceeded default: %ld\n", alloc_exceed_default);
	printf("Bytes lost to internal fragmentation: %ld\n", internal_frag);
	new_region_calls = 0;
	times_region_skipped = 0;
	alloc_exceed_default = 0;
	internal_frag = 0;

	return 0;
}
