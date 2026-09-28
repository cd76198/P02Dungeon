# UE5 포트폴리오 제출용 프로젝트

2026-09-27 · UE 5.7.4 · 원본 보존, 별도 복사본.

## 실행

저장소 루트의 `P02Dungeon.uproject`를 UE 5.7.4에서 열고 Play를 실행합니다. 이 폴더는 UE 프로젝트이며, 엔진 없이 실행하는 패키징 EXE는 아직 만들지 않았습니다.

- 시작 필드: `/Game/Portfolio02/Levels/던전외부필드_v09-17_A`
- 연결 던전: `/Game/Portfolio02/Tests/FinalBossCandidates/던전내부최종_보수_v02`
- 조작: WASD 이동, 왼쪽 마우스 공격, E 상호작용. 중간 문은 E 유지.
- Windows Editor Development 빌드 성공. 로컬 제출 ZIP에는 에디터 모듈을 포함했지만, Git에는 빌드 생성물을 포함하지 않습니다.

## 변경한 내용

| 파일 | 변경 |
|---|---|
| 선택한 필드 `.umap` | `P02OutdoorReviewController_0.Destination`을 원본 던전에서 `던전내부최종_보수_v02`로 변경 |
| `Config/DefaultEngine.ini` | `EditorStartupMap`, `GameDefaultMap`을 선택한 필드로 변경. 원 제작 PC에만 있는 자동 실행 Python 스크립트 2줄은 주석 처리 |
| `Config/DefaultGame.ini` | 나중에 패키징할 때 동적 전환 대상이 빠지지 않도록 `MapsToCook`에 선택한 필드·던전 명시 |
| `P02Dungeon.uproject` | 저장소에 포함되지 않은 개발 도구 `UnrealMCP` 플러그인을 비활성화하여 복사본을 실행 가능하게 설정 |

게임플레이 C++ 소스는 수정하지 않았습니다. 던전 보수_v02 파일도 원본과 SHA-256이 같습니다. 필드 액터 이름·위치·크기·충돌, 정적 메시·재질 참조의 조회값에 추가 변경이 없는 것을 대조했습니다. NullRHI 검증에서 생성되지 않는 세션용 `ChaosDebugDrawActor`는 배치 액터 변경에서 제외했습니다.

원본 `outputs/P02Dungeon`의 파일 1,512개는 작업 전후 해시가 모두 동일하고 Git 변경 사항이 없습니다. 원본·제출본의 차이는 `verification/integrity.json`에 기록했습니다.

**문서의 1,860 uu · 약 4.17초는 기존 설계 계산으로 유지합니다. 문서, 몬스터 스폰 좌표, 속도, 공격·지연 설정을 바꾸지 않았습니다.** 이번 검증 중 생성된 시간·거리 로그를 기존 플레이테스트 결과나 설계 계산으로 대체하면 안 됩니다.

## 실제 실행 검증

| 항목 | 결과 | 근거 |
|---|---|---|
| 기본 시작 맵 | 명시적 맵 인자 없이 게임 모드 실행 시 v09-17_A 로드 확인 | `verification/standalone-startup.log` |
| 오른쪽 필드 경로 → 던전 | 실제 이동·충돌로 정면 입구 진입, 보수_v02 로드 | `runtime-events.json`, `runtime-positions.json` |
| 왼쪽 필드 경로 → 던전 | 실제 이동·충돌로 정면 입구 진입, 보수_v02 로드 | 같은 파일, `left-dungeon-entry.json` |
| 던전 시작점 일치 | 두 전환 직후 캐릭터 위치 모두 `(0, -1200, 252.150002) uu`. 배치 PlayerStart는 `(0, -1200, 260) uu`; 착지한 캐릭터 중심과 구분 | `RESULTS.json`, `submission-asset-readback.json` |
| 다리 파괴 | 실제 공격 입력 8회로 파괴 | `confirmed-runtime-lines.txt`, `bridge-after-input.json` |
| 중간 문 | 실제 E 입력 5초 유지로 열림, 캐릭터 통과 | `mid-door-after-input.json`, 실행 로그 |
| 적 처치·계단 통로 | 실제 공격으로 적 처치, 2초 후 계단 장치 작동·차단 해제. 연결 계단을 걸어서 하층까지 통과 | `midboss-defeated.json`, 실행 로그·좌표 |
| 다리 복구 | 레버 근처 E 입력으로 복구 | `lever-bridge-restored.json`, 실행 로그 |
| 최종 문 | Blue → Red → Green E 입력 성공. 문과 아치 2개가 하강·숨김 처리되고 충돌 해제 | `final-door-open.json`, 실행 로그 |
| END | 최종문을 지나 종료 범위에 들어가 END 이벤트·완료 플래그 발생 | `end-before-movement-attempt.json`, 실행 로그 |
| END 이후 이동 정지 | 이동 입력 무시=true, 속도=0. W·D 각 2초, AddMovementInput 2초 시도에서 좌표 변화 각각 0 uu | `end-after-W.json`, `end-after-D.json`, `end-after-add-movement.json` |

## 검증 방법과 한계

- 제출본과 같은 게임플레이 소스 및 동일한 맵 파일을 사용하는 별도 검증 복사본에서 PIE로 실행했습니다. 테스트 도구는 좌표 기반 이동 방향 입력·키 입력과 상태 조회만 추가했고, 제출 프로젝트에는 넣지 않았습니다.
- 시작점에서 입구, 던전의 다리·문·계단·퍼즐·END까지 캐릭터를 연속 이동했습니다. 순간이동, 직접 피해 적용, 강제 문 개방, 강제 퍼즐 완료, 강제 END 호출은 사용하지 않았습니다.
- 자동 경유점이 난간·기둥·분수·계단 옆면·퍼즐 발판을 향한 경우 경유점을 조정했습니다. 해당 시도와 정체 로그도 보존했습니다. 모든 경로가 아무 장애물 없이 직선 통과된다는 뜻은 아닙니다. 지형이나 충돌은 수정하지 않았습니다.
- 이번 기록은 알려진 경로를 따라가는 기술 검증이며 참가자 플레이테스트·탐색 성공률·이동시간 측정이 아닙니다. 중간 대기와 경로 조정이 포함되어 실행 로그의 소요 시간을 포트폴리오 성능 수치로 쓰면 안 됩니다.
- 제출본 자체는 빌드와 기본 맵 로드, 저장된 맵 속성 재조회까지 확인했습니다. 별도 NullRHI 게임 시작 확인 프로세스는 맵 로드 후 파생 데이터 준비 중 종료했고, 전체 진행 검증은 PIE에서 수행했습니다.
- 패키징 EXE의 실행·쿠킹, 모든 오답/실패/대체 경로 조합, END 화면 연출의 육안 검수는 이번에 하지 않았습니다.

## 검증 파일

- `verification/RESULTS.json`: 핵심 판정, 두 진입 좌표, END 이후 이동 변화량.
- `verification/integrity.json`, `original-hashes.json`: 원본 보존 및 제출본 변경 파일.
- `verification/map-change.json`, `submission-asset-readback.json`: 저장된 연결값, 맵 배치·참조 대조.
- `verification/runtime.log`, `runtime-events.json`, `runtime-positions.json`: 원 실행 로그, 입력 단계, 좌표 표본.
- `verification/confirmed-runtime-lines.txt`: 던전 진행·END의 확인된 게임 로그 발췌.
- `verification/submission-build.log`, `standalone-startup.log`, `asset-readback.log`: 빌드·기본 시작 맵·저장값 재조회 근거.
- `verification/*.py`: 별도 검증 복사본에서 사용한 절차 기록. 경로가 이 작업 환경에 고정되어 있으므로 제출본에서 자동 실행하지 않습니다.

압축본에는 Content·Config·Source·원본 문서와 실행에 필요한 에디터 바이너리를 포함하며, 재생성 가능한 Intermediate·Saved·DerivedDataCache와 대용량 디버그 PDB는 제외합니다.

## Git 공개본 안내

이 보고서는 원본 보존 제출 복사본의 검증 기록입니다. Git 공개본은 같은 4개 변경 파일을 저장소 루트에 적용했습니다. 원본 해시·절대 경로는 당시 검증 환경의 기록이며, 업로드된 도구 스크립트는 그 환경을 기록한 참고용으로 자동 실행되지 않습니다.
