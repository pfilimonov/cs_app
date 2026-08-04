
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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE 4             /* Word and header/footer size (bytes) */
#define DSIZE 8             /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define PAYLOAD_SIZE(bp) (GET_SIZE(HDRP(bp)) - 2 * WSIZE)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define PUT_PRED(bp, pred_p)                                                   \
  (PUT(bp, (unsigned int)((char *)pred_p - (char *)mem_heap_lo())))
#define PUT_SUCC(bp, succ_p)                                                   \
  (PUT(bp + WSIZE, (unsigned int)((char *)succ_p - (char *)mem_heap_lo())))
#define CLEAR_PRED(bp) (PUT(bp, 0))
#define CLEAR_SUCC(bp) (PUT(bp + WSIZE, 0))
#define GET_PRED(bp) ((char *)mem_heap_lo() + GET(bp))
#define GET_SUCC(bp) ((char *)mem_heap_lo() + GET((char *)bp + WSIZE))
#define IS_FREE_LIST_HEAD(bp) (GET(bp) == 0)
#define IS_FREE_LIST_TAIL(bp) (GET((char *)bp + WSIZE) == 0)

// segregated lists
int msb_index(unsigned x) {
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  // теперь x = 0...0 1...1, где число единиц = позиция msb + 1

  // popcount через ту же технику parallel bits
  unsigned c = x - ((x >> 1) & 0x55555555);
  c = (c & 0x33333333) + ((c >> 2) & 0x33333333);
  c = (c + (c >> 4)) & 0x0f0f0f0f;
  c = c + (c >> 8);
  c = c + (c >> 16);
  return (int)(c & 0x3f) - 1;
}
#define N_LISTS 16
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define GET_CLASS_BY_SIZE(size) (MIN(msb_index(size), N_LISTS - 1))
#define IS_FREE_LIST_INITIALIZED(n)                                            \
  (GET((char *)mem_heap_lo() + n * WSIZE) != 0)
#define GET_FREE_LIST_HEAD(n)                                                  \
  ((char *)mem_heap_lo() + GET(((char *)mem_heap_lo() + n * WSIZE)))
#define CLEAR_FREE_LIST_HEAD(n) PUT(((char *)mem_heap_lo() + n * WSIZE), 0)
#define PUT_FREE_LIST_HEAD(bp, n)                                              \
  (PUT(((char *)mem_heap_lo() + n * WSIZE),                                    \
       (unsigned int)((char *)bp - (char *)mem_heap_lo())))

/* Global variables */
static char *heap_listp;

static void dump_free_list(char *msg, char *free_listp);

int check_heap(void);
int check_free_list(char *free_listp);
int check_overlap(void);

int check_free_list(char *free_listp) {
  if (free_listp == NULL) {
    // printf("[ERROR] Free list pointer is null\n");
    return 1;
  }

  // • Do the pointers in a heap block point to valid heap addresses?
  for (char *bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    if (GET_ALLOC(HDRP(bp)))
      continue;

    if (!IS_FREE_LIST_HEAD(bp)) {
      char *pred = GET_PRED(bp);
      if (pred < (char *)mem_heap_lo() || pred > (char *)mem_heap_hi()) {
        printf("[ERROR] Invalid predecessor pointer %p\n", pred);
        return 0;
      }
    }
    if (!IS_FREE_LIST_TAIL(bp)) {
      char *succ = GET_SUCC(bp);
      if (succ < (char *)mem_heap_lo() || succ > (char *)mem_heap_hi()) {
        printf("[ERROR] Invalid successor pointer %p\n", succ);
        return 0;
      }
    }
  }

  // • Is every block in the free list marked as free?
  for (char *bp = free_listp;; bp = GET_SUCC(bp)) {
    if (GET_ALLOC(HDRP(bp))) {
      printf("[ERROR] Allocated block found in free list: %p, %x\n", bp,
             *HDRP(bp));
      return 0;
    }
    // • Do the pointers in the free list point to valid free blocks?
    if (!IS_FREE_LIST_HEAD(bp)) {
      if (GET_ALLOC(HDRP(GET_PRED(bp)))) {
        printf("[ERROR] Allocated block found in predecessor: %p.\n", bp);
        return 0;
      }
    }
    if (IS_FREE_LIST_TAIL(bp))
      break;
  }

  // • Is every free block actually in the free list?
  for (char *bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    if (GET_ALLOC(HDRP(bp)))
      continue;

    int found_in_free_list = 0;
    for (int off = 1; off <= N_LISTS; off++) {
      char *flistp = GET_FREE_LIST_HEAD(off);
      for (char *fbp = flistp;; fbp = GET_SUCC(fbp)) {
        if (fbp == bp) {
          found_in_free_list = 1;
          break;
        }
        if (IS_FREE_LIST_TAIL(fbp))
          break;
      }
      if (found_in_free_list)
        break;
    }

    if (!found_in_free_list) {
      printf("[ERROR] Free block not found in free list: %p\n", bp);
      return 0;
    }
  }

  // • Are there any contiguous free blocks that somehow escaped coalescing?
  int prev_block_was_free = 0;
  for (char *bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    if (!prev_block_was_free && !GET_ALLOC(HDRP(bp)))
      prev_block_was_free = 1;
    else if (prev_block_was_free && !GET_ALLOC(HDRP(bp))) {
      printf("[ERROR] Two free blocks without coalescing\n");
      return 0;
    } else if (prev_block_was_free)
      prev_block_was_free = 0;
  }

  return 1;
}

int check_overlap(void) {
  // • Do any allocated blocks overlap?
  for (char *bp1 = heap_listp; GET_SIZE(HDRP(bp1)) > 0; bp1 = NEXT_BLKP(bp1)) {
    char *bp2 = NEXT_BLKP(bp1);
    if ((GET_ALLOC(HDRP(bp1)) == 1 && GET_ALLOC(HDRP(bp2)) == 1) &&
        (bp1 + PAYLOAD_SIZE(bp1) > HDRP(bp2))) {
      printf("[ERROR] Found overlapping payloads in allocated blocks: %p and "
             "%p\n",
             bp1, bp2);
      return 0;
    }
  }
  return 1;
}

int check_heap(void) {

  for (int off = 1; off <= N_LISTS; off++) {
    if (!check_free_list(GET_FREE_LIST_HEAD(off))) {
      printf("[CRITICAL] FREE LIST CHECK FAILED\n");
      dump_free_list("", GET_FREE_LIST_HEAD(off));
      exit(1);
    }
  }

  if (!check_overlap()) {
    printf("[CRITICAL] HEAP OVERLAP CHECK FAILED\n");
    exit(1);
  }

  // printf("[INFO] Heap check passed!\n");
  return 1;
}

static void delete_from_free_list(char *bp) {
  // printf("Deleting %p from free list\n", bp);
  // dump_free_list("DELETE");

  if (GET_ALLOC(HDRP(bp))) {
    printf("[ERROR] Attempting to delete non-free block %p\n", bp);
    exit(1);
  }

  int size = GET_SIZE(HDRP(bp));
  int class = GET_CLASS_BY_SIZE(size);

  if (!IS_FREE_LIST_INITIALIZED(class)) {
    printf("[ERROR] Deleting from empty free list %d. Size=%d\n", class, size);
    exit(1);
  }

  // update free list
  if (IS_FREE_LIST_HEAD(bp)) {
    CLEAR_PRED(GET_SUCC(bp));
    if (IS_FREE_LIST_TAIL(bp))
      CLEAR_FREE_LIST_HEAD(class);
    else
      PUT_FREE_LIST_HEAD(GET_SUCC(bp), class);
  } else if (IS_FREE_LIST_TAIL(bp)) {
    CLEAR_SUCC(GET_PRED(bp));
  } else {
    // middle list element
    PUT_PRED(GET_SUCC(bp), GET_PRED(bp));
    PUT_SUCC(GET_PRED(bp), GET_SUCC(bp));
  }
}

static void insert_into_free_list(char *bp) {
  // dump_free_list("INSERT");

  int size = GET_SIZE(HDRP(bp));
  int class = GET_CLASS_BY_SIZE(size);

  // printf("Inserting %p into free list. Size=%d, class=%d\n", bp, size,
  // class);

  if (!IS_FREE_LIST_INITIALIZED(class)) {
    // this is the first free block
    // printf("Free list not initialized. Class=%d\n", class);
    CLEAR_PRED(bp);
    CLEAR_SUCC(bp);
    PUT_FREE_LIST_HEAD(bp, class);
    return;
  }

  char *free_listp = GET_FREE_LIST_HEAD(class);

  void *block_with_lesser_addr = NULL;
  for (void *bp2 = free_listp;; bp2 = GET_SUCC(bp2)) {
    if ((unsigned long)bp2 > (unsigned long)bp) {
      if (IS_FREE_LIST_HEAD(bp2))
        break;

      block_with_lesser_addr = GET_PRED(bp2);
      break;
    }
    if (IS_FREE_LIST_TAIL(bp2)) {
      block_with_lesser_addr = bp2;
      break;
    }
  }

  if (block_with_lesser_addr == NULL) {
    // new block goes to head
    PUT_PRED(free_listp, bp);
    CLEAR_PRED(bp);
    PUT_SUCC(bp, free_listp);
    PUT_FREE_LIST_HEAD(bp, class);
  } else if (IS_FREE_LIST_TAIL(block_with_lesser_addr)) {
    // new block goes to tail
    PUT_SUCC((char *)block_with_lesser_addr, bp);
    PUT_PRED(bp, block_with_lesser_addr);
    CLEAR_SUCC(bp);
  } else {
    // new block goes to the middle
    char *succ = GET_SUCC(block_with_lesser_addr);
    PUT_PRED(succ, bp);
    PUT_SUCC((char *)block_with_lesser_addr, bp);
    PUT_SUCC(bp, succ);
    PUT_PRED(bp, block_with_lesser_addr);
  }
}

static void dump_free_list(char *msg, char *free_listp) {
  printf("\n==========\n[FREE LIST DUMP %s]\n", msg);
  if (free_listp == NULL) {
    printf("NULL\n==========\n");
    return;
  }

  for (void *bp = free_listp;; bp = GET_SUCC(bp)) {
    printf("BLOCK %p:    size=%u | alloc=%d | pred=%p | succ=%p \n", bp,
           GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)),
           IS_FREE_LIST_HEAD(bp) ? NULL : GET_PRED(bp),
           IS_FREE_LIST_TAIL(bp) ? NULL : GET_SUCC(bp));
    if (IS_FREE_LIST_TAIL(bp)) {
      printf("\n==========\n");
      return;
    }
  }
}

static void *coalesce(void *bp) {
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if (prev_alloc && next_alloc) { /* Case 1 */
    return bp;
  }

  else if (prev_alloc && !next_alloc) { /* Case 2 */
    delete_from_free_list(bp);
    delete_from_free_list(NEXT_BLKP(bp));
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    insert_into_free_list(bp);
  }

  else if (!prev_alloc && next_alloc) { /* Case 3 */
    delete_from_free_list(bp);
    delete_from_free_list(PREV_BLKP(bp));
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
    insert_into_free_list(bp);
  }

  else { /* Case 4 */
    delete_from_free_list(bp);
    delete_from_free_list(NEXT_BLKP(bp));
    delete_from_free_list(PREV_BLKP(bp));
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
    insert_into_free_list(bp);
  }
  return bp;
}

static void *extend_heap(size_t words) {
  char *bp;
  size_t size;

  /* Allocate an even number of words to maintain alignment */
  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
  if (size < WSIZE * 4) // hdr + ftr + succ + pred
    size = WSIZE * 4;

  if ((long)(bp = mem_sbrk(size)) == -1)
    return NULL;

  /* Initialize free block header/footer and the epilogue header */
  PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
  PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

  insert_into_free_list(bp);

  /* Coalesce if the previous block was free */
  return coalesce(bp);
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
  /* Create the initial empty heap */
  if ((heap_listp = mem_sbrk(4 * WSIZE + WSIZE * N_LISTS)) == (void *)-1)
    return -1;
  PUT(heap_listp, 0); /* Alignment padding */
  // free lists
  for (int off = 1; off <= N_LISTS; off++) {
    PUT(heap_listp + (off * WSIZE), 0);
  }
  PUT(heap_listp + ((N_LISTS + 1) * WSIZE),
      PACK(DSIZE, 1)); /* Prologue header */
  PUT(heap_listp + ((N_LISTS + 2) * WSIZE),
      PACK(DSIZE, 1));                                   /* Prologue footer */
  PUT(heap_listp + ((N_LISTS + 3) * WSIZE), PACK(0, 1)); /* Epilogue header */
  heap_listp += (2 * WSIZE) + WSIZE * N_LISTS;

  /* Extend the empty heap with a free block of CHUMSIZE bytes */
  if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
    return -1;

  // check_heap();

  return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void mm_free(void *bp) {
  // printf("Free call %p\n", bp);
  size_t size = GET_SIZE(HDRP(bp));

  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));

  insert_into_free_list(bp);
  coalesce(bp);

  // check_heap();

  // dump_free_list("AFTER FREE");
}

static void *find_fit(size_t asize) {
  for (int class = GET_CLASS_BY_SIZE(asize); class <= N_LISTS; class++) {
    if (!IS_FREE_LIST_INITIALIZED(class))
      continue;

    char *free_listp = GET_FREE_LIST_HEAD(class);

    size_t cur_best_size = 0;
    char *cur_fit = NULL;
    for (void *bp = free_listp;; bp = GET_SUCC(bp)) {
      size_t sz = GET_SIZE(HDRP(bp));
      if (sz >= asize && (!cur_best_size || sz < cur_best_size)) {
        cur_best_size = sz;
        cur_fit = bp;
      }

      if (IS_FREE_LIST_TAIL(bp))
        break;
    }

    if (cur_fit)
      return cur_fit;
  }

  return NULL;
}

static void place(void *bp, size_t asize) {
  delete_from_free_list(bp);
  size_t csize = GET_SIZE(HDRP(bp));
  if (csize - asize >= DSIZE * 2) {
    size_t asize1 = asize;
    size_t asize2 = csize - asize;
    PUT(HDRP(bp), PACK(asize1, 1));
    PUT(FTRP(bp), PACK(asize1, 1));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(asize2, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(asize2, 0));
    insert_into_free_list(NEXT_BLKP(bp));
  } else {
    PUT(HDRP(bp), PACK(csize, 1));
    PUT(FTRP(bp), PACK(csize, 1));
  }
}

void *mm_malloc(size_t size) {
  // printf("malloc call %zu\n", size);
  size_t asize;      /* Adjusted block size */
  size_t extendsize; /* Amount to extend heap if no fit */
  char *bp;

  /* Ignore spurious requests */
  if (size == 0)
    return NULL;

  /* Adjust block size to include overhead and alignment reqs. */
  if (size <= DSIZE)
    asize = 2 * DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

  /* Search the free list for a fit */
  if ((bp = find_fit(asize)) != NULL) {
    place(bp, asize);
    // check_heap();
    // dump_free_list("AFTER_MALLOC_FIT");
    // printf("Allocated %p\n", bp);
    return bp;
  }

  /* No fit found. Get more memory and place the block */
  extendsize = MAX(asize, CHUNKSIZE);
  if ((bp = extend_heap(extendsize / WSIZE)) == NULL) {
    printf("Failed to extend heap\n");
    return NULL;
  }
  place(bp, asize);

  // check_heap();
  // dump_free_list("AFTER_MALLOC_NO_FIT");

  // printf("Allocated %p\n", bp);
  return bp;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc_(void *ptr, size_t size) {
  void *oldptr = ptr;
  void *newptr;
  size_t copySize;
  newptr = mm_malloc(size);
  if (newptr == NULL)
    return NULL;
  copySize = GET_SIZE(HDRP(oldptr)) - DSIZE;
  if (size < copySize)
    copySize = size;
  memcpy(newptr, oldptr, copySize);
  mm_free(oldptr);
  return newptr;
}

void *mm_realloc(void *ptr, size_t size) {
  void *oldptr = ptr;
  size_t old_size = GET_SIZE(HDRP(oldptr));
  size_t old_payload_size = PAYLOAD_SIZE(oldptr);

  if (ptr == NULL)
    return mm_malloc(size);

  if (size == 0) {
    mm_free(ptr);
    return NULL;
  }

  if (size <= old_payload_size) {
    size_t asize;
    if (size <= DSIZE)
      asize = 2 * DSIZE;
    else
      asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    size_t csize = old_size;
    if (csize - asize >= DSIZE * 2) {
      size_t asize1 = asize;
      size_t asize2 = csize - asize;
      PUT(HDRP(oldptr), PACK(asize1, 1));
      PUT(FTRP(oldptr), PACK(asize1, 1));
      PUT(HDRP(NEXT_BLKP(oldptr)), PACK(asize2, 0));
      PUT(FTRP(NEXT_BLKP(oldptr)), PACK(asize2, 0));
      insert_into_free_list(NEXT_BLKP(oldptr));
    }

    return oldptr;
  }

  // new size is bigger than the old one
  char *next_blk = NEXT_BLKP(oldptr);
  size_t next_size = GET_SIZE(HDRP(next_blk));

  // next free block is allocated or too small - do full realloc
  if (GET_ALLOC(HDRP(next_blk)) || size - old_size + 2 * WSIZE > next_size) {
    void *newptr;
    size_t copySize;
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = GET_SIZE(HDRP(oldptr)) - DSIZE;
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
  }

  // "allocate" next free block

  delete_from_free_list(next_blk);

  size_t asize;
  if (size <= DSIZE)
    asize = 2 * DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

  size_t csize = old_size + next_size;
  if (csize - asize >= DSIZE * 2) {
    size_t asize1 = asize;
    size_t asize2 = csize - asize;
    PUT(HDRP(oldptr), PACK(asize1, 1));
    PUT(FTRP(oldptr), PACK(asize1, 1));
    PUT(HDRP(NEXT_BLKP(oldptr)), PACK(asize2, 0));
    PUT(FTRP(NEXT_BLKP(oldptr)), PACK(asize2, 0));
    insert_into_free_list(NEXT_BLKP(oldptr));
  } else {
    PUT(HDRP(oldptr), PACK(csize, 1));
    PUT(FTRP(oldptr), PACK(csize, 1));
  }

  // check_heap();

  return oldptr;
}
