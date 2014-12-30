//
//  monteCarlo.c
//  
//
//  Created by Ze Li on 9/26/14.
//
//

#include <tgmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>

#define N 512
#define M 256

//forward declarations
char** parse(char[]);
void change_name(void);
void sigint_handler(int);
void sigstop_handler(int);
void signal_handler(int);

//variable declarations
jmp_buf env;
pid_t ret_pid,child_pid;
int precision = 3,limit = 0,print = 1;
int main(int argc, char** argv){
    printf("This is hw2 by Ze Li, zli48, 676755673\n");
    if(argc == 2){
        precision = atoi(argv[1]);
    }
    int status,exit_status;
    size_t n = N;
    struct rusage *rus;
    while(1){
        signal(SIGINT,sigint_handler);
        signal(SIGTSTP,sigstop_handler);
        int i = 0;
        printf("zli48$");
        char**args = malloc(sizeof(char*)*M);
        char* line = malloc(sizeof(char)*N);
        getline(&line,&n,stdin);
        char* tmptr = line;
        
        while(*tmptr!='\n'){
            tmptr++;
        }
        *tmptr = '\0';
        if(strcmp(line,"exit") == 0){
            break;
        }
        if(strcmp(line,"")==0){
            continue;
        }
        char*tokens = malloc(sizeof(char)*N);
        tokens = strtok(line," ");
        while(tokens!=NULL){
            args[i] = malloc(sizeof(char)*N);
            args[i] = tokens;
            i++;
            tokens = strtok(NULL, " ");
        }
        int j = 0;
        ret_pid = fork();
        child_pid = getpid();
        
        //fork error
        if(ret_pid < 0){
            printf("fork error\n");
            continue;
        }
        //read from command line
        if(ret_pid == 0){
            execvp(args[0], args);
            exit(0);
        }
        else{
            //            wait4(child_pid,&status,WUNTRACED,rus);
            waitpid(-1,&status,0);
            signal(SIGUSR1,signal_handler);
            if(WIFEXITED(status)){
                exit_status = WEXITSTATUS(status);
                if(exit_status > 128){
                    exit_status -= 256;
                }
                printf("zli48$Child %d return - %d\n",child_pid, WEXITSTATUS(status));
                
            }else{
                if(WIFSIGNALED(status)){
                    printf("Child died on signal - %d\n", WTERMSIG(status));
                }
            }
        }
        
    }
    return 0;
}
void sigint_handler(int sig){
    signal(SIGINT,SIG_IGN);
    printf("Ctrl+C is disabled in the shell. Enter exit to quit.\n");
}
void sigstop_handler(int sig){
    signal(SIGTSTP,SIG_IGN);
    printf("Ctrl+Z is disabled in shell. Enter exit to quit.\n");
}

void signal_handler(int signo)
{
    limit++;
    print*=10;
    printf("Now:%d\n",print);
    if(limit == precision) /* it is the parent */
    {
        kill(ret_pid, SIGUSR2);
        limit = 0;
    }
}