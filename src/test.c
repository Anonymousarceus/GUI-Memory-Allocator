#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/allocator.h"

// Test function prototypes
void test_basic_allocation(void);
void test_free_and_reuse(void);
void test_fragmentation(void);
void test_merge_blocks(void);
void test_realloc(void);
void test_calloc(void);
void test_edge_cases(void);
void test_stress(void);
void benchmark_vs_stdlib(void);

// Utility functions
void print_test_header(const char* test_name);
void print_test_result(const char* test_name, int passed);

int main(void) {
    printf("Dynamic Memory Allocator Test Suite\n");
    printf("===================================\n\n");
    
    // Initialize allocator
    allocator_init();
    
    // Run all tests
    test_basic_allocation();
    test_free_and_reuse();
    test_fragmentation();
    test_merge_blocks();
    test_realloc();
    test_calloc();
    test_edge_cases();
    test_stress();
    
    // Performance benchmark
    benchmark_vs_stdlib();
    
    // Final heap status
    print_heap_status();
    
    // Validate heap integrity
    if (validate_heap()) {
        printf("✓ Heap validation passed\n");
    } else {
        printf("✗ Heap validation failed\n");
    }
    
    // Cleanup
    allocator_cleanup();
    
    printf("\nAll tests completed!\n");
    return 0;
}

void print_test_header(const char* test_name) {
    printf("\n--- %s ---\n", test_name);
}

void print_test_result(const char* test_name, int passed) {
    printf("%s: %s\n", test_name, passed ? "✓ PASSED" : "✗ FAILED");
}

void test_basic_allocation(void) {
    print_test_header("Basic Allocation Test");
    
    // Test different allocation sizes
    void* ptr1 = my_malloc(100);
    void* ptr2 = my_malloc(200);
    void* ptr3 = my_malloc(50);
    
    int success = (ptr1 != NULL && ptr2 != NULL && ptr3 != NULL);
    
    if (success) {
        // Write data to verify memory is usable
        strcpy((char*)ptr1, "Hello, World!");
        strcpy((char*)ptr2, "This is a longer string for testing purposes.");
        strcpy((char*)ptr3, "Short");
        
        // Verify data integrity
        success = (strcmp((char*)ptr1, "Hello, World!") == 0 &&
                  strcmp((char*)ptr2, "This is a longer string for testing purposes.") == 0 &&
                  strcmp((char*)ptr3, "Short") == 0);
    }
    
    print_heap_status();
    
    // Free all allocated memory
    my_free(ptr1);
    my_free(ptr2);
    my_free(ptr3);
    
    print_test_result("Basic Allocation", success);
}

void test_free_and_reuse(void) {
    print_test_header("Free and Reuse Test");
    
    // Allocate memory
    void* ptr1 = my_malloc(100);
    void* ptr2 = my_malloc(100);
    
    // Free first block
    my_free(ptr1);
    
    // Allocate same size - should reuse the freed block
    void* ptr3 = my_malloc(100);
    
    int success = (ptr1 == ptr3); // Should get the same address
    
    print_test_result("Free and Reuse", success);
    
    // Cleanup
    my_free(ptr2);
    my_free(ptr3);
}

void test_fragmentation(void) {
    print_test_header("Fragmentation Test");
    
    // Create fragmentation by allocating and freeing alternately
    void* ptrs[10];
    
    // Allocate 10 blocks
    for (int i = 0; i < 10; i++) {
        ptrs[i] = my_malloc(50);
    }
    
    // Free every other block
    for (int i = 1; i < 10; i += 2) {
        my_free(ptrs[i]);
    }
    
    printf("After creating fragmentation:\n");
    print_heap_status();
    
    int frag_count = get_fragmentation_count();
    
    // Try to allocate a large block that requires merging
    void* large_ptr = my_malloc(200);
    
    printf("After attempting large allocation:\n");
    print_heap_status();
    
    int success = (frag_count > 1); // Should have fragmentation
    
    print_test_result("Fragmentation", success);
    
    // Cleanup
    for (int i = 0; i < 10; i += 2) {
        my_free(ptrs[i]);
    }
    if (large_ptr) my_free(large_ptr);
}

void test_merge_blocks(void) {
    print_test_header("Block Merging Test");
    
    // Allocate three adjacent blocks
    void* ptr1 = my_malloc(100);
    void* ptr2 = my_malloc(100);
    void* ptr3 = my_malloc(100);
    
    printf("After allocating 3 blocks:\n");
    print_heap_status();
    
    // Free middle block
    my_free(ptr2);
    
    printf("After freeing middle block:\n");
    print_heap_status();
    
    // Free first block - should merge with middle
    my_free(ptr1);
    
    printf("After freeing first block (should merge):\n");
    print_heap_status();
    
    // Free last block - should merge all three
    my_free(ptr3);
    
    printf("After freeing last block (should merge all):\n");
    print_heap_status();
    
    int success = (get_fragmentation_count() <= 1); // Should have minimal fragmentation
    
    print_test_result("Block Merging", success);
}

void test_realloc(void) {
    print_test_header("Realloc Test");
    
    // Test realloc with expanding size
    void* ptr = my_malloc(50);
    strcpy((char*)ptr, "Initial data");
    
    ptr = my_realloc(ptr, 100);
    int success = (ptr != NULL && strcmp((char*)ptr, "Initial data") == 0);
    
    // Test realloc with shrinking size
    ptr = my_realloc(ptr, 30);
    success = success && (ptr != NULL);
    
    // Test realloc with NULL pointer (should act like malloc)
    void* ptr2 = my_realloc(NULL, 75);
    success = success && (ptr2 != NULL);
    
    // Test realloc with size 0 (should act like free)
    void* ptr3 = my_realloc(ptr2, 0);
    success = success && (ptr3 == NULL);
    
    print_test_result("Realloc", success);
    
    // Cleanup
    my_free(ptr);
}

void test_calloc(void) {
    print_test_header("Calloc Test");
    
    // Test calloc with zero initialization
    int* arr = (int*)my_calloc(10, sizeof(int));
    
    int success = (arr != NULL);
    
    // Verify all elements are zero
    for (int i = 0; i < 10 && success; i++) {
        if (arr[i] != 0) {
            success = 0;
        }
    }
    
    print_test_result("Calloc", success);
    
    // Cleanup
    my_free(arr);
}

void test_edge_cases(void) {
    print_test_header("Edge Cases Test");
    
    int success = 1;
    
    // Test malloc with size 0
    void* ptr1 = my_malloc(0);
    success = success && (ptr1 == NULL);
    
    // Test free with NULL
    my_free(NULL); // Should not crash
    
    // Test very large allocation
    void* ptr2 = my_malloc(HEAP_SIZE + 1);
    success = success && (ptr2 == NULL);
    
    // Test double free detection (should print error but not crash)
    void* ptr3 = my_malloc(100);
    my_free(ptr3);
    my_free(ptr3); // Should detect double free
    
    print_test_result("Edge Cases", success);
}

void test_stress(void) {
    print_test_header("Stress Test");
    
    const int NUM_ALLOCATIONS = 100;
    void* ptrs[NUM_ALLOCATIONS];
    int success = 1;
    
    // Random allocation and deallocation
    srand((unsigned int)time(NULL));
    
    // Initial allocations
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        size_t size = (rand() % 100) + 1;
        ptrs[i] = my_malloc(size);
        if (ptrs[i] == NULL) {
            // Out of memory is acceptable in stress test
            break;
        }
    }
    
    // Random free and realloc operations
    for (int i = 0; i < NUM_ALLOCATIONS / 2; i++) {
        int idx = rand() % NUM_ALLOCATIONS;
        if (ptrs[idx] != NULL) {
            if (rand() % 2) {
                my_free(ptrs[idx]);
                ptrs[idx] = NULL;
            } else {
                size_t new_size = (rand() % 200) + 1;
                ptrs[idx] = my_realloc(ptrs[idx], new_size);
            }
        }
    }
    
    printf("After stress operations:\n");
    print_heap_status();
    
    // Cleanup remaining allocations
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        if (ptrs[i] != NULL) {
            my_free(ptrs[i]);
        }
    }
    
    // Validate heap after stress test
    success = validate_heap();
    
    print_test_result("Stress Test", success);
}

void benchmark_vs_stdlib(void) {
    print_test_header("Performance Benchmark");
    
    const int NUM_OPERATIONS = 10000;
    clock_t start, end;
    
    // Benchmark custom allocator
    start = clock();
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        void* ptr = my_malloc(64);
        if (ptr) my_free(ptr);
    }
    end = clock();
    double custom_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Benchmark standard allocator
    start = clock();
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        void* ptr = malloc(64);
        if (ptr) free(ptr);
    }
    end = clock();
    double stdlib_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Custom allocator time: %.6f seconds\n", custom_time);
    printf("Standard allocator time: %.6f seconds\n", stdlib_time);
    printf("Performance ratio: %.2fx\n", custom_time / stdlib_time);
}