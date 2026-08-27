#ifndef CACHE_H
#define CACHE_H
#include "csapp.h"

typedef struct cache_entry {
  char *key;
  char *data;
  size_t size;
  unsigned long long touched;
} cache_entry_t;

void free_cache_entry(cache_entry_t *entry);

typedef struct cache {
  size_t n_entries;
  cache_entry_t **entries;
  size_t total_size;

  // sharing
  int readcnt;
  sem_t mutex, w;
} cache_t;

void init_cache(cache_t *cache);
size_t read_cache_entry(cache_t *cache, char *key,
                        char **data); // NULL if not found
void write_cache_entry(cache_t *cache, char *key, char *data, size_t size);
void evict_cache_entry_(cache_t *cache);
void free_cache(cache_t *cache);
#endif
