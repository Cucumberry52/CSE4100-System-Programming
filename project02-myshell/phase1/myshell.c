/* $begin shellmain */
#include <ctype.h>
#include "csapp.h"
#include "myshell.h"
#include<errno.h>

#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 




int main() 
{
    char cmdline[MAXLINE]; /* Command line */

    /////////////////////////////
   

    Signal(SIGINT,  sigint_handler);   
    Signal(SIGTSTP, sigtstp_handler);  
    Signal(SIGCHLD, sigchild_handler);  

    initjobs(jobs);

    //////////////////////////////


    while (1) {
	/* Read */
	printf("CSE4100-SP-P2> ");        

	fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
	eval(cmdline);
    } 
}
/* $end shellmain */
  







/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
   
///////////////////////////////////// 과제 속 시그널 블락 부분선언
    sigset_t mask_all, mask_one, prev_one, empty_one, prev_all;
    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);
    Sigemptyset(&empty_one);
/////////////////////////////

    char temp[MAXLINE];
    char *temp_argv[MAXARGS];

    strcpy( temp, cmdline );
    bg = parseline(  temp, temp_argv); // 여기서 bg 값만 따옴,  그러지않으면 buf에 \0들이 들어가 아래 if문에서 |를 못찾음

    if (strchr(buf, '|')) {
        
        sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
        
        if ((pid = Fork()) == 0) {

            // 이게 진짜 중요했던 부분인데, 파이프 로직을 처리하기 위해서 잡리스트로 하나만 올려야 하니까 포크를 했습니다. 근데 그 포크된 워랩퍼함수를 기준으로 |전후 자식들을 또 포크해서 실행해야하는데 이때 고정된 시그널 값에 의해서 오류가 발생하여 디폴트로 수정하였습ㄴ디ㅏ.
        /////////////////////////////
        Signal(SIGCHLD, SIG_DFL); 
        Signal(SIGINT,  SIG_DFL); 
        Signal(SIGTSTP, SIG_DFL); 
        ////////////////////////////////

            sigprocmask(SIG_SETMASK, &prev_one, NULL);
            
            setpgid(0, 0); // d이게 중요 - 자식 프로세스 그룹을 만들기
            
            phase2_pipe(buf); //재귀 호출
            
            while (waitpid(-1, NULL, 0) > 0); // 자식 프로세스가 모두 종료될 때까지 기다림 

            exit(0);
        }


        int Jstate;
        if (bg) Jstate = BG;        
        else Jstate = FG; 

        // 잡리스트에 추가
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all );
        addjob(jobs, pid, Jstate, cmdline); 
        sigprocmask(SIG_SETMASK, &prev_all, NULL);

        // 기존에 있던 코드 phase1에서 wait에서 sigsuspend로 바꿔줌
        if (!bg) {
            while (find_fg_pid(jobs) == pid)    sigsuspend(&prev_one);
        }
        
        else printf("[%d] %d\n", find_jid(pid), pid);
        
        sigprocmask(SIG_SETMASK, &prev_one, NULL);
        return;
    }
    //////////////////////////////////////////// 여기까지 파이프 처리 부분


    bg = parseline(buf, argv); 
   

    if (argv[0] == NULL)  return;   /* Ignore empty lines */    

    if (!builtin_command(argv)) { //quit -> exit(0), & -> ignore, other -> run  
            
        sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
    
        if ((pid = Fork()) == 0) { // 자식 프로세스
  
            setpgid(0, 0);
            sigprocmask(SIG_SETMASK, &prev_one, NULL);

            if (execvp(argv[0], argv) < 0) {	//ex) /bin/ls ls -al &
            
            Sio_puts(argv[0]);
            Sio_puts(": Command not found.\n");

            exit(0);
            }

        }

        
        int Jstate;
        if (bg) Jstate = BG;        
        else Jstate = FG; 

        // 잡리스트에 추가
      sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        addjob(jobs, pid, Jstate, cmdline); 
        sigprocmask(SIG_SETMASK, &prev_all, NULL);    
        //sigprocmask(SIG_SETMASK, &prev_one, NULL);

	/* Parent waits for foreground job to terminate */
        if (!bg){ 
             while (find_fg_pid(jobs) == pid) sigsuspend(&prev_one); 
        }
     
        
        
        //when there is backgrount process!
        else{ 
            Sio_puts("[");
            Sio_putl((long)find_jid(pid));
            Sio_puts("] ");
            Sio_putl((long)pid);
            Sio_puts("\n");
        }                

           // 원상복귀

        sigprocmask(SIG_SETMASK, &prev_one, NULL);

        
    }
    return;
}



/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "quit")) /* quit command */
	exit(0);  
    
    if (!strcmp(argv[0], "exit")) exit(0);  

    if (!strcmp(argv[0], "cd")) {
        if (!argv[1]) { // 인자가 없는 경우
            if (getenv("HOME"))   chdir(getenv("HOME"));
        } 

        else if (chdir(argv[1]) < 0) {
            Sio_puts(argv[1]);
            Sio_puts(": No such file or directory\n");
        }
            
        
        return 1;
    }
    
    
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
	return 1;


    // bg, fg 명령어 처리 부분
    if (!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg")) {
        phase3(argv);  
        return 1;
    }


    if (!strcmp(argv[0], "kill")) {
        if (!argv[1] ) {
           Sio_puts("kill: usage: kill [-s sigspec | -n signum | -sigspec] pid | jobspec ... or kill -l [sigspec] \n");
        }

        else {
            struct myjob *jobp;

            // 인자가 jobid인 경우
            if (argv[1][0] == '%') jobp = find_job_jid_ptr(jobs, atoi(&argv[1][1]));
            // 인자가 pid인 경우
            else jobp = find_job_ptr(jobs, atoi(argv[1]));
            
            if (jobp ) {
                pid_t temp = jobp->pid;


                 sigset_t  mask_one, prev_one;
                 Sigemptyset(&mask_one);
                 Sigaddset(&mask_one, SIGCHLD);



                Sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
                kill(-jobp->pid, SIGKILL); 

                while (find_job_ptr(jobs, temp) != NULL)  sigsuspend(&prev_one);
                sigprocmask(SIG_SETMASK, &prev_one, NULL);
            
                } 

                else Sio_puts("kill: no such job/process\n");    
        }

        return 1;
    }

//////////////////////////////////////
    if (!strcmp(argv[0], "jobs")) {
        listjobs(jobs); // 전역 변수 jobs 리스트를 출력함
        return 1;
    }
/////////////////////////////
    return 0;                     /* Not a builtin command */
}

/* $end buitin */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    argc = 0;

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    
    int quote_flag = 0; 

    while (*buf != '\0') {
        // 간격 공백 제거
        while (!quote_flag && *buf == ' ' && *buf != '\0') buf++;
        
        if (*buf == '\0') break;

        argv[argc++] = buf;

        // 한 단어의 시작에서부터 끝을 찾기
        while (*buf != '\0') {

            
            if (*buf == '\"'){
                if(!quote_flag) quote_flag = 1;
                else quote_flag = 0;
            }   
            
            if (*buf == ' ' && !quote_flag)  break; 
            
            buf++;
        }

        if (*buf != '\0') {
           
            * buf = '\0';
            buf++;
        }
        if (*buf == '\0') break;
    }
    argv[argc] = NULL;
    


for (int i = 0; i < argc; i++) {
    
    int j = 0;

        while (argv[i][j] != '\0') {  // 문장 끝일 때까지
            
            if (argv[i][j] == '\"') { // "을 찾으면
                
                int k = j; // j를 보관하는게 포인트
                
                while (argv[i][k] != '\0'){
                
                    argv[i][k] = argv[i][k + 1];
                    k++;
                        // 땡기 면서 다 지워라
                }
            
            } 
        else  j++;
        
    }
}

    if (argc == 0)  return 1;

    int last_idx = argc - 1;
    int len = strlen(argv[last_idx]);

    if (argv[last_idx][len - 1] == '&') {
        bg = 1;
        
        if(len != 1) argv[last_idx][len - 1] = '\0';   // &이외의 다른 문자도 같이 있는 경우, &만 제거

        else {  // &만 있는 경우
            argv[last_idx] = NULL;
            argc--;
        } 
    } 

    else  bg = 0;
    

    return bg;
}
/* $end parseline */


void phase2_pipe( char *cmdline ) {

    char *argv[MAXARGS];

    int pipe_index[2];

    pid_t pid;
    int status;

    char* index_I = strchr(cmdline, '|');

    if (index_I) {

        char *next_cmdline = index_I + 1;
        *index_I     = '\0';

        if (pipe(pipe_index) < 0)   unix_error("pipe funtion error");

        if ((pid = Fork()) == 0){ 
            
            Close(pipe_index[0]);               
            Dup2(pipe_index[1], STDOUT_FILENO); 
            Close(pipe_index[1]);
            
            parseline(cmdline, argv);

            if (builtin_command(argv))  exit(0); 
    

            if (execvp(argv[0], argv) < 0) {
                Sio_puts(argv[0]);
                Sio_puts(": Command not found.\n");
                exit(0);
            }
        } 
        
        else { 
            
            Close(pipe_index[1]);              
            Dup2(pipe_index[0], STDIN_FILENO);  // stdin을 pipe_index[0]으로 바꾼다.
            Close(pipe_index[0]);            
            // 위 자식문이 작동하는 if문이랑은 반대로 인자를 넘긴다.
            
            phase2_pipe(next_cmdline);    // 재귀 부분   
          //  Waitpid(pid, &status, 0);     
        
        }
    } 
    
    else { //eval속 명령어 처리를 그대로 복사 붙여넣기
        if ((pid = Fork()) == 0) {
            parseline(cmdline, argv);
        
            if (builtin_command(argv))  exit(0);

            if (execvp(argv[0], argv) < 0) {
                Sio_puts(argv[0]);
                Sio_puts(": Command not found.\n");
                exit(0);
            }

        }
        //Waitpid(pid, &status, 0);
        
        return; 
    }


}







/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
///////////////디버깅용 공간 나누기/////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////




void sigchild_handler(int sig) {
    int olderrno = errno;
   
    pid_t pid;
    int status;

    sigset_t mask_all, prev;
   
    sigfillset(&mask_all);

    // CTRL +z로 중지된 자식 프로세스도 처리위해 NTRACED 추가
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        
        sigprocmask(SIG_BLOCK, &mask_all, &prev);

        int jid = find_jid(pid);

        if (WIFEXITED(status)) deletejob(jobs, pid);
         

        // 시그널에 의해서 종료된 경우
        else if (WIFSIGNALED(status)) {
            if (jid > 0 && WTERMSIG(status) != SIGINT){

            Sio_puts("Job [");
            Sio_putl(jid);
            Sio_puts("] (");
            Sio_putl(pid);
            Sio_puts(") terminated by signal ");
            Sio_putl(WTERMSIG(status));
            Sio_puts("\n");

            }
            deletejob(jobs, pid);

        } 
        
        // 시그널에 의해서 중지된 경우
        else if (WIFSTOPPED(status)) {
            if (jid > 0  &&WSTOPSIG(status) != SIGTSTP) {

                Sio_puts("Job [");
                Sio_putl(jid);
                Sio_puts("] (");
                Sio_putl(pid);
                Sio_puts(") stopped by signal ");
                Sio_putl(WSTOPSIG(status));
                Sio_puts("\n");
            }
            find_job_ptr(jobs, pid)->state = ST;
        }
        
        // 복구
        sigprocmask(SIG_SETMASK, &prev, NULL);
    }
    errno = olderrno;
}

void sigint_handler(int sig) {
    int olderrno = errno;
    pid_t pid = find_fg_pid(jobs);
   
    if (pid != 0)  kill(-pid, SIGINT);
    
    write(1, "\n", 1);
    errno = olderrno;
}

void sigtstp_handler(int sig) {
    
    int olderrno = errno;
    pid_t pid = find_fg_pid(jobs);

    if (pid != 0)  kill(-pid, SIGTSTP);
    
    write(1, "\n", 1);
    errno = olderrno;
}

/* 잡 리스트 관리 헬퍼 함수들 */


void initjobs(struct myjob *jobs) {

    for (int i = 0; i < MAXARGS; i++){

        jobs[i].pid = 0; 
        jobs[i].jid = 0; 
        jobs[i].state = UNDEF;
        jobs[i].cmdline[0] = '\0';

}
}


int addjob(struct myjob *jobs, pid_t pid, int state, char *cmdline) {
    if (pid < 1) return 0;
 
    for (int i = 0; i < MAXARGS; i++) {
 
        if (jobs[i].pid == 0) {
            jobs[i].pid = pid; jobs[i].state = state;
            jobs[i].jid = next_jid++;
            
            // 오버플로우 처리
            if (next_jid > MAXARGS) next_jid = 1;

            strcpy(jobs[i].cmdline, cmdline);
            return 1;
        }

    }

    Sio_puts("addjob error!\n");
    return 0;
}

int deletejob(struct myjob *jobs, pid_t pid) {

    if (pid < 1) return 0;

    for (int i = 0; i < MAXARGS; i++) {

        if (jobs[i].pid == pid) {
            
            // 초기화 부분
            jobs[i].pid = 0; 
            jobs[i].jid = 0; 
            jobs[i].state = UNDEF;
            jobs[i].cmdline[0] = '\0';

            int max = 0;
            for (int j = 0; j < MAXARGS; j++)  
                if (jobs[j].jid > max) max = jobs[j].jid;
                // 차례대로 jid 갖기

            next_jid = max + 1;

            return 1;
        }
    }

    return 0;
}

struct myjob *find_job_ptr(struct myjob *jobs, pid_t pid) {
    if (pid < 1) return NULL;

    for (int i = 0; i < MAXARGS; i++)  if (jobs[i].pid == pid) return &jobs[i];
    return NULL;
}


int find_jid(pid_t pid) {
    if (pid < 1) return 0;
    
    for (int i = 0; i < MAXARGS; i++)  if (jobs[i].pid == pid) return jobs[i].jid;
    return 0;
}

pid_t find_fg_pid(struct myjob *jobs) {
    for (int i = 0; i < MAXARGS; i++)  if (jobs[i].state == FG) return jobs[i].pid;
    return 0;
}

void listjobs(struct myjob *jobs) {
    for (int i = 0; i < MAXARGS; i++) {

        if (jobs[i].pid != 0) {

            Sio_puts("[");
            Sio_putl(jobs[i].jid);
            Sio_puts("] (");
            Sio_putl(jobs[i].pid);
            Sio_puts(") ");

            if(jobs[i].state ==BG) Sio_puts("Running ");
            
            else if(jobs[i].state == FG) Sio_puts("Foreground ");

            else if(jobs[i].state == ST) Sio_puts("Stopped ");
            
            else Sio_puts("listjobs: Internal error\n");

            Sio_puts(jobs[i].cmdline);
        }
    }
}





void phase3(char **argv) 
{
    struct myjob *jobp = NULL;
    char *id = argv[1];
    int jid;
    pid_t pid;

    // 명령어 인자 부족
    if (!argv[1]) {
        Sio_puts(argv[0]);
        Sio_puts(" command requires PID or %%jobid argument\n");
       
        return;
    }

    if (!(id[0] == '%' || isdigit( id[0] ) ) ){ 
        Sio_puts(argv[0]);
        Sio_puts(": argument must be a PID or %%jobid\n");
      
        return;
    }
/////////// 여기까지 예외 탈출


    // 인자가 jobid인 경우
    if (id[0] == '%') { 
        jid = atoi(&id[1]);

        if (!(jobp = find_job_jid_ptr(jobs, jid))) {
            Sio_puts(argv[0]);
            Sio_puts(": No such job\n");
            return;
        }

    } 
    
    // 인자가 pid인 경우
    else if (isdigit(id[0])) { 
        
        pid = atoi(id);
        
        if (!(jobp = find_job_ptr(jobs, pid))) {
            Sio_puts(argv[0]);
            Sio_puts(": No such process\n");
            return;
        }

    } 
    
    //
    kill(-(jobp->pid), SIGCONT);

    if (!strcmp(argv[0], "bg")) {

        jobp->state = BG;

        Sio_puts("[");
        Sio_putl(jobp->jid);
        Sio_puts("] (");
        Sio_putl(jobp->pid);
        Sio_puts(") ");
        Sio_puts(jobp->cmdline);
    } 
    

    else if (!strcmp(argv[0], "fg")) {
        jobp->state = FG;
        
        sigset_t mask_one, prev_one;
    
        sigemptyset(&mask_one);
        sigaddset(&mask_one, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask_one, &prev_one);

        while (find_fg_pid(jobs) == jobp->pid) sigsuspend(&prev_one); 

        sigprocmask(SIG_SETMASK, &prev_one, NULL);
    
    }

    return;
}



struct myjob *find_job_jid_ptr(struct myjob *jobs, int jid) {
    if (jid < 1) return NULL;

    for (int i = 0; i < MAXARGS; i++)
        if (jobs[i].jid == jid) return &jobs[i];
    
        return NULL;
}
//////////////////////////////////////////////////////////////