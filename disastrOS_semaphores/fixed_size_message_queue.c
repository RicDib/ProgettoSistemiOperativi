#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"
#include "fixed_size_message_queue.h"

void FixedSizeMessageQueue_init(FixedSizeMessageQueue* q,
				int size_max){
  q->messages = (char**)malloc(size_max*sizeof(char*));
  q->size=0;
  q->front_idx=0;
  q->size_max=size_max;
  int sem_f = disastrOS_semOpen(1,0);
  int sem_e = disastrOS_semOpen(2,q->size_max);
  q->sem_full = sem_f;
  q->sem_empty = sem_e;
  int semp = disastrOS_semOpen(3,1);
  int semc = disastrOS_semOpen(4,1);
  q->sem_p = semp;
  q->sem_c = semc;
}

void FixedSizeMessageQueue_pushBack(FixedSizeMessageQueue*q,
				    char* message){
  disastrOS_semWait(q->sem_empty);
  disastrOS_semWait(q->sem_p);

  int tail_idx=(q->front_idx+q->size)%q->size_max;
  q->messages[tail_idx]=message;
  ++q->size;

  disastrOS_semPost(q->sem_p);
  disastrOS_semPost(q->sem_full);
}

char* FixedSizeMessageQueue_popFront(FixedSizeMessageQueue* q){
  char* message_out=0;
  disastrOS_semWait(q->sem_full);
  disastrOS_semWait(q->sem_c);

  message_out=q->messages[q->front_idx];
  q->front_idx=(q->front_idx+1)%q->size_max;
  --q->size;

  disastrOS_semPost(q->sem_c);
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
  disastrOS_semClose(q->sem_c);
  disastrOS_semClose(q->sem_p);
}
