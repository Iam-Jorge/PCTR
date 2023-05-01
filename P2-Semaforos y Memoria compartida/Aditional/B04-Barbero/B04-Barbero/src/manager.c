#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <memoriaI.h>
#include <semaforoI.h>

#include <global.h>
void controlador (int senhal);
void finalizarprocesos(int);
void liberarecursos();
pid_t pids[BARBEROS+N_CLIENTES];

int main (int argc, char *argv[]) {
  int i,shmDataPrioridad,k;
  int j=0;
  struct TData *prio;
  memset (pids, 0, sizeof(pid_t)*(BARBEROS+N_CLIENTES));
  srand((int)getpid());

  // Creación de semáforos y segmentos de memoria compartida.

  // Manejo de variables compartidas (mutex)
  crear_sem(MSILLAS,1);
  crear_sem(MPIE,1);
  crear_sem(MCAJA,1);
  crear_sem(MPRIO,1);
  // Semaforos lentos
  crear_sem(DESPERTAR_L,0);
  // Semaforos medios
  crear_sem(DESPERTAR_M,0);
  // Semaforos rapidos
  crear_sem(DESPERTAR_R,0);
  //Sem
  crear_sem(PREPARADO, 0);
  crear_sem(SENTADO, 0);
  crear_sem(PAGADO, 0);
  crear_sem(AGRADECER, 0);

  crear_sem(SILLAS, N_SILLAS);
  
  //Variables compartidas
   crear_var(ENPIE,0);
   crear_var(SENTADOS,0);
   crear_var(CAJA,0);
   crear_var(PROPINA,0);
   shmDataPrioridad = shm_open(PRIORIDAD, O_CREAT | O_RDWR, 0666);
   if (shmDataPrioridad == -1) {
		fprintf(stderr, "Error al crear la variable: %s\n",strerror(errno));
		exit(1);
   }
   if(ftruncate(shmDataPrioridad, sizeof(struct TData))==-1){
		fprintf(stderr, "Error %s\n",strerror(errno));
		exit(1);
   }
   prio = mmap(NULL, sizeof(struct TData), PROT_READ | PROT_WRITE, MAP_SHARED, shmDataPrioridad, 0);
   if (prio == MAP_FAILED) {
		fprintf(stderr, "Error %s\n", strerror(errno));
		exit(1);
   }
	for(k=0;k<S; k++) prio->array[k]=0;
	munmap(prio,sizeof(struct TData));

  // Manejo de Ctrol+C.
  if (signal(SIGINT, controlador) == SIG_ERR) {
    fprintf(stderr, "Abrupt termination.\n");    exit(1);
  }

  // Lanzar los 3 barberos
   int time=0;
   char *tiempo;
   tiempo=(char*)malloc(sizeof(int));
   for (i = 0; i < BARBEROS; i++) {
   	if((pids[j++]= fork())==0) {
		switch(i){
	      // Mismo proceso; distintos datos.
		case 0: //Barbero <rapido>
			time=S;
			sprintf(tiempo,"%d",time);
			execl("./exec/barbero", "barbero","rapido",tiempo, DESPERTAR_R, PREPARADO, SENTADO, PAGADO, AGRADECER, PROPINA,NULL);
			break;
		case 1://Barbero <lento>            
			time=S*3;
			sprintf(tiempo,"%d",time);
			execl("./exec/barbero", "barbero","lento",tiempo,DESPERTAR_L, PREPARADO, SENTADO, PAGADO, AGRADECER, PROPINA, NULL);
			break;
		case 2://Barbero <medio>
			time=S*2;
			sprintf(tiempo,"%d",time);
			execl("./exec/barbero", "barbero","medio",tiempo,DESPERTAR_M, PREPARADO, SENTADO, PAGADO, AGRADECER, PROPINA,NULL);
        		break;
        	}//Fin switch
	}// Fin if
  }// Fin for.
  for (i = 1; i <= N_CLIENTES; i++) {
    if ((pids[j++] = fork())==0) {      
		execl("./exec/cliente", "cliente", MSILLAS, MPIE, MCAJA,DESPERTAR_R,DESPERTAR_M,DESPERTAR_L, PREPARADO, SENTADO, PAGADO, AGRADECER, ENPIE, SENTADOS, CAJA, PROPINA,SILLAS,MPRIO,PRIORIDAD, NULL);
    }// Fin if
  }// Fin for.
  for (; j>=BARBEROS; j--) waitpid(pids[j], 0, 0);
  finalizarprocesos(0);
  int negocio, ganancias;
  ganancias=obtener_var(CAJA);
  consultar_var(ganancias,&negocio);
  liberarecursos();
  printf("\n------------- Ganancias Finales -------------\n");
  printf("La barbería ha ganado %d\n",negocio);
  return 0;
}

void controlador (int senhal) {
  printf("\nCtrl+c capturada.\n");
  printf("Finalizando...\n\n");
  finalizarprocesos(1);
  liberarecursos();
  printf("OK!\n");
  // Salida del programa.
  exit(0);
}
void liberarecursos (void) {
  printf ("\n----- Liberando recursos ----- \n");
  printf("Recursos generales...\n");
  destruir_sem(MSILLAS);
  destruir_sem(MPIE);
  destruir_sem(MCAJA);
  destruir_sem(MPRIO);
  printf("Recursos relativos a los 3 barberos ...\n");
  // Semaforos lentos
  destruir_sem(DESPERTAR_L);
  // Semaforos medios
  destruir_sem(DESPERTAR_M);
  // Semaforos rapidos
  destruir_sem(DESPERTAR_R);
  //Semaforos
  destruir_sem(SENTADO);
  destruir_sem(PAGADO);
  destruir_sem(AGRADECER);

  destruir_sem(SILLAS);
  
  //Variables compartidas
   destruir_var(ENPIE);
   destruir_var(SENTADOS);
   destruir_var(CAJA);
   destruir_var(PROPINA);

}
void finalizarprocesos(int todos) {
  /* Si todos == 0, se manda señal sólo a procesos B */
  /* Si todos == 1, se manda señal a todos los procesos */
  int i, nproc;
  
  if (todos) nproc = BARBEROS+N_CLIENTES; else nproc = BARBEROS;
  printf ("\n-------------- Finalización de procesos ------------- \n");
  for (i=0; i<nproc; i++) {
    if (pids[i]) {
      printf ("Finalizando proceso [%d]...", pids[i]);  
      kill(pids[i], SIGINT); printf ("<Ok>\n");
    }
  }
}
