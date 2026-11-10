# Project 02 · MyShell

[← 과제 목록](../README.md)

Linux 셸을 구현하며 프로세스 제어, 파이프와 시그널 처리를 실습한 과제입니다.

| 단계 | 학습·구현 내용 |
| --- | --- |
| Phase 1 | 명령어 파싱, 프로세스 생성·실행, `cd`·`exit` 등 내장 명령 |
| Phase 2 | 파이프를 통한 여러 명령어 연결 |
| Phase 3 | 백그라운드 실행, `jobs`·`bg`·`fg`·`kill`, 시그널과 프로세스 그룹 관리 |

제출본에서는 세 단계의 `myshell.c`·`myshell.h`가 같은 통합 코드입니다. 단계별 폴더와 원래 설명은 유지했습니다.

## 실행

최종 단계 기준:

```bash
cd phase3
make
./myshell
```

셸 입력 예시:

```bash
pwd
ls -l | wc -l
sleep 30 &
jobs
exit
```

`Ctrl+C`·`Ctrl+Z` 및 `fg`·`bg`를 사용해 작업 상태를 다룹니다. 작업 번호는 실제 `jobs` 출력에 맞춰 사용합니다.

## 빌드 파일 정리

제출본에서 참조하던 `csapp.c`·`csapp.h`를 수업 제공 코드에서 가져와 `common/`에 두었습니다. 각 Phase의 Makefile은 이 공통 파일을 참조하도록 정리했으며, 셸 구현 코드는 변경하지 않았습니다.

## 원래 단계별 설명

- [Phase 1](phase1/README.md)
- [Phase 2](phase2/README.md)
- [Phase 3](phase3/README.md)
