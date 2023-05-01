/*
====================================================================
Concurrent and Real-Time Programming
Faculty of Computer Science
University of Castilla-La Mancha (Spain)

Contact info: http://www.libropctr.com

You can redistribute and/or modify this file under the terms of the
GNU General Public License ad published by the Free Software
Foundation, either version 3 of the License, or (at your option) and
later version. See <http://www.gnu.org/licenses/>.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
====================================================================
*/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Program logic */
void run(int t_wait);

/* Auxiliar functions */
void install_signal_handler();
void parse_argv(const int argc, char *argv[], int *t_wait);
void signal_handler(int signo);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  int t_wait; 

  install_signal_handler(); 
  parse_argv(argc, argv, &t_wait);

  run(t_wait);

  return EXIT_SUCCESS;
}

/******************** Program logic ********************/

void run(int t_wait) {
  while(1) {
    printf("[PB %d] sleeps %d seconds.\n", getpid(), t_wait);
    sleep(t_wait);
  }
}

/******************** Auxiliar functions ********************/

void install_signal_handler() {
if (signal(SIGINT, signal_handler) == SIG_ERR) {
    fprintf(stderr, "[PB %d] Error installing handler: %s.\n", 
	    getpid(), strerror(errno));    
    exit(EXIT_FAILURE);
  }
}

void parse_argv(int argc, char *argv[], int *t_wait) {
  if (argc != 2) {
    fprintf(stderr, "[PB %d] Error in the command line.\n", getpid());
    exit(EXIT_FAILURE);
  }

  /* Set the seed for a new sequence of pseudo-random integer */
  srand((int)getpid()); 
  /* Set the waiting time */
  *t_wait = 1 + (rand() % atoi(argv[1])); 
}

void signal_handler(int signo) {
  printf("[PB %d] terminated (SIGINT).\n", getpid());
  exit(EXIT_SUCCESS);
}
