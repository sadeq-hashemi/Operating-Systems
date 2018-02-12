#include "cs537.h"
#include "server_impl.h"
#include "pthread.h"
#include "unistd.h"
#include "request.h"
//
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//
int threads;
pthread_t *thread_list;
pthread_mutex_t mutex; 
pthread_cond_t full;
pthread_cond_t empty;

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
void init_buff(int maxSize){
  buff = malloc(sizeof(buff_t)); 
  if (buff == NULL) {
    fprintf(stderr, "Buff failed to malloc\n");
    exit(1);
  }
  buff->maxSize = maxSize;
  buff->size = 0;
  buff->head = NULL;
  buff->tail = NULL;
   
}
void free_all() {
  free(buff);
}
void init_thread() {
  thread_list = malloc(threads*sizeof(buff_t)); 
  if ( thread_list == NULL ) {
    fprintf(stderr, "Failed at allocating threads\n");
    exit(1);
  }
}
void init_mutex(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
  int ret; 
  ret = pthread_mutex_init(mutex, attr);
  if ( ret != 0 ) {
    fprintf(stderr, "Failed at initializing mutex\n");
    exit(1);
  }
}

void init_cond(pthread_cond_t *cond, const pthread_condattr_t *attr) {
  int ret; 
  ret = pthread_cond_init(cond, attr);
  if ( ret != 0 ) {
    fprintf(stderr, "Failed at initializing condition\n");
    exit(1);
  }
}

void wait_cond(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  int ret; 
  ret = pthread_cond_wait(cond, mutex);
  if ( ret != 0 ) {
    fprintf(stderr, "Failed at waiting for condition\n");
    exit(1);
  }
}

void signal_cond(pthread_cond_t *cond) {
  int ret; 
  ret = pthread_cond_signal(cond);
  if ( ret != 0 ) {
    fprintf(stderr, "Failed at signalling for condition\n");
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
/*
  if ( buff->size + 1 > buff->maxSize ) {
    return -1; 
  }
*/
  myreq->connfd = connfd;
  myreq->prev = NULL;

  if(buff->size == 0) {
  myreq->next = NULL; 
  buff->head = myreq; 
  buff->tail = myreq; 
  buff->size = buff->size + 1; 
  } else {

  myreq->next = buff->head;
  (buff->head)->prev = myreq;
  buff->head = myreq; 
  buff->size = buff->size + 1; 
  }
  return 0;
}

request_t* rem_request(request_t* request) {
  if(buff == NULL) {
    fprintf(stderr, "illegal remove\n");
    exit(1);
  }
  if ( buff->size < 0 ) {
    fprintf(stderr, "Buff length is less that Zero\n");
    exit(1);
  }
  request = buff->tail;
  buff->tail = (buff->tail)->prev;

  buff->size = buff->size - 1; 

  if(buff->size!= 0)
    (buff->tail)->next = NULL;
  if(buff->size == 0) 
    buff->head = NULL;  
  return request;
}

void *consumer_thread() {
  request_t* myreq;
  myreq = NULL;
  while(1) {

    pthread_mutex_lock(&mutex);
      while(buff->size == 0) {
//printf("asleep\n");
        wait_cond(&full, &mutex);
      }

//printf("awake\n");
    myreq = rem_request(myreq);

/* //TEST
request_t* p= buff->head;
int c = 0;
printf("consumer: \n");
while(p != NULL) {
printf("ind:%d connfd:%d size:%d\n   head:%d tail:%d\n", c, p->connfd, buff->size, buff->head->connfd, buff->tail->connfd);
c++;
p = p->next;
}
printf("done\n");
*/
   if(myreq == NULL) {
      fprintf(stderr, "Error in removing from struct: returned null\n");
      exit(1);
    }
    signal_cond(&empty);            
    pthread_mutex_unlock(&mutex);
    requestHandle(myreq->connfd);
    free(myreq);
  }
   
    return NULL;
}


void server_init(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <port> <thread> <buffer>\n", argv[0]);
        exit(1);
    }
    init_buff(atoi(argv[3]));
    threads = atoi(argv[2]);
    if ( threads <= 0 ) {
      fprintf(stderr, "Invalid number of threads\n");
      exit(1);
    }
    if ( buff->maxSize <= 0 ) {
      fprintf(stderr, "Invlid number of buffers\n");
      exit(1);
    }

    //INITIALIZE LOCKS
    init_mutex(&mutex, NULL);
    //CREATING CONDITION
    init_cond(&full, NULL);
    init_cond(&empty,NULL);
    //CREATING ARRAY OF THREADS

    thread_list= malloc(threads*sizeof(pthread_t));
    if ( thread_list == NULL ) {
      printf("Failed at allocating memory for thread_list\n");
      exit(1);
    }
    for (int x = 0; x < threads; x++) {
      create_thread( &thread_list[0], NULL, consumer_thread, NULL);
    }
}

void server_dispatch(int connfd) {
    //while (1) {
        request_t *newreq;
        newreq = (request_t*) malloc(sizeof(request_t));

          if ( newreq == NULL ) {
            fprintf(stderr, "Malloc in while loop failed \n");
            exit(1); 
          }

         pthread_mutex_lock(&mutex);
/* //TEST
printf("before adding: ");
request_t* p= buff->head;
int c = 0;
while(p != NULL) {
printf("ind:%d connfd:%d size:%d\n   head:%d tail:%d\n", c, p->connfd, buff->size, buff->head->connfd, buff->tail->connfd);
c++;
p = p->next;
}*/
          while (buff->size == buff->maxSize) {
             wait_cond(&empty, &mutex);
           }
           int ret = add_request(buff, newreq, connfd);
           signal_cond(&full);
           pthread_mutex_unlock(&mutex);

   // }
}






