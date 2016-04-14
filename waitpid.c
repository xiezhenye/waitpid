#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    return 1;
  }
  pid_t pid = atoi(argv[1]);
  int ret;
  if (ptrace(PTRACE_ATTACH, pid, 0, 0)) {
    return 2;
  }
  waitpid(pid, &ret, 0);
  if (ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXIT)) {
    return 3; 
  }
  if (ptrace(PTRACE_CONT, pid, 0, 0)) {
    return 4;
  }
  while (1) {
    waitpid(pid, &ret, 0);
    if ( (WSTOPSIG(ret) == SIGTRAP) && (ret & (PTRACE_EVENT_EXIT << 8)) ) {
      break;
    }
    if (ptrace(PTRACE_CONT, pid, 0, WSTOPSIG(ret))) {
      return 5;
    }   
  }
  if (ptrace(PTRACE_GETEVENTMSG, pid, 0, &ret)) {
    return 6;
  } 
  printf("pid %d exit %d\n", pid, ret);
  ptrace(PTRACE_CONT, pid, 0, 0);
  return 0;
}

