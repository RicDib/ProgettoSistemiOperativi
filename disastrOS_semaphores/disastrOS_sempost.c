#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){

    int sem_fd = running->syscall_args[0];
    SemDescritpor* sem_des = SemDescriptorList_byFd(&running->sem_descriptors, sem_fd);

    //checking the presence of sem_des in the list
    if(!sem_des){
		running->syscall_retvalue=DSOS_ESEMAPHORENOFD;
		return;
	}

     //we save in sem the semaphore associated to the relative descriptor
     Sempahore* sem = sem_des->semaphore;

    //checking errors in saving the sem
     if(!sem){
		running->syscall_retvalue= DSOS_ESEMAPHORENOTAVAILABLE;
		return;
	 }

    //incrementation
    sem->count = (sem->count)+1;

    //sem_count after incrementation will take a different value, so we check if it's <= 0
    if (sem_count <= 0){

        //we insert the running process in the ready list
        List_insert(&ready_list, ready_list.last, (ListItem*)running);

        //we save in sem_des_ptr the first element in sem->waiting_descriptors
        SemDescriptorPtr* sem_des_ptr = (SemDescriptorPtr*) List_detach(&(sem->waiting_descriptors), sem->waiting_descriptors.first);



        List_detach(&(sem->waiting_descriptors),(ListItem*)sem_des_ptr);
        List_insert(&running_list, (ListItem*) waiting);

        running->status = Running;
    }
    running->syscall_retvalue = 0;
    //if everything is ok we return 0
	return;

}






















