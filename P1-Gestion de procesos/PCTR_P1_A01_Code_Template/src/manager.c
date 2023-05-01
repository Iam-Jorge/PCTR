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

#define _POSIX_SOURCE

#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <definitions.h>

/* Total number of processes */
int g_nProcesses;          
/* 'Process table' (child processes) */
struct TProcess_t *g_process_table; 

/* Process management */
void create_processes_by_class(enum ProcessClass_t class, int n_new_processes,
			       int index_process_table, char *s_tmax_wait);
pid_t create_single_process(const char *path, const char *str_process_class, const char *arg);
void get_str_process_info(enum ProcessClass_t class, char **path, char **str_process_class);
void init_process_table(int nPA, int nPB);
void terminate_processes(void);
void wait_processes(int nPA);

/* Auxiliar functions */
void free_resources();
void install_signal_handler();
void parse_argv(int argc, char *argv[], int *nPA, int *nPB, char **s_tmax_wait);
void signal_handler(int signo);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  char *s_tmax_wait = NULL;
  int nPA, nPB; 

  parse_argv(argc, argv, &nPA, &nPB, &s_tmax_wait);
  install_signal_handler();

  init_process_table(nPA, nPB);
  create_processes_by_class(PB, nPB, 0, s_tmax_wait);
  create_processes_by_class(PA, nPA, nPB, s_tmax_wait);
  wait_processes(nPA);

  printf("\n[MANAGER] Program termination (all the PA processes terminated).\n");
  terminate_processes();
  free_resources();

  return EXIT_SUCCESS;
}

/******************** Process management ********************/

void create_processes_by_class(enum ProcessClass_t class, int n_new_processes,
			       int index_process_table, char *s_tmax_wait) {
  char *path = NULL, *str_process_class = NULL;
  int i;
  pid_t pid;

  get_str_process_info(class, &path, &str_process_class);

  for (i = index_process_table; i < (index_process_table + n_new_processes); i++) {
    pid = create_single_process(path, str_process_class, s_tmax_wait);

    g_process_table[i].class = class;
    g_process_table[i].pid = pid;
    g_process_table[i].str_process_class = str_process_class;
  }

  printf("[MANAGER] %d %s processes created.\n", n_new_processes, str_process_class);
  sleep(1);
}

pid_t create_single_process(const char *path, 
			    const char *str_process_class, const char *arg) {

  /* INCLUDE REMAINING CODE HERE */

}

void get_str_process_info(enum ProcessClass_t class, char **path, char **str_process_class) {
  switch (class) {
  case PA:
    *path = PA_PATH;
    *str_process_class = PA_CLASS;
    break;
  case PB:
    *path = PB_PATH;
    *str_process_class = PB_CLASS;
    break;
  }
}

void init_process_table(int nPA, int nPB) {
  int i;

  /* Number of processes to be created */
  g_nProcesses = nPA + nPB; 
  /* Allocate memory for the 'process table' */
  g_process_table = malloc(g_nProcesses * sizeof(struct TProcess_t)); 

  /* Init the 'process table' */
  for (i = 0; i < g_nProcesses; i++) {
    g_process_table[i].pid = 0;
  }
}

void terminate_processes(void) {
  int i;
  
  printf("\n----- [MANAGER] Terminating running child processes ----- \n");
  for (i = 0; i < g_nProcesses; i++) {
    /* Child process alive */
    if (g_process_table[i].pid != 0) { 
      printf("[MANAGER] Terminating %s process [%d]...\n", 
	     g_process_table[i].str_process_class, g_process_table[i].pid);
      if (kill(g_process_table[i].pid, SIGINT) == -1) {
	fprintf(stderr, "[MANAGER] Error using kill() on process %d: %s.\n", 
		g_process_table[i].pid, strerror(errno));
      }
    }
  }
}

void wait_processes(int nPA) {
  int i;
  pid_t pid;

  /* Wait for the termination of PA processes */
  while (nPA > 0) {
    /* Wait for any PA process */
    pid = wait(NULL); 
    for (i = 0; i < g_nProcesses; i++) {
      if (pid == g_process_table[i].pid) {
	/* Update the 'process table' */
        g_process_table[i].pid = 0;           
	/* Decrement the number of running PA processes */
        if (g_process_table[i].class == PA) { 
	  nPA--; 
	}
	/* Child process found */
        break; 
      }
    }
  }
}

/******************** Auxiliar functions ********************/

void free_resources() {
  /* Free the 'process table' memory */
  free(g_process_table); 
}

void install_signal_handler() {
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    fprintf(stderr, "[MANAGER] Error installing signal handler: %s.\n", 
	    strerror(errno));    
    exit(EXIT_FAILURE);
  }
}

void parse_argv(int argc, char *argv[], int *nPA, int *nPB, char **s_tmax_wait) {
  if (argc < 4) {
    fprintf(stderr, "Error. Use: ./exec/manager <n_processes_PA> <n_processes_PB> <t_max_wait>.\n");    
    exit(EXIT_FAILURE); 
  }

  /* Number of PA/PB processes and max waiting time */
  *nPA = atoi(argv[1]);  
  *nPB = atoi(argv[2]); 
  *s_tmax_wait = argv[3];
}

void signal_handler(int signo) {
  printf("\n[MANAGER] Program termination (Ctrl + C).\n");
  terminate_processes();
  free_resources();
  exit(EXIT_SUCCESS);
}
