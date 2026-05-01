#undef NDEBUG
#include <assert.h>
#include <stdio.h>

void bubble_a(long *data, long count);
void bubble_b(long *data, long count);

int test_case_1() {
  long arr_a[] = {3, 1, 4, 2, 5};
  long arr_b[] = {3, 1, 4, 2, 5};

  bubble_a(arr_a, 5);
  bubble_b(arr_b, 5);

  for (int i = 0; i < 5; i++) {
    if (arr_a[i] != arr_b[i])
      return 0;
  }

  return 1;
}

int test_case_2() {
  long arr_a[] = {1, 2, 3, 4, 5};
  long arr_b[] = {1, 2, 3, 4, 5};

  bubble_a(arr_a, 5);
  bubble_b(arr_b, 5);

  for (int i = 0; i < 5; i++) {
    if (arr_a[i] != arr_b[i])
      return 0;
  }

  return 1;
}

int test_case_3() {
  long arr_a[] = {5, 4, 3, 2, 1};
  long arr_b[] = {5, 4, 3, 2, 1};

  bubble_a(arr_a, 5);
  bubble_b(arr_b, 5);

  for (int i = 0; i < 5; i++) {
    if (arr_a[i] != arr_b[i])
      return 0;
  }

  return 1;
}

int test_case_4() {
  long arr_a[] = {5, 5, 3, 3, 1};
  long arr_b[] = {5, 5, 3, 3, 1};

  bubble_a(arr_a, 5);
  bubble_b(arr_b, 5);

  for (int i = 0; i < 5; i++) {
    if (arr_a[i] != arr_b[i])
      return 0;
  }

  return 1;
}

int main(int argc, char *argv[]) {

  assert(test_case_1());
  assert(test_case_2());
  assert(test_case_3());
  assert(test_case_4());

  return 0;
}

void bubble_a(long *data, long count) {
  long i, last;
  for (last = count - 1; last > 0; last--) {
    for (i = 0; i < last; i++) {
      if (data[i + 1] < data[i]) {
        long t = data[i + 1];
        data[i + 1] = data[i];
        data[i] = t;
      }
    }
  }
}

void bubble_b(long *data, long count) {
  long i, last;
  for (last = count - 1; last > 0; last--) {
    for (i = 0; i < last; i++) {
      if (*(data + i + 1) < *(data + i)) {
        long t = *(data + i + 1);
        *(data + i + 1) = *(data + i);
        *(data + i) = t;
      }
    }
  }
}
