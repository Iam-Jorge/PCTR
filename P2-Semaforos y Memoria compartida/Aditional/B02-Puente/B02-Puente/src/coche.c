#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <semaforoI.h>
#include <memoriaI.h>

#define MAX_TIME_CRUZAR 5

void coche ();
void cruzar ();

int main (int argc, char *argv[]) {
  coche(argv[1], argv[2], argv[3], argv[4]);
  return 0;
}

/* salida permite identificar el origen del coche (N o S) */
void coche (char *salida, char *id_puente, 
	    char *id_mutex, char *id_num_coches) {
  int num_coches_handle, valor;
  sem_t *puente, *mutex;

  srand((int)getpid());

  puente = get_sem(id_puente);  
  mutex = get_sem(id_mutex);
  num_coches_handle = obtener_var(id_num_coches);

  /* Acceso a la variable compartida 'num coches' */
  wait_sem(mutex);

  consultar_var(num_coches_handle, &valor);
  modificar_var(num_coches_handle, ++valor);
  /* Primer coche que intenta cruzar desde su extremo */
  if (valor == 1)
    wait_sem(puente); /* Espera el acceso al puente */

  signal_sem(mutex);

  cruzar(salida); /* Tardará un tiempo aleatorio */

  wait_sem(mutex);

  consultar_var(num_coches_handle, &valor);
  modificar_var(num_coches_handle, --valor);
  /* Último coche en cruzar desde su extremo */
  if (valor == 0)
    signal_sem(puente); /* Libera el puente */

  signal_sem(mutex);
}

void cruzar (char *salida) {
  if (strcmp(salida, "N") == 0)
    printf("%d cruzando de N a S...\n", getpid());
  else
    printf("%d cruzando de S a N...\n", getpid());
  sleep(rand() % MAX_TIME_CRUZAR + 1);
}
