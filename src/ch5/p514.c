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
  data_t sum = (data_t)0;

  for (i = 0; i < limit; i += 6) {
    sum = sum + udata[i] * vdata[i] + udata[i + 1] * vdata[i + 1] +
          udata[i + 2] * vdata[i + 2] + udata[i + 3] * vdata[i + 3] +
          udata[i + 4] * vdata[i + 4] + udata[i + 5] * vdata[i + 5];
  }
  for (; i < length; i++) {
    sum = sum + udata[i] * vdata[i];
  }
  *dest = sum;
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
