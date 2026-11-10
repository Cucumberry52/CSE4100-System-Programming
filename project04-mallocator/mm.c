/*
 * mm.c - segregated free list allocator!!
 * 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h> 
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * Fill these fields with your own information before handin.
 ********************************************************/
team_t team = {
    /* Your student ID */
    "CSE4100",
    /* Your full name*/
    "student",
    /* Your email address */
    "",
};

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)

//////// 내가 정의한 함수//////////////////////////////
#define LISTS 20
#define REALLOC_SMALL_BUFFER 128
#define REALLOC_BIG_BUFFER (1 << 12)
#define REALLOC_SPLIT_LIMIT (REALLOC_BIG_BUFFER + MIN_FREE_BLOCK)
///////////////////////////////////////////////////////////

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (unsigned int)(val))
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))



/////////////내가 정의한 함수/////////////////////////////////////
#define PREV_free(bp) (*(char **)(bp))
#define SUCC(bp) (*(char **)((char *)(bp) + sizeof(char *)))

#define OVERHEAD_BYTES ( WSIZE + WSIZE)
#define MIN_FREE_BLOCK (ALIGN(OVERHEAD_BYTES + 2 * sizeof(char *)))

static char *heap_pointer = NULL;
static char *list_areas = NULL;

#define LIST_HEAD(i) (*(char **)(list_areas + ((i) * sizeof(char *))))

static int get_list_index(size_t size)
{
    int index;

    for (index = 0, size >>= 4; (index < LISTS - 1) && size > 1;index++) {
        size >>= 1;
    }

    return index;
}

static size_t make_block_size(size_t bytes)
{
    size_t block_size;

    if (bytes == 0)
        return 0;

    if (bytes > 96 && bytes <= 112) bytes = 128;
    else if (bytes > 384 && bytes <= 448)  bytes = 512;
    

    block_size = ALIGN(bytes + OVERHEAD_BYTES);

    if (block_size < MIN_FREE_BLOCK)
        return MIN_FREE_BLOCK;

    return block_size;
}

static void add_free_block(void *block)
{
    int list_index = get_list_index(GET_SIZE(HDRP(block)));
    char *first = LIST_HEAD(list_index);

    PREV_free(block) = NULL;
    SUCC(block) = first;

    if (first) PREV_free(first) = block;

    LIST_HEAD(list_index) = block;
}

static void detach_free_block(void *block)
{
    int list_index;
    char *before;
    char *after;



    list_index = get_list_index(GET_SIZE(HDRP(block)));
    before = PREV_free(block);
    after = SUCC(block);

    if (before) SUCC(before) = after;
    else LIST_HEAD(list_index) = after;

    if (after) PREV_free(after) = before;
}

static void set_block(void *block, size_t need_size, size_t old_size)
{
    size_t remain = old_size - need_size;
    char *left;

    if (remain < MIN_FREE_BLOCK) {
        PUT(HDRP(block), PACK(old_size, 1));
        PUT(FTRP(block), PACK(old_size, 1));
        return;
    }

    PUT(HDRP(block), PACK(need_size, 1));
    PUT(FTRP(block), PACK(need_size, 1));

    left = NEXT_BLKP(block);
    
    PUT(HDRP(left), PACK(remain, 0));
    PUT(FTRP(left), PACK(remain, 0));
    add_free_block(left);
}






static void *coalesce(void *bp)
{
    size_t left_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t right_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t block_size = GET_SIZE(HDRP(bp));
    
    char *left = PREV_BLKP(bp);
    char *right = NEXT_BLKP(bp);

    if (left_alloc && right_alloc) {
        add_free_block(bp);
        return bp;
    }

    if (left_alloc && !right_alloc) {
        detach_free_block(right);
        
        block_size += GET_SIZE(HDRP(right));
        
        PUT(HDRP(bp), PACK(block_size, 0));
        PUT(FTRP(bp), PACK(block_size, 0));
    } 
    
    else if (!left_alloc && right_alloc) {
        detach_free_block(left);
        
        block_size += GET_SIZE(HDRP(left));
       
        PUT(FTRP(bp), PACK(block_size, 0));
        PUT(HDRP(left), PACK(block_size, 0));
        
        bp = left;
    } 
    
    else {
        detach_free_block(left); 
        detach_free_block(right);
        
        block_size += GET_SIZE(HDRP(left)) + GET_SIZE(HDRP(right));
        
        PUT(HDRP(left), PACK(block_size, 0));
        PUT(FTRP(right), PACK(block_size, 0));
        
        bp = left;
    }

   // 필수입니다. 하 이거
    add_free_block(bp);
    return bp;
}


static void *extend_heap(size_t bytes)
{
    char *block;
    size_t extend_size;

    extend_size = ALIGN(bytes);
    if (extend_size < MIN_FREE_BLOCK)
        extend_size = MIN_FREE_BLOCK;

    block = mem_sbrk((int)extend_size);
    if (block == (void *)-1)
        return NULL;

    PUT(HDRP(block), PACK(extend_size, 0));
    PUT(FTRP(block), PACK(extend_size, 0));
    PUT(HDRP(NEXT_BLKP(block)), PACK(0, 1));

    return coalesce(block);
}


static void *find_fit(size_t need_size)
{
    int list_index;
    char *block;
    
    char *candydate;
    
    size_t blocksize;
    size_t candidatesize;

    for (list_index = get_list_index(need_size);
         list_index < LISTS;
         list_index++) {
        
            candydate = NULL;
        candidatesize = (size_t)-1;

        for (block = LIST_HEAD(list_index);
             block != NULL;
             block = SUCC(block)) {
            blocksize = GET_SIZE(HDRP(block));

            if (blocksize < need_size) continue;

            if (blocksize == need_size) return block;

            if (blocksize < candidatesize) {
                candydate = block;
                candidatesize = blocksize;
            }
        }

        if (candydate)
            return candydate;
    }

    return NULL;
}

static void *place(void *block, size_t need_size)
{
    size_t block_size;
    size_t remain;
    
    char *picked_block;

    block_size = GET_SIZE(HDRP(block));
    remain = block_size - need_size;

    detach_free_block(block);

    if (need_size > 256 || remain < MIN_FREE_BLOCK) {
        set_block(block, need_size, block_size);
        return block;
    }

    PUT(HDRP(block), PACK(remain, 0));
    PUT(FTRP(block), PACK(remain, 0));
    add_free_block(block);

    picked_block = NEXT_BLKP(block);
    
    PUT(HDRP(picked_block), PACK(need_size, 1));
    PUT(FTRP(picked_block), PACK(need_size, 1));

    return picked_block;
}


int mm_init(void)
{
    int i;
    size_t list_bytes = ALIGN(LISTS * sizeof(char *));
    
    char *start_p = mem_sbrk((int)(list_bytes + 4 * WSIZE));

    if (start_p == (void *)-1) return -1;

    list_areas = start_p;

    for (i = 0; i < LISTS; i++) LIST_HEAD(i) = NULL;

    heap_pointer = start_p + list_bytes;
    PUT(heap_pointer, 0);
    PUT(heap_pointer + WSIZE, PACK(DSIZE, 1));
    PUT(heap_pointer + 2 * WSIZE, PACK(DSIZE, 1));
    PUT(heap_pointer + 3 * WSIZE, PACK(0, 1));
    heap_pointer += 2 * WSIZE;

    return 0;
}


void *mm_malloc(size_t size)
{
    size_t asize = make_block_size(size);
    size_t extend_size;
    char *bp;

    if (asize == 0) return NULL;

    bp = find_fit(asize);
    
    if (bp) return place(bp, asize);

    extend_size = MAX(asize, CHUNKSIZE);
    bp = extend_heap(extend_size);
    if (bp == NULL) return NULL;
    return place(bp, asize);
}


void mm_free(void *ptr)
{
    if (ptr == NULL) return;

    size_t size;

    if (ptr == NULL) return;

    size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}


void *mm_realloc(void *ptr, size_t size)
{
    size_t need_size;
    size_t new_size;
    size_t old_size;
    size_t copy_bytes;
    
    size_t extra;
    
    char *right;
    char *left;
    
    void *new_ptr;

    if (ptr == NULL) return mm_malloc(size);

    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    need_size = make_block_size(size);
    old_size = GET_SIZE(HDRP(ptr));

    copy_bytes = old_size - OVERHEAD_BYTES;
    
    if (size < copy_bytes)    copy_bytes = size;

    if (need_size <= old_size) {
    
        size_t remain = old_size - need_size;

        if (remain >= REALLOC_SPLIT_LIMIT) {
            PUT(HDRP(ptr), PACK(need_size, 1));
            PUT(FTRP(ptr), PACK(need_size, 1));

            right = NEXT_BLKP(ptr);
            PUT(HDRP(right), PACK(remain, 0));
            PUT(FTRP(right), PACK(remain, 0));
            coalesce(right);
        }
        return ptr;
    }

    if (need_size - old_size <= 64) extra = REALLOC_SMALL_BUFFER;
    else extra = REALLOC_BIG_BUFFER;

    new_size = make_block_size(size + extra);
    right = NEXT_BLKP(ptr);
    left = PREV_BLKP(ptr);

    if (!GET_ALLOC(HDRP(right))) {
        size_t total = old_size + GET_SIZE(HDRP(right));

        if (total >= new_size) {
            detach_free_block(right);
            set_block(ptr, new_size, total);
            return ptr;
        }
    }

    if (!GET_ALLOC(FTRP(left))) {
        size_t total = old_size + GET_SIZE(HDRP(left));

        if (total >= new_size) {
            detach_free_block(left);
            memmove(left, ptr, copy_bytes);
            set_block(left, new_size, total);
            return left;
        }
    }

    if (!GET_ALLOC(FTRP(left)) && !GET_ALLOC(HDRP(right))) {
        size_t total = old_size + GET_SIZE(HDRP(left)) + GET_SIZE(HDRP(right));

        if (total >= new_size) {
            detach_free_block(left);
            detach_free_block(right);
            
            memmove(left, ptr, copy_bytes);
            set_block(left, new_size, total);
            return left;
        }
    }

    if (GET_SIZE(HDRP(right)) == 0) {
        size_t need_more = new_size - old_size;

        if (extra == REALLOC_BIG_BUFFER) {
            if (mem_sbrk((int)need_more) == (void *)-1) return NULL;

            PUT(HDRP(ptr), PACK(old_size + need_more, 1));
            PUT(FTRP(ptr), PACK(old_size + need_more, 1));
            
            PUT(HDRP(NEXT_BLKP(ptr)), PACK(0, 1));
            
            return ptr;
        }

        if (!GET_ALLOC(FTRP(left))) {
            size_t left_size = GET_SIZE(HDRP(left));
            size_t grow_size = 0;

            if (need_more > left_size)  grow_size = ALIGN(need_more - left_size);

            if (grow_size > 0 && mem_sbrk((int)grow_size) == (void *)-1)return NULL;

            detach_free_block(left);
            
            memmove(left, ptr, copy_bytes);

            set_block(left, new_size, old_size + left_size + grow_size);
            
            PUT((char *)left + old_size + left_size + grow_size - WSIZE, PACK(0, 1));
            
            return left;
        }

        if (mem_sbrk((int)need_more) == (void *)-1) return NULL;

        PUT(HDRP(ptr), PACK(old_size + need_more, 1));
        PUT(FTRP(ptr), PACK(old_size + need_more, 1));
        PUT(HDRP(NEXT_BLKP(ptr)), PACK(0, 1));
        return ptr;
    }

    new_ptr = mm_malloc(size + extra);
    if (new_ptr == NULL) return NULL;

    memcpy(new_ptr, ptr, copy_bytes);
    mm_free(ptr);
    return new_ptr;
}

int mm_check(void)
{
    return heap_pointer != NULL;
}
// 끝