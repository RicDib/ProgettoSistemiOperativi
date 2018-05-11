#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "linked_list.h"

void internal_semWait(){
  // step 1 - we take from syscall_args the semaphore 
	int sem_fd = running->syscall_args[0];
  // we take sem_des from the SemDescriptor list
	SemDescriptor* sem_des = SemDescriptorList_byFd(&running->sem_descriptors, sem_fd);
  // return error if sem_des isn't found
	if(!sem_des){
		running->syscall_retvalue=DSOS_ESEMAPHORENOFD;
		return;
	}
  // we save the sempahore associated with the specified semaphore descriptor 
	Sempahore* sem = sem_des->semaphore;
  // error handler. it returns error if the semaphore it's not found
	if(!sem){
		running->syscall_retvalue= DSOS_ESEMAPHORENOTAVAILABLE;
		return;
	}
	
  // we decrease semcount
	sem->count = sem->count-1;
  // we check the value of semcount after decreasing
	if(sem->count<0){
	// we insert the running process in the waiting list
		List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
		running->status = Waiting;
	}

	running->syscall_retvalue = 0; 
  //if everything is ok we return 0
	return;

}










	
}
