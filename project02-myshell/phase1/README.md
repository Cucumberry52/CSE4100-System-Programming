\[system programming lecture]

\-project 2 baseline

csapp.{c,h}
CS:APP3e functions

shellex.c
Simple shell example



Phase 1 

해당 부분에서는 쉘에 대하여 기본적인 기능인 프로세스 생성과 실행을 구현했습니다.



내부 구현 내용

커맨드 입력 받기 및 수행 관련: 먼저 기존의 스켈레톤 코드를 바탕으로 작성하였습니다. parseline로 커맨드 라인을 단어 단위로 쪼개고 이를 eval 함수에서 처리합니다.



프로세스 제어 관련: fork()를 사용해서 자식 프로세스를 생성하였고 execvp()로 내용을 바꿔치워 작동합니다. phase3와 통합이 되어 변경한 내용으론 부모 프로세스 속 wait()부분이  sigsuspend()를 통해 자식 프로세스가 종료됨을 시그널 받아 진행하도록 하였습니다.



Built-in 처리 관련: cd, exit 명령어들은 builtin\_command()를 사용하여 프로세스를 생성하지 않고 직접 셸이 수행할 수 있도록 하였습니다. 

