#include "helpers.h"
#include "pool.h"
#include "rio.h"

void check_clients(pool *p);

int byte_cnt = 0; /* Counts total bytes received by server */

int main(int argc, char **argv) {
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  static pool pool;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  listenfd = Open_listenfd(argv[1]);
  init_pool(listenfd, &pool);
  while (1) {
    /* Wait for listening/connected descriptor(s) to become ready */
    pool.ready_set = pool.read_set;
    pool.nready = Select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

    /* If listening descriptor ready, add new client to pool */
    if (FD_ISSET(listenfd, &pool.ready_set)) {
      clientlen = sizeof(struct sockaddr_storage);
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      add_client(connfd, &pool);
    }

    /* Echo a text line from each ready connected descriptor */
    check_clients(&pool);
  }
}

void check_clients(pool *p) {
  int i, connfd, n;
  char buf[MAXLINE];
  rio_t rio;

  for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
    connfd = p->clientfd[i];
    rio = p->clientrio[i];

    /* If the descriptor is ready, echo a text line from it */
    if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
      p->nready--;
      if ((n = rio_readnb(&rio, buf, 1)) != 0) {
        // read the rest of the buffer
        if (rio.rio_cnt > 0) {
          n += rio_readnb(&rio, buf + 1,
                          MAXLINE > rio.rio_cnt ? rio.rio_cnt : MAXLINE);
        }

        byte_cnt += n;
        printf("Server received %d (%d total) bytes on fd %d\n", n, byte_cnt,
               connfd);
        Rio_writen(connfd, buf, n);
      }

      /* EOF detected, remove descriptor from pool */
      else {
        printf("Closing client %d\n", connfd);
        Close(connfd);
        FD_CLR(connfd, &p->read_set);
        p->clientfd[i] = -1;
      }
    }
  }
}
