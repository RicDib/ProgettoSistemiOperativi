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

    /*
     * if(!sem_des){
		running->syscall_retvalue=DSOS_ESEMAPHORENOFD;
		return;
	}
     */

     Sempahore* sem = sem_des->semaphore;

     /*
      * if(!sem){
		running->syscall_retvalue= DSOS_ESEMAPHORENOTAVAILABLE;
		return;
	}*/

    sem->count = (sem->count)+1;

    SemDescriptorPtr* sem_des_ptr = SemDescriptorPtr_alloc(sem_des);

    if (sem_count <= 0){

        List_detach(&(sem->waiting_descriptors),(ListItem*)sem_des_ptr);
        List_insert(&running_list, (ListItem*) waiting);

        running->status = Running;
    }
    running->syscall_retvalue = 0;
    //if everything is ok we return 0
	return;

}






















