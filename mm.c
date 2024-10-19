/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "team 11",
    /* First member's full name */
    "DAHYUN HA",
    /* First member's email address */
    "halover111@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

//////////MACRO//////////
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)                               // 힙메모리 초기 가용 크기 (2**10)

#define MAX(x, y)           (x > y? x: y)
#define PACK(size, alloc)   (size | alloc)              // 입력받은 주소에 allocated값을 추가하여 리턴
#define GET(p)              (*(unsigned int *)p)
#define PUT(p, val)         (*(unsigned int*)p = val)
#define GET_SIZE(p)         (GET(p) & ~0x7)             // allocated 값 제외한 size만 리턴
#define GET_ALLOC(p)        (GET(p) & 0x1)              // size 제외한 allocated만 리턴

#define HDRP(bp)            ((char *)bp - WSIZE)                            // 헤더포인터
#define FTRP(bp)            ((char *)bp + GET_SIZE(HDRP(bp)) - DSIZE)       // 푸터포인터
#define NEXT_BLKP(bp)       ((char *)bp + GET_SIZE((char *)bp - WSIZE))     // 다음 가용 블록의 블록포인터
#define PREV_BLKP(bp)       ((char *)bp - GET_SIZE((char *)bp - WSIZE))     // 이전 가용 블록의 블록포인터

static char *heap_listp;
static void *extended_heap(size_t);


/* 최초 가용 블록으로 힙 생성: : 패딩 블록, 프롤로그 헤더/풋터, 에필로그 헤더 추가 */
int mm_init(void)
{
    if (heap_listp = mem_sbrk(4*WSIZE) == (void *)-1) return -1;

    PUT(heap_listp, 0);                             // 최소정렬기준 (alignment padding block)
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    // prologue header(8/1)
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    // prologue footer(8/1)
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));        // epilogue header(0/1)
    heap_listp += (2*WSIZE);
    
    if(extended_heap(CHUNKSIZE/WSIZE) == NULL) return -1;
     
    return 0;
}   

/* 새 가용 블록으로 힙 확장 */
static void *extended_heap(size_t words) {
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1) return NULL;

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp);
}

/* 가용리스트에서 블록 할당 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














