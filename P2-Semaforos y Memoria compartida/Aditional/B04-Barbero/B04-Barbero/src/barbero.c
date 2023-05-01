#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <semaforoI.h>
#include <memoriaI.h>
#include <global.h>

void barbero ();
void controlador (int senhal) { 
  printf ("[Barbero %ld] Finalizado (SIGINT)\n", (long)getpid());  exit(1);
}

int main (int argc, char *argv[]) {
  if (signal(SIGINT, controlador) == SIG_ERR) {
    fprintf(stderr, "Abrupt termination.\n");    exit(1);
  }
  barbero(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8]);  
  return 0;
}

void barbero (char *tipo,char *tiempo, char *id_despertar, char *id_preparado, char *id_sentado, 
						char *id_pagado, char *id_agradecer, char *propina ) {    
    sem_t *despertar, *preparado, *sentado, *pagado, *agradecer;
    int propina_handle, P;
    int pelando=atoi(tiempo);
    despertar = get_sem(id_despertar);  
    preparado = get_sem(id_preparado);
    sentado = get_sem(id_sentado);
    pagado = get_sem(id_pagado);
    agradecer = get_sem(id_agradecer);
    propina_handle=obtener_var(propina);
	while(1){
		printf("[Barbero %ld] : Barbero <%s> durmiendo zZzZzZzz\n",(long)getpid(),tipo);
		wait_sem(despertar);
		signal_sem(preparado);
		wait_sem(sentado);
		//CORTANDO PELO
		printf("[Barbero %ld] : Barbero <%s> cortando el pelo\n",(long)getpid(),tipo);
		sleep(pelando);
		printf("[Barbero %ld] : Barbero <%s> fin de corte\n",(long)getpid(),tipo);
		wait_sem(pagado);
		consultar_var(propina_handle,&P);
		if(P!=0) printf("[Barbero %ld] : Barbero <%s> agradece propina %d\n",(long)getpid(),tipo,P);	
		signal_sem(agradecer);		
	}//fin WHILE
}//fin BARBERO

