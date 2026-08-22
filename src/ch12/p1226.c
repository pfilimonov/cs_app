#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <string.h>

#include "wrap.h"

static sem_t mutex;

struct hostent *gethostbyname_ts(const char *name);

struct hostent *gethostbyname_ts(const char *name) {
  struct hostent *result;

  P(&mutex);

  struct hostent *private_p = gethostbyname(name);
  result = malloc(sizeof(*private_p));

  // h_name
  result->h_name = malloc(strlen(private_p->h_name) + 1);
  strcpy(result->h_name, private_p->h_name);

  // aliases
  int len = 0;
  while (private_p->h_aliases[len] != NULL)
    len++;

  result->h_aliases = malloc((len + 1) * sizeof(char *));
  for (int i = 0; i <= len; i++) {
    if (private_p->h_aliases[i] == NULL) {
      result->h_aliases[i] = NULL;
      continue;
    }
    result->h_aliases[i] = malloc(strlen(private_p->h_aliases[i]) + 1);
    strcpy(result->h_aliases[i], private_p->h_aliases[i]);
  }

  // addtype, length
  result->h_addrtype = private_p->h_addrtype;
  result->h_length = private_p->h_length;

  // addr list
  len = 0;
  while (private_p->h_addr_list[len] != NULL)
    len++;

  result->h_addr_list = malloc((len + 1) * sizeof(char *));
  for (int i = 0; i <= len; i++) {
    if (private_p->h_addr_list[i] == NULL) {
      result->h_addr_list[i] = NULL;
      continue;
    }
    result->h_addr_list[i] = malloc(private_p->h_length);
    memcpy(result->h_addr_list[i], private_p->h_addr_list[i],
           private_p->h_length);
  }

  V(&mutex);

  return result;
}

int main(int argc, char *argv[]) {
  sem_init(&mutex, 0, 1);

  if (argc != 2) {
    printf("Usage %s <hostname>\n", argv[0]);
    exit(0);
  }

  struct hostent *expected = gethostbyname(argv[1]);
  struct hostent *actual = gethostbyname_ts(argv[1]);

  assert(expected->h_length == actual->h_length);
  assert(expected->h_addrtype == actual->h_addrtype);

  // printf("%s %s\n", expected->h_name, actual->h_name);
  assert(strcmp(expected->h_name, actual->h_name) == 0);

  char *alias = expected->h_aliases[0];
  if (alias != NULL) {
    assert(strcmp(alias, actual->h_aliases[0]) == 0);
    int i = 0;
    while (alias != NULL) {
      assert(strcmp(actual->h_aliases[i], alias) == 0);
      alias = expected->h_aliases[++i];
    }
  } else {
    assert(actual->h_aliases[0] == NULL);
  }

  char *addr = expected->h_addr_list[0];
  if (addr != NULL) {
    assert(memcmp(addr, actual->h_addr_list[0], actual->h_length) == 0);
    int i = 0;
    while (addr != NULL) {
      assert(memcmp(actual->h_addr_list[i], addr, actual->h_length) == 0);
      addr = expected->h_addr_list[++i];
    }
  } else {
    assert(actual->h_addr_list[0] == NULL);
  }

  return 0;
}
