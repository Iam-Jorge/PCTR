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
void decode_data(mqd_t q_handler_encoded_data, mqd_t q_handler_decoded_data);

/* Auxiliar functions */
void decode_single_character(char *c);

/******************** Main function ********************/

int main(int argc, char *argv[]) {
  mqd_t q_handler_encoded_data, q_handler_decoded_data;
  mode_t mode_read_only = O_RDONLY;
  mode_t mode_write_only = O_WRONLY;
  
  /* Open message queues */
  open_message_queue(MQ_ENCODED_DATA, mode_read_only, &q_handler_encoded_data);
  open_message_queue(MQ_DECODED_DATA, mode_write_only, &q_handler_decoded_data);

  /* Task management */
  decode_data(q_handler_encoded_data, q_handler_decoded_data);

  return EXIT_SUCCESS;
}

/******************** Message queue management ********************/

/* INCLUDE THE REMAINING CODE HERE */

/******************** Task management ********************/

/* INCLUDE THE REMAINING CODE HERE */

/******************** Auxiliar functions ********************/

/* INCLUDE THE REMAINING CODE HERE */
