
#include "./lib/vec.h"
#include <assert.h>
#include <stdlib.h>

/* inner product. accumulate in temporary */
void inner4(vec_ptr u, vec_ptr v, data_t *dest) {
  long i;
  long length = vec_length(u);
  long limit = length - 6 + 1;
  data_t *udata = get_vec_start(u);
  data_t *vdata = get_vec_start(v);
  data_t sum0 = (data_t)0;
  data_t sum1 = (data_t)0;
  data_t sum2 = (data_t)0;
  data_t sum3 = (data_t)0;
  data_t sum4 = (data_t)0;
  data_t sum5 = (data_t)0;

  for (i = 0; i < limit; i += 6) {
    sum0 = sum0 + udata[i] * vdata[i];
    sum1 = sum1 + udata[i + 1] * vdata[i + 1];
    sum2 = sum2 + udata[i + 2] * vdata[i + 2];
    sum3 = sum3 + udata[i + 3] * vdata[i + 3];
    sum4 = sum4 + udata[i + 4] * vdata[i + 4];
    sum5 = sum5 + udata[i + 5] * vdata[i + 5];
  }
  for (; i < length; i++) {
    sum0 = sum0 + udata[i] * vdata[i];
  }
  *dest = sum0 + sum1 + sum2 + sum3 + sum4 + sum5;
}

int main(int argc, char *argv[]) {
  vec_ptr u = new_vec(4);
  vec_ptr v = new_vec(4);

  data_t *arr = (data_t *)malloc(sizeof(data_t) * 4);
  arr[0] = 0;
  arr[1] = 1;
  arr[2] = 2;
  arr[3] = 3;

  set_vec_start(u, arr);
  set_vec_start(v, arr);

  data_t res;
  inner4(u, v, &res);

  assert(res == 1 + 4 + 9);
  return 0;
}
