#include "cs537.h"
#include "server_impl.h"
#include "pthread.h"
#include "unistd.h"
//
// server.c: A very, very simple web server
//<F9><F10>
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//
sem_t *lock;
sem_t *fill;
sem_t *empty;

typedef struct request_t{
  int connfd;
  struct request_t* next;
  struct request_t* prev;
} request_t;

typedef struct{
  request_t* head;
  request_t* tail;
  int size;
  int maxSize;
} buff_t;

buff_t *buff;

void init_sem(sem_t *sem, int pshared, unsigned int value) {
  int ret;
  ret = sem_init(sem, pshared, value); 
  
  if(ret != 0) {
    fprintf(stderr, "Failed at initializing semaphore\n");
    exit(1);
  }
}

void wait_sem(sem_t *sem) {
  int ret;
  
  ret = sem_wait(sem);

  if(ret != 0) {
      fprintf(stderr, "Failed at waiting for semaphore\n");
      exit(1);
  }
}

void post_sem(sem_t *sem) {
  int ret;

  ret = sem_post(sem);

  if(ret != 0) {
      fprintf(stderr, "Failed at posting for semaphore\n");
      exit(1);
  }
}

void getargs(int *port, buff_t* buff, int *threads, int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <port> <thread> <buffer>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    buff->size = atoi(argv[2]);
    *threads = atoi(argv[3]);
    if ( buff->size == 0 ) {
      fprintf(stderr, "Invlid number of buffers\n");
      exit(1);
    }
    if ( *threads == 0 ) {
      fprintf(stderr, "Invalid number of threads\n");
      exit(1);
    }
}

void create_thread(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *argi) {
  int ret; 
  ret = pthread_create( thread, attr, start_routine, argi);
  if ( ret != 0 ) {
    fprintf(stderr, "Failed at creating thread\n");
    exit(1);
  }
}

int  add_request(buff_t* buff, request_t *myreq, int connfd) {
  if(buff == NULL || connfd < 0) {
    fprintf(stderr, "illegal adding\n");
  }
  if ( buff->size + 1 >= buff->maxSize ) {
    return -1; 
  }
  /*while(!pthread_mutex_trylock(mutex)) {
    sleep(1);
  }*/
  myreq->connfd = connfd;
  myreq->prev = NULL;

  //pthread_mutex_lock(mutex);
  myreq->next = buff->head;
  buff->head = myreq; 
  buff->size = buff->size + 1;
  //pthread_mutex_unlock(mutex);
  return 0;
}

void rem_request(buff_t* buff, request_t* request) {
  if(buff == NULL) {
    fprintf(stderr, "illegal remove\n");
    exit(1);
  }
  if ( buff->size - 1  < 0 ) {
    fprintf(stderr, "Buff length is less that Zero\n");
    exit(1);
  }
  /*while(!pthread_mutex_trylock(mutex)) {
    sleep(1);
  }*/

  //pthread_mutex_lock(mutex);
  request = buff->tail;
  buff->tail = (buff->tail)->prev;
  (buff->tail)->next = NULL;
  buff->size = buff->size - 1; 
  //pthread_mutex_unlock(mutex);
}

void *consumer_thread() {
  request_t* myreq;
  myreq = NULL;
  while(1) {
    wait_sem(lock);
      while(buff->size == 0) {
        wait_sem(fill);
      }
    rem_request(buff, myreq);
    if(myreq == NULL) {
      fprintf(stderr, "Error in removing from struct: returned null\n");
      exit(1);
    }
    post_sem(empty);
    post_sem(lock);
    server_dispatch(myreq->connfd);
    free(myreq);
    return NULL;
  }   
}

int main(int argc, char *argv[]) {
    int listenfd, connfd, port, threads, clientlen;
    struct sockaddr_in clientaddr;

    //INITIALIZE SEMAPHORES
    init_sem(lock, 0, 1);
    init_sem(fill, 0, 0);
    init_sem(empty, 0, 0);

    //GETS AND READS SERVER ARGUMENTS
    getargs(&port, buff, &threads, argc, argv);
    server_init(argc, argv);
    listenfd = Open_listenfd(port);

    //CREATING ARRAY OF THREADS
    pthread_t *thread_list[threads]; 
    for (int x = 0; x < threads; x++) {
      create_thread( thread_list[0], NULL, consumer_thread, NULL);
    }


    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        request_t *newreq;
        newreq = (request_t*) malloc(sizeof(request_t));
          if ( newreq == NULL ) {
            fprintf(stderr, "Malloc in while loop failed \n");
            exit(1); 
          }
         wait_sem(lock);
           while (buff->size == buff->maxSize)
             wait_sem(empty);
           add_request(buff, newreq, connfd);
           post_sem(fill);
         post_sem(lock);
    }
}






