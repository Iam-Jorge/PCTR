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
#define _BSD_SOURCE

#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <definitions.h>

/* Total number of processes */
int g_nProcesses;
/* 'Process table' (child processes) */
struct TProcess_t *g_process_table;

/* Process management */
void create_processes_by_class(enum ProcessClass_t class, int n_processes, int index_process_table);
pid_t create_single_process(const char *class, const char *path, const char *argv);
void get_str_process_info(enum ProcessClass_t class, char **path, char **str_process_class);
void init_process_table(int n_processors, int n_decoders);
void terminate_processes();
void wait_processes();

/* Message queue management */
void create_message_queue(const char *mq_name, mode_t mode, long mq_maxmsg, long mq_msgsize,
			  mqd_t *q_handler);
void close_message_queues(mqd_t q_handler_raw_data, mqd_t q_handler_processed_data,
			  mqd_t q_handler_encoded_data, mqd_t q_handler_decoded_data);

/* Task management */
void send_raw_data(int key, int n_subvectors, struct MsgProcessor_t *msg_task,
		   struct MsgDecoder_t *msg_result, mqd_t q_handler_raw_data);
void receive_encoded_data(int n_subvectors, struct MsgProcessor_t *msg_task,
			  struct MsgDecoder_t *msg_result, mqd_t q_handler_processed_data);
void decode(struct MsgDecoder_t *msg_result, mqd_t q_handler_encoded_data,
	    mqd_t q_handler_decoded_data);

/* Auxiliar functions */
void free_resources();
void generate_message_with_input_data(struct MsgDecoder_t *msg_result, char *encoded_input_data);
void install_signal_handler();
void parse_argv(int argc, char *argv[], char **p_encoded_input_data, int *key,
		int *n_processors, int *n_subvectors);
void print_result(struct MsgDecoder_t *msg_result);
void signal_handler(int signo);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  mqd_t q_handler_raw_data, q_handler_processed_data;
  mqd_t q_handler_encoded_data, q_handler_decoded_data;
  mode_t mode_creat_read_only = (O_RDONLY | O_CREAT);
  mode_t mode_creat_write_only = (O_WRONLY | O_CREAT);
  struct MsgProcessor_t msg_task;
  struct MsgDecoder_t msg_result;
  
  char *encoded_input_data;
  int key, n_processors, n_subvectors;

  /* Install signal handler and parse arguments*/
  install_signal_handler();
  parse_argv(argc, argv, &encoded_input_data, &key, &n_processors, &n_subvectors);

  /* Init the process table*/
  init_process_table(n_processors, NUM_DECODERS);

  /* Create message queues */
  create_message_queue(MQ_RAW_DATA, mode_creat_write_only, n_subvectors,
		       sizeof(struct MsgProcessor_t), &q_handler_raw_data);
  create_message_queue(MQ_PROCESSED_DATA, mode_creat_read_only, n_subvectors,
		       sizeof(struct MsgProcessor_t), &q_handler_processed_data);
  create_message_queue(MQ_ENCODED_DATA, mode_creat_write_only, 1,
		       sizeof(struct MsgDecoder_t), &q_handler_encoded_data);
  create_message_queue(MQ_DECODED_DATA, mode_creat_read_only, 1,
		       sizeof(struct MsgDecoder_t), &q_handler_decoded_data);

  /* Create processes */
  create_processes_by_class(PROCESSOR, n_processors, 0);
  create_processes_by_class(DECODER, NUM_DECODERS, n_processors);

  /* Generate a message with the input data */
  generate_message_with_input_data(&msg_result, encoded_input_data);

  /* Manage tasks */
  send_raw_data(key, n_subvectors, &msg_task, &msg_result, q_handler_raw_data);
  receive_encoded_data(n_subvectors, &msg_task, &msg_result, q_handler_processed_data);
  decode(&msg_result, q_handler_encoded_data, q_handler_decoded_data);

  /* Wait for the decoder process */
  wait_processes();

  /* Print the decoded text */
  print_result(&msg_result);

  /* Free resources and terminate */
  close_message_queues(q_handler_raw_data, q_handler_processed_data,
		       q_handler_encoded_data, q_handler_decoded_data);
  terminate_processes();
  free_resources();

  return EXIT_SUCCESS;
}

/******************** Process Management ********************/

void create_processes_by_class(enum ProcessClass_t class, int n_processes, int index_process_table) {
  char *path = NULL, *str_process_class = NULL;
  int i;
  pid_t pid;

  get_str_process_info(class, &path, &str_process_class);

  for (i = index_process_table; i < (index_process_table + n_processes); i++) {
    pid = create_single_process(path, str_process_class, NULL);

    g_process_table[i].class = class;
    g_process_table[i].pid = pid;
    g_process_table[i].str_process_class = str_process_class;
  }

  printf("[MANAGER] %d %s processes created.\n", n_processes, str_process_class);
  sleep(1);
}

pid_t create_single_process(const char *path, const char *class, const char *argv) {
  pid_t pid;

  switch (pid = fork()) {
  case -1 :
    fprintf(stderr, "[MANAGER] Error creating %s process: %s.\n", 
	    class, strerror(errno));
    terminate_processes();
    free_resources();
    exit(EXIT_FAILURE);
    /* Child process */
  case 0 : 
    if (execl(path, class, argv, NULL) == -1) {
      fprintf(stderr, "[MANAGER] Error using execl() in %s process: %s.\n", 
	      class, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  /* Child PID */
  return pid;
}

void get_str_process_info(enum ProcessClass_t class, char **path, char **str_process_class) {
  switch (class) {
  case PROCESSOR:
    *path = PROCESSOR_PATH;
    *str_process_class = PROCESSOR_CLASS;
    break;    
  case DECODER:
    *path = DECODER_PATH;
    *str_process_class = DECODER_CLASS;
    break;
  }
}

void init_process_table(int n_processors, int n_decoders) {
  int i;

  /* Number of processes to be created */
  g_nProcesses = n_processors + n_decoders; 
  /* Allocate memory for the 'process table' */
  g_process_table = malloc(g_nProcesses * sizeof(struct TProcess_t)); 

  /* Init the 'process table' */
  for (i = 0; i < g_nProcesses; i++) {
    g_process_table[i].pid = 0;
  }
}

void terminate_processes() {
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

void wait_processes() {
  int i;
  pid_t pid;

  /* Wait for the termination of the DECODER process */
  pid = wait(NULL); 
  for (i = 0; i < g_nProcesses; i++) {
    if (pid == g_process_table[i].pid) {
      /* Update the 'process table' */
      g_process_table[i].pid = 0;           
      /* Child process found */
      break; 
    }
  }
}

/******************** Message queue management ********************/

void create_message_queue(const char *mq_name, mode_t mode, long mq_maxmsg, long mq_msgsize,
			  mqd_t *q_handler) {
  struct mq_attr attr;

  attr.mq_maxmsg = mq_maxmsg;
  attr.mq_msgsize = mq_msgsize;
  *q_handler = mq_open(mq_name, mode, S_IWUSR | S_IRUSR, &attr);
}

void close_message_queues(mqd_t q_handler_raw_data, mqd_t q_handler_processed_data,
			  mqd_t q_handler_encoded_data, mqd_t q_handler_decoded_data) {
  mq_close(q_handler_raw_data);
  mq_close(q_handler_processed_data);
  mq_close(q_handler_encoded_data);
  mq_close(q_handler_decoded_data);
}

/******************** Task management ********************/

void send_raw_data(int key, int n_subvectors, struct MsgProcessor_t *msg_task,
		   struct MsgDecoder_t *msg_result, mqd_t q_handler_raw_data) {
  int i;
  
  msg_task->key = key;
  /* n_subvectors tasks to be sent */
  for (i = 0; i < n_subvectors; i++) {
    /* Set the subvector indexes */
    msg_task->index_start = i * (msg_result->n_elements / n_subvectors);
    msg_task->n_elements = msg_result->n_elements / n_subvectors;
    /* Last task -> adjust the value of n_elements */
    if (i == n_subvectors - 1) {
      msg_task->n_elements = msg_result->n_elements - msg_task->index_start;
    }
    /* Beware! Copy only the data related to a single subvector */
    memcpy(msg_task->data, &(msg_result->data[msg_task->index_start]),
	   msg_task->n_elements * sizeof(char));
    mq_send(q_handler_raw_data, (const char *)msg_task, sizeof(struct MsgProcessor_t), 0);
  }

  printf("\n----- [MANAGER] Tasks sent ----- \n\n");
}

void receive_encoded_data(int n_subvectors, struct MsgProcessor_t *msg_task,
			  struct MsgDecoder_t *msg_result, mqd_t q_handler_processed_data) {
  int i;

  /* n_subvectors tasks to be received */
  for (i = 0; i < n_subvectors; i++) {
    mq_receive(q_handler_processed_data, (char *)msg_task, sizeof(struct MsgProcessor_t), NULL);
    /* Beware! Copy only the data related to the processed subvector */
    memcpy(&(msg_result->data[msg_task->index_start]), msg_task->data,
	   msg_task->n_elements * sizeof(char));
  }
}

void decode(struct MsgDecoder_t *msg_result, mqd_t q_handler_encoded_data,
	    mqd_t q_handler_decoded_data) {
  /* Rendezvous with the DECODER process */
  mq_send(q_handler_encoded_data, (const char *)msg_result, sizeof(struct MsgDecoder_t), 0);
  mq_receive(q_handler_decoded_data, (char *)msg_result, sizeof(struct MsgDecoder_t), NULL);
}

/******************** Auxiliar functions ********************/

void free_resources() {
  printf("\n----- [MANAGER] Freeing resources ----- \n");

  /* Free the 'process table' memory */
  free(g_process_table); 

  /* Remove message queues */
  mq_unlink(MQ_RAW_DATA);
  mq_unlink(MQ_PROCESSED_DATA);
  mq_unlink(MQ_ENCODED_DATA);
  mq_unlink(MQ_DECODED_DATA);
}

void generate_message_with_input_data(struct MsgDecoder_t *msg_result, char *encoded_input_data) {
  int i = 0;
  char *encoded_character;
  
  msg_result->data[0] = atoi(strtok(encoded_input_data, SEPARATOR));
  while ((encoded_character = strtok(NULL, SEPARATOR)) != NULL) {
    msg_result->data[++i] = atoi(encoded_character);
  }
  msg_result->n_elements = ++i;
}

void install_signal_handler() {
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    fprintf(stderr, "[MANAGER] Error installing signal handler: %s.\n", strerror(errno));    
    exit(EXIT_FAILURE);
  }
}

void parse_argv(int argc, char *argv[], char **p_encoded_input_data, int *key,
		int *n_processors, int *n_subvectors) {
  if (argc != 5) {
    fprintf(stderr, "Synopsis: ./exec/manager <encoded_data> <key> <n_processors> <n_subvectors>.\n");
    exit(EXIT_FAILURE); 
  }
  
  *p_encoded_input_data = argv[1];
  *key = atoi(argv[2]);
  *n_processors = atoi(argv[3]);  
  *n_subvectors = atoi(argv[4]);  
}

void print_result(struct MsgDecoder_t *msg_result) {
  int i;
  
  printf("\n----- [MANAGER] Printing result ----- \n");
  printf("Decoded result: ");
  for (i = 0; i < msg_result->n_elements; i++) {
    putchar(msg_result->data[i]);
  }
  printf("\n");
}

void signal_handler(int signo) {
  printf("\n[MANAGER] Program termination (Ctrl + C).\n");
  terminate_processes();
  free_resources();
  exit(EXIT_SUCCESS);
}
