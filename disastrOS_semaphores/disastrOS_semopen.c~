#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  
// step 1 - get the id of semaphore (that must be opened) from the pcb 

int sem_id = running->syscall_args[0];
int oflag = running->syscall_args[1];
int count = running->syscall_args[2];

printf("Process with pid %d require the opening of semaphore %d \n", running->pid, sem_id);


// step 2 - check if the semaphore required has already been opened

Semaphore* sem = SemaphoreList_byId(&semaphores_list, sem_id);

if (oflag & DSOS_CREATE){
	if (sem){
		running->syscall_retvalue = DSOS_ESEMAPHORECREATE;
		return;
	}

printf("I'm allocating semaphore with id %d because it doesn't exist", sem_id); 

sem = Semaphore_alloc(sem_id, count);

//inserting sem to list

List_insert(&semaphores_list, semaphores_list.last, &(sem->list));

}

// step 3 - error handler

if (!sem){
	running->syscall_retvalue = DSOS_ESEMAPHOREOPEN;
	return;
}

// step 4 - when there's an exclusive opening but a resource retains the semafor return an error

if ((oflag & DSOS_EXCL) && sem->descriptors.size){
	running->syscall_retvalue = DSOS_ESEMAPHORENOEXCL;
	return;
}


// step 5 - create the descriptor for the resource in this process, and add it to the process descriptor list. Assign to the resource a new fd

SemDescriptor* desc = SemDescriptor_alloc(running->last_sem_fd, sem, running);
if (!desc){
	running->syscall_retvalue = DSOS_ERESOURCENOFD;
	return;
}
//increment the last sem_fd 
running->last_sem_fd++;
List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) desc);

// step 6 - add desc to the list SemDescriptorPtr

SemDescriptorPtr* desc_ptr = SemDescriptorPtr_alloc(desc);
desc->ptr = desc_ptr;
List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) desc_ptr);

// return the semaphore descriptor to the process 

running->syscall_retvalue = desc->fd;


}
























