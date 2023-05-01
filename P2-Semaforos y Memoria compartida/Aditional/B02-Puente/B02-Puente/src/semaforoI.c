#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <semaforoI.h>

sem_t *crear_sem (const char *name, unsigned int valor) {
  sem_t *sem;

  sem = sem_open(name, O_CREAT, 0644, valor);
  if (sem == SEM_FAILED) {
    fprintf(stderr, "Error al crear el sem.: %s\n", 
	    strerror(errno));
    exit(1);
  }

  return sem;
}

sem_t *get_sem (const char *name) {
  sem_t *sem;

  sem = sem_open(name, O_RDWR);
  if (sem == SEM_FAILED) {
    fprintf(stderr, "Error al obtener el sem.: %s\n", 
	    strerror(errno));
    exit(1);
  }

  return sem;
}

void destruir_sem (const char *name) {
  sem_t *sem = get_sem(name);

  /* Se cierra el semáforo */
  if ((sem_close(sem)) == -1) {
    fprintf(stderr, "Error al cerrar el sem.: %s\n", 
	    strerror(errno));
    exit(1);
  }

  /* Se elimina el sem */
  if ((sem_unlink(name)) == -1) {
    fprintf(stderr, "Error al destruir el sem.: %s\n", 
	    strerror(errno));
    exit(1);
  }
}

void signal_sem (sem_t *sem) {
  if ((sem_post(sem)) == -1) {
    fprintf(stderr, "Error al modificar el sem.: %s\n", 
	    strerror(errno));
    exit(1);
  }
}

void wait_sem (sem_t *sem) {
  if ((sem_wait(sem)) == -1) {
    fprintf(stderr, "Error al modificar el sem.: %s\n", 
	    strerror(errno));
    exit(1);
  }
}
