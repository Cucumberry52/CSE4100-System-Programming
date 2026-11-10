\[system programming lecture]

\-project 2 baseline

csapp.{c,h}
CS:APP3e functions

shellex.c
Simple shell example



Phase 3



백그라운드, 포그라운드,  시그널 핸들링, 작업 관리를 구현한다.



잡 구조체 및 함수: job의 상태(FG, BG)를 추적, 개별 프로세스들을 관리하기 위해서 구현한다. 구조체를 만들고 배열을 선언하여, 해당 배열에 add, delete 작업을 수행한다. 또한 fg, bg, jobs 명령어들이 작동하도록 phase3()함수로 구현하였다.



백그라운드 관련 : 명령어 가장 끝에 \&가 붙으면 부모가 자식을 기다리지 않고 작업하도록한다.  sigchild\_handler()를 설정하여 자식이 죽으면 부모에게 시그널로 리핑하도록 한다.



포그라운드 관련:  원자화된 sigsuspend를 사용하여 자식이 먼저 죽지 않도록 설계한다. 



시그널 핸들링 관련: SIGINT(Ctrl-C)와 SIGTSTP(Ctrl-Z) 시그널이 쉘이 아닌 포그라운드 프로세스 그롭에 작용되도록 핸들러를 구현하였다. 또 SIGCHLD관련하여 핸들러를 작성하고 sigprocmask()를 작성하여 race현상이 발생하지 않도록 추가하였다.

