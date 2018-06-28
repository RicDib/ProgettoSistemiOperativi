#include <stdlib.h>
#include <stdio.h>

#include "fixed_size_message_queue.h"
#include "disastrOS.h"
#include "disastrOS_constants.h"

void FixedSizeMessageQueue_init(FixedSizeMessageQueue* q,
				int size_max){


  printf("qwertyu\n");

  q->messages = (char**)malloc(size_max*sizeof(char*));

  printf("poiuyt\n");
  q->size=0;
  q->front_idx=0;
  q->size_max=size_max;

  q->sem_full = disastrOS_semOpen(1, 0);
  q->sem_empty = disastrOS_semOpen(2,q->size_max);
  q->sem_prod = disastrOS_semOpen(3,1);
  q->sem_cons = disastrOS_semOpen(4,1);

}

void FixedSizeMessageQueue_pushBack(FixedSizeMessageQueue*q,
				    char* message){

  disastrOS_semWait(q->sem_empty);
  disastrOS_semWait(q->sem_prod);


  int tail_idx=(q->front_idx+q->size)%q->size_max;
  q->messages[tail_idx]=message;
  ++q->size;

  disastrOS_semPost(q->sem_prod);
  disastrOS_semPost(q->sem_full);
}

char* FixedSizeMessageQueue_popFront(FixedSizeMessageQueue*q){
  char* message_out=0;

  disastrOS_semWait(q->sem_full);
  disastrOS_semWait(q->sem_cons);


  message_out=q->messages[q->front_idx];
  q->front_idx=(q->front_idx+1)%q->size_max;
  --q->size;

  disastrOS_semPost(q->sem_cons);
  disastrOS_semPost(q->sem_empty);

  return message_out;
}

int FixedSizeMessageQueue_sizeMax(FixedSizeMessageQueue* q) {
  return q->size_max;
}

int FixedSizeMessageQueue_size(FixedSizeMessageQueue* q){
  return q->size; // critical section??
}

void FixedSizeMessageQueue_destroy(FixedSizeMessageQueue* q){
  free(q->messages);
  q->size=0;
  q->front_idx=0;
  q->size_max=0;

  disastrOS_semClose(q->sem_full);
  disastrOS_semClose(q->sem_empty);
  disastrOS_semClose(q->sem_cons);
  disastrOS_semClose(q->sem_prod);

}
