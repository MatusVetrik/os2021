
// fork.c: create a new process

#include "kernel/types.h"
#include "user/user.h"

int
main()
{
  int pid;

  pid = fork();

  if(pid == 0){
    printf("child\n");
    printf("fork() returned %d\n", pid);
  } else {
    wait(0);
    printf("parent\n");
    printf("fork() returned %d\n", pid);
  }

  exit(0);
}
