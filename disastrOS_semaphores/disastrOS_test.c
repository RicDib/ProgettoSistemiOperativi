#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include "disastrOS.h"
#include "disastrOS_constants.h"
#include "fixed_size_message_queue.h"

#define CICLES 5

FixedSizeMessageQueue* mq;


void produce(FixedSizeMessageQueue* q){
    for (int i = 0; i < CICLES; i++){



        char buf[1024];
        int length=strlen(buf);
        char* msg=(char*)malloc(length);
        strcpy(msg, buf);
        FixedSizeMessageQueue_pushBack(q, msg);
    }
}

void consume(FixedSizeMessageQueue* q){
    for (int i = 0; i < CICLES; i++){

        FixedSizeMessageQueue_popFront(q);

    }
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




  if (disastrOS_getpid() <10) {
    produce(mq);
  }

  if (disastrOS_getpid() >10 && disastrOS_getpid() < 20){
    consume(mq);
  }


  printf("PID: %d, terminating\n", disastrOS_getpid());



  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  printf("I feel like to spawn  nice threads\n");
  int alive_children=0;


  int queue_size=10;

  printf("dfgq\n");


  FixedSizeMessageQueue_init(mq, queue_size);

  printf("dfgq\n");

  for (int i=0; i<20; ++i) {
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
