#ifndef __SEMAFOROI_H__
#define __SEMAFOROI_H__

#include <semaphore.h>

// Crea un semáforo POSIX.
sem_t *crear_sem (const char *name, unsigned int valor);

// Obtiene un semáforo POSIX (ya existente).
sem_t *get_sem (const char *name);

// Cierra un semáforo POSIX.
void destruir_sem (const char *name);

// Incrementa el semáforo.
void signal_sem (sem_t *sem);

// Decrementa el semáforo.
void wait_sem (sem_t *sem);

#endif
