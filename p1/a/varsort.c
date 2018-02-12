//Author: Sadeq Hashemi Nejad
//Username: sadeq
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>

int records, column;

void
usage(char *prog) {
  fprintf(stderr, "Usage: ./varsort -i inputfile -o outputfile [-c column]\n");
  exit(1);
}

/*
 * read_file: this function starts reading from the first record. 
 * it allocates memory dynamically that holds each record
 * within every element, it further allocates memory for every data array
 * 
 * @param (fd) the file being read
 * @param (records) number of records being read
 * @return (table) an array of all the records including their index number
 * 	, number of data, and data. 
 */ 
rec_dataptr_t*
read_file(int * fd) {
int rc;

//  reads header and stores number of records
rc = read(*fd, &records, sizeof(records));
if (rc != sizeof(unsigned int)) {
  fprintf(stderr, "Error in reading header\n");
  exit(1);
  }

rec_dataptr_t * table = (rec_dataptr_t*)
  malloc(records* sizeof(rec_dataptr_t));  //  allocates space for table

//  READS EACH HEADER and stores data
int index;
int recordLeft = records;
rec_nodata_t r;

while(recordLeft) {
    index = records - recordLeft;

    rc = read(*fd, &r, sizeof(rec_nodata_t));  //  reads one record

    if (rc != sizeof(rec_nodata_t)) {
       fprintf(stderr, "Error in reading fixed data in record #%d \n", index);
       exit(1);
      }

  assert(r.data_ints <= MAX_DATA_INTS);

  table[index].index = r.index;
  table[index].data_ints = r.data_ints;

  //  allocates space for data
  table[index].data_ptr =
  (unsigned int*) malloc( r.data_ints * sizeof(unsigned int));

  //  reads variable data
  rc = read(*fd, table[index].data_ptr, r.data_ints * sizeof(unsigned int));

  if (rc !=  r.data_ints * sizeof(unsigned int)) {
    fprintf(stderr, "Error in reading variable data in record #%d\n", index);
    exit(1);
  }

  recordLeft--;
  }

return table;
}

/**
 * write_file: takes the data table and writes the table
 * to the an outputfile given in the argument
 *
 * @param int* fd - pointer to the file being read
 * @param rec_dataptr_t* table - pointer to the data table
 */
void
write_file(int* fd, rec_dataptr_t* table) {
int rc, data_size;

  //  writes header
  rc = write(*fd, &records, sizeof(records));
  if (rc != sizeof(records)) {
    fprintf(stderr, "Error: error in wrting header \n");
    exit(1);
  }

for(int ind = 0; ind < records; ind++) {
  //  writes index number and data number
  rc = write(*fd, &(table[ind]), 2*sizeof(unsigned int) );
    if (rc != 2*sizeof(unsigned int)) {
      fprintf(stderr,
"Error: error in writing fixed sized data in record #%d\n", ind);
      exit(1);
      }
  data_size = table[ind].data_ints  * sizeof(unsigned int);

  rc = write(*fd, table[ind].data_ptr, data_size);
    if (rc != data_size) {
      fprintf(stderr,
"Error: error in writing variable  sized data in record #%d\n", ind);
      exit(1);
      }

  free((void*)table[ind].data_ptr);  // frees the allocated memory
  table[ind].data_ptr = NULL;  // removes pointer address
  }
free((void*)table);  // frees allocated memory
table = NULL;  // removes pointer address
}

/**
 * Compare: compares two data values from column and returns 
 * the comparison value
 *
 * @param const void *p - pointer to first record
 * @param const void *q - pointer to second record
 * @return 0 if first <= second, 1 if first>second
 */
int compare(const void *p, const void *q) {
  int ret;  //  value to be returned
  rec_dataptr_t first_table, second_table;
  unsigned int first;  // value of first record at column
  unsigned int second;  // value of second record at column

  first_table = *(rec_dataptr_t*) p;
  second_table = *(rec_dataptr_t*) q;

  //  for each record, take the data value at column, or last data if
  //  column is greater that number of data
  if (first_table.data_ints-1 < column) {
  first = (unsigned int)(first_table.data_ptr[first_table.data_ints - 1]);
  }
  else  first = (unsigned int)first_table.data_ptr[column];

  if (second_table.data_ints-1 < column) {
    second = (unsigned int) second_table.data_ptr[second_table.data_ints - 1];
    }
  else  second = (unsigned int)  second_table.data_ptr[column];

  ret = (int)(first > second);

  return ret;
}
int main(int argc, char *argv[]) {
  char  *inFile = NULL;
  char *outFile   = NULL;
  int c;
  column = 0;

  opterr = 0;
  while ((c = getopt(argc, argv, "i:o:c:")) != -1) {
    switch (c) {
    case 'i':
    inFile = strdup(optarg);
    break;
    case 'o':
    outFile = strdup(optarg);
    break;
    case 'c':
    column = atoi(optarg);
    break;
    default:
    usage(argv[0]);
    }
  }
//  test for appropriate number of arguments
if (argc < 3) {
  usage(argv[0]);
  exit(1);
  }
//  test for input file presence
if (inFile == NULL) {
  usage(argv[0]);
  exit(1);
}
//  test for output file presence
if (outFile == NULL) {
  usage(argv[0]);
  exit(1);
}
//  test for valid column number
if (column < 0) {
  fprintf(stderr, "Error: Column should be a non-negative integer\n");
  exit(1);
  }


//  OPENS INPUT FILE
int fd = open(inFile, O_RDONLY);

if (fd < 0) {
  fprintf(stderr, "Error: Cannot open file %s\n", inFile);
  exit(1);
  }

rec_dataptr_t* ret;  // table pointer


ret =(rec_dataptr_t*) read_file(&fd);
  if (ret == NULL) {
  fprintf(stderr, "Error: Error in reading file \n");
  exit(1);
  }

qsort(ret, records, sizeof(rec_dataptr_t), compare);

(void) close(fd);


//  CREATES and OPENS output file
fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
  if (fd < 0) {
    fprintf(stderr, "Error: Cannot open file %s\n", outFile);
    exit(1);
  }

write_file(&fd, ret);

(void) close(fd);

free(inFile);
free(outFile);
inFile = NULL;
outFile = NULL;
return 0;
}
