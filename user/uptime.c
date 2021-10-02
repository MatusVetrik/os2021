// tu bude implementacia sleep

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[] ){
  int ticks;

  ticks = uptime();
  printf("%d\n",ticks);

exit(0);
}
