/* Recommended max cache and object sizes */
#include "cache.h"

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

void init_cache(cache_t *cache) {
  cache->n_entries = 0;
  cache->entries = NULL;
  cache->total_size = 0;
  cache->readcnt = 0;
  sem_init(&cache->mutex, 0, 1);
  sem_init(&cache->w, 0, 1);
}

size_t read_cache_entry(cache_t *cache, char *key, char **data) {
  *data = NULL;
  size_t size = 0;

  int increase_touch = 0;
  P(&cache->mutex);
  cache->readcnt++;
  if (cache->readcnt == 1) { // first in
    P(&cache->w);
    increase_touch = 1;
  }
  V(&cache->mutex);

  printf("Reading cache entry: %s\n", key);

  // read cache
  for (size_t i = 0; i < cache->n_entries; i++) {
    if (cache->entries[i] == NULL)
      continue;
    if (strcmp(key, cache->entries[i]->key) != 0)
      continue;

    if (increase_touch)
      cache->entries[i]->touched++;

    size = cache->entries[i]->size;

    *data = malloc(sizeof(char) * size);
    memcpy(*data, cache->entries[i]->data, size);
    break;
  }

  P(&cache->mutex);
  cache->readcnt--;
  if (cache->readcnt == 0) // last out
    V(&cache->w);
  V(&cache->mutex);

  printf("Read cache entry result: %s, size: %zu\n", key, size);
  return size;
}

void write_cache_entry(cache_t *cache, char *key, char *data, size_t size) {
  if (size > MAX_OBJECT_SIZE) {
    printf("Entry is too large for cache, skip");
    return;
  }

  P(&cache->w);

  // write cache

  while (cache->total_size + size > MAX_CACHE_SIZE)
    evict_cache_entry_(cache);

  cache_entry_t *entry = malloc(sizeof(cache_entry_t));
  entry->size = size;
  entry->touched = 1;
  entry->key = malloc(sizeof(char) * strlen(key) + 1);
  entry->data = malloc(sizeof(char) * size);
  memcpy(entry->key, key, strlen(key) + 1);
  memcpy(entry->data, data, size);

  int found_place = 0;
  for (size_t i = 0; i < cache->n_entries; i++) {
    if (cache->entries[i] != NULL)
      continue;

    found_place = 1;
    cache->entries[i] = entry;
    break;
  }

  if (!found_place) {
    cache->entries =
        realloc(cache->entries, sizeof(cache_entry_t *) * (++cache->n_entries));
    cache->entries[cache->n_entries - 1] = entry;
  }

  cache->total_size += size;

  printf("Written cache entry: %s\n", key);

  V(&cache->w);
}

void evict_cache_entry_(cache_t *cache) {
  size_t evict_index = 0;
  unsigned long long min_touch = 0;

  for (size_t i = 0; i < cache->n_entries; i++) {
    if (cache->entries[i] == NULL)
      continue;

    if (!min_touch || cache->entries[i]->touched < min_touch) {
      evict_index = i;
      min_touch = cache->entries[i]->touched;
    }
  }

  if (min_touch == 0)
    // nothing to evict
    return;

  printf("Evicting cache entry %s\n", cache->entries[evict_index]->key);
  cache_entry_t *evict = cache->entries[evict_index];
  cache->entries[evict_index] = NULL;
  if (evict_index == cache->n_entries - 1)
    cache->n_entries--;
  cache->total_size -= evict->size;
  free_cache_entry(evict);
}

void free_cache_entry(cache_entry_t *entry) {
  free(entry->key);
  free(entry->data);
  free(entry);
}

void free_cache(cache_t *cache) {
  for (size_t i = 0; i < cache->n_entries; i++) {
    if (cache->entries[i] == NULL)
      continue;

    free_cache_entry(cache->entries[i]);
  }
}
