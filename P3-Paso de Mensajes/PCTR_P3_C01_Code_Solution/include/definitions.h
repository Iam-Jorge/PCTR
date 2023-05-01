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

#define MQ_RAW_DATA       "/mq_raw_data"
#define MQ_PROCESSED_DATA "/mq_processed_data"
#define MQ_ENCODED_DATA   "/mq_encoded_data"
#define MQ_DECODED_DATA   "/mq_decoded_data"

#define PROCESSOR_CLASS   "PROCESSOR"
#define PROCESSOR_PATH    "./exec/processor"
#define DECODER_CLASS     "DECODER"
#define DECODER_PATH      "./exec/decoder"

#define MAX_ARRAY_SIZE 1024
#define NUM_DECODERS 1
#define SEPARATOR "."
#define TRUE 1
#define FALSE 0

/* Used in MQ_RAW_DATA and MQ_PROCESSED_DATA */
struct MsgProcessor_t {
  char data[MAX_ARRAY_SIZE]; /* Data of the subvector to be processed */
  int  index_start;          /* Start subvector index */
  int  n_elements;           /* Number of elements in the subvector */ 
  int  key;                  /* Key to carry out the 'processing' */
};

/* Used in MQ_ENCODED_DATA and MQ_DECODED_DATA */
struct MsgDecoder_t {
  char data[MAX_ARRAY_SIZE]; /* Full vector to be decoded */
  int  n_elements;           /* Number of elements to be decoded */
};

enum ProcessClass_t {PROCESSOR, DECODER}; 

struct TProcess_t {          
  enum ProcessClass_t class; /* PROCESSOR or DECODER */
  pid_t pid;                 /* Process ID */
  char *str_process_class;   /* String representation of the process class */
};
