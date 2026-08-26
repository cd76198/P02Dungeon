# P02Dungeon — Portfolio 02 「UE5 쿼터뷰 던전 레벨디자인」

LOST ARK Mobile 레벨기획 계약직(신규) 지원용 포트폴리오. 제출 목표 **2026-08-31**, 2~3분 분량 쿼터뷰 던전.
목적은 예쁜 환경이나 기술 데모가 아니라 **플레이 레벨 구성 / 필드·던전 레벨디자인 / 오브젝트 배치 설계 / UE5 사용 경험**을 실제 플레이 가능한 레벨과 설계 과정으로 증명하는 것.

## 최우선 규칙 — 역할 경계

**"게임이 돌아가게 만드는 시스템은 AI가 도와도 되지만, 그 시스템 안에서 플레이어가 어떤 공간을 경험할지는 사용자가 직접 설계한다."**

### AI가 적극적으로 해도 되는 것
쿼터뷰 카메라 기술 구현 / 이동·대시 / 최소 공격·피격 / 테스트용 적 / 처치 판정 / Trigger·Door / START·END / 최소 HUD / Blueprint·C++ / 액터 반복 배치 / **거리·고저차·시간 실측** / 테스트 환경 구축 / 반복 작업 자동화 / 로그·오류 분석 / 문서 정리

### AI가 절대 임의로 결정하지 않는 것
START 위치 · 레벨 레이아웃 · 공간 개수 · 지형 · 높낮이 · 고저차 · 벽 위치/높이 · 이동 동선 · 경로 폭 · 전투 공간 크기·형태 · 입구/출구 위치 · 주요 오브젝트 배치 · 시야 개방/차단 · 목적지 노출 방식 · 플레이어 유도 방식 · 보스 공간 구조 · 장애물 배치 · 안전/위험 영역 · 관찰 지점 · **플레이테스트 후 공간 수정안**

### 레벨 문제를 발견했을 때
자동 수정 금지. 아래 형식으로 **보고만** 하고 수정 방향은 사용자가 결정한다.

```
## 관찰      실제로 확인된 현상
## 가능한 문제  왜 플레이에 문제가 되는가
## 근거      viewport / 플레이테스트 / 거리 / 시간 / 좌표 등 확인 가능한 정보
## 수정 가능 변수  예: 벽 높이, 경로 폭, 오브젝트 위치, 시야 개방 정도
```

### 확정된 외부 진입 구간 — 보호 대상
START → 거의 직진 → 높은 위치의 붉은 시각 단서 인지 → 접근하며 던전 입구 방향/일부 인지 → 정면 접근 불가 확인 → 우측 우회 경로 → 상승 → 던전 입구 도달.

우회 경로가 단순하다는 이유만으로 갈림길·퍼즐·수집 요소·점프·불필요한 코너·추가 오브젝트를 **추가하지 않는다.** 히든박스·모코코·사이드 보상 경로는 삭제 확정. 실제 플레이테스트에서 문제가 확인된 경우에만 수정 검토.

## 정량 데이터 규칙

거리·시간·경사·성공률을 **가정하지 않는다.** UE5 좌표와 실제 플레이테스트로 측정한 값만 쓴다. 측정하지 않은 숫자를 포트폴리오 데이터처럼 제시하지 않는다.

## 버전 보존

레벨디자인 판단이 바뀌는 의미 있는 변경 전에 보존: git commit / 레벨 애셋 복제(`L_*_v01` → `v02`, 덮어쓰기 금지) / 탑뷰 캡처 / 설계 로그 기록. 사소한 수정까지 전부 버전화할 필요는 없다.

설계 로그: `Docs/Portfolio02_Entrance_Design_Log.md` — 확정 / 가설 / 미정 / 삭제 / 검증 항목 / 변경 이력을 구분해 기록.

## 환경 (실측 확인됨, 재조사 불필요)

| 항목 | 값 |
|---|---|
| 엔진 | UE **5.7.4** — `C:\Program Files\Epic Games\UE_5.7` |
| 프로젝트 | `C:\Users\user\Desktop\P02Dungeon\P02Dungeon.uproject` |
| 템플릿 | Third Person (Blueprint) + **Variant = Combat** |
| 타입 | C++ 프로젝트 (`Source/` 는 빈 모듈 — MCP 플러그인 컴파일용) |
| 빌드 | `Build.bat P02DungeonEditor Win64 Development -Project=...` |
| MCP | `Plugins/UnrealMCP/` (kks3800/Unreal_MCP), TCP `127.0.0.1:55557` |
| MCP 설정 | `.mcp.json`, `DYNAMIC_MODE=1` → 19 core + 3 meta + 452 dynamic |
| Python | `Plugins/UnrealMCP/Server/.venv` (3.12.10) |

**빌드는 에디터를 닫고 해야 한다** (Live Coding이 DLL을 잡음).

## 템플릿이 이미 제공하는 것 — 새로 만들지 말 것

`BP_CombatCharacter`(콤보·차지 공격) · `BP_CombatEnemy` + `BP_CombatAIController` + `ST_CombatEnemy`(StateTree) + EQS 4종 · `BP_Combat_EnemySpawner` · `BPI_Damageable`/`BPI_Attacker` · `BP_Combat_ActivationVolume`(트리거) · `BP_Combat_CheckpointVolume` · `BP_Combat_Dummy`/`DamageableBox` · `UI_LifeBar`(HUD) · `LevelPrototyping` 그레이박스 메시(`SM_Cube`, `SM_Ramp`, `SM_Cylinder`, `SM_QuarterCylinder`, `SM_ChamferCube`, `SM_Plane`) + `MI_PrototypeGrid_*` 격자 머티리얼

**남은 기술 구현:** 고정 쿼터뷰 카메라 / 대시 / 던전 입구 문 / START·END 연결 / 측정 로깅

**주의:** `GlobalDefaultGameMode` 가 `BP_ThirdPersonGameMode` 임. 새 레벨은 World Settings에서 `BP_CombatGameMode` 로 오버라이드해야 전투가 동작한다.

## MCP 알려진 제약 (검증 완료)

1. `add_component_to_blueprint` — 짧은 이름 실패. **풀 경로 필수**: `/Script/Engine.StaticMeshComponent`
2. `take_editor_screenshot` — `viewport has zero size` 로 실패(숨겨진 뷰포트 클라이언트를 먼저 잡는 구현 문제). **탑뷰 캡처는 수동 High Resolution Screenshot 사용.** 4분할 레이아웃에서 되는지는 미검증
3. asset 삭제 명령 없음 — 테스트 에셋은 수동 정리
4. Python `mcp` 패키지는 `<2` 고정 유지 (2.0에서 `mcp.server.fastmcp` 제거됨)
5. Niagara 미작동, MetaSound·PCG는 EXPERIMENTAL
6. 머티리얼 C++에 deprecation 경고 다수 — 다음 엔진 버전에서 컴파일 실패 예상
7. BP CDO/상속 컴포넌트 프로퍼티 **조회 툴 없음** → 값 확인은 레벨 스폰 후 `get_actor_properties` (응답 ~59k자, grep 필수)
8. `spawn_blueprint_actor` 는 `/Game/Blueprints/<name>` 하드코딩 + 디스크 저장된 애셋만 인식
9. `find_actors_by_name` 동작 안 함(항상 빈 배열) → `get_actors_in_level` 사용
10. `get_viewport_camera` 의 rotation 값 신뢰 불가 (항상 `[0,0,0]`)
11. `add_blueprint_event_node` 는 엔진 내부 이름 필요 (`ReceiveBeginPlay`)
12. `set_component_property` 는 **상속 컴포넌트 접근 불가** → 자식 BP 대신 복제본 사용
13. `set_component_property` 의 값은 **JSON 원시 타입** 필수 (숫자 `1200`, bool `false`. 문자열은 실패)
14. **`compile_blueprint` 가 에디터를 크래시시킴** (`UnrealMCPBlueprintCommands.cpp:985`, 캐릭터급 BP + 구조체 프로퍼티 기록 후). 컴파일은 에디터 Compile 버튼으로 한다. 복제 직후 `save_asset` 먼저.
15. **캐릭터급 BP 의 컴포넌트 프로퍼티를 MCP 로 만지지 말 것.** `set_component_property` 후에는 저장·컴파일이 둘 다 크래시(순환 함정, `UnrealMCPAssetCommands.cpp:790`). Details 패널에서 수동 설정
16. MCP 명령은 **에디터 부팅 완료 후에만** 전송. 판정: `Saved/Logs/P02Dungeon.log` 에 `LogLoad: Took ... to LoadMap` + 프레임 카운터 > 0

**검증 완료:** Actor CRUD·Transform · BP 읽기/생성/컴파일 · Material introspection · 뷰포트 카메라 · PIE 시작/정지

## 토큰 원칙

Dynamic Mode 유지 — 필요한 툴만 검색해서 호출하고 전체 카탈로그를 반복 조회하지 않는다. 같은 Unreal 상태를 반복해서 읽지 않는다. 이미 확인한 구조를 재탐색하지 않는다. 단, **토큰 절약 때문에 검증을 생략하지 않는다. 정확성이 우선.**
