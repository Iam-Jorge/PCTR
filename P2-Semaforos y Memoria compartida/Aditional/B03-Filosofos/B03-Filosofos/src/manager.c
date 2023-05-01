#define _POSIX_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <memoriaI.h>
#include <semaforoI.h>

#define FILOSOFOS 5

void esperar (int senhal);
void controlador (int senhal);

int main (int argc, char *argv[]) {
	pid_t pid_hijo; 
	int i;

  	/* Creación de semáforos y segmentos de memoria compartida */
  	crear_sem(PUENTE, 1); 
  	crear_sem(MUTEXN, 1);      
  	crear_sem(MUTEXS, 1);
  	crear_var(COCHESNORTE, 0); 
  	crear_var(COCHESSUR, 0);

  	/* Se espera la terminación de los procesos... */
  	if (signal(SIGCHLD, esperar) == SIG_ERR) {
    	fprintf(stderr, "Error al ejecutar signal.\n");
    	exit(EXIT_FAILURE);
  	}

  	/* Manejo de Ctrol+C */
  	if (signal(SIGINT, controlador) == SIG_ERR) {
    	fprintf(stderr, "Error al ejecutar signal.\n");
    	exit(EXIT_FAILURE);
  	}

  	/* Se lanzan los filosofos... */
  	for (i = 0; i < FILOSOFOS; i++) {

    	switch (pid_hijo = fork()) {
    		case 0:
      		/* Mismo proceso; distintos datos */
      			if ((i % 2) == 0) /* Coche Norte  */
					execl("./exec/coche", "coche", "N", PUENTE, MUTEXN, COCHESNORTE, NULL);
      			else              /* Coche Sur    */
					execl("./exec/coche", "coche", "S", PUENTE, MUTEXS, COCHESSUR, NULL);
      		break;
    	}

    sleep(rand() % MAX_T_LANZAR + 1);

  	}

  	while(1) 
    	pause(); /* A la espera de señales... */

  	return 0;
}

void esperar (int senhal) {
	int status; 
	wait(&status);

	/* Se rearma el manejador */
  	if (signal(SIGCHLD, esperar) == SIG_ERR) {
    	fprintf(stderr, "Error al ejecutar signal.\n");
    	exit(EXIT_FAILURE);
  	}
}

void controlador (int senhal) {
  	printf("\nCtrl+c capturada.\n");
  	printf("Finalizando...\n\n");

  	destruir_sem(PUENTE);
  	destruir_sem(MUTEXN);
  	destruir_sem(MUTEXS);
  	destruir_var(COCHESNORTE);
  	destruir_var(COCHESSUR);

  	sleep(1);
  	printf("<OK!>\n");
  	exit(EXIT_SUCCESS);
}
