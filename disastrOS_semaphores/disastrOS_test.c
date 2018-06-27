#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "fixed_size_message_queue.h"
#include "disastrOS.h"
#include "disastrOS_constants.h"


#define CICLES 5


volatile int num_consumers_alive=0;
volatile int num_producers_alive=0;

FixedSizeMessageQueue* queue;

void* produce(){

  ++num_producers_alive;
  printf("INFO, PRODUCER %d START\n", disastrOS_getpid());

  for (int i=0; i<CICLES; ++i) {
    char buf[1024];
    sprintf(buf, "msg from %d, cycle: %d", disastrOS_getpid(),i);
    int length=strlen(buf);
    char* msg=(char*)malloc(length);
    strcpy(msg, buf);

    printf("INFO, PRODUCER  %d sending [%s] \n", disastrOS_getpid(),msg);

    FixedSizeMessageQueue_pushBack(queue, msg);

  }

  printf("INFO, PRODUCER %d END\n", disastrOS_getpid());
  --num_producers_alive;
  return 0;
}

void* consume(){



    ++num_consumers_alive;

    printf("INFO, CONSUMER %d START\n", disastrOS_getpid());
        for (int i=0; CICLES; ++i) {
      printf("INFO, CONSUMER  %d waiting\n", disastrOS_getpid());
      char* msg=FixedSizeMessageQueue_popFront(queue);
      printf("INFO, CONSUMER  %d receiving [%s] \n", disastrOS_getpid(),msg);

  }
  printf("INFO, CONSUMER %d END\n", disastrOS_getpid());
  --num_consumers_alive;
  return 0;

}




// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n", disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}




void childFunction(void* args){
  printf("Hello, I am the child function %d\n", disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(), type, mode);
  printf("fd=%d\n", fd);




  printf("                                    \n\nWait...\n\n\n");
  disastrOS_sleep(15);
  printf("      THE 3rd PROCESS PRODUCES THE RESOURCE, THE 4th CONSUMES IT\n");


  if (disastrOS_getpid() == 0) {

      produce();
  }
  if (disastrOS_getpid() == 1) {

      produce();
  }
  if (disastrOS_getpid() == 2) {

      produce();
  }
  if (disastrOS_getpid() == 3) {

      produce();
  }
  if (disastrOS_getpid() == 4) {

      produce();
  }
  if (disastrOS_getpid() == 5) {

      produce();
  }


  if (disastrOS_getpid() == 6) {

      consume();
  }
  if (disastrOS_getpid() == 7) {

      consume();
  }
  if (disastrOS_getpid() == 8) {

      consume();
  }
  if (disastrOS_getpid() == 9) {

      consume();
  }
  if (disastrOS_getpid() == 10) {

      consume();
  }




  printf("PID: %d, terminating\n", disastrOS_getpid());
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;

  for (int i=0; i<11; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);


    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
   // disastrOS_printStatus();
    //printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	  // pid, retval, alive_children);
    --alive_children;
  }
  disastrOS_printStatus();
  printf("shutdown!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){

  FixedSizeMessageQueue queue;
  int queue_size=5;
  FixedSizeMessageQueue_init(&queue, queue_size);


  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}

