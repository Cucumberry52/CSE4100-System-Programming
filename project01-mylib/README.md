# Project 01 · MyLib

[← 과제 목록](../README.md)

List, Hash Table, Bitmap을 입력 명령으로 조작하는 과제입니다. 제공된 자료구조 API에 명령어를 연결하고, 생성·조회·수정·삭제와 메모리 해제를 처리했습니다.

## 구성

- `main.c`: 명령어 해석과 자료구조별 작업 호출
- `list.c`, `hash.c`, `bitmap.c`: 자료구조 라이브러리와 확장 연산
- 기타 헤더·지원 파일: 디버깅과 비트맵 출력 등

## 실행

```bash
make
./testlib
```

입력 예시:

```text
create list list0
list_push_back list0 10
list_push_back list0 20
dumpdata list0
delete list0
quit
```

입력 파일을 사용하려면 `./testlib < input.txt`로 실행합니다.

## 주요 내용

- 이름으로 여러 자료구조를 생성하고 관리
- 리스트 삽입·삭제·정렬·교환·섞기 등의 연산 연결
- 해시 테이블 탐색·삽입·삭제와 원소별 연산
- 비트맵 설정·검사·탐색·확장

제공 라이브러리를 바탕으로 작성한 과제입니다.
