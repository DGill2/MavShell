//Dharampreet Gill
//1000835781

// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.



#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports five arguments

int pid_store[15]; //will store the number of pids that were executed and then print from here
int p_counter=0;

char *store_cmd[MAX_COMMAND_SIZE]; 
int cmd_counter=0;

void childsig(int sig)
{
    waitpid(-1, NULL, WNOHANG);
}

 void do_nothing(int sig)
 {
   
 }
void exit_function()
{
  exit(0);
}

int main()
{
  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    //signal for ctrl-c and ctrl-z, when pressed program shouldn't stop
    signal(SIGINT, do_nothing);
    signal(SIGTSTP, do_nothing);

    //use to suspend a child process
    signal(SIGCHLD,childsig);
 
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;    //keeping track of number of argument in the cmd line                       
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );  

    //making use of the working_str, which store the original cmd
    //whenever ever cmd == num of arguments,(in else if)
    //duplicating the original string (working_str) into my own array(store_cmd) to be able to print out the cmd hisotry
    //all this has to be done before tokenizing the original string
    if(cmd_counter < 15) 
    {
        store_cmd[cmd_counter]= strdup(working_str);
        cmd_counter++;
    }
    else if(cmd_counter == 15)
    { 
      //need this loop so we can bascially delete the value at 0th index and over write the next 14 values
      //then store the 15th value with the new cmd enter (which is what line 105 is for)
      int x;
      for(x=0; x < cmd_counter-1; x++)
      {
          store_cmd[x] = store_cmd[x+1];
      }
      store_cmd[cmd_counter-1] = strdup(working_str); //replaces the new cmd with the older one(so we can print the LAST 15)
    }              


    //make the existing history cmd, the new cmd(working_str) that executes
    if(working_str[0] == '!') //if '!' is detected in the original input(working_str)
    {                         
      if(strlen(working_str) == 3) //then check the lenght of the input, if 3(with the terminating null, then its 2 digit #)
      {
        working_str = strdup(store_cmd[(working_str[1]-'0')]);
      }
      else if(strlen(working_str) == 4)
      {
        working_str = strdup(store_cmd[((working_str[1]-'0')*10)+working_str[2]-'0']);
      }
    }

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenizeds input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    /*int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      //printf("token[%d] = %s\n", token_index, token[token_index] );  

    }*/

    //shell continues even if their are no inputs(requir 1)
    if(token[0] == NULL) 
    {
      continue;
    }

    if(strcasecmp(token[0], "quit") == 0|| strcasecmp(token[0], "exit") == 0)
    {
      exit_function(); 
    }
    else if(strcmp(token[0], "cd") == 0) //changed directories
    {
      if(token_count == 2) //this will work for cd, meaning go to the hoe directory
      {
        chdir(getenv("HOME"));
      }
      else //this is to change directory
      {
        chdir(token[1]);
      }
      
    }
    else if(strcmp(token[0], "history") == 0) //prints out the last 15 history
    {
      int i;
      for(i=0; i<cmd_counter; i++)
      {
        printf("%d: %s", i, store_cmd[i]);
      }
    }
    else if(strcmp(token[0], "bg") == 0)
    {
      //if bg inserted, then it signals to suspend the last current running process in backgroud
      kill(pid_store[p_counter-1],SIGCONT);
    }
    else
    {
      
      //shell should support the syntax(built in cmd's; like cd, ls, etc)
      //and those paths could be included in /bin or /usr/bin or /usr/local/bin 
      //following will store the path (requir 10)
      char *bin = (char*)malloc(MAX_COMMAND_SIZE);
      char *user = (char*)malloc(MAX_COMMAND_SIZE);
      char *local = (char*)malloc(MAX_COMMAND_SIZE);

      //following will go inside any of the directories(bin, local or usr) 
      //and concat with the built in directories
      // and excecute the cmd by the user(example: ls which is inside /bin )
      strcpy(bin,"/bin/");
      strcat(bin,token[0]);
      strcpy(user,"/usr/bin/");
      strcat(user,token[0]);
      strcpy(bin,"/usr/local/bin");
      strcat(local,token[0]);
      
      
      pid_t child_pid = fork();

      if(child_pid == 0) // if or when fork is 0, its in child process
      { 
        if(strcmp(token[0], "listpids") == 0)
        {
          int i;
          for(i=0; i<p_counter; i++)
          {
            printf("%d: %d\n", i, pid_store[i]);
          }
          continue;
        }

        execvp(local, token);
        execvp(user, token);
        execvp(bin, token);
        
        printf("%s: Command not found.\n",token[0]);
      }
      else if(child_pid < 0) //incase of crash, child process will be neg
      {
        printf("could not fork\n");
      }
      else
      {
        pause();
        //storing 15 process id's in pid_store
        //in else deleting the pid at 0th index and storing the new one
        //at 14th place 
        if(p_counter < 15)
        {
          pid_store[p_counter] = child_pid;
          p_counter++;
        }
        else
        {
          int i;
          for(i=0; i<p_counter-1; i++)
          {
            pid_store[i] = pid_store[i+1];
          }
          pid_store[p_counter-1] = child_pid; //storing the new pid
        }
      }
    }
    free( working_root );
  }
  return 0;
}
