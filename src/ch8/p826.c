#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#define _POSIX_C_SOURCE 200809L

#include "csapp.h"

#define MAXARGS 128
#define JOB_POOL_SIZE 256
#define MAX_MSG_LEN 128

/* Helpers */
ssize_t Sio_puts(char s[]);
void Sio_error(char s[]);

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

void unix_error(char *msg) /* Unix-style error */
{
  fprintf(stderr, "%s: %s\n", msg, strerror(errno));
  exit(0);
}

pid_t Fork(void) {
  pid_t pid;

  if ((pid = fork()) < 0)
    unix_error("Fork error");
  return pid;
}

char *Fgets(char *ptr, int n, FILE *stream) {
  char *res = fgets(ptr, n, stream);
  if (res == NULL) {
    unix_error("fgets error");
  }

  return res;
}

typedef enum {
  None,
  BG,
  FG,
  STP,
} JobState;

typedef struct {
  pid_t pid;
  int jid;
  JobState state;
} job_info_t;

void init_pool(job_info_t *pool);
void init_pool(job_info_t *pool) {
  for (int i = 0; i < JOB_POOL_SIZE; i++) {
    pool[i].pid = 0;
    pool[i].jid = 0;
    pool[i].state = None;
  }
}

int find_free_pool_slot(job_info_t *pool);
int find_free_pool_slot(job_info_t *pool) {
  for (int i = 0; i < JOB_POOL_SIZE; i++) {
    if (pool[i].pid == 0)
      return i;
  }

  return -1;
}

job_info_t *add_job(job_info_t *pool, pid_t pid, JobState state);
job_info_t *add_job(job_info_t *pool, pid_t pid, JobState state) {
  int slot = find_free_pool_slot(pool);
  printf("Found free slot %d\n", slot);
  if (slot == -1) {
    return NULL;
  }

  printf("Adding job %d\n", pid);
  pool[slot].pid = pid;
  pool[slot].jid = slot + 1; // avoid 0 number
  pool[slot].state = state;

  return &pool[slot];
}

job_info_t *find_job(job_info_t *pool, pid_t pid, int jid);
job_info_t *find_job(job_info_t *pool, pid_t pid, int jid) {
  for (int i = 0; i < JOB_POOL_SIZE; i++) {
    if (pool[i].pid == pid || pool[i].jid == jid)
      return &pool[i];
  }
  return NULL;
}

void delete_job(job_info_t *pool, int jid, int wait);
void delete_job(job_info_t *pool, int jid, int wait) {
  for (int i = 0; i < JOB_POOL_SIZE; i++) {
    if (pool[i].jid == jid) {
      int status;
      if (wait && waitpid(pool[i].pid, &status, 0) < 0) {
        unix_error("failed to wait for child");
      }
      printf("Delete job pid=%d jid=%d\n", pool[i].pid, pool[i].jid);
      pool[i].pid = 0;
      pool[i].jid = 0;
      pool[i].state = None;
      return;
    }
  }
}

void list_jobs(job_info_t *pool);
void list_jobs(job_info_t *pool) {
  for (int i = 0; i < JOB_POOL_SIZE; i++) {
    if (pool[i].pid != 0 && (pool[i].state == BG || pool[i].state == STP)) {
      printf("JID=%d, PID=%d, STATE=%d\n", pool[i].jid, pool[i].pid,
             pool[i].state);
    }
  }
}

job_info_t job_pool[JOB_POOL_SIZE];

void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);

void reap(int opt, JobState excl_state) {

  int status;

  for (int i = 0; i < JOB_POOL_SIZE; i++) {
    if (job_pool[i].state == excl_state || job_pool[i].state == None ||
        job_pool[i].pid == 0)
      continue;

    Sio_puts("Waiting for process\n");
    Sio_putl(job_pool[i].pid);
    Sio_puts(" to finish\n");
    if (waitpid(job_pool[i].pid, &status, opt) > 0) {
      if (WIFSIGNALED(status)) {
        char s[MAX_MSG_LEN];
        sprintf(s, "Process %d terminated by signal %d\n", job_pool[i].pid,
                WTERMSIG(status));
        Sio_puts(s);
      }
      job_info_t *job = find_job(job_pool, job_pool[i].pid, 0);
      if (job == NULL) {
        unix_error("Reaping non-existend job");
      }
      if (WIFEXITED(status))
        delete_job(job_pool, job->jid, 0);
    }
  }
}

void sigchld_handler(int sig) {
  Sio_puts("Sigchld handler\n");
  reap(WNOHANG, FG);
  Sio_puts("Sigchld handler finish\n");
}

void sigint_handler(int sig) {
  sigset_t mask, prev_mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGCHLD);

  sigprocmask(SIG_BLOCK, &mask, &prev_mask);

  Sio_puts("Sigint handler\n");

  for (int i = 0; i < JOB_POOL_SIZE; i++) {
    if (job_pool[i].pid != 0) {
      if (job_pool[i].state == STP) {
        Sio_puts("Continue stopped child before interrupt\n");
        kill(job_pool[i].pid, SIGCONT);
      }

      Sio_puts("Sending SIGINT to child\n");
      Sio_putl(job_pool[i].pid);
      Sio_puts("\n");
      kill(job_pool[i].pid, SIGINT);
    }
  }

  reap(0, None);

  sigprocmask(SIG_SETMASK, &prev_mask, NULL);
  Sio_puts("Sigint handler finish\n");
  exit(0);
}

void sigtstp_handler(int sig) {
  Sio_puts("Sigtstp handler\n");

  for (int i = 0; i < JOB_POOL_SIZE; i++) {
    if (job_pool[i].state == FG)
      kill(job_pool[i].pid, SIGTSTP);
  }

  Sio_puts("Sigtstp handler finish\n");
}

int main(void) {
  if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
    unix_error("signal error");
  }
  if (signal(SIGINT, sigint_handler) == SIG_ERR) {
    unix_error("signal error");
  }

  if (signal(SIGTSTP, sigtstp_handler) == SIG_ERR) {
    unix_error("signal error");
  }

  init_pool(job_pool);

  char cmdline[MAXLINE]; /* Command line */

  while (1) {
    /* Read */
    printf("> ");
    Fgets(cmdline, MAXLINE, stdin);
    if (feof(stdin))
      exit(0);

    /* Evaluate */
    eval(cmdline);
  }
}
/* eval - Evaluate a command line */
void eval(char *cmdline) {
  char *argv[MAXARGS]; /* Argument list execve() */
  char buf[MAXLINE];   /* Holds modified command line */
  int bg;              /* Should the job run in bg or fg? */
  pid_t pid;           /* Process id */

  strcpy(buf, cmdline);
  bg = parseline(buf, argv);
  if (argv[0] == NULL)
    return; /* Ignore empty lines */

  if (!builtin_command(argv)) {
    if ((pid = Fork()) == 0) { /* Child runs user job */
      setpgid(0, 0);
      if (execve(argv[0], argv, environ) < 0) {
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
      }
    }

    /* Parent waits for foreground job to terminate */
    if (!bg) {
      int status;
      job_info_t *job = add_job(job_pool, pid, FG);
      if (job == NULL) {
        unix_error("Failed to add fg job");
      }
      printf("wait for foreground job\n");

      if (waitpid(pid, &status, WUNTRACED) < 0)
        unix_error("waitfg: waitpid error");

      if (WSTOPSIG(status)) {
        job->state = STP;
        printf("fg job stopped\n");
      } else {
        printf("fg job exited\n");
      }

    } else {
      job_info_t *job = add_job(job_pool, pid, BG);
      if (job == NULL) {
        unix_error("Failed to add bg job");
      }
      printf("%%%d %d %s", job->jid, pid, cmdline);
    }
  }
  return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) {
  if (!strcmp(argv[0], "quit")) /* quit command */
    exit(0);
  if (!strcmp(argv[0], "&")) /* Ignore singleton & */
    return 1;
  if (!strcmp(argv[0], "jobs")) {
    list_jobs(job_pool);
    return 1;
  }
  if (!strcmp(argv[0], "bg")) {
    long pid;
    job_info_t *job = NULL;
    printf("argv[1]=%s\n", argv[1]);
    if ((pid = strtol(argv[1], NULL, 10)) != 0) {
      printf("Restart pid=%ld in background\n", pid);
      job = find_job(job_pool, pid, 0);
    } else if (argv[1][0] == '%') {
      printf("Restart jid=%s in background\n", argv[1] + 1);
      job = find_job(job_pool, 0, strtol(argv[1] + 1, NULL, 10));
    } else {
      printf("Invalid fg usage\n");
      return 1;
    }

    if (job == NULL) {
      printf("Error: unknown pid\n");
      return 1;
    }
    kill(job->pid, SIGCONT);
    job->state = BG;
    return 1;
  }
  if (!strcmp(argv[0], "fg")) {
    long pid;
    printf("argv[1]=%s\n", argv[1]);
    job_info_t *job = NULL;

    if ((pid = strtol(argv[1], NULL, 10)) != 0) {
      printf("Restart pid=%ld in foreground\n", pid);
      job = find_job(job_pool, pid, 0);

    } else if (argv[1][0] == '%') {
      printf("Restart jid=%s in background\n", argv[1] + 1);
      job = find_job(job_pool, 0, strtol(argv[1] + 1, NULL, 10));

    } else {
      printf("Invalid fg usage\n");
      return 1;
    }

    if (job == NULL) {
      printf("Error: unknown pid\n");
      return 1;
    }

    kill(job->pid, SIGCONT);
    job->state = FG;

    printf("wait for foreground job\n");

    int status;
    if (waitpid(job->pid, &status, WUNTRACED) < 0)
      unix_error("waitfg: waitpid error");

    if (WSTOPSIG(status)) {
      job->state = STP;
      printf("fg job stopped\n");
    } else {
      printf("fg job exited\n");
    }

    return 1;
  }

  return 0; /* Not a builtin command */
}

/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) {
  char *delim; /* Points to first space delimiter */
  int argc;    /* Number of args */
  int bg;      /* Background job? */

  buf[strlen(buf) - 1] = ' ';   /* Replace trailing ‘\n’ with space */
  while (*buf && (*buf == ' ')) /* Ignore leading spaces */
    buf++;

  /* Build the argv list */
  argc = 0;
  while ((delim = strchr(buf, ' '))) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')) /* Ignore spaces */
      buf++;
  }
  argv[argc] = NULL;

  if (argc == 0) /* Ignore blank line */
    return 1;

  /* Should the job run in the background? */
  if ((bg = (*argv[argc - 1] == '&')) != 0)
    argv[argc - 1] = NULL;

  return bg;
}
