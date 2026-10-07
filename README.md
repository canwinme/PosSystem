# POS System

C 언어와 Linux 환경에서 구현한 **편의점 POS 업무 관리 CLI 프로그램**입니다.

사용자 로그인부터 재고·잔고·매출 조회, 유통기한 관리, 상품 판매/환불, 입고/검색/삭제, 근무 종료 정산까지 편의점 업무 흐름을 하나의 프로그램으로 구성했습니다.

## 📌 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| 프로젝트 | 편의점 POS System |
| 개발 기간 | 2026-09-07 ~ 2026-09-09 |
| 개발 환경 | C / Linux / Visual Studio Code |
| 저장 방식 | 구조체 · 고정 배열 · CSV |
| 실행 방식 | Linux 터미널 CLI |

## 🛠 사용 기술

- **Language**: C
- **Environment**: Linux, VS Code
- **Data Storage**: CSV
- **Linux API**: termios, unistd
- **Time Handling**: time_t, time, localtime, ctime
- **Core Concepts**: 구조체, 배열, 문자열 처리, 파일 입출력, 포인터

## ✨ 주요 기능

- 사용자 등록 및 로그인
- Linux `termios` 기반 비밀번호 입력 마스킹
- 재고 현황 조회
- 현재 잔고 / 오늘 매출 조회
- 상품 판매 및 환불
- 신규/기존 상품 입고
- 상품 검색 및 삭제
- 판매/환불 거래 내역 확인
- 성인 상품 구매 가능 여부 확인
- 로그인/로그아웃 시간을 이용한 근무 시간 및 임금 계산

## 📦 Batch 기반 유통기한 관리

같은 상품이라도 입고 시점이 다르면 만료 시점이 달라지므로, 유통기한 상품은 `Batch` 단위로 재고를 관리합니다.

```text
Item
 ├─ 전체 재고
 └─ Batch[]
      ├─ stock
      └─ input_time
```

각 Batch에 입고 수량과 입고 시간을 저장하고, 현재 시간과 비교해 남은 유통기한을 계산합니다.

- 만료된 Batch는 전체 재고에서 제거
- 유통기한 상품 판매 시 먼저 입고된 Batch부터 차감
- 입고 시 새로운 Batch 추가
- 판매 후 수량이 0이 된 Batch는 정리

## 🔄 판매 흐름

```text
상품 검색
   ↓
재고 / 판매 수량 확인
   ↓
성인 상품 여부 확인
   ↓
결제 금액 확인
   ↓
재고 차감
   ↓
매출 / 거래 내역 갱신
   ↓
CSV 저장
```

환불 시에는 실행 중 저장된 `Sale` 배열을 이용해 실제 판매 수량을 확인하고, 판매한 수량을 초과한 환불을 방지합니다.

## 🗃 데이터 구조

### Item
상품명, 가격, 전체 재고, 유통기한, 성인상품 여부와 Batch 목록을 관리합니다.

### User
사용자 ID, 비밀번호, 사용자 이름을 저장합니다.

### Sale
판매/환불 상품명, 수량, 금액, 거래 유형을 관리합니다.

### Batch
입고 시점별 재고 수량과 입고 시간을 저장합니다.

## 📁 프로젝트 구조

```text
PosSystem/
├── README.md
├── .gitignore
├── src/
│   └── possystem.c
├── data/
│   ├── items.csv
│   ├── users.csv
│   └── batches.csv
└── docs/
    └── 김종호_POS_System.docx
```

## ▶ 실행 방법

Linux 환경에서 GCC로 컴파일합니다.

```bash
gcc possystem.c -o possystem
```

프로그램은 실행 위치의 `items.csv`, `users.csv`, `batches.csv`를 읽으므로 다음처럼 data 폴더에서 실행하거나 CSV 파일을 실행 위치에 복사해서 사용할 수 있습니다.

```bash
cp data/*.csv .
./possystem
```

## 💾 CSV 파일

- `items.csv`: 상품명, 가격, 재고, 유통기한, 성인상품 여부
- `users.csv`: 사용자 ID, 비밀번호, 사용자 이름
- `batches.csv`: 상품별 입고 수량과 입고 시각

> 현재 학습용 구현에서는 비밀번호를 평문 CSV로 저장합니다. 실제 서비스에서는 해시 기반 비밀번호 저장이 필요합니다.

## 💡 구현하면서 학습한 점

- `fgets + strtok + atoi`를 이용한 CSV 파싱
- 구조체 배열과 CSV 데이터 연결
- `find_* / save_*` 함수로 검색과 저장 역할 분리
- `termios`를 이용한 터미널 입력 제어
- `time_t` 기반 유통기한 및 근무시간 계산
- Batch와 FIFO를 이용한 입고 시점별 재고 관리
- 잘못된 숫자 입력, 재고 부족, 중복 상품, 성인 확인, 환불 수량 등 예외 처리

## 🚧 한계와 개선 방향

- 사용자 권한이 동일함 → 점장 / 아르바이트생 권한 분리
- 고정 크기 배열 사용 → 동적 메모리 또는 DB 적용
- CSV 기반 저장 → DB 기반 영구 저장
- 비밀번호 평문 저장 → 비밀번호 해시 적용
- 거래 내역이 실행 중 메모리에만 존재 → 파일 또는 DB에 영구 저장

## 🎥 시연 영상

[POS System 시연 영상](https://youtu.be/siYG3JVHGxA)
