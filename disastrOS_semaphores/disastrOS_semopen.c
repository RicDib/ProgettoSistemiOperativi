#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"

void internal_semOpen(){

// step 1 - get the id of semaphore (that must be opened) from the pcb

int sem_id = running->syscall_args[0];
int count = running->syscall_args[1];

// step 2 - check if the semaphore required has already been opened

Semaphore* sem = SemaphoreList_byId(&semaphores_list, sem_id);

if(!sem){
    sem = Semaphore_alloc(sem_id, count);
    //inserting sem to list
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem);
}

// step 3 - create the descriptor for the resource in this process, and add it to the process descriptor list. Assign to the resource a new fd

SemDescriptor* desc = SemDescriptor_alloc(running->last_sem_fd, sem, running);
if (!desc){
	running->syscall_retvalue = DSOS_ESEMAPHORENOFD;
	return;
}

List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) desc);
//increment the last sem_fd
running->last_sem_fd++;

// step 4 - add desc to the list SemDescriptorPtr

SemDescriptorPtr* desc_ptr = SemDescriptorPtr_alloc(desc);

//error handler
if(!desc_ptr){
    running->syscall_retvalue = DSOS_ESEMAPHOREDESC;
    return;
}

desc->ptr = desc_ptr;
List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) desc_ptr);

// return the semaphore descriptor to the process

running->syscall_retvalue = desc->fd;
return;
}
