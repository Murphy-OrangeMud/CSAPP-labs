/*
 * mm.c
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

//#define NEXT_FIT

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
//#define DEBUG
#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* in 64 bit machines the requirement for alignment is 16 */
#define ALIGNMENT 16
#define WSIZE 8
#define DSIZE 16
#define MASK (~0xf)
#define CHUNK (mem_pagesize())

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT - 1)) & MASK)
#define ASIZE(size) ((((size_t)(size) + (ALIGNMENT - 1)) / ALIGNMENT) * ALIGNMENT)

/* some function macros */
#define GET(p) (*(size_t *)(p))
#define PUT(p, val) (*(size_t *)(p) = (val))

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define PACK(size, alloc) (((size)&MASK) | (alloc))

#define HDRP(p) ((char *)(p)-WSIZE)

/* the size and whether allocated and whether the previous block allocated of the block pointer p */
#define GET_SIZE(p) (GET(p) & (long) MASK)
#define GET_ALLOC(p) (GET(p) & 0x1l)

/* implicit link parameters */
#define FTRP(p) ((char *)(p) + GET_SIZE(HDRP(p)) - DSIZE)

/* the next and previous block of the block pointer p */
#define NEXT_BLKP(p) ((char *)(p) + GET_SIZE(HDRP(p)))
#define PREV_BLKP(p) ((char *)(p)-GET_SIZE(HDRP(p) - WSIZE))

/* whether the previous and next block allocated */
#define PREV_ALLOC(p) (GET_ALLOC(HDRP(PREV_BLKP(p))))
#define NEXT_ALLOC(p) (GET_ALLOC(HDRP(NEXT_BLKP(p))))

static char *heap_listp = 0;

#ifdef NEXT_FIT
static char *current_find
#endif

    /* some help functions */
    static void
    place(void *ptr, size_t asize);
static void *find_fit(size_t size);
static void *merge(void *p);
static void *extend_heap(size_t size);

/*
 * Initialize: return -1 on error, 0 on success.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);
    PUT(heap_listp + (WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
    heap_listp += 2 * WSIZE;
#ifdef NEXT_FIT
    current_find = heap_listp;
#endif
    if ((heap_listp = extend_heap(CHUNK)) == (void *)-1)
        return -1;
    return 0;
}

/*
 * malloc
 */
void *malloc(size_t size)
{
    size_t asize;
    void *ptr;

    dbg_printf("%ld\n", (long)heap_listp);
    if (heap_listp == 0)
        mm_init();
    if (size == 0)
        return NULL;
    asize = ASIZE(size + DSIZE);
    dbg_printf("malloc size: %ld %ld\n", size, asize);

    mm_checkheap(120);

    if ((ptr = find_fit(asize)) != NULL)
    {
        place(ptr, asize);
        mm_checkheap(125);
        return ptr;
    }
    else
    {
        ptr = extend_heap(asize);
        mm_checkheap(131);
        if (ptr == NULL)
            return NULL;
        place(ptr, asize);
        mm_checkheap(135);
        return ptr;
    }
}

/*
 * free
 */
void free(void *ptr)
{
    if (!ptr)
        return;
    if (heap_listp == 0)
        mem_init();
    size_t size = GET_SIZE(ptr);
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    merge(ptr);
    mm_checkheap(148);
}

/*
 * realloc - you may want to look at mm-naive.c
 */
void *realloc(void *oldptr, size_t size)
{
    if (oldptr == NULL)
        return malloc(size);
    if (size == 0)
    {
        free(oldptr);
        mm_checkheap(160);
        return NULL;
    }
    void *newptr = malloc(size);
    if (newptr == NULL)
        return NULL;
    size_t osize = GET_SIZE(HDRP(oldptr));
    memcpy(newptr, oldptr, MIN(size, osize));
    mm_checkheap(168);
    free(oldptr);
    mm_checkheap(170);
    return newptr;
}

/*
 * calloc - you may want to look at mm-naive.c
 * This function is not tested by mdriver, but it is
 * needed to run the traces.
 */
void *calloc(size_t nmemb, size_t size)
{
    size_t actsize = nmemb * size;
    void *newptr = malloc(actsize);
    memset(newptr, 0, actsize);
    mm_checkheap(190);
    return newptr;
}

/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void *p)
{
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
static int aligned(const void *p)
{
    return (size_t)ALIGN(p) == (size_t)p;
}

/*
 * mm_checkheap
 */
void mm_checkheap(int lineno)
{
    printf("enter checkheap in line %d\n", lineno);
    if (*(long *)(mem_heap_lo()) != (long)(0))
    {
        printf("Error: no padding at beginning, line %d\n", lineno);
        exit(1);
    }
    if (GET_SIZE(HDRP(heap_listp) - DSIZE) != 2 * WSIZE)
    {
        printf("Error: no prologue block, line %d\n", lineno);
        exit(1);
    }
    dbg_printf("begin loop...\n");
    void *p;
    #ifdef DEBUG
    int cnt = 0;
    #endif
    for (p = heap_listp; GET_SIZE(HDRP(p)) > 0; p = NEXT_BLKP(p))
    {
        if (!GET_ALLOC(HDRP(p)) && !NEXT_ALLOC(p)) {
            printf("Error: adjacent free blocks in address %ld, line %d\n", (long)p, lineno);
        }
        dbg_printf("the %d-th block with size %ld\n", cnt++, GET_SIZE(HDRP(p)));
        if (!aligned(p))
        {
            printf("Error: address not aligned in %ld, line %d\n", (long)p, lineno);
            exit(1);
        }
        if (!in_heap(HDRP(p)) || !in_heap(FTRP(p)))
        {
            printf("Error: address %ld out of heap range, line %d\n", (long)p, lineno);
            exit(1);
        }
        if ((*(long *)(HDRP(p))) != (*(long *)(FTRP(p))))
        {
            printf("Error: header and footer not consistent in %ld, line %d\n", (long)p, lineno);
            exit(1);
        }
    }
    if (*(long *)(HDRP(p)) != 1l) {
        printf("Error: no epilogure block, line %d\n", lineno);
        exit(1);
    }
    printf("quit checkheap in line %d\n", lineno);
}

/* return the pointer pointing to the extended free block, 
 * return -1 when fail to extend the heap
 * merge if there are adjacent free blocks */
static void *extend_heap(size_t size)
{
    size_t asize = ASIZE(size);
    char *bp;
    if ((bp = mem_sbrk(asize)) == (char *)-1)
        return NULL;
    PUT(HDRP(bp), PACK(asize, 0));
    PUT(FTRP(bp), PACK(asize, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    return merge(bp);
}

/* merge the adjacent free blocks and returns a pointer which points to the block */
static void *merge(void *p)
{
    size_t newsize = GET_SIZE(HDRP(p));
    if (GET_ALLOC(HDRP(p)))
        return NULL;
    if (PREV_ALLOC(p) && NEXT_ALLOC(p))
        return p;
    else if (PREV_ALLOC(p) && !NEXT_ALLOC(p))
    {
        newsize += GET_SIZE(HDRP(NEXT_BLKP(p)));
        PUT(HDRP(p), PACK(newsize, 0));
        PUT(FTRP(p), PACK(newsize, 0));
    }
    else if (!PREV_ALLOC(p) && NEXT_ALLOC(p))
    {
        newsize += GET_SIZE(HDRP(PREV_BLKP(p)));
        PUT(HDRP(PREV_BLKP(p)), PACK(newsize, 0));
        p = PREV_BLKP(p);
        PUT(FTRP(p), PACK(newsize, 0));
    }
    else if (!PREV_ALLOC(p) && !NEXT_ALLOC(p))
    {
        newsize += (GET_SIZE(HDRP(PREV_BLKP(p))) + GET_SIZE(HDRP(NEXT_BLKP(p))));
        PUT(HDRP(PREV_BLKP(p)), PACK(newsize, 0));
        p = PREV_BLKP(p);
        PUT(FTRP(p), PACK(newsize, 0));
    }
#ifdef NEXT_FIT
    if (current_find > p && current_find < FTRP(p))
    {
        current_find = p;
    }
#endif
    mm_checkheap(323);
    return p;
}

/* return the pointer pointing to the free block fitting the size */
static void *find_fit(size_t size)
{
#ifdef NEXT_FIT
    char *old_find = current_find;
    for (; current_find < mem_heap_hi(); current_find = NEXT_BLKP(current_find))
    {
        if (!GET_ALLOC(HDRP(current_find)) && GET_SIZE(HDRP(current_find)) >= size)
        {
            return current_find;
        }
    }
    for (current_find = heap_listp; current_find < old_find; current_find = NEXT_BLKP(current_find))
    {
        if (!GET_ALLOC(HDRP(current_find)) && GET_SIZE(HDRP(current_find)) >= size)
        {
            return current_find;
        }
    }
    return NULL;
#else
    for (void *ptr = heap_listp; GET_SIZE(HDRP(ptr)) > 0; ptr = NEXT_BLKP(ptr))
    {
        if (!GET_ALLOC(HDRP(ptr)) && (GET_SIZE(HDRP(ptr)) >= size))
        {
            return ptr;
        }
    }
    return NULL;
#endif
}

/* place memory of asize to where ptr points */
static void place(void *ptr, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(ptr));
    if (csize - asize >= (DSIZE * 2))
    {
        PUT(HDRP(ptr), PACK(asize, 1));
        PUT(FTRP(ptr), PACK(asize, 1));
        ptr = NEXT_BLKP(ptr);
        PUT(HDRP(ptr), PACK((csize - asize), 0));
        PUT(FTRP(ptr), PACK((csize - asize), 0));
    }
    else
    {
        PUT(HDRP(ptr), PACK(csize, 1));
        PUT(FTRP(ptr), PACK(csize, 1));
    }
    //mm_checkheap(378);
}