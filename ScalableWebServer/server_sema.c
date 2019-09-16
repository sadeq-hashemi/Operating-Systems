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
pthread_t *thread_list;
sem_t lock;
sem_t fill;
sem_t empty;
int threads;

typedef struct request_t {
  int connfd;
  struct request_t* next;
  struct request_t* prev;
} request_t;

typedef struct {
  request_t* head;
  request_t* tail;
  int size;
  int maxSize;
} buff_t;

buff_t *buff;

void init_buff(int maxSize) {
  buff = malloc(sizeof(buff_t));
  if (buff == NULL) {
    fprintf(stderr, "Malloc failed for buff");
    exit(1);
  }
  buff->head = NULL;
  buff->tail = NULL;
  buff->size = 0;
  buff->maxSize = maxSize;
}

void free_buff() {
  free(buff);
}

void init_thread() {
  thread_list = malloc(threads*sizeof(buff_t));
  if (thread_list == NULL) {
    fprintf(stderr, "Failed to initialize threads");
    exit(1);
  }
}

void init_sem(sem_t *sem, int pshared, unsigned int value) {
  int ret;
  ret = sem_init(sem, pshared, value);
  if (ret != 0) {
    fprintf(stderr, "Failed at initializing semaphore\n");
    exit(1);
  }
}

void wait_sem(sem_t *sem) {
  int ret;

  ret = sem_wait(sem);

  if (ret != 0) {
      fprintf(stderr, "Failed at waiting for semaphore\n");
      exit(1);
  }
}

void post_sem(sem_t *sem) {
  int ret;

  ret = sem_post(sem);

  if (ret != 0) {
      fprintf(stderr, "Failed at posting for semaphore\n");
      exit(1);
  }
}

void create_thread(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *argi) {
  int ret;
  ret = pthread_create(thread, attr, start_routine, argi);
  if ( ret != 0 ) {
    fprintf(stderr, "Failed at creating thread\n");
    exit(1);
  }
}

int  add_request(buff_t* buff, request_t *myreq, int connfd) {
  if (buff == NULL || connfd < 0) {
    fprintf(stderr, "illegal adding\n");
  }
  myreq->connfd = connfd;
  myreq->prev = NULL;

  if (buff->size ==0) {
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
  if (buff == NULL) {
    fprintf(stderr, "illegal remove\n");
    exit(1);
  }
  if ( buff->size  < 0 ) {
    fprintf(stderr, "Buff length is less that Zero\n");
    exit(1);
  }

  request = buff->tail;
  buff->tail = (buff->tail)->prev;
  buff->size = buff->size - 1;

  if (buff->size!= 0)
    (buff->tail)->next = NULL;
  if (buff->size == 0)
    buff->head = NULL;
  return request;
}

void *consumer_thread() {
  request_t* myreq;
  myreq = NULL;
  while (1) {
      wait_sem(&fill);
     // while(buff->size == 0) {
          wait_sem(&lock);
     // }
    myreq =  rem_request(myreq);
    if (myreq == NULL) {
      fprintf(stderr, "Error in removing from struct: returned null\n");
      exit(1);
    }
    post_sem(&lock);
    post_sem(&empty);
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
    if (buff->maxSize <= 0) {
      fprintf(stderr, "Invalid buff\n");
      exit(1);
    }
     // INITIALIZE SEMAPHORES
    init_sem(&lock, 0, 1);
    init_sem(&fill, 0, 0);
    init_sem(&empty, 0, buff->maxSize);
    // CREATING ARRAY OF THREADS
    init_thread();
    for (int x = 0; x < threads; x++) {
      create_thread(&thread_list[0], NULL, consumer_thread, NULL);
    }
}

void server_dispatch(int connfd) {
        request_t *newreq;
        newreq = (request_t*) malloc(sizeof(request_t));
          if ( newreq == NULL ) {
            fprintf(stderr, "Malloc in while loop failed \n");
            exit(1);
          }
           wait_sem(&empty);
          // while (buff->size == buff->maxSize) {
               wait_sem(&lock);
          // }
          add_request(buff, newreq, connfd);
          post_sem(&lock);
          post_sem(&fill);
}






