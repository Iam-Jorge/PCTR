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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <definitions.h>
#include <semaphoreI.h>

/* Semaphores and shared memory retrieval */
void get_shm_segments(int *shm_symbol, struct TSymbol_t **p_symbol);
void get_sems(sem_t **p_sem_symbol_ready, sem_t **p_sem_symbol_decoded);

/* Task management */
void get_and_process_task(sem_t *sem_symbol_ready, sem_t *sem_symbol_decoded,
			  struct TSymbol_t *symbol);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  struct TSymbol_t *symbol;
  int shm_symbol;  
  sem_t *sem_symbol_ready, *sem_symbol_decoded;

  /* Get shared memory segments and semaphores */
  get_shm_segments(&shm_symbol, &symbol);
  get_sems(&sem_symbol_ready, &sem_symbol_decoded);

  /* Will work until killed by the manager */
  while (TRUE) {
    get_and_process_task(sem_symbol_ready, sem_symbol_decoded, symbol);
  }

  return EXIT_SUCCESS;
}

/******************** Semaphores and shared memory retrieval ********************/

void get_shm_segments(int *shm_symbol, struct TSymbol_t **p_symbol) {
  *shm_symbol = shm_open(SHM_SYMBOL, O_RDWR, 0644);
  *p_symbol = mmap(NULL, sizeof(struct TSymbol_t), PROT_READ | PROT_WRITE, MAP_SHARED,
		   *shm_symbol, 0); 
}

void get_sems(sem_t **p_sem_symbol_ready, sem_t **p_sem_symbol_decoded) {
  *p_sem_symbol_ready = get_semaphore(SEM_SYMBOL_READY);
  *p_sem_symbol_decoded = get_semaphore(SEM_SYMBOL_DECODED);
}

/******************** Task management ********************/

void get_and_process_task(sem_t *sem_symbol_ready, sem_t *sem_symbol_decoded,
			  struct TSymbol_t *symbol) {
  wait_semaphore(sem_symbol_ready);
    switch (symbol->value){
    case 53: symbol->value = 46; break;
    case 54: symbol->value = 44; break;
    case 55: symbol->value = 33; break;
    case 56: symbol->value = 63; break;
    case 57: symbol->value = 95; break;  
    }
  signal_semaphore(sem_symbol_decoded);
}
