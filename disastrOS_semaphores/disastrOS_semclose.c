#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_constants.h"

void internal_semClose(){

    //step 1 - retrieve the fd of the sem to close

    int sem_fd = running->syscall_args[0];

    SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors, sem_fd);


    //step 2 - an error occur if the sem is not in the process

    if(!sem_desc){
        //printf("Semaphore not in process\n");
        running->syscall_retvalue = DSOS_ESEMAPHORECLOSE;
        return;
    }

    //step 3 - the semaphore descriptor is removed from the running process list

    List_detach(&running->sem_descriptors, (ListItem*)sem_desc);

    Semaphore* sem = sem_desc->semaphore;
    if(!sem){
	running->return_value=DSOS_ESEMAPHORENOAVAILABLE;
        return;
    }

    //the pointer to the semaphore descriptor is removed and the error is handled
    SemDescriptorPtr* semDescPtr = (SemDescriptorPtr*)List_detach(&(sem->descriptors), (ListItem*)(sem_desc->ptr));
    assert(semDescPtr);

    if(sem->descriptors.size == 0 && sem->waiting_descriptors.size==0){
        //printf("Removing sem %d\n", sem->id);
        List_detach(&semaphores_list, (ListItem*)sem);
	Semaphore_free(sem);
    }

    SemDescriptor_free(sem_desc);
    SemDescriptorPtr_free(semDescPtr);
    running->syscall_retvalue = 0;
    return;
}
