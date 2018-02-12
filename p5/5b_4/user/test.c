#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void
test_failed()
{       
        printf(1, "TEST FAILED\n");
        exit();
}

void
test_passed()
{
 printf(1, "TEST PASSED\n");
 exit();
}

int
main(int argc, char *argv[])
{
  int fd;
  struct stat st;

  char *filename = "test_file.txt";
  if((fd = open(filename, O_CREATE | O_SMART | O_WRONLY)) < 0){
    printf(1, "Failed to create a smart file\n");
    test_failed();
  }

  char wbuf[BSIZE] = {0};
  memmove(wbuf + 1, "MAGIC_BEGIN", 11);
  memmove(wbuf + BSIZE - 9, "MAGIC_END", 9);
  for (int i = 0; i < NDIRECT + 1; i++) {
    wbuf[0] = i;
    if (write(fd, wbuf, BSIZE) < BSIZE) {
      printf(1, "Write to smart file block #%d failed.\n", i);
      test_failed();
    }
  }

  if(fstat(fd, &st) < 0){
    close(fd);
    printf(1, "Failed to get stat on the smart file\n");
    test_failed();
  }
  close(fd);

  char rbuf[BSIZE];
  for (int i = 0; i < NDIRECT + 1; i++) {
    // printf(1, "#%d[%x]\n", i, st.addrs[i]);
    block(rbuf, st.dev, st.addrs[i]);
    if (rbuf[0] != i) {

      printf(1, "Block #%d data not written correctly.\n", i);
      test_failed();
    }
      printf(1, "rbuf: %d == %d\n", rbuf[0],i );
}
                                                                     
  block(rbuf, st.dev, ((uint*)rbuf)[1]);
  if (rbuf[0] != NDIRECT + 1) {

      printf(1, "rbuf: %d == %d\n", rbuf[0], NDIRECT + 1);
      printf(1, "Block #%d data not written correctly.\n", NDIRECT+1);
      test_failed();
  }

  if((fd = open(filename, O_RDONLY)) < 0){
    printf(1, "Failed to reopen a smart file\n");
    test_failed();
  }

  for (int i = 0; i < NDIRECT + 2; i++) {
    if (read(fd, rbuf, BSIZE) < BSIZE) {
      printf(1, "Read from smart file block #%d failed.\n", i);
      test_failed();
    }
    if (rbuf[0] != i) {
      printf(1, "Block #%d data not match\n", i);
      test_failed();
    }
  }

  test_passed();
}
