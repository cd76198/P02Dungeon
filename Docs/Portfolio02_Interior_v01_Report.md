# Portfolio 02 — 내부 던전 v01 Graybox 검증 기록

작성일: 2026-08-25

> 이 문서의 모든 좌표·거리·폭·높이는 `TEST VALUE / 미확정`이다.
> 사용자 플레이 리뷰를 위한 v01이며 최종 레벨 설계값이 아니다.

## 구현 위치

- 맵: `/Game/Portfolio02/Levels/L_Interior_v01`
- 기존 Entrance 맵은 수정하지 않음
- World Settings GameMode Override: `BP_CombatGameMode`
- 고정 카메라: Pitch -50°, Yaw 0°, Distance 1700uu, FOV 45

## 구성 흐름

Entrance → 4단 하강 → Mid Boss → 열린 출구 프레임 → 6단 상승 → 높은 Transition → 짧은 압축 → Final Boss → END 패드

## TEST VALUE / 미확정

- PlayerStart: `(0, -1200, 260)`
- Entrance 바닥: `800 × 900uu`, 상단 Z `160uu`
- 하강: `40uu × 4단`, Mid Boss 바닥 상단 Z `0uu`
- Mid Boss 바닥: `2700 × 2000uu`, 단순 직사각형 평지
- Mid Boss 출구: 두 기둥과 상부 프레임, 현재 통로는 열린 상태
- Transition 상승: `40uu × 6단`, 총 `240uu`
- Transition 높은 경로: 폭 `800uu`, 길이 `2600uu`, 바닥 상단 Z `240uu`
- Mid Boss 방향 난간: 높이 `100uu`
- Transition 바깥 벽: 높이 `260uu`
- Final Boss 진입 압축: `700 × 600uu`
- Final Boss 바닥: `3200 × 3000uu`, 바닥 상단 Z `240uu`
- 중앙 대형 구조물: `900 × 700 × 220uu`, 1개
- END 패드: `500 × 700uu`, 낮은 패드만 배치

## Encounter Clear → 경로 개방

- 기존 시스템은 레벨에 문 Actor를 배치하는 것만으로 자동 연결되지 않음.
- 이번 v01에서는 기능 작업을 확대하지 않고 `전투 후 열릴 자리`를 열린 출구 프레임으로 표시함.
- 실제 Encounter Clear → Door Open 연결은 미구현.

## PIE 관찰

### Mid Boss 진입

- 짧은 하강 뒤 넓은 평지로 연결되는 관계는 확인됨.
- 장애물이 없는 기준 전투 공간으로 구성됨.

### Transition — Mid Boss 재노출

## 관찰

- Transition 테스트 좌표 `(3000, 1100, 340)`의 실제 고정 카메라 화면에서 지나온 Mid Boss 공간이 보이지 않음.

## 가능한 문제

- `D.png` / `another.png`의 핵심인 과거 공간 재노출 관계가 현재 v01에서는 작동하지 않음.

## 근거

- `Saved/Screenshots/InteriorV01/02_Transition_MidBossReExposure.png`

## 수정 가능 변수

- Mid Boss와 Transition의 평면 간격
- Transition의 위치와 높이
- Mid Boss 방향 경계/난간 위치와 높이
- 재노출 확인 지점

### Transition — 전방 진행

- 테스트 좌표 `(3800, 1100, 340)`에서 높은 경로와 전방 진행축은 명확하게 보임.
- 과거 공간보다 현재 전방 경로가 강하게 읽히는 상태임.

### Final Boss 진입

- 테스트 좌표 `(7000, 1100, 340)`에서 넓어진 바닥과 중앙 구조물 상부가 보임.
- 중앙 구조물은 낮은 단일 덩어리지만, 현재 화면에서는 상단 프레임에 가깝게 크게 잡힘.
- 좌우 우회 판단이 실제 전투 이동에서 적절한지는 사용자 플레이 검토가 필요함.

## 스크린샷

- `Saved/Screenshots/InteriorV01/01_MidBossEntry.png`
- `Saved/Screenshots/InteriorV01/02_Transition_MidBossReExposure.png`
- `Saved/Screenshots/InteriorV01/03_Transition_ForwardDirection.png`
- `Saved/Screenshots/InteriorV01/04_FinalBossEntry.png`

스크린샷 2~4는 동일 카메라 검증을 위해 PlayerStart만 임시 테스트 좌표로 옮겨 PIE를 재실행했다. 촬영 후 PlayerStart는 `(0, -1200, 260)`으로 복구하고 맵을 저장했다.

## 사용자 판단 필요 — 최대 3개

1. Mid Boss 재노출이 보이도록 공간 관계를 수정할지, 현재 Transition 흐름을 유지할지
2. Mid Boss 출구를 실제 Encounter Clear 문 로직과 연결할지
3. Final Boss 중앙 구조물 `900 × 700 × 220uu`의 크기와 진입 시 노출 강도가 적절한지
