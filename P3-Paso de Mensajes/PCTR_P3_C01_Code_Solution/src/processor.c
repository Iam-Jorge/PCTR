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
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <definitions.h>

/* Message queue management */
void open_message_queue(const char *mq_name, mode_t mode, mqd_t *q_handler);

/* Task management */
void process_raw_data(mqd_t q_handler_raw_data, mqd_t q_handler_processed_data);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  mqd_t q_handler_raw_data, q_handler_processed_data;
  mode_t mode_read_only = O_RDONLY;
  mode_t mode_write_only = O_WRONLY;
  
  /* Open message queues */
  open_message_queue(MQ_RAW_DATA, mode_read_only, &q_handler_raw_data);
  open_message_queue(MQ_PROCESSED_DATA, mode_write_only, &q_handler_processed_data);

  /* Task management */
  while (TRUE) {
    process_raw_data(q_handler_raw_data, q_handler_processed_data);
  }

  return EXIT_SUCCESS;
}

/******************** Message queue management ********************/

void open_message_queue(const char *mq_name, mode_t mode, mqd_t *q_handler) {
  *q_handler = mq_open(mq_name, mode);
}

/******************** Task management ********************/

void process_raw_data(mqd_t q_handler_raw_data, mqd_t q_handler_processed_data) {
  int i;
  struct MsgProcessor_t msg_task;
  
  mq_receive(q_handler_raw_data, (char *)&msg_task, sizeof(struct MsgProcessor_t), NULL);
  /* Only process the data related to the subvector received */
  for (i = 0; i < msg_task.n_elements; i++) {
    msg_task.data[i] += msg_task.key;
  }
  mq_send(q_handler_processed_data, (const char *)&msg_task, sizeof(struct MsgProcessor_t), 0);

  printf("[PROCESSOR] %d | Start: %d End: %d\n", getpid(), msg_task.index_start,
	 msg_task.index_start + msg_task.n_elements - 1);
  /* Dont remove; simulates complexity */
  sleep(1);
}
