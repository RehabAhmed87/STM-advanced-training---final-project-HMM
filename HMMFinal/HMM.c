/*
 * Project Name : Heap Memory Manager
 * Author name : Rehab Ahmed
 * Under supervision of Eng. Reda Maher.
 *
 * */
/**********************Includes *********************/

#include "HMM.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include <sys/resource.h>
#include <unistd.h> // for brk and sbrk system calls

/*
 * At the beginning of the program we have the program break = eend
 * ,so we don't even have a heap to allocate in
 * first of all we will make a global pointer that only indicates base of heap
 * it will be initiated to NULL, so we can check it later
 * if it was NULL, so we should begin to have a heap
 * */
/* indicating base of the heap */
ptrBlock g_baseHeap = NULL;
ptrBlock g_lastHeap = NULL;


ptrBlock heap_Init(void) {
/* first we make a pointer to block to hold information of new page */
    ptrBlock b;
    b = sbrk(PAGESALLOC);
    if (b == SBRKFAILED) {
        /*On  success,  sbrk()  returns  the  previous  program break.
         * (If the break was increased, then this value is a pointer to the start of the newly allocated memory).
         * On error, (void *) -1 is returned, and errno  is  set  to ENOMEM.
         * */
        return (NULL);
    } else {
        /* now we have extended our heap with a new whole page */
        b->size = PAGESALLOC - METADATASIZE;
        b->next = NULL;
        b->prev = NULL;
        b->free = FREE;
        return (b);
    }
}

void heap_Extent(void) {
    /*This function only extends heap by taking last node and then expand it
     * with extra size allocated by sbrk*/
    size_t flag = (size_t) sbrk(PAGESALLOC);
    if (flag != HEAPEXTENDFAILED) {
       ptrBlock newLastBlock = ((void *)g_lastHeap)+ METADATASIZE + g_lastHeap->size;
       newLastBlock ->size = PAGESALLOC - METADATASIZE;
       newLastBlock->prev = g_lastHeap;
       newLastBlock ->next =NULL;
       newLastBlock->free=FREE;
       g_lastHeap = newLastBlock;
    }

}

void *find_Block(ptrBlock traversingBegin, size_t size) {
    /* our first station for searching is g_baseHeap, so we initialize a local variable with its value*/
    ptrBlock traversingBlock = traversingBegin;
    /*ptrBlock lastUsedBlock = NULL;*/
    void *returnedBlock = NULL;
    /* traversing all available blocks */
    while (((traversingBlock->free == USED) || (traversingBlock->size) < size) && (traversingBlock->next != NULL)) {
        traversingBlock = traversingBlock->next;
    }
    if (traversingBlock->next == NULL)
    {
        g_lastHeap = traversingBlock;
    }
    /*if I found block with needed size  */
    if (traversingBlock->free == FREE && (traversingBlock->size == size ||
                                           (traversingBlock->size >= size &&
                                           traversingBlock->size < size + METADATASIZE + EIGHTBYTES))) {
        traversingBlock->free = USED;
        returnedBlock = (void *) (((void *) traversingBlock) + METADATASIZE);
        return returnedBlock;
    } else if (traversingBlock->size >= size + METADATASIZE + EIGHTBYTES && traversingBlock ->free ==FREE ) {
    /* if size of found block exceeded aligned size and remaining size is greater than METADATASIZE and at least  extra eight bytes we can split the block which will increase fragmentation,
    * but we will solve it when we free them it also may be solved with best fit algorithm instead of first fit, but it may be also large blocks, so we will also need splitting block but calling it will be less in this case */
        split_Block(traversingBlock, size);
            traversingBlock->free = USED;
            returnedBlock = (void *) (((void *) traversingBlock) + METADATASIZE);
            return returnedBlock;
    }
      else {
        /*updating last heap block for extending the heap*/
        returnedBlock = NULL;
        return returnedBlock;
    }
}

void split_Block(ptrBlock block, size_t size) {
    /* We will split our block into two small blocks, first we initialize newBlock */
    ptrBlock newBlock;
   /* if (block ->free == FREE) {*/
        /* newBlock will point to a certain point in the big block given by following equation*/
        newBlock = ((void *) block) + METADATASIZE + size;
        /* newBlock size will be (big block size) - (given size for splitting) - (METADATASIZE for
         * preparing new node to be added to linked list ) */
        newBlock->size = block->size - size - METADATASIZE;
        /* new we add new block to linked list keeping updating old block till the end
         * , so we won't lose track of linked list*/
        newBlock->next = block->next;
        newBlock->prev = block;
        /* newBlock is a free block now which we can efficiently use*/
        newBlock->free = FREE;
        /* updating old block data */
        block->next = newBlock;
        block->size = size;
}

ptrBlock merge_Blocks(ptrBlock block) {
    ptrBlock checkLastBlock = NULL;
    /* First we check if given block isn't last one , and next block is free*/
    if ((block->next != NULL) && (block->next->free == FREE)) {
        /* we extend block size to fit two blocks */
        block->size += METADATASIZE + block->next->size;
        /* updating block next */
        block->next = block->next->next;
        if (block->next != NULL) {
            /* if my new next wasn't NULL then I need to update my next block prev to point to my merged block */
            block->next->prev = block;
        } else if (block->next == NULL && block->free == FREE) {
            checkLastBlock = block;
        }
    }
    /* First we check if given block isn't first one, and previous block is free this covers it I had a block wasn't merged previously as it's next block
     * wasn't free, but it's free now, so we can collect 3 blocks at once to reduce fragmentation */
    if ((block->prev != NULL) && (block->prev->free == FREE)) {
        /*updating previous block size to fit both two blocks */
        block->prev->size += METADATASIZE + block->size;
        /* updating block next */
        block->prev->next = block->next;
        if (block->next != NULL) {
            /* if my new next wasn't NULL then I need to update my next block prev to point to my merged block */
            block->next->prev = block->prev;
        } else if (block->next == NULL && block->free == FREE) {
            checkLastBlock = block->prev;
        }
    }
    /* here I check for last block whether it was a big freed block or it is now a big free block after merge
     * I can low the program break as this block is sufficiently large block according to glibc determination 128k */
    if (checkLastBlock != NULL && checkLastBlock->free == FREE && checkLastBlock->size >= LARGEBLOCK) {
        /* lower the program break */
        sbrk(-LARGEBLOCK);
    }
    return block;
}

void free(void *ptr) {

    /*FREE description from man page :
       The  free() function frees the memory space pointed to by ptr,
       which must have been returned by a previous call to malloc(), calloc(), or realloc().
       Otherwise, or if free(ptr) has already been called before, undefined  behavior occurs.
       If ptr is NULL, no operation is performed.*/

    /* First my malloc returns address skipping metadata block
     * ,so I should first Check if given pointer lies on my heap boundaries */
    if (ptr >= (void *) g_baseHeap && ptr <= CURRENTPROGRAMBREAK && ptr != NULL) {
        /* If true I should make sure that user gave me the right starting address block
         * I step back to metadata to check first if ptr = the real beginning of block
         * which was stored in ptrToData field if yes so it's a valid pointer which can be freed
         * and merged if it's possible
         * else then user gave me a random address can't be freed
         * also I check first if the block was actually used in order to avoid
         * freeing the same block twice */
        ptrBlock freedBlock = ((void *) (ptr)) - METADATASIZE;
            freedBlock->free = FREE;
            merge_Blocks(freedBlock);
    }
}

void * malloc(size_t size) {
    /* MALLOC description from man page : The  malloc()  function  allocates size bytes and returns a pointer to the allocated memory.
       The memory is not initialized.  If size is 0, then malloc() returns either NULL, or a unique pointer value that can later be successfully passed to free(). */
    ptrBlock block = NULL;
    size_t alignedSize;
    if (size == ZERO)
    {
        return NULL;
    }
    else if (size % ALIGNMENTVALUE != ALIGNED) {/* first we align needed size to smallest nearest multiple of 8 */
        alignedSize = align8(size);
    } else {
        alignedSize = size;
    }
    /* we check if g_baseHeap == NULl then it's malloc first call
     * , so we extend heap */
    if (g_baseHeap == NULL) {
        block = heap_Init();
        if (block == NULL) {
            return NULL;
        }
        g_baseHeap = block;
    }
    /* if it's not first malloc call then we try to find suitable block */
    block = find_Block(g_baseHeap, alignedSize);
    while (block == NULL) {
        heap_Extent();
        block = find_Block(g_lastHeap, alignedSize);
    }
    return ((void *) (block));

}

void * calloc(size_t nmemb, size_t size) {
    /* CALLOC from man page description which we will follow exactly in our design : The  calloc() function allocates memory for an array of nmemb elements of
      size bytes each and returnsa pointer to the allocated memory. The memory is set to zero.  If nmemb or size is 0, then calloc() returns either NULL,
       or  a unique pointer value that can later be successfully passed to free(). If the multiplication of nmemb and size would result in integer overflow,
       then calloc() returns an error.  By contrast, an integer overflow  would not  be  detected  in the following call to malloc(), with the result that an incorrectly sized block of memory would be allocated:*/
    /* first we initialize pointer block to hold malloced block */
    ptrBlock callocedBlock =NULL;
    if (nmemb == 0 || size ==0 )
    {
        /*return NULL */
        return callocedBlock;
    }
    else if ( nmemb * size > SIZE_MAX)
    {
        perror("ERROR! nmemb * size would result in integer overflow. \n");
        return NULL;
    }
    else {
        /* then we malloc block with my required size */
        callocedBlock = malloc(nmemb * size);
        if (callocedBlock != NULL) {
            callocedBlock--;
            size_t blockSize = callocedBlock->size;
            callocedBlock++;
            /* we step back to the beginning of block */
            /* here I use memset function to initialize the block with zeros */
            memset((void *)callocedBlock, 0, blockSize);
        }
        /* then we return callocedBlock block casting it to void pointer */
        return (void *) (callocedBlock);
    }
}

void * realloc(void *ptr, size_t size) {
    /* REALLOC description from man page which we will follow to cover all test cases: The realloc() function changes the size of the memory block pointed to by ptr to size bytes.
        The contents will be  unchanged in the range from the start of the region up to the minimum of the old and new sizes.  If the new size is larger than the old size, the added memory will
        not be initialized.  If ptr is NULL, then the  call  is equivalent  to  malloc(size), for  all values of size; if size is equal to zero, and ptr is not NULL, then the call
        is equivalent to free(ptr).  Unless ptr is NULL, it must have been returned by an  earlier  call  to  malloc(), calloc(), or realloc().
        If the area pointed to was moved, a free(ptr) is done.*/
    /* first if ptr = NULL then realloc will act like malloc(size)*/
    if ( ptr == NULL)
    { return malloc(size); }
    /* if ptr != NULL and size = zero realloc will act like free and returns null ptr */
    else if (size == ZERO)
    {   free(ptr);
        return NULL; }
    else {
        ptrBlock oldBlock = ptr;
        oldBlock--;
        size_t blockSize = oldBlock->size;
        /* here we check for given size if it was <= block size then we return same ptr */
        if (blockSize >= size) {
            return ptr;
        }
        /* we don't resize the given block unless it's size was greater than block size */
        else {
            ptrBlock newBlock = malloc(size);
            if (newBlock != NULL) {
                /* I use memcpy to copy data from src : oldBlock to dest : newBlock with size of : oldBlock*/
                memcpy((void *)newBlock, (const void *)ptr, blockSize);
            }
            /* freeing old block to reuse again, avoiding fragmentation */
            oldBlock--;
            oldBlock->free = FREE;
            return (void *) (newBlock);
        }
    }
}

