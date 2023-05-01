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
#include <semaforoI.h>
#include <memoriaI.h>
#include <global.h>

void cliente ();
int get_posicion();
void actualizar_posicion();

void controlador (int senhal) { 
  printf ("[Cliente %ld] Finalizado (SIGINT)\n",(long) getpid());  exit(1);
}

int main (int argc, char *argv[]) {
  if (signal(SIGINT, controlador) == SIG_ERR) {
    fprintf(stderr, "Abrupt termination.\n");    exit(1);
  }

  cliente(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],
							argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
  return 0;
}

void cliente (char *idm_sillas, char *idm_pie, char *idm_caja, char *id_despertar_R, char *id_despertar_M, char *id_despertar_L, 
		char *id_preparado, char *id_sentado, char *id_pagado, char *id_agradecer, char *enpie, 
					char *cl_sentados, char *caja,char *propi, char *id_sillas, char *idm_prioridad, char *id_prioridad) {
	sem_t *despertarR, *despertarM, *despertarL, *preparado, *sentado, *pagado, *agradecer, *sillas, *msillas, *mpie, *mcaja;
	//Semaforos que controlan acciones
	despertarR=get_sem(id_despertar_R);
	despertarM=get_sem(id_despertar_M);
	despertarL=get_sem(id_despertar_L);
	preparado=get_sem(id_preparado);
	sentado = get_sem(id_sentado);
	pagado = get_sem(id_pagado);
	agradecer = get_sem(id_agradecer);
	sillas = get_sem(id_sillas);
	//Cerrojos sobre variables compartidas
	msillas = get_sem(idm_sillas);
	mpie = get_sem(idm_pie);
	mcaja = get_sem(idm_caja);
	//mprioridad = get_sem(idm_prioridad);
	//Obtener variables compartidas
	int cajaBarb_handler, sillas_handler, propi_handler, depie_handler,posicion;
	cajaBarb_handler = obtener_var(caja);
	sillas_handler = obtener_var(cl_sentados);
	propi_handler = obtener_var(propi);
	depie_handler = obtener_var(enpie);
	int sentados,levantados,cajaBarberia,P;
	char *tipo;
	wait_sem(msillas);
	consultar_var(sillas_handler,&sentados);
	wait_sem(mpie);
	consultar_var(depie_handler,&levantados);
	if(sentados+levantados<CAPACIDAD_TOTAL){
		printf("[Cliente %ld] : entra en la barberia\n",(long)getpid());
		signal_sem(mpie);
		if(sentados>=N_SILLAS){
			signal_sem(msillas);
			wait_sem(mpie);
			consultar_var(depie_handler,&levantados);
			modificar_var(depie_handler,++levantados);
			printf("[Cliente %ld] : ocupa un sitio en la barberia (quedan %d sitios libres)\n",(long)getpid(),CAPACIDAD_TOTAL-(sentados+levantados));
			signal_sem(mpie);
			wait_sem(sillas);
			wait_sem(msillas);
			printf("[Cliente %ld] : ocupa una silla libre\n",(long)getpid());
			consultar_var(sillas_handler,&sentados);
			modificar_var(sillas_handler,++sentados);			
			signal_sem(msillas);
			wait_sem(mpie);
			consultar_var(depie_handler,&levantados);
			modificar_var(depie_handler,--levantados);
			signal_sem(mpie);
		}else{
			consultar_var(sillas_handler,&sentados);
			modificar_var(sillas_handler,++sentados);
			wait_sem(mpie);
			consultar_var(depie_handler,&levantados);			
printf("[Cliente %ld] : ocupa un sitio en la barberia (quedan %d sitios libres)\n",(long)getpid(),CAPACIDAD_TOTAL-(sentados+levantados));
			printf("[Cliente %ld] : ocupa una silla libre\n",(long)getpid());
			signal_sem(msillas);
			signal_sem(mpie);
			wait_sem(sillas);
		}
		posicion=get_posicion(id_prioridad,idm_prioridad);
		if(posicion==0){
			tipo="Rapido";
			printf("[Cliente %ld] : elige al barbero <%s>\n",(long)getpid(),tipo);
			signal_sem(despertarR);
		}
		else if (posicion==1){
			tipo="Medio";
			printf("[Cliente %ld] : elige al barbero <%s>\n",(long)getpid(),tipo);
			signal_sem(despertarM);
		}
		else{
			tipo="Lento";
			printf("[Cliente %ld] : elige al barbero <%s>\n",(long)getpid(),tipo);
			signal_sem(despertarL);
		}
		printf("[Cliente %ld] : despierta al barbero <%s>\n",(long)getpid(),tipo);
		//wait_sem(preparado);
		
		printf("[Cliente %ld] : libera una sillon\n",(long)getpid());

		actualizar_posicion(id_prioridad,idm_prioridad,posicion);

		wait_sem(msillas);
		consultar_var(sillas_handler,&sentados);
		modificar_var(sillas_handler,--sentados);
		signal_sem(msillas);
		signal_sem(sillas);
		signal_sem(sentado);
		wait_sem(mcaja);
		consultar_var(cajaBarb_handler,&cajaBarberia);
		modificar_var(cajaBarb_handler,cajaBarberia+K);
		signal_sem(mcaja);
		srand((int)getpid());
		int clipro=rand()%MAX_PROPINA;
		consultar_var(propi_handler,&P);
		modificar_var(propi_handler,clipro);
		printf("[Cliente %ld] : da una propina de %d €\n",(long)getpid(),clipro);
		signal_sem(pagado);
		wait_sem(agradecer);
		printf("[Cliente %ld] : se va de la barberia (quedan %d sitios libres)\n",(long)getpid(),CAPACIDAD_TOTAL-(sentados+levantados));

	}else{
		printf("[Cliente %ld] : No coge en la barberia, pase mas tarde\n",(long)getpid());
		signal_sem(msillas);
		signal_sem(mpie);
		exit(1);
	}
	
}
int get_posicion(char *id_prioridad, char *idm_prioridad){

	int i,shmDataPrioridad;
	sem_t *mprioridad;
	mprioridad = get_sem(idm_prioridad);
	struct TData *prio;
	shmDataPrioridad = shm_open(id_prioridad,O_RDWR, 0666);
	prio = mmap(NULL, sizeof(struct TData), PROT_READ | PROT_WRITE, MAP_SHARED, shmDataPrioridad, 0);

	//acceso a vmc//
	wait_sem(mprioridad);

	for(i=0;i<S;i++){
		if(prio->array[i]==0){ 
			prio->array[i]=1;
			break;
		}
	}
	signal_sem(mprioridad);
	return i;

}
void actualizar_posicion(char *id_prioridad, char *idm_prioridad, int posicion){
	int shmDataPrioridad;
	sem_t *mprioridad;
	mprioridad = get_sem(idm_prioridad);
	struct TData *prio;
	shmDataPrioridad = shm_open(id_prioridad,O_RDWR, 0666);
	prio = mmap(NULL, sizeof(struct TData), PROT_READ | PROT_WRITE, MAP_SHARED, shmDataPrioridad, 0);

	//acceso a vmc//
	wait_sem(mprioridad);
		prio->array[posicion]=0;
	signal_sem(mprioridad);
}	
