#ifndef HEAPMEMORYMANAGER_HEAPMEMORYMANAGER_H
#define HEAPMEMORYMANAGER_HEAPMEMORYMANAGER_H

/********************** Includes *********************/
#include <sys/types.h> // to use asystem types
#include <stdint.h>
/********************** Macros and Defines *******************/
#define  ARCH_x86_64
#ifdef ARCH_x86_64
#define align8(size) ( ( (size) % 8 == 0 ) ? ( (size) ): ( ( ( (size) + 8 ) ) - ( (size) % 8 ) ) )
#endif
#define METADATASIZE 32
#define PAGESIZE 4096
#define FACTOR 33
#define PAGESALLOC (PAGESIZE*FACTOR)
#define EIGHTBYTES 8
#define ALIGNMENTVALUE 8
#define  ALIGNED 0
#define SBRKFAILED ((ptrBlock) -1)
#define HEAPEXTENDFAILED ((size_t)-1)
#define FREE 1
#define USED 0
#define ZERO 0
#define CURRENTPROGRAMBREAK sbrk(0)
#define LARGEBLOCK 128*1024

#define SKIPMETADATA 1
#define DEBUG
/********************** Structs, Enums & typedefs ************/
/* I will use size_t According to the C language  standard,
     * it  shall  be an unsigned integer type capable of storing values in the range [0, SIZE_MAX].
     * Ac‐cording to POSIX, the implementation shall support one or more programming environments where the
     * width of size_t is no greater than the width of the type long.*/
typedef struct blockMetaData *ptrBlock;
struct blockMetaData {
    size_t size;
    ptrBlock next;
    ptrBlock prev;
    size_t free;

};
// size of struct will be 40 byte of metadata which will be aligned

/****************** functions prototypes ********************/
ptrBlock heap_Init(void);

void heap_Extent(void);

void *find_Block(ptrBlock last, size_t size);

void split_Block(ptrBlock block, size_t size);

ptrBlock merge_Blocks(ptrBlock block);

void  free(void *ptr);

void * malloc(size_t size);

void * realloc(void *ptr, size_t size);

void * calloc(size_t nmemb, size_t size);

#endif //HEAPMEMORYMANAGER_HEAPMEMORYMANAGER_H


