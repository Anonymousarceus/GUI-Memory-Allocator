#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../include/allocator.h"

int main(void) {
    printf("Dynamic Memory Allocator Demo\n");
    printf("=============================\n\n");
    
    // Initialize the allocator
    allocator_init();
    
    printf("1. Basic Memory Allocation\n");
    printf("--------------------------\n");
    
    // Allocate memory for different data types
    int* numbers = (int*)my_malloc(10 * sizeof(int));
    char* message = (char*)my_malloc(100 * sizeof(char));
    double* values = (double*)my_malloc(5 * sizeof(double));
    
    if (numbers && message && values) {
        // Initialize data
        for (int i = 0; i < 10; i++) {
            numbers[i] = i * i;
        }
        
        strcpy(message, "Hello from custom allocator!");
        
        for (int i = 0; i < 5; i++) {
            values[i] = i * 3.14159;
        }
        
        // Display data
        printf("Numbers: ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");
        
        printf("Message: %s\n", message);
        
        printf("Values: ");
        for (int i = 0; i < 5; i++) {
            printf("%.2f ", values[i]);
        }
        printf("\n\n");
    }
    
    print_heap_status();
    
    printf("2. Dynamic Array Example\n");
    printf("------------------------\n");
    
    // Create a dynamic array that grows
    int* array = (int*)my_malloc(5 * sizeof(int));
    int capacity = 5;
    int size = 0;
    
    // Add elements
    for (int i = 0; i < 12; i++) {
        if (size >= capacity) {
            // Need to grow the array
            capacity *= 2;
            printf("Growing array to capacity %d\n", capacity);
            array = (int*)my_realloc(array, capacity * sizeof(int));
        }
        array[size++] = i + 1;
    }
    
    printf("Dynamic array contents: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n\n");
    
    print_heap_status();
    
    printf("3. String Management\n");
    printf("-------------------\n");
    
    // Allocate and manipulate strings
    char* str1 = (char*)my_malloc(20);
    strcpy(str1, "Initial string");
    printf("String 1: %s\n", str1);
    
    // Expand string
    str1 = (char*)my_realloc(str1, 50);
    strcat(str1, " - expanded!");
    printf("Expanded string 1: %s\n", str1);
    
    // Zero-initialized string
    char* str2 = (char*)my_calloc(30, sizeof(char));
    strcpy(str2, "Zero-initialized");
    printf("String 2: %s\n", str2);
    
    print_heap_status();
    
    printf("4. Memory Pattern Demonstration\n");
    printf("-------------------------------\n");
    
    // Show fragmentation and merging
    void* ptrs[6];
    
    printf("Allocating 6 blocks...\n");
    for (int i = 0; i < 6; i++) {
        ptrs[i] = my_malloc(50);
        printf("Block %d allocated at %p\n", i, ptrs[i]);
    }
    
    print_heap_status();
    
    printf("Freeing every other block to create fragmentation...\n");
    for (int i = 1; i < 6; i += 2) {
        my_free(ptrs[i]);
        printf("Block %d freed\n", i);
    }
    
    print_heap_status();
    
    printf("Freeing remaining blocks (should trigger merging)...\n");
    for (int i = 0; i < 6; i += 2) {
        my_free(ptrs[i]);
        printf("Block %d freed\n", i);
    }
    
    print_heap_status();
    
    printf("5. Performance Test\n");
    printf("------------------\n");
    
    // Quick performance test
    clock_t start = clock();
    
    for (int i = 0; i < 1000; i++) {
        void* ptr = my_malloc(64);
        if (ptr) {
            my_free(ptr);
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("1000 allocate/free cycles took: %f seconds\n", time_taken);
    
    // Final cleanup
    my_free(numbers);
    my_free(message);
    my_free(values);
    my_free(array);
    my_free(str1);
    my_free(str2);
    
    printf("\n6. Final Status\n");
    printf("--------------\n");
    print_heap_status();
    
    if (get_total_allocated() == 0) {
        printf("✓ All memory successfully freed!\n");
    } else {
        printf("⚠ Memory leak detected: %zu bytes still allocated\n", get_total_allocated());
    }
    
    allocator_cleanup();
    
    printf("\nDemo completed!\n");
    return 0;
}