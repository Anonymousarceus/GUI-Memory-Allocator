# Dynamic Memory Allocator

A custom implementation of dynamic memory allocation in C, featuring custom `malloc`, `free`, `realloc`, and `calloc` functions. This project demonstrates deep understanding of memory management concepts including pointers, memory segmentation, and linked list-based free block management.

## Overview

This project implements a complete memory allocation system that manages a static heap using a doubly-linked list of free blocks. The allocator includes advanced features like block splitting, merging adjacent free blocks, and memory alignment.

## Key Features

- **Custom Memory Functions**: Complete implementation of `malloc`, `free`, `realloc`, and `calloc`
- **Linked List Free Block Management**: Efficient tracking of available memory using doubly-linked lists
- **Block Merging**: Automatic coalescing of adjacent free blocks to reduce fragmentation
- **Memory Alignment**: Proper alignment of allocated memory for optimal performance
- **Interactive GUI Visualizer**: Real-time visual representation of memory operations
- **Debugging Tools**: Heap validation, memory dump, and statistics tracking
- **Comprehensive Testing**: Extensive test suite covering edge cases and stress testing

## Core Concepts Demonstrated

### 1. Pointers
- Manipulation of raw memory addresses
- Pointer arithmetic for block traversal
- Type casting between different pointer types
- Validation of pointer boundaries

### 2. Memory Segmentation
- Static heap management within a fixed memory region
- Block header structure for metadata storage
- Separation of allocated and free memory regions
- Memory layout optimization

### 3. Linked List Free Blocks
- Doubly-linked list of free memory blocks
- Efficient insertion and removal of free blocks
- First-fit allocation strategy
- Block splitting for optimal memory usage

## Project Structure

```
memory_allocator/
├── include/
│   └── allocator.h          # Header file with function declarations
├── src/
│   ├── allocator.c          # Core allocator implementation
│   ├── test.c               # Comprehensive test suite
│   └── demo.c               # Usage demonstration
├── Makefile                 # Build configuration
└── README.md                # This file
```

## Data Structures

### Block Header
```c
typedef struct block_header {
    size_t size;                    // Size of the block (excluding header)
    int is_free;                    // 1 if block is free, 0 if allocated
    struct block_header* next;      // Next block in the free list
    struct block_header* prev;      // Previous block in the free list
} block_header_t;
```

### Memory Layout
```
[Header][Data][Header][Data][Header][Data]...
   ^       ^      ^       ^      ^       ^
   |       |      |       |      |       |
   +-------+      +-------+      +-------+
   Block 1        Block 2        Block 3
```

## Algorithm Details

### Allocation Process (`my_malloc`)
1. Initialize allocator if not already done
2. Align requested size to memory boundary
3. Search free list for suitable block using first-fit strategy
4. Split block if significantly larger than needed
5. Remove block from free list and mark as allocated
6. Return pointer to data area (after header)

### Deallocation Process (`my_free`)
1. Validate pointer and get block header
2. Mark block as free
3. Add block back to free list
4. Merge with adjacent free blocks to reduce fragmentation

### Block Merging
Adjacent free blocks are automatically merged to create larger contiguous free regions:
```
Before: [Free A][Free B][Alloc C] → After: [Free A+B][Alloc C]
```

### Memory Alignment
All allocations are aligned to 8-byte boundaries for optimal performance across different architectures.

## Interactive GUI Visualizer

The project includes a Windows GUI application that provides real-time visualization of memory allocation operations:

### GUI Features
- **Visual Memory Map**: Horizontal bar chart showing allocated/free blocks
- **Interactive Controls**: Buttons for malloc, free, realloc, calloc operations
- **Real-time Statistics**: Live heap status and fragmentation metrics
- **Pointer Management**: List of allocated pointers with sizes and addresses
- **Color-coded Blocks**: 
  - 🟢 Green: Free memory blocks
  - 🔴 Red: Allocated memory blocks  
  - ⚪ Gray: Block headers
- **Stress Testing**: Built-in stress test functionality
- **Block Merging**: Manual trigger for free block coalescing

### Running the GUI
```bash
# Build all components including GUI
.\build.bat

# Launch the GUI (Windows only)
Start-Process -FilePath "build\gui.exe"
# or use the PowerShell helper
.\run_gui.ps1
```

### GUI Controls
1. **Size Input**: Enter allocation size in bytes
2. **malloc()**: Allocate new memory block
3. **free()**: Free selected allocated block
4. **realloc()**: Resize selected block to new size
5. **calloc()**: Allocate zero-initialized memory
6. **Clear All**: Free all allocated blocks
7. **Stress Test**: Allocate multiple blocks automatically
8. **Merge Blocks**: Trigger manual block coalescing


## Building and Running

### Prerequisites
- GCC compiler
- Windows OS (for GUI component)
- Make utility (optional)

### Build Commands
```bash
# Build all components (console + GUI)
.\build.bat

# Run comprehensive tests
.\build\memory_allocator_test.exe

# Run the demo program
.\build\demo.exe

# Launch interactive GUI
Start-Process -FilePath "build\gui.exe"

# Clean build files
.\clean.bat
```

### Unix/Linux (Console applications only)
```bash
# Build the test suite
make

# Run comprehensive tests
make test

# Run the demo program
make demo

# Clean build files
make clean
```

## Testing

The project includes a comprehensive test suite that covers:

- **Basic Allocation**: Simple malloc/free operations
- **Memory Reuse**: Verification of freed block reuse
- **Fragmentation**: Testing memory fragmentation scenarios
- **Block Merging**: Validation of adjacent block coalescing
- **Realloc Operations**: Dynamic memory resizing
- **Calloc Zero-initialization**: Zeroed memory allocation
- **Edge Cases**: Boundary conditions and error handling
- **Stress Testing**: High-volume allocation/deallocation
- **Performance Benchmarks**: Comparison with standard library

### Running Tests
```bash
make test
```

### Sample Test Output
```
Dynamic Memory Allocator Test Suite
===================================

--- Basic Allocation Test ---
Memory allocator initialized with 1048576 bytes
✓ Basic Allocation: PASSED

--- Free and Reuse Test ---
✓ Free and Reuse: PASSED

--- Fragmentation Test ---
✓ Fragmentation: PASSED
...
```

## API Reference

### Core Functions

#### `void* my_malloc(size_t size)`
Allocates a block of memory of the specified size.
- **Parameters**: `size` - Number of bytes to allocate
- **Returns**: Pointer to allocated memory, or NULL on failure

#### `void my_free(void* ptr)`
Frees a previously allocated block of memory.
- **Parameters**: `ptr` - Pointer to memory block to free

#### `void* my_realloc(void* ptr, size_t size)`
Resizes a memory block to a new size.
- **Parameters**: `ptr` - Existing memory block, `size` - New size
- **Returns**: Pointer to resized memory block

#### `void* my_calloc(size_t num, size_t size)`
Allocates zero-initialized memory for an array.
- **Parameters**: `num` - Number of elements, `size` - Size per element
- **Returns**: Pointer to zero-initialized memory

### Utility Functions

#### `void allocator_init(void)`
Initializes the memory allocator (called automatically).

#### `void print_heap_status(void)`
Prints current heap statistics and memory usage.

#### `size_t get_total_allocated(void)`
Returns total allocated memory in bytes.

#### `size_t get_total_free(void)`
Returns total free memory in bytes.

#### `int validate_heap(void)`
Validates heap integrity and returns success status.

## Performance Characteristics

### Time Complexity
- **Allocation**: O(n) worst case (linear search of free list)
- **Deallocation**: O(1) average case, O(n) worst case (merging)
- **Reallocation**: O(n) (may require copying data)

### Space Complexity
- **Overhead**: 32 bytes per block (header structure)
- **Fragmentation**: Minimized through block merging
- **Alignment**: 8-byte alignment for all allocations

### Memory Efficiency
The allocator minimizes waste through:
- Block splitting for size optimization
- Automatic merging of adjacent free blocks
- Aligned allocations for performance
- Minimum block size enforcement

## Advanced Features

### Debug Support
- Heap validation checks
- Memory leak detection
- Double-free detection
- Boundary checking

### Statistics Tracking
- Total allocated memory
- Total free memory
- Fragmentation count
- Allocation patterns

### Error Handling
- Invalid pointer detection
- Out-of-memory conditions
- Double-free protection
- Heap corruption detection

## Example Usage

```c
#include "allocator.h"

int main(void) {
    // Initialize allocator
    allocator_init();
    
    // Allocate memory
    int* array = (int*)my_malloc(10 * sizeof(int));
    
    // Use memory
    for (int i = 0; i < 10; i++) {
        array[i] = i * i;
    }
    
    // Resize memory
    array = (int*)my_realloc(array, 20 * sizeof(int));
    
    // Print heap status
    print_heap_status();
    
    // Free memory
    my_free(array);
    
    return 0;
}
```

## Implementation Notes

### Design Decisions
1. **Static Heap**: Uses a fixed-size heap for simplicity and predictability
2. **First-Fit Strategy**: Balances allocation speed with fragmentation
3. **Doubly-Linked Free List**: Enables efficient block removal and merging
4. **Immediate Coalescing**: Merges free blocks immediately to reduce fragmentation

### Limitations
- Fixed heap size (1MB by default)
- No thread safety (single-threaded use only)
- No virtual memory support
- Linear search for free blocks (could be optimized with better data structures)

### Potential Improvements
- Implement best-fit or buddy allocation strategies
- Add thread safety with mutexes
- Use balanced trees or segregated lists for free block management
- Support dynamic heap expansion
- Add memory debugging features (bounds checking, leak detection)

## Educational Value

This project demonstrates:
- **Low-level Memory Management**: Direct manipulation of memory layout
- **Data Structure Implementation**: Linked list operations in C
- **Algorithm Design**: Memory allocation strategies and optimizations
- **System Programming**: Understanding of memory segmentation and alignment
- **Testing Methodology**: Comprehensive validation of memory management code

## License

This project is intended for educational purposes and demonstrates fundamental concepts in systems programming and memory management.

## Contributing

This project serves as an educational example. Suggestions for improvements or additional features are welcome for learning purposes.

---

*This implementation showcases the fundamental concepts behind memory allocation systems and provides a foundation for understanding how malloc/free work at a low level.*#   G U I - M e m o r y - A l l o c a t o r  
 