\[system programming lecture]

\-project 2 baseline

csapp.{c,h}
CS:APP3e functions

shellex.c
Simple shell example



Phase 2

파이프라인(|) 기능 추가하여 파이프의 개념 이해하기 



파이프 기능 구현: 명령어 라인에 | 기호를 감지하여 eval에서 포크로 분기 처리합니다. pipe()를 사용하여 프로세스 간의 통로(파이프)를 만들었고, dup2()를 바탕으로 입출력 파일을 제어한다. 



다중 파이프 지원: 재귀 구조로 실행될 수 있도록 phase2\_pipe를 작성하였다. (과제 명세서 그림과는 다름) 셀 자체가 후에 시그널을 통해서 죽어버리거나 멈추면 안되므로 재귀 구조로 생성되는 프로세스를 컨트롤하기 위한 프로세스를 하나 fork()하였다. 이 랩퍼 프로세를 pgid로 하여 자식들을 그룹으로 묶어 다룬다. 

