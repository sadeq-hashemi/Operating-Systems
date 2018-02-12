#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int arg_size;
int path_size;
char error_message[30] = "An error has occurred\n";
// ctags -R * 

//RETURNS: -1 error in argument 
//RETURNS: 0 no redirection
//RETURNS: 1 redirection
int here_string(char** arg) {
  int found = 0; 
  for(int x=0; x<arg_size-1; x++) { 
    if( strcmp(arg[x], "<<<") == 0 ) { 

      if (arg[x+1] != NULL && strcmp(arg[x+1], "<<<") != 0 ) {

        if(arg[x+2] == NULL) {

          if(found == 0) {
            found = x; 
          } else if (found > 0){
            found = -1; 
            break; 

          }

        } else {
          found = -1;
          break; 
          } 
      } else {
        found = -1;  
        break; 
        }
    }
 }
//TEST
//printf("%d\n", found);
 return found; 
}

//RETURNS: -1 error in argument 
//RETURNS: 0 no redirection
//RETURNS: 1 redirection
int redirect(char** arg) {
  int found = 0; 
  for(int x=0; x<arg_size-1; x++) { 
    if( strcmp(arg[x], ">") == 0 ) { 

      if (arg[x+1] != NULL && strcmp(arg[x+1], ">") != 0 ) {

        if(arg[x+2] == NULL || strcmp(arg[x+2], "<<<") == 0) {

          if(found == 0) {
            found = x; 
          } else if (found > 0){
            found = -1; 
            break; 

          }

        } else {
          found = -1;
          break; 
          } 
      } else {
        found = -1;  
        break; 
        }
    }
 }
//TEST
//printf("%d\n", found);
 return found; 
} 
int find_path(char** paths, char** arg) {

  char* check = malloc(128); 
    if(check == NULL) {
       write(STDERR_FILENO, error_message, strlen(error_message)); 
   } 
  char * p; 
  char * f;
  struct stat buf; 
  f =  arg[0];  
  for(int x= 0; x<path_size; x++) {
    p = paths[x];
    check[0] = '\0'; 
    (void)strcat(check, p);
    (void)strcat(check, "/");
    (void)strcat(check, f); 
    if( stat(check, &buf) == 0) { 
    arg[0] = check;
    return 0;  
    }
  }
  return -1; 
  }

int type(char** paths, char ** arg) { 
  int ret; 
  if(arg_size < 2) {
   write(STDERR_FILENO, error_message, strlen(error_message)); 
   return 1; 
  }

  if( strcmp(arg[1], "exit") == 0 ||
      strcmp(arg[1], "cd") == 0  ||
      strcmp(arg[1], "path") == 0 || 
      strcmp(arg[1], "type") == 0 ) {
        printf("%s is a shell builtin\n", arg[1]); 
    }
  else {
    char temp[128]; 
    (void) strcpy(temp, arg[1]); 
    ret = find_path(paths, &arg[1]);
    if (ret == 0) 
      printf("%s is %s\n", temp, arg[1]);
    else {
     write(STDERR_FILENO, error_message, strlen(error_message)); 
     return 1;  
    }
  }
  return 0; 
}

int add_path(char ** paths, char * str) {
  if(path_size >= 128) 
    return 1; 
  if (paths == NULL || str == NULL) 
    return 1; 

  paths[path_size] = malloc(strlen(str) + 1);
  (void) strcpy(paths[path_size], str);

  path_size ++;
  return 0;  
}


int rem_all_path(char ** paths) {
  if(path_size > 128) 
    return 1; 

  // frees each path entry
  for(int x=0; x<path_size; x++){
    free(paths[x]);
    paths[x] = NULL;
    path_size =0;   
  }
  return 0; 
}

int path(char** paths, char ** args) {
//TO DO GET RETURNS 
  if (arg_size > 1) {
    rem_all_path(paths); 
    for(int x=1; x<arg_size; x++) {
      add_path(paths, args[x]); 
    }
  } else {
      for(int x=0; x<path_size; x++) {
        printf("%s\n", paths[x]);
      }
  }
 return 0; 
}
int cd(char** arg){
  if(arg[2] != NULL) {
    write(STDERR_FILENO, error_message, strlen(error_message)); 
    return 1;
  } 

  //  CASE 1: "cd"
  if(arg[1] ==NULL) {
    char * path = getenv("HOME");

      if (path == NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        return 1; 
        }
    return chdir(path); 
     
  } else { //  CASE 2: "cd arg2"
  /*   char * path = getenv(arg[1]);

      if (path == NULL) {
        fprintf(stderr, "no path found\n"); 
        exit(1); 
        } */
     return chdir(arg[1]); 
  }
  return 0; 
}

void parse(char* input_str, char **arg, const char* delim){
  int i = 0;
 if( strcmp(input_str, "\n") != 0) {
 //Gets first argument
 arg[0] = strtok(input_str, delim);
  if (arg[0] == NULL) {
    write(STDERR_FILENO, error_message, strlen(error_message)); 
  }

  arg_size++;

  do {
    i++;
    arg[i] = strtok(NULL, delim);


  }while(arg[i] != NULL);
  
  arg_size = arg_size + (i-1); //  Gets size

  arg[arg_size -1] = strtok( arg[arg_size - 1], "\n"); //removes \n from last argument
  }

}

char * prompt(int exit_status, char* input_str) {
//NOTE: FIGURE OUT ERROR AND REST OF PROMPT
  char dir[1024];
  char *ret = NULL;

  
  //Gets path of current directory and prints prompt
  ret = getcwd(dir, sizeof(dir));
    if ( ret == NULL) {
      write(STDERR_FILENO, error_message, strlen(error_message)); 
      return NULL;
      }
  printf("[%s]\n%d> ", dir, exit_status);
  fflush(stdout); 

  ret = NULL; //resets return char pointer
  ret = fgets(input_str, 128, stdin);
    if (ret == NULL) {
      write(STDERR_FILENO, error_message, strlen(error_message)); 
    }
  return input_str; 
}

int main(int argc, char *argv[]) {
  int exit_status = 0;
  path_size = 0; 
  char *input_str = NULL;
  char ** arg;
  char ** paths;  
  const char delim[2] = " "; 

  arg  = malloc(128); //FREE
    if(arg == NULL) {
      write(STDERR_FILENO, error_message, strlen(error_message)); 
    }
  paths = malloc(128); //FREE
     if(paths == NULL) {
      write(STDERR_FILENO, error_message, strlen(error_message)); 
   }
  input_str = malloc(128); //FREE
    if(input_str == NULL) {
      write(STDERR_FILENO, error_message, strlen(error_message)); 
   }

  /*FILE * fp = NULL; 

  fp = fopen("input_str.txt", "w+");
    if (fp == NULL) {
      fprintf(stderr, "opening file failed \n");
      exit(1);  
    }*/
  int ret; 


  ret = add_path(paths, "/bin"); 
    if(ret < 0)
      write(STDERR_FILENO, error_message, strlen(error_message)); 
/*ret = add_path(paths, "/usr/bin/"); 
    if(ret < 0) 
       fprintf(stderr, "add_path");
*/

while(1){
  input_str = prompt(exit_status, input_str);
  
  arg_size= 0;
  parse(input_str, arg, delim);
  if(arg_size == 0) 
    continue; 
  if (strcmp(arg[0], "exit") == 0) {

    exit(0); 

  } else if ((strcmp(arg[0], "cd\n") == 0) || (strcmp(arg[0], "cd") == 0) ) {

    exit_status = cd(arg); 

  } else if (strcmp(arg[0], "path") == 0) {

    exit_status = path(paths, arg); 

  } else if (strcmp(arg[0], "type") == 0) {

    exit_status = type(paths, arg); 

  } else {
      find_path(paths, arg);
      int red_ind =  redirect(arg); 
      int str_ind =  here_string(arg); 
      int mypipe[2]; 

      if(str_ind > 0) { 
        ret = pipe(mypipe);
 
        if(ret < 0) {
          write(STDERR_FILENO, error_message, strlen(error_message)); 
          exit_status = 1; 
          continue;
        }
        arg[str_ind] = NULL; 
     }


      int rc = fork(); 

      if (rc == 0) {
         // child

          if (str_ind > 0) {
            close(STDIN_FILENO); 
            dup(mypipe[0]); 
            char buff[128]; 
            close(mypipe[1]);
            close(mypipe[0]);  
           // ret = read(mypipe[0], STDIN_FILENO, 128); 
     
          }
         if(red_ind > 0) {
          char *out = malloc(128); //FREE
          strcpy(out, arg[red_ind + 1]); 
          strcat(out, ".out");
          close(STDOUT_FILENO); 
          int out_file = open(out, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU); 

          char *err = malloc(128); //FREE
          strcpy(err, arg[red_ind + 1]); 
          strcat(err, ".err");
          close(STDERR_FILENO); 
          int err_file = open(err, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU); 
          arg[red_ind] = NULL;  

       } 
        ret = execv(arg[0], arg);
        if(ret == -1) {
         exit_status = 1;  
        }

      } else if (rc > 0) {

          //  Parent
          if(str_ind > 0) { 
            FILE * rc = fdopen(mypipe[1], "w"); 
            if(rc == NULL) {
              write(STDERR_FILENO, error_message, strlen(error_message)); 
              exit_status = 1; 
              continue;
              }
           close(mypipe[0]); 

           fputs(arg[str_ind + 1], rc);
           fputs("\n", rc);
           fclose(rc);  

           close(mypipe[1]);
        }

          wait(NULL); 

      } else {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit_status = 1;
        continue;
      }
  }
}

  //fclose(fp);
  return 0;
}
