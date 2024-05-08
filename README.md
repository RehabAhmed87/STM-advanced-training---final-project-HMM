# HMM (Heap Memory Manager)

HMM (Heap Memory Manager) is a fundamental component of any programming environment, responsible for dynamically allocating and deallocating memory during program execution. This project aims to develop a custom Heap Memory Manager, tailored to meet the specific requirements of a given system or application. Key goals include dynamic memory allocation, deallocation, implementing various memory management policies, optimizing performance, and integrating error handling mechanisms.

## Implementation Overview

The project provides functions for initializing the heap, extending it, finding suitable memory blocks, splitting blocks when necessary, merging adjacent free blocks to reduce fragmentation, and providing interfaces for memory allocation (`malloc`), deallocation (`free`), reallocation (`realloc`), and (`calloc`).

### Malloc Key Features

- **Dynamic Memory Allocation:** Allocates memory of the specified size.
- **First-Fit Strategy:** Searches for a suitable free block in the heap using the first-fit algorithm.
- **Heap Initialization:** If it's the first call to `malloc`, it initializes the heap using the `heap_Init` function.
- **Heap Extension:** Extends the heap if no suitable block is found by calling the `heap_Extent` function.
- **Block Splitting:** Splits blocks if necessary to accommodate the requested size.
- **Metadata Management:** Manages metadata associated with memory blocks for bookkeeping.

### Free Key Features

- **Memory Deallocation:** Frees the memory block pointed to by the given pointer.
- **Block Marking:** Marks the memory block as free to indicate that it's available for future allocations.
- **Block Merging:** Merges adjacent free blocks to reduce fragmentation and optimize memory usage.
- **Heap Management:** Efficiently manages the heap structure to maintain a list of free and allocated blocks.
- **Validity Check:** Ensures that the given pointer is within the boundaries of the heap and points to a valid allocated block before freeing it.

### Calloc Key Features

- **Safe Memory Allocation:** Ensures secure allocation of memory for arrays while guarding against potential integer overflow.
- **Zero Initialization:** Initializes the allocated memory block with zeros, guaranteeing a clean slate for data storage.
- **Error Detection:** Detects and handles integer overflow scenarios during memory size calculation, preventing undefined behavior.
- **Integration with malloc:** Seamlessly utilizes the existing `malloc` function for memory allocation, maintaining consistency and reliability.

### Realloc Key Features

- **Dynamic Memory Resizing:** Allows for dynamic resizing of memory blocks, enabling flexible memory management during runtime.
- **Memory Preservation:** Preserves the contents of the original memory block up to the minimum of the old and new sizes, ensuring data integrity.
- **Efficient Memory Handling:** Optimizes memory utilization by reallocating memory only when necessary, reducing wastage and fragmentation.
- **Integration with `mymalloc` and `myfree`:** Seamlessly integrates with existing memory allocation and deallocation functions, maintaining compatibility and reliability.
- **Error Handling:** Handles NULL pointer inputs gracefully, treating them as equivalent to a call to `mymalloc` for allocation or `myfree` for deallocation.
- **Fragmentation Reduction:** Minimizes memory fragmentation by consolidating memory blocks and freeing the old block after reallocation, enhancing memory efficiency.
