#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_constants.h"
#include "disastrOS_globals.h"
#include "linked_list.h"


void internal_semWait(){

  //we take from syscall_args the semaphore
	int sem_fd = running->syscall_args[0];
  // we take sem_des from the SemDescriptor list
	SemDescriptor* sem_des = SemDescriptorList_byFd(&running->sem_descriptors, sem_fd);
  // return error if sem_des isn't found
	if(!sem_des){
		running->syscall_retvalue=DSOS_ESEMAPHORENOFD;
		return;
	}

  //we take the pointer from the previous SemDescriptor
	SemDescriptorPtr* descptr = sem_des->ptr;
	if(!descptr){
		running->syscall_retvalue = DSOS_ESEMAPHOREDESC;
		return;
	}

  // we save the sempahore associated with the specified semaphore descriptor
	Semaphore* sem = sem_des->semaphore;
  // error handler. it returns error if the semaphore it's not found
	if(!sem){
		running->syscall_retvalue= DSOS_ESEMAPHORENOAVAILABLE;
		return;
	}


	PCB* p;

  // we decrease semcount
	sem->count = sem->count-1;
  // we check the value of semcount after decreasing
	if(sem->count<0){
		List_detach(&sem->descriptors, (ListItem*)descptr);
		//we insert sem_des_ptr in semaphore waiting descriptor list
	        List_insert(&(sem->waiting_descriptors), sem->waiting_descriptors.last, (ListItem*)sem_des->ptr);
		// we insert the running process in the waiting list
		List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
		running->status = Waiting;
		p = (PCB*)List_detach(&ready_list, (ListItem*)ready_list.first);
		running = (PCB*)p;
	}

	running->syscall_retvalue = 0;
  	//if everything is ok we return 0
	return;

}












