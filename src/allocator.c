#include "../include/allocator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

// Global variables
char heap[HEAP_SIZE];                  // Static heap memory (exposed for GUI)
static block_header_t* free_list_head; // Head of free blocks list
static int allocator_initialized = 0;  // Initialization flag
static size_t total_allocated = 0;     // Total allocated memory
static size_t total_free = HEAP_SIZE;  // Total free memory

// Initialize the allocator
void allocator_init(void) {
    if (allocator_initialized) {
        return;
    }
    
    // Initialize the first free block covering the entire heap
    free_list_head = (block_header_t*)heap;
    free_list_head->size = HEAP_SIZE - HEADER_SIZE;
    free_list_head->is_free = 1;
    free_list_head->next = NULL;
    free_list_head->prev = NULL;
    
    total_allocated = 0;
    total_free = HEAP_SIZE - HEADER_SIZE;
    allocator_initialized = 1;
    
    printf("Memory allocator initialized with %zu bytes\n", HEAP_SIZE);
}

// Align size to the specified alignment boundary
size_t align_size(size_t size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

// Find a free block that can accommodate the requested size
block_header_t* find_free_block(size_t size) {
    // First check if we have any free blocks at all
    if (free_list_head == NULL) {
        return NULL;
    }
    
    block_header_t* current = free_list_head;
    
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL; // No suitable block found
}

// Split a block if it's larger than needed
block_header_t* split_block(block_header_t* block, size_t size) {
    if (block->size <= size + HEADER_SIZE + MIN_BLOCK_SIZE) {
        // Block is too small to split
        return NULL;
    }
    
    // Create new block header after the allocated portion
    block_header_t* new_block = (block_header_t*)((char*)block + HEADER_SIZE + size);
    new_block->size = block->size - size - HEADER_SIZE;
    new_block->is_free = 1;
    new_block->next = NULL;
    new_block->prev = NULL;
    
    // Update the original block size
    block->size = size;
    
    // Add the new block to the free list
    add_to_free_list(new_block);
    
    return new_block;
}

// Add block to the free list
void add_to_free_list(block_header_t* block) {
    if (free_list_head == NULL) {
        free_list_head = block;
        block->next = NULL;
        block->prev = NULL;
        return;
    }
    
    // Insert at the beginning of the free list
    block->next = free_list_head;
    block->prev = NULL;
    if (free_list_head) {
        free_list_head->prev = block;
    }
    free_list_head = block;
}

// Remove block from the free list
void remove_from_free_list(block_header_t* block) {
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        free_list_head = block->next;
    }
    
    if (block->next) {
        block->next->prev = block->prev;
    }
    
    block->next = NULL;
    block->prev = NULL;
}

// Merge adjacent free blocks
void merge_free_blocks(void) {
    char* heap_start = heap;
    char* heap_end = heap + HEAP_SIZE;
    char* current_pos = heap_start;
    
    while (current_pos < heap_end) {
        block_header_t* current_block = (block_header_t*)current_pos;
        
        if (current_block->is_free) {
            char* next_pos = current_pos + HEADER_SIZE + current_block->size;
            
            // Check if next block exists and is adjacent
            if (next_pos < heap_end) {
                block_header_t* next_block = (block_header_t*)next_pos;
                
                if (next_block->is_free) {
                    // Merge blocks
                    remove_from_free_list(next_block);
                    current_block->size += HEADER_SIZE + next_block->size;
                    continue; // Don't advance current_pos, check for more merges
                }
            }
        }
        
        current_pos += HEADER_SIZE + current_block->size;
    }
}

// Custom malloc implementation
void* my_malloc(size_t size) {
    if (!allocator_initialized) {
        allocator_init();
    }
    
    if (size == 0) {
        return NULL;
    }
    
    // Align the requested size
    size = align_size(size);
    
    // Ensure minimum block size
    if (size < MIN_BLOCK_SIZE) {
        size = MIN_BLOCK_SIZE;
    }
    
    // Find a suitable free block
    block_header_t* block = find_free_block(size);
    
    if (block == NULL) {
        // Try to merge free blocks and search again
        merge_free_blocks();
        block = find_free_block(size);
        
        if (block == NULL) {
            printf("Error: Out of memory. Cannot allocate %zu bytes.\n", size);
            return NULL;
        }
    }
    
    // Remove block from free list
    remove_from_free_list(block);
    
    // Split block if it's larger than needed
    split_block(block, size);
    
    // Mark block as allocated
    block->is_free = 0;
    
    // Update statistics
    total_allocated += block->size;
    total_free -= block->size;
    
    // Return pointer to data (after header)
    return (char*)block + HEADER_SIZE;
}

// Custom free implementation
void my_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    // Get block header from pointer
    block_header_t* block = (block_header_t*)((char*)ptr - HEADER_SIZE);
    
    // Validate pointer
    if ((char*)block < heap || (char*)block >= heap + HEAP_SIZE) {
        printf("Error: Invalid pointer passed to my_free\n");
        return;
    }
    
    if (block->is_free) {
        printf("Error: Double free detected\n");
        return;
    }
    
    // Mark block as free
    block->is_free = 1;
    
    // Update statistics
    total_allocated -= block->size;
    total_free += block->size;
    
    // Add block back to free list
    add_to_free_list(block);
    
    // Merge adjacent free blocks
    merge_free_blocks();
}

// Custom realloc implementation
void* my_realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return my_malloc(size);
    }
    
    if (size == 0) {
        my_free(ptr);
        return NULL;
    }
    
    block_header_t* block = (block_header_t*)((char*)ptr - HEADER_SIZE);
    size_t old_size = block->size;
    
    // If new size fits in current block, no need to reallocate
    if (size <= old_size) {
        return ptr;
    }
    
    // Allocate new block
    void* new_ptr = my_malloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    // Copy old data to new block
    memcpy(new_ptr, ptr, old_size);
    
    // Free old block
    my_free(ptr);
    
    return new_ptr;
}

// Custom calloc implementation
void* my_calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    
    // Check for overflow
    if (num != 0 && total_size / num != size) {
        return NULL;
    }
    
    void* ptr = my_malloc(total_size);
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }
    
    return ptr;
}

// Get total allocated memory
size_t get_total_allocated(void) {
    return total_allocated;
}

// Get total free memory
size_t get_total_free(void) {
    return total_free;
}

// Count memory fragmentation (number of free blocks)
int get_fragmentation_count(void) {
    int count = 0;
    block_header_t* current = free_list_head;
    
    while (current != NULL) {
        if (current->is_free) {
            count++;
        }
        current = current->next;
    }
    
    return count;
}

// Print heap status
void print_heap_status(void) {
    printf("\n=== Heap Status ===\n");
    printf("Total heap size: %d bytes\n", HEAP_SIZE);
    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Total free: %zu bytes\n", total_free);
    printf("Fragmentation: %d free blocks\n", get_fragmentation_count());
    printf("==================\n\n");
}

// Validate heap integrity
int validate_heap(void) {
    char* heap_start = heap;
    char* heap_end = heap + HEAP_SIZE;
    char* current_pos = heap_start;
    
    while (current_pos < heap_end) {
        block_header_t* block = (block_header_t*)current_pos;
        
        // Check if block header is within heap bounds
        if ((char*)block + HEADER_SIZE > heap_end) {
            printf("Error: Block header extends beyond heap\n");
            return 0;
        }
        
        // Check if block data is within heap bounds
        if ((char*)block + HEADER_SIZE + block->size > heap_end) {
            printf("Error: Block data extends beyond heap\n");
            return 0;
        }
        
        current_pos += HEADER_SIZE + block->size;
    }
    
    return 1; // Heap is valid
}

// Dump heap contents for debugging
void dump_heap(void) {
    printf("\n=== Heap Dump ===\n");
    char* heap_start = heap;
    char* heap_end = heap + HEAP_SIZE;
    char* current_pos = heap_start;
    int block_num = 0;
    
    while (current_pos < heap_end) {
        block_header_t* block = (block_header_t*)current_pos;
        printf("Block %d: Size=%zu, Free=%s, Address=%p\n", 
               block_num++, block->size, 
               block->is_free ? "Yes" : "No", 
               (void*)block);
        
        current_pos += HEADER_SIZE + block->size;
        
        if (current_pos >= heap_end) break;
    }
    printf("================\n\n");
}

// Cleanup allocator
void allocator_cleanup(void) {
    if (allocator_initialized) {
        printf("Allocator cleanup: %zu bytes still allocated\n", total_allocated);
        allocator_initialized = 0;
    }
}