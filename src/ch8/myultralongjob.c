
#include "csapp.h"

ssize_t Sio_puts(char s[]);
void Sio_error(char s[]);

void sigstp_h(int sig) { Sio_puts("Ultra long job received stop signal\n"); }

void sigint_h(int sig) {
  Sio_puts("Ultra long job received interrupt signal\n");
}

int main(void) {
  sleep(60);
  return 0;
}
