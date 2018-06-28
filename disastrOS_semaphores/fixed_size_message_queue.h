#pragma once

#include "disastrOS.h"
#include "disastrOS_constants.h"

typedef struct FixedSizeMessageQueue{
  char** messages;
  int size;
  int size_max;
  int front_idx;
  int sem_full;
  int sem_empty;
  int sem_prod;
  int sem_cons;

} FixedSizeMessageQueue;

void FixedSizeMessageQueue_init(FixedSizeMessageQueue* q,
				int size_max);

void FixedSizeMessageQueue_destroy(FixedSizeMessageQueue* q);

void FixedSizeMessageQueue_pushBack(FixedSizeMessageQueue*q,
				    char* message);

char* FixedSizeMessageQueue_popFront(FixedSizeMessageQueue*q);

int FixedSizeMessageQueue_sizeMax(FixedSizeMessageQueue* q);

int FixedSizeMessageQueue_size(FixedSizeMessageQueue* q);
