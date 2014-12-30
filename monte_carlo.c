//
//  zli48Shell.c
//  
//
//  Created by Ze Li on 9/26/14.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <limits.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
//#define UMAX 50000000
/*
 * forward declaration of functions
 */

int random_generator(void);
void print_info(void);
void sigsegv_handler(int);
void sigint_handler(int);
void sigtstop_handler(int);
void sigalarm_handler(int);
void sigusr1_handler(int);


unsigned long int count_eval = 0;
unsigned long int count_seg = 0;
float timer = 0;
int i = 0;
unsigned long int val = 10;
pid_t ppid;

jmp_buf env;

int main(int argc, const char*argv[]){
    signal(SIGALRM,sigalarm_handler);
    signal(SIGUSR2,sigusr1_handler);
    
    time_t start_t = time(NULL);
    if(argc == 2){
        timer = atof(argv[1]);
    }
    if((int)timer > 0){
        alarm((int)timer);
    }
    else{
        printf("No alarm is set.\n");
    }
    srand(time(NULL));
    
    for(i = 0; i<ULONG_MAX; i++){
        sigsetjmp(env,1);
        count_eval++;
        int *addr;
        if(count_eval == val){
            val*=10;
            kill(getppid(),SIGUSR1);
        }
        signal(SIGINT, sigint_handler);
        signal(SIGTSTP,sigtstop_handler);
        signal(SIGSEGV,sigsegv_handler);
        addr = (int*)rand();
        //
        int r = *addr;
        
        
    }
    print_info();
    return 0;
}

int random_generator(){
    srand(time(NULL));
    return rand();
}

void print_info(){
    printf("Total number of segmentation fault: %lu\n",count_seg);
    printf("Total number of evalutation attempted: %lu\n",count_eval);
    printf("Percentage of attempts that resulted in seg fault: %f\n",(double)(count_seg)*100/count_eval);
}

void sigsegv_handler(int sig){
    signal(SIGSEGV,SIG_IGN);
    count_seg++;
    if(count_eval==ULONG_MAX){
        printf("seg: %lu\teval: %lu\n",count_seg,count_eval);
        exit(count_seg*100/count_eval);
    }
    siglongjmp(env,1);
    signal(SIGSEGV,SIG_IGN);
}

void sigint_handler(int sig){
    signal(SIGINT,SIG_IGN);
    print_info();
    exit((int)100*count_seg/count_eval);
}

void sigtstop_handler(int sig){
    signal(SIGTSTP,SIG_IGN);
    print_info();
    longjmp(env,1);
}

void sigalarm_handler(int sig){
    printf("TIME UP!!!!\n");
    print_info();
    exit((int)100*count_seg/count_eval);
    
}

void sigusr1_handler(int sig){
    if(sig == SIGUSR2){
        print_info();
        exit((int)100*count_seg/count_eval);
    }
}

