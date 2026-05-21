#include <stdlib.h>

typedef long data_t;

/* Create abstract data type for vector */
typedef struct {
  long len;
  data_t *data;
} vec_rec, *vec_ptr;

long vec_length(vec_ptr v);
data_t *get_vec_start(vec_ptr v);
vec_ptr new_vec(long len);
void set_vec_start(vec_ptr v, data_t *data);

long vec_length(vec_ptr v) { return v->len; }
data_t *get_vec_start(vec_ptr v) { return v->data; }
vec_ptr new_vec(long len) {
  vec_ptr v = malloc(sizeof(vec_rec));
  if (!v)
    return NULL;
  v->data = malloc(len * sizeof(data_t));
  if (!v->data)
    return NULL;
  v->len = len;
  return v;
}
void set_vec_start(vec_ptr v, data_t *data) { v->data = data; }
