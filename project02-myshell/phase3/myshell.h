

#include <errno.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define MAXLINE 8192 // 명세서 내용대로 설정
#define MAXARGS 128

//잡의 상태를 선언하는 부분으로 관례를 따랐습니다.
#define UNDEF 0 
#define FG 1    
#define BG 2    
#define ST 3    

struct myjob {              
    pid_t pid;              
    int jid;                
        
    int state;              // FG, BG, or ST 
    char cmdline[MAXLINE];  
};



struct myjob jobs[MAXARGS]; /* The job list */

volatile int next_jid = 1;            /* Next job ID to allocate */

void sigchild_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void initjobs(struct myjob *jobs);
int addjob(struct myjob *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct myjob *jobs, pid_t pid);


struct myjob *find_job_ptr(struct myjob *jobs, pid_t pid);
int find_jid(pid_t pid);

pid_t find_fg_pid(struct myjob *jobs);
void listjobs(struct myjob *jobs);
void phase2_pipe( char *cmdline );
void phase3(char **argv);


struct myjob *find_job_jid_ptr(struct myjob *jobs, int jid);
