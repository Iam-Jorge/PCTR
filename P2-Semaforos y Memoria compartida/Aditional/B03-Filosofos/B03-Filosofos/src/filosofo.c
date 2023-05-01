#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <semaforoI.h>
#include <memoriaI.h>

#define MAX_TIME_CRUZAR 5

void filosofo ();

int main (int argc, char *argv[]) {
  coche(argv[1], argv[2], argv[3], argv[4]);
  return 0;
}

void filosofo () {
 
}
