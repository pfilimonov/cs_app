#include "cachelab.h"
#include "error.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <limits.h>

#define PTR_SIZE sizeof(void *)

unsigned COUNTER = 0;

typedef struct {
  char valid;
  int B;
  int tag;
  char *block;
  unsigned last_touch;
} line_t;

typedef struct {
  line_t *lines;
  int E;
} set_t;

typedef struct {
  set_t *sets;
  int s;
  int t;
  int b;
  int S;
  int E;
  size_t line_size;
  size_t set_size;
  size_t cache_size;
} cache_t;

cache_t *initCache(int s, int E, int b);
void freeCache(cache_t *cache);

cache_t *initCache(int s, int E, int b) {
  int S = 1 << s;
  int B = 1 << b;
  int t = (PTR_SIZE - s - b);

  cache_t *cache = malloc(sizeof(cache_t));
  if (!cache)
    return NULL;

  cache->sets = malloc(S * sizeof(set_t));
  if (!cache->sets)
    return NULL;

  for (int i = 0; i < S; i++) {
    cache->sets[i].E = E;
    cache->sets[i].lines = malloc(E * sizeof(line_t));
    if (!cache->sets[i].lines)
      return NULL;

    for (int j = 0; j < E; j++) {
      cache->sets[i].lines[j].valid = 0;
      cache->sets[i].lines[j].tag = 0;
      cache->sets[i].lines[j].B = B;
      cache->sets[i].lines[j].block = calloc(B, sizeof(char));
      cache->sets[i].lines[j].last_touch = 0;
      if (!cache->sets[i].lines[j].block)
        return NULL;
    }
  }

  cache->S = S;
  cache->t = t;
  cache->b = b;
  cache->s = s;
  cache->E = E;

  return cache;
}

void freeCache(cache_t *cache) {
  if (!cache)
    return;

  for (int i = 0; i < cache->S; i++) {
    for (int j = 0; j < cache->E; j++) {
      free(cache->sets[i].lines[j].block);
    }
    free(cache->sets[i].lines);
  }

  free(cache->sets);

  free(cache);
}

typedef enum { Hit, Miss } CacheRes;

void parseAddr(int b, int s, unsigned long addr, int *tag, int *set_idx,
               int *block_offset);

CacheRes touchCache(cache_t *cache, unsigned long addr, int size, int *evicted);

void parseAddr(int b, int s, unsigned long addr, int *tag, int *set_idx,
               int *block_offset) {
  *tag = addr >> (b + s);
  *set_idx = (addr >> b) & ~(-1 << s);
  *block_offset = addr & ~(-1 << b);
}

CacheRes touchCache(cache_t *cache, unsigned long addr, int size,
                    int *evicted) {
  CacheRes res = Miss;

  int tag = 0;
  int set_idx = 0;
  int block_offset = 0;
  parseAddr(cache->b, cache->s, addr, &tag, &set_idx, &block_offset);

  line_t *free_line = NULL;
  line_t *lru_line = NULL;
  unsigned min_touch_time = UINT_MAX;

  for (int j = 0; j < cache->E; j++) {
    line_t *line = &(cache->sets[set_idx].lines[j]);
    if (line->last_touch < min_touch_time) {
      min_touch_time = line->last_touch;
      lru_line = line;
    }
    if (line->tag == tag && line->valid) {
      res = Hit;
      line->last_touch = ++COUNTER;
      break;
    } else if (!line->valid && !free_line) {
      free_line = line;
    }
  }

  if (res == Miss) {
    if (!free_line)
      free_line = lru_line; //&(cache->sets[set_idx].lines[rand() % cache->E]);
                            //// random

    *evicted = free_line->valid != 0;
    free_line->tag = tag;
    free_line->valid = 1;
    free_line->last_touch = ++COUNTER;
  }

  return res;
}

char *parseArgs(int argc, char *argv[], int *verbose, int *s, int *E, int *b);
void parseTrace(char *file, cache_t *cache, int *misses, int *hits,
                int *evictions);

char *HELP = "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
             "Options:"
             " -h         Print this help message.\n"
             " -v         Optional verbose flag.\n"
             " -s <num>   Number of set index bits.\n"
             " -E <num>   Number of lines per set.\n"
             " -b <num>   Number of block offset bits.\n"
             " -t <file>  Trace file.\n"
             "\n"
             "Examples:\n"
             "linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
             "linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace";

char *parseArgs(int argc, char *argv[], int *verbose, int *s, int *E, int *b) {
  if (argc <= 1) {
    error(1, 0, "Missing required command line argument\n%s", HELP);
  }

  int scan_s = 0, scan_b = 0, scan_E = 0, scan_t = 0;

  char *file = NULL;

  for (int i = 1; i < argc; i++) {

    // help
    if (strcmp(argv[i], "-h") == 0) {
      printf("%s\n", HELP);
      return NULL;
    }

    if (strcmp(argv[i], "-v") == 0) {
      *verbose = 1;
      continue;
    }

    // value flags
    if (strcmp(argv[i], "-t") == 0) {
      scan_t = 1;
      continue;
    } else if (strcmp(argv[i], "-s") == 0) {
      scan_s = 1;
      continue;

    } else if (strcmp(argv[i], "-E") == 0) {
      scan_E = 1;
      continue;

    } else if (strcmp(argv[i], "-b") == 0) {
      scan_b = 1;
      continue;
    }

    // get values
    if (scan_t == 1) {
      if (file != NULL) {
        error(1, 1, "Two files can't be specified\n%s", HELP);
      }
      file = malloc((strlen(argv[i]) + 1) * sizeof(char));
      strcpy(file, argv[i]);
      scan_t = 0;
      continue;
    } else if (scan_b == 1) {
      *b = strtol(argv[i], NULL, 10);
      scan_b = 0;
      continue;
    } else if (scan_s == 1) {
      *s = strtol(argv[i], NULL, 10);
      scan_s = 0;
      continue;
    } else if (scan_E == 1) {
      *E = strtol(argv[i], NULL, 10);
      scan_E = 0;
      continue;
    }
  }

  if (!file) {
    error(1, 0, "Missing required command line argument\n%s", HELP);
  }

  return file;
}

void parseTrace(char *file, cache_t *cache, int *misses, int *hits,
                int *evictions) {
  FILE *f = fopen(file, "r");
  if (!f)
    error(1, 2, "Can't open the file %s\n", file);

  char buf[100];
  while (fgets(buf, sizeof(buf), f)) {
    if (strlen(buf) < 1)
      continue;
    if (buf[0] == 'I' || buf[0] == '\n')
      continue;

    char op;
    unsigned long addr;
    int size;
    printf("Trace line: %s\n", buf);
    sscanf(buf, " %c %lx,%d\n", &op, &addr, &size);

    printf(" %c %lx,%d\n", op, addr, size);

    if (op != 'L' && op != 'M' && op != 'S')
      continue;
    if (size == 0)
      error(1, 3, "Invalid size 0\n");

    int evicted = 0;
    CacheRes res;
    if (op == 'S') {
      res = touchCache(cache, addr, size, &evicted);
    } else if (op == 'L') {
      res = touchCache(cache, addr, size, &evicted);
    } else // (op == 'M')
    {
      res = touchCache(cache, addr, size, &evicted);
      (*hits)++;
    }

    *evictions += evicted;

    switch (res) {
    case Hit:
      (*hits)++;
      break;
    case Miss:
      (*misses)++;
      break;
    }
  }
}

int main(int argc, char *argv[]) {

  int verbose = 0;
  int s = 0, E = 0, b = 0;
  char *file = parseArgs(argc, argv, &verbose, &s, &E, &b);

  if (!file) {
    // help case
    return 0;
  }

  cache_t *cache = initCache(s, E, b);

  if (!cache)
    error(1, 3, "Failed to allocate memory");

  int misses = 0, hits = 0, evictions = 0;
  parseTrace(file, cache, &misses, &hits, &evictions);

  printSummary(hits, misses, evictions);

  free(file);
  freeCache(cache);
  return 0;
}
