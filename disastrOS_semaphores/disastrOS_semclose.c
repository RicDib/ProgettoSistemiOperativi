#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){

    //step 1 - retrieve the fd of the sem to close

    int sem_fd = running->syscall_args[0];

    SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors, sem_fd);


    //step 2 - an error occur if the sem is not in the process

    if(!semDesc){
        printf("Semaphore not in process\n");
        running->syscall_retvalue = DSOS_ESEMAPHORECLOSE;
        return;
    }

    //step 3 - the process list's descriptor is removed

    semDesc = (SemDescriptor*)List_detach(&running->sem_descriptors, &(semDesc->list));
    assert(semDesc);

    Semaphore* sem = semDesc->semaphore;

    //the resource list's descriptor pointer is removed
    SemDescriptorPtr* semDescPtr = (SemDescriptorPtr*)List_detach(&(sem->descriptors), (ListItem*)(semDesc->ptr));
    assert(semDescPtr);

    if(sem->descriptors.size == 0){
        printf("Removing sem %d\n", sem->id);
        List_detach(&semaphores_list, (ListItem*)sem);
    }

    SemDescriptor_free(semDesc);
    SemDescriptorPtr_free(semDescPtr);
    running->syscall_retvalue = 0;
}
