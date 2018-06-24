#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){

    int sem_fd = running->syscall_args[0];
    SemDescriptor* sem_des = SemDescriptorList_byFd(&running->sem_descriptors, sem_fd);

    //checking the presence of sem_des in the list
    if(!sem_des){
		running->syscall_retvalue=DSOS_ESEMAPHORENOFD;
		return;
	}

     //we save in sem the semaphore associated to the relative descriptor
     Semaphore* sem = sem_des->semaphore;

    //checking errors in saving the sem
     if(!sem){
		running->syscall_retvalue= DSOS_ESEMAPHORENOAVAILABLE;
		return;
	 }

    //incrementation
    sem->count = (sem->count)+1;

    //sem_count after incrementation will take a different value, so we check if it's <= 0
    if (sem->count <= 0){
	
	//we save in sem_des_ptr the first process in sem->waiting_descriptors
        SemDescriptorPtr* sem_des_ptr = (SemDescriptorPtr*) List_detach(&(sem->waiting_descriptors),(ListItem*) sem->waiting_descriptors.first);

	//removing process' pcb from waiting list
        List_detach(&waiting_list, (ListItem*)sem_des_ptr->descriptor->pcb);

        //we insert this process in the ready list
        List_insert(&ready_list, ready_list.last, (ListItem*)sem_des_ptr->descriptor->pcb);
        
	List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_des_ptr);
        

        sem_des_ptr->descriptor->pcb->status = Ready;


    }
    running->syscall_retvalue = 0;
    //if everything is ok we return 0
	return;

}
