#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

// Memory block header structure
typedef struct block_header {
    size_t size;                    // Size of the block (excluding header)
    int is_free;                    // 1 if block is free, 0 if allocated
    struct block_header* next;      // Next block in the free list
    struct block_header* prev;      // Previous block in the free list
} block_header_t;

// Constants
#define HEAP_SIZE (1024 * 1024)     // 1MB heap size
#define MIN_BLOCK_SIZE 16           // Minimum allocation size
#define HEADER_SIZE sizeof(block_header_t)
#define ALIGNMENT 8                 // Memory alignment requirement

// External heap for GUI access
extern char heap[HEAP_SIZE];

// Memory allocator functions
void* my_malloc(size_t size);
void my_free(void* ptr);
void* my_realloc(void* ptr, size_t size);
void* my_calloc(size_t num, size_t size);

// Utility functions
void allocator_init(void);
void allocator_cleanup(void);
void print_heap_status(void);
size_t get_total_allocated(void);
size_t get_total_free(void);
int get_fragmentation_count(void);

// Internal helper functions (for testing and debugging)
void merge_free_blocks(void);
block_header_t* split_block(block_header_t* block, size_t size);
block_header_t* find_free_block(size_t size);
void add_to_free_list(block_header_t* block);
void remove_from_free_list(block_header_t* block);

// Memory alignment utility
size_t align_size(size_t size);

// Debugging and validation
int validate_heap(void);
void dump_heap(void);

#endif // ALLOCATOR_H