#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
typedef struct {
  int valid;
  int pid;
  char birth[25];
//  struct timeval start_time;
  int start_sec;
  double start_msec;
  int elapsed_sec;
  double elapsed_msec;
} stats_t; 
int
main(int argc, char* argv[]) {

char birth[25]; 

    time_t rawtime; 
    struct tm * timeinfo; 
    struct tm   timeinfoBuffer; 
    char *result; 
     
    time(&rawtime); 
    /* call localtime */ 
    timeinfo = localtime_r(&rawtime , &timeinfoBuffer); 
    /* allocate memory for the result of asctime call*/ 
    result = malloc(26 * sizeof(char)); 
    /* call reentrant asctime function */ 
    result = asctime_r(timeinfo, result); 
 
 (void) strcpy((char*)&birth, result);  
printf("%s\n", birth);


/*   clock_t after_t, begin_t; 
   time_t after, begin; 
begin = time(NULL);
begin_t = clock(); 
sleep(3.03); 
  after = time(NULL); 
  after_t = clock(); 

double diff = difftime(after, begin); 
double diff_t = (after_t - begin_t)/CLOCKS_PER_SEC;

printf("%f\n", diff); 
printf("%f\n", diff_t); */
void * stupid; 
struct timeval start; 
struct timeval end; 

(void) gettimeofday(&start, stupid); 
printf("%f\n",(double) start.tv_usec);

int start_sec = (int) start.tv_sec; 
double start_usec = (double) start.tv_usec;

printf("sec: %d msec: %f size is %d\n", start_sec, start_usec, (int)sizeof(stats_t)); 
sleep(5); 
(void) gettimeofday(&end, stupid); 
int end_sec = (int) end.tv_sec; 
double  end_usec = (double) end.tv_usec;
printf("sec: %d msec: %f size is %d\n", end_sec, end_usec, (int)sizeof(stats_t)); 
return 0; 
}
