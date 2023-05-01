#define N_CLIENTES 10
#define BARBEROS 3
#define CAPACIDAD_TOTAL 10
#define N_SILLAS 2
#define K 10 /*importe fijo por corte de pelo*/
#define MAX_PROPINA 5 /*propina maxima*/
/*Mutex asociado a vmc*/
#define MSILLAS "mtx_sillas"
#define MPIE "mtx_pie"
#define MCAJA "mtx_caja"
#define MPRIO "mtx_prioridad"
#define S 3 /*Tiempo base que tarda cada barbero en cortar el pelo*/
/*SEMAFOROS*/
#define PREPARADO "preparado"
#define SENTADO "sentado"
#define PAGADO "pagado"
#define AGRADECER "agradecer"
/*RAPIDO*/
#define DESPERTAR_R "despertarRapido"

/*MEDIO*/
#define DESPERTAR_M "despertarMedio"
/*LENTO*/
#define DESPERTAR_L "despertarLento"

/*Variable memoria compartida*/
#define SILLAS "sillasEspera"
#define ENPIE "EsperanEnPie"
#define SENTADOS "EsperanSentados"
#define CAJA "CajaBarberia"
#define PROPINA "PropinaBarbero"
#define PRIORIDAD "prioridad"

struct TData{
	int array [S];
};
