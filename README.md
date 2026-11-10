<div align="center">

<h1>CSE4100 · System Programming</h1>

서강대학교 시스템프로그래밍 · 2026년 1학기<br />
자료구조 라이브러리부터 프로세스, 동시성 서버와 동적 메모리 관리까지

<p>
  <img src="https://img.shields.io/badge/C-A8B9CC?style=flat-square&amp;logo=c&amp;logoColor=black" alt="C" />
  <img src="https://img.shields.io/badge/Linux-FCC624?style=flat-square&amp;logo=linux&amp;logoColor=black" alt="Linux" />
  <img src="https://img.shields.io/badge/POSIX-Threads-526D82?style=flat-square" alt="POSIX Threads" />
  <img src="https://img.shields.io/badge/Build-GCC%20%2B%20Make-3B7A57?style=flat-square" alt="GCC and Make" />
</p>

[과제 목록](#과제-목록) · [실행 환경](#실행-환경)

</div>

---

## 소개

시스템프로그래밍 수업에서 수행한 과제 1~4의 제출 코드를 정리했습니다. C로 자료구조를 다루고, Linux의 프로세스·시그널·소켓·스레드를 사용하며, 메모리 할당기의 동작을 구현한 기록입니다.

수업 제공 코드와 CS:APP 지원 코드를 바탕으로 작성했습니다.

## 과제 목록

| 과제 | 프로젝트 | 주요 구현 | 핵심 개념 |
| :---: | --- | --- | --- |
| 01 | [MyLib](project01-mylib) | 명령어로 List·Hash Table·Bitmap 생성 및 조작 | 포인터, 자료구조 API, 동적 메모리 |
| 02 | [MyShell](project02-myshell) | 명령 실행, 파이프, 백그라운드 실행과 작업 제어 | 프로세스, IPC, 시그널 |
| 03 | [Concurrent Stock Server](project03-stockserver) | `select` 서버와 스레드 풀 서버에서 주식 조회·매수·매도 처리 | 소켓, I/O 다중화, 동기화 |
| 04 | [Mallocator](project04-mallocator) | `malloc`·`free`·`realloc`에 대응하는 동적 메모리 할당기 | 분리 가용 리스트, 분할·병합, 단편화 |

### 01 · MyLib

입력 명령을 해석해 리스트·해시 테이블·비트맵을 생성하고 조회·수정·삭제하는 프로그램입니다. 제공된 자료구조 라이브러리와 명령 처리 코드를 연결했습니다.

### 02 · MyShell

프로세스 생성과 실행에서 시작해 파이프 통신, 백그라운드 실행, `jobs`·`bg`·`fg` 등의 작업 관리로 확장했습니다. 시그널 처리와 프로세스 그룹을 사용해 포그라운드 작업을 제어합니다.

### 03 · Concurrent Stock Server

같은 주식 거래 기능을 두 가지 동시성 모델로 구현했습니다. `task_1`은 `select`로 연결을 관리하고, `task_2`는 제한된 연결 버퍼와 미리 생성한 작업 스레드를 사용합니다. 스레드 버전에서는 세마포어로 공유 재고에 대한 접근을 조정합니다.

### 04 · Mallocator

크기별로 나눈 가용 리스트에서 블록을 탐색하고, 할당 시 분할·해제 시 인접 블록 병합을 수행합니다. `realloc`에서는 인접 공간 활용과 데이터 복사 등을 통해 기존 할당 크기를 조정합니다.

## 실행 환경

**Linux 또는 WSL의 Linux 환경에서 GCC와 Make를 사용합니다.** 과제 2·3은 POSIX 프로세스·소켓·스레드 API를 사용하며, 과제 4의 기본 Makefile은 32비트 빌드(`-m32`)를 사용합니다.

```bash
git clone https://github.com/Cucumberry52/CSE4100-System-Programming.git
cd CSE4100-System-Programming
```

| 실행 대상 | 빌드·실행 안내 |
| --- | --- |
| 자료구조 명령 처리 | [MyLib README](project01-mylib/README.md) |
| 셸 Phase 1~3 | [MyShell README](project02-myshell/README.md) |
| 서버·클라이언트 | [Stock Server README](project03-stockserver/README.md) |
| 메모리 할당기 평가 | [Mallocator README](project04-mallocator/README.md) |
