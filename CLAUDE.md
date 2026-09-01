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

**빌드 경로는 두 개다. 대부분 에디터를 닫을 필요가 없다.**

| 변경 종류 | 방법 | 에디터 |
|---|---|---|
| CVar 값 조정 | `set_cvar` | 그대로 (빌드 불필요) |
| `UPROPERTY` 값 조정 | Details 패널 | 그대로 (빌드 불필요) |
| 함수 내용 수정 | 에디터에서 **`Ctrl+Alt+F11`** (Live Coding) | 그대로 |
| **새 `UCLASS` 추가** | `Build.bat` | **닫아야 함** |
| **`UPROPERTY` 추가/삭제** | `Build.bat` | 닫아야 함 |

`Build.bat` 은 Live Coding이 DLL을 잡고 있으면 실패하므로 그때만 닫는다. 새 클래스·`UPROPERTY` 추가는 **한 번에 몰아서** 처리해 재시작 횟수를 줄인다. 2026-08-27 세션에서 4회 재시작 중 2회는 F11로 충분했던 것.

## 템플릿이 이미 제공하는 것 — 새로 만들지 말 것

`BP_CombatCharacter`(콤보·차지 공격) · `BP_CombatEnemy` + `BP_CombatAIController` + `ST_CombatEnemy`(StateTree) + EQS 4종 · `BP_Combat_EnemySpawner` · `BPI_Damageable`/`BPI_Attacker` · `BP_Combat_ActivationVolume`(트리거) · `BP_Combat_CheckpointVolume` · `BP_Combat_Dummy`/`DamageableBox` · `UI_LifeBar`(HUD) · `LevelPrototyping` 그레이박스 메시(`SM_Cube`, `SM_Ramp`, `SM_Cylinder`, `SM_QuarterCylinder`, `SM_ChamferCube`, `SM_Plane`) + `MI_PrototypeGrid_*` 격자 머티리얼

**남은 기술 구현:** 고정 쿼터뷰 카메라 / 대시 / 던전 입구 문 / START·END 연결 / 측정 로깅

**주의:** `GlobalDefaultGameMode` 가 `BP_ThirdPersonGameMode` 임. 새 레벨은 World Settings에서 `BP_CombatGameMode` 로 오버라이드해야 전투가 동작한다.

## MCP 알려진 제약 (검증 완료)

1. `add_component_to_blueprint` — 짧은 이름 실패. **풀 경로 필수**: `/Script/Engine.StaticMeshComponent`
2. `take_editor_screenshot` — `viewport has zero size` 로 실패(숨겨진 뷰포트 클라이언트를 먼저 잡는 구현 문제). **탑뷰 캡처는 수동 High Resolution Screenshot 사용.** 4분할 레이아웃에서 되는지는 미검증
3. asset 삭제 명령 없음 — 테스트 에셋은 수동 정리
4. Python `mcp` 패키지는 `<2` 고정 유지 (2.0에서 `mcp.server.fastmcp` 제거됨). **도구가 아예 안 잡히면 서버가 import 단계에서 죽은 것부터 의심.** 진단: `cd Plugins/UnrealMCP/Server && DYNAMIC_MODE=1 ./.venv/Scripts/python.exe unreal_mcp_server.py < /dev/null` → `ModuleNotFoundError: No module named 'mcp'` 이면 복구: `uv pip install --python ./.venv/Scripts/python.exe "mcp[cli]>=1.4.1,<2"`. 복구 후 **Claude Code 재시작 필요**(세션 시작 시 실패한 서버는 세션 중 재시도되지 않음). 2026-08-26 발생
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
17. `add_component_to_blueprint` 의 `component_properties` 는 **무시됨** (BoxExtent 넘겼으나 기본값 32 유지)
18. **중첩 구조체 프로퍼티 접근 불가** — `CollisionProfileName` 은 `BodyInstance` 안에 있어 `set_component_property` 가 "not found". 콜리전 프리셋은 에디터에서 수동 또는 지오메트리로 회피
20. **`save_asset` 를 레벨(World)에 쓰지 말 것.** 플러그인이 `GetAssetPackageExtension()` 하드코딩(`UnrealMCPAssetCommands.cpp:785`) → 맵을 `.umap` 이 아니라 **`.uasset` 으로 저장**한다. 같은 패키지 경로에 파일이 2개 생기고, 실제 `.umap` 은 갱신되지 않으며 에디터가 `.umap` 을 다시 읽으면 작업이 날아간다. `set_actor_folders` 도 `save: false` 로 호출. **레벨 저장은 에디터 Ctrl+S(Save Current Level)로만.** 2026-08-26 액터 8개 소실
19. `BoxComponent` 기본 프로파일 `OverlapAllDynamic` 은 **WorldStatic 도 Overlap** → 트리거가 벽·바닥에 닿으면 BeginPlay 즉시 발동. 박스를 정적 지오메트리에서 떼어 배치할 것
16. MCP 명령은 **에디터 부팅 완료 후에만** 전송. 판정: `Saved/Logs/P02Dungeon.log` 에 `LogLoad: Took ... to LoadMap` + 프레임 카운터 > 0

21. **`set_actor_transform` 은 액터 패키지를 dirty 로 표시하지 않는다.** MCP 로 옮긴 액터는 Ctrl+S 와 종료 시 저장 프롬프트에서 **통째로 누락**되고, 에디터를 닫으면 사라진다. 2026-08-27 `L_Entrance_v03` 액터 9개(입구 45° 회전 + 좌측 틈) 소실. 판정: 로그에 `set_actor_transform` 호출은 있는데 이후 `LogFileHelpers: Saving Package: /Game/__ExternalActors__/...` 가 없으면 안 써진 것.
    머티리얼 저장도 `save_directory` 를 쓴다. 강제 저장하면 파일 크기가 눈에 띄게 커진다(`M_WorldGrid_Fadeable` 28,010 → 33,284 bytes).

22. **`set_material_node_property` 는 이전에 MCP 로 추가한 다른 expression 노드를 지운다.** 2026-08-27 `M_WorldGrid_Fadeable` 에서 두 번 재현 — 호출 직후 `OneMinus` + `DitherTemporalAA` 가 사라지고 `OpacityMask` 연결이 끊겼다(39 → 37 노드). `M_PrototypeGrid` 도 107 → 104. **순서를 지킬 것: 파라미터 노드 추가 → `set_material_node_property` 로 프로퍼티 설정 → 그 다음에 나머지 노드 추가·연결.** 한번 그래프를 완성한 뒤에는 이 툴을 다시 호출하지 않는다. 판정: `compile_material_detailed` 의 `total_nodes` 와 `material_outputs[].connected`.
    **해결: `save_directory(directory_path="/Game/__ExternalActors__/Portfolio02/Levels/<맵이름>", recursive=true, only_if_dirty=false)`** — dirty 플래그를 무시하고 강제 저장한다. #20 과 달리 맵을 `.umap` 으로 올바르게 쓰므로 레벨 저장에 안전한 유일한 MCP 경로다. **MCP 로 액터를 옮기거나 지운 뒤에는 반드시 이 호출로 마무리한다.** (2026-08-27 57개 파일 기록 확인)

**검증 완료:** Actor CRUD·Transform · BP 읽기/생성/컴파일 · Material introspection · 뷰포트 카메라 · PIE 시작/정지

## 토큰 원칙

Dynamic Mode 유지 — 필요한 툴만 검색해서 호출하고 전체 카탈로그를 반복 조회하지 않는다. 같은 Unreal 상태를 반복해서 읽지 않는다. 이미 확인한 구조를 재탐색하지 않는다. 단, **토큰 절약 때문에 검증을 생략하지 않는다. 정확성이 우선.**
