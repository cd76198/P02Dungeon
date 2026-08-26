# Portfolio 02 — 외부 진입 구간 설계 로그

- 대상: LOST ARK Mobile 레벨기획 계약직(신규) 지원용 Portfolio 02 「UE5 쿼터뷰 던전 레벨디자인」
- 제출 목표: 2026-08-31
- 이 문서의 범위: **START ~ 던전 입구까지의 외부 진입 구간**. 던전 내부는 미설계.
- 작성 시작: 2026-08-23

---

## 0. 작업 역할 경계 (고정)

| 구분 | 내용 |
|---|---|
| Claude Code 담당 | UE5 기술 구현, 이동/대시, 최소 공격·피격, 테스트용 적, 처치 판정, 문/트리거, START·END, 최소 HUD, 쿼터뷰 카메라, BP/C++, 반복작업 자동화, 테스트 환경, 측정/문서 정리 |
| 본인(설계자) 전담 | 레벨 레이아웃, 지형·높낮이, 벽 위치·높이, 이동 동선, 전투 공간 크기·형태, 입구/출구 위치, 주요 오브젝트 배치, 시야 개방·차단, 목표 인지 방식, 보스룸 공간, 플레이테스트 후 수정안 |

원칙: 시스템은 도움 받되, **플레이어가 어떤 공간을 경험할지는 설계자가 직접 결정한다.**
공간 문제가 보이면 임의 수정 금지 — 문제와 이유를 보고하고 수정 방향은 설계자가 정한다.

---

## 1. 확정 사항 (FIXED)

### 1-1. 플레이 흐름
```
START
 → 전진(거의 직진)
 → 높은 위치의 붉은 시각 단서 인지
 → 접근하며 던전 입구 방향/일부 인지
 → 정면 접근 불가능 확인
 → 우측 우회 경로 진입
 → 오르막 상승
 → 던전 입구 도달
```

### 1-2. 공간 조건
- 플레이어는 **개방된 야외 공간**에서 시작한다.
- START 직후에는 던전 입구를 **명확히 알 수 없다.**
- 던전 입구는 **START보다 높은 위치**에 있다.
- 입구로 **정면 접근은 불가능**하다.
- 우회 경로는 **우측**으로 돌아가며, 오르막으로 입구 높이까지 상승한다.
- 진행 기본 방향은 러프 탑뷰 기준 **화면 상단**.

### 1-3. 시각 단서의 성격
- 붉은 빛(또는 유사 시각 단서)은 **입구를 직접 지시하는 정답 마커가 아니다.**
- 역할은 "저 방향에 중요한 목적지가 있다"는 **보조적 암시**까지.
- 메인 유도는 **공간 구조**(지형 / 경로 방향 / 경사 / 벽·공간 경계 / 시야 개방·차단)가 담당한다.
- 바닥 색만 따라가면 풀리는 구조로 만들지 않는다.

### 1-4. 이 구간의 레벨디자인 목적
1. **단계적 목표 인지** — 시각적 징후 → 목적지 방향 → 실제 입구 순으로 정보 획득
2. **높이를 정보 전달에 사용** — 장식적 수직성이 아니라 목적지 암시 / 직접접근과 실제경로 구분 / 이동 중 시야 변화 / 공간 전환감
3. **직접 접근이 아닌 우회 접근** — 우회하며 현재 위치와 목적지의 공간 관계를 이해
4. **공간 우선 유도** — 조명·색은 보조 수단

### 1-5. 추가 금지 규칙 (현 시점)
검증 전 선제적 추가 금지 항목: 갈림길, 퍼즐, 상호작용, 점프, 수집 요소, 불필요한 방향 전환, 히든박스, 모코코류 수집물, 사이드 보상 경로.
"단순해 보인다"는 이유는 추가 근거가 되지 않는다. 별도 요청 전까지 현재 형태 유지.

### 1-6. 요소 추가 판정 기준
어떤 요소든 추가하려면 다음 두 질문에 답할 수 있어야 한다.
- 왜 필요한가?
- 이 요소 때문에 **플레이어 행동 또는 정보 인지**가 무엇이 달라지는가?

"심심해서 / 보기 좋아서 / 복잡해 보이려고"는 사유가 아니다.

---

## 2. 현재 가설 (HYPOTHESIS — UE5 플레이테스트로 검증 필요)

> 아래는 "그렇게 될 것으로 의도한" 항목이며 아직 증명되지 않았다.

| # | 가설 | 반증 시 나타날 현상 |
|---|---|---|
| H1 | 붉은 시각 단서만으로 플레이어가 전진 방향을 잡는다 | 시작 지점에서 방향을 못 잡고 배회 |
| H2 | 접근 과정에서 입구(또는 일부)를 자연스럽게 인지한다 | 입구를 못 보고 지나침 / 처음부터 다 보여서 단계적 인지가 사라짐 |
| H3 | 정면 접근 불가를 플레이어가 "막혔다"가 아니라 "돌아가야 한다"로 읽는다 | 벽 앞에서 정지, 길을 잃었다고 판단 |
| H4 | 우측 우회 경로를 별도 안내 없이 발견한다 | 우회 입구를 못 찾음 |
| H5 | 오르막 상승 중 시야 변화로 목적지 접근감을 느낀다 | 상승 구간이 무미건조한 이동 시간으로만 느껴짐 |
| H6 | 현재의 단순한 우회 경로가 지루하지 않다 | 이동 중 공간 정보가 전혀 변하지 않아 단조로움 확인 |

### H6 반증 시에만 검토할 수정 후보 (현재 미확정, 선제 적용 금지)
- 이동 중 입구의 가림 / 재노출
- 상승에 따른 시야 변화
- 경로 방향의 1회 변화
- 경로 폭 변화

---

## 3. 미정 사항 (UNDECIDED — 임의 결정 금지)

수치는 만들어내지 않는다. UE5에서 실제로 배치·측정한 뒤 이 문서에 기록한다.

**외부 구간**
- 외부 공간의 정확한 크기
- START ↔ 입구 실제 거리
- 경사도
- 고저차 수치
- 경로 폭
- 벽 높이
- 붉은 시각 단서의 정확한 형태
- 입구가 처음 보이는 정확한 위치
- 우회 중 입구가 가려지는지 여부
- 최종 환경 테마 디테일

**외부 구간 이후**
- 던전 내부 레이아웃
- 중간보스 공간
- 최종보스 공간
- 내부 전투 / 기믹
- 실제 플레이 시간

---

## 4. 삭제한 아이디어 (DROPPED)

| 아이디어 | 삭제 사유 |
|---|---|
| 메인 경로 옆 짧은 보너스 분기 | 외부 진입 구간의 핵심 목적과 직접 관계 없음 / 메인 진행 구조 검증 방해 |
| 히든박스 | 위와 동일 |
| 모코코형 수집 요소 | IP 상징 요소보다 LOST ARK Mobile의 장르·쿼터뷰 특성에 맞는 공간 설계 증명이 우선 |

별도 요청 전까지 재도입하지 않는다.

---

## 5. UE5에서 검증할 항목 (TEST BACKLOG)

### 5-1. 측정 (가정 금지, 실측)
- [ ] START ↔ 던전 입구 직선 거리 / 실제 이동 거리 (uu)
- [ ] START ↔ 입구 고저차 (uu)
- [ ] 우회 경로 경사도 (deg)
- [ ] 경로 폭 (uu)
- [ ] 구간별 통과 시간 (초) — START→단서 인지 / 단서→정면 도달 / 우회 진입→입구 도달
- [ ] 고정 쿼터뷰 기준 화면에 들어오는 유효 시야 범위

### 5-2. 인지 검증 (H1~H6 대응)
- [ ] START 위치에서 붉은 단서가 화면에 보이는가 (쿼터뷰 고정 각도 기준)
- [ ] 입구가 최초로 보이는 지점은 어디인가 → 좌표 기록
- [ ] 정면 벽 도달 시 우회 경로가 시야에 들어오는가
- [ ] 우회 경로 진입까지 걸린 시도 횟수 / 배회 여부
- [ ] 상승 중 시야 정보가 실제로 변하는가 (변하지 않으면 H6 반증)

### 5-3. 기록물 (설계 증거 보존)
- [ ] 러프 탑뷰 (초기안) 보존
- [ ] UE5 탑뷰 캡처 (버전별)
- [ ] 실제 플레이 화면 영상
- [ ] 각 버전 시점의 설계 의도 / 발견 문제 / 수정 이유

**원칙: 최초 버전을 덮어쓰지 않는다.** 중요 변경 전후로 위 자료를 분리 보존한다.

---

## 6. 버전별 변경 기록 (CHANGELOG)

| 버전 | 날짜 | 상태 | 내용 | 근거 |
|---|---|---|---|---|
| v0.1 | 2026-08-23 | 개념 확정 | 러프 탑뷰 기준 외부 진입 구간 흐름 확정 (START → 붉은 단서 → 입구 인지 → 정면 불가 → 우측 우회 → 상승 → 입구 도달). 보너스 분기·히든박스·모코코 삭제. | 설계자 결정 |
| | | | UE5 구현 전 — 수치 전무 | |

> 기록 규칙: 변경할 때마다 **무엇을 / 왜 / 어떤 테스트 결과 때문에** 를 함께 남긴다. 테스트 근거 없는 변경은 "설계자 결정"으로 명시한다.

---

## 7. 프로젝트 환경 현황 (2026-08-23)

- 엔진: UE 5.7 (`C:\Program Files\Epic Games\UE_5.7`)
- 프로젝트: `C:\Users\user\Desktop\P02Dungeon\P02Dungeon.uproject`
- 템플릿: Games → Third Person (Blueprint), **Variant = Combat**
- 이 문서 위치: `P02Dungeon/Docs/`
- 충돌하는 기존 구현/문서: 없음
- 참고: `C:\Users\user\unreal-mcp` 는 에디터 원격 제어용 MCP 도구 저장소(외부 샘플). 본 프로젝트와 무관.

### 7-1. 템플릿이 이미 제공하는 것 (직접 구현 불필요)

| 필요 항목 | 기존 애셋 |
|---|---|
| 이동 / 입력 | `Input/IMC_Default`, `IA_Move`, `IA_Look`, `IA_Jump` |
| 최소 공격 | `IA_ComboAttack`, `IA_ChargedAttack`, `AM_ComboAttack`, `AM_ChargedAttack` |
| 피격 / 데미지 | `BPI_Damageable`, `BPI_Attacker`, `AN_AttackDamage`, `NS_Damage` |
| 테스트용 적 | `BP_CombatEnemy`, `BP_CombatAIController`, `ST_CombatEnemy`(StateTree), EQS 4종 |
| 적 스폰 | `BP_Combat_EnemySpawner` |
| 테스트 타깃 | `BP_Combat_Dummy`, `BP_Combat_DamageableBox` |
| 트리거 볼륨 | `BP_Combat_ActivationVolume` |
| 체크포인트 | `BP_Combat_CheckpointVolume` |
| 최소 HUD | `UI_LifeBar` |
| 그레이박스 메시 | `LevelPrototyping/Meshes`: `SM_Cube`, `SM_Ramp`, `SM_Cylinder`, `SM_QuarterCylinder`, `SM_ChamferCube`, `SM_Plane` |
| 그레이박스 머티리얼 | `MI_PrototypeGrid_Gray`, `_Gray_02`, `_Round`, `_TopDark`, `M_FlatCol` |
| 모바일 조작 참고 | `Input/Touch/UI_Thumbstick` (가상 조이스틱) |

### 7-2. 남은 기술 구현 항목

- [ ] 고정 쿼터뷰 카메라 (SpringArm 각도/거리/FOV를 조정 가능한 변수로 노출)
- [ ] 대시
- [ ] 던전 입구 문 / 진입 트리거
- [ ] START / END 판정
- [ ] 측정 유틸 (구간 통과 시간, 이동 거리 로깅)

### 7-3. 기본 설정 현황 / 조치 필요

- `GameDefaultMap` = `/Game/ThirdPerson/Lvl_ThirdPerson` (템플릿 기본)
- `GlobalDefaultGameMode` = `BP_ThirdPersonGameMode` — **Combat 게임모드가 아님.**
  → 신규 레벨(`L_Entrance_v01`) 생성 시 World Settings에서 `BP_CombatGameMode` 로 오버라이드 필요.
- 캐릭터 무브먼트 실측값은 **아직 미확인**. `BP_CombatCharacter` 를 열어 실제 값을 읽고 기록할 것. 추정 금지.

| 항목 | 실측값 | 확인일 |
|---|---|---|
| 캡슐 반높이 / 반지름 | 90 / 35 (uu) | 2026-08-24 |
| Max Walk Speed | 400 uu/s | 2026-08-24 |
| Walkable Floor Angle | 44.765083° (WalkableFloorZ 0.71) | 2026-08-24 |
| Max Step Height | 45 uu | 2026-08-24 |

측정 방법: `BP_CombatCharacter` 를 `/Game/Blueprints/TMP_ProbeCharacter` 로 복제 → 레벨에 스폰 → MCP `get_actor_properties` 로 CapsuleComponent / CharacterMovement 값을 직접 읽음 → 액터·임시 애셋 정리. (MCP 에는 BP CDO 프로퍼티 조회 툴이 없어 스폰 후 읽는 우회가 필요했음)

같은 덤프에서 함께 확인된 값 (참고, 표 항목은 아님):
`MaxAcceleration` 2048 · `BrakingDecelerationWalking` 2048 · `GroundFriction` 8 · `JumpZVelocity` 420 · `GravityScale` 1 · `CrouchedHalfHeight` 40

파생 기준값
- 캐릭터 전체 높이 = 180 uu, 폭 = 70 uu → 통로 폭·천장 높이 판단 기준
- 경사 상한 = **44.765°** 초과 시 걸어 올라갈 수 없음 (기울기 약 1:1.01). 오르막 우회 경로 설계 시 상한.
- 45 uu 이하 단차는 계단/장애물이 아니라 그냥 걸어 넘어감 → 걸림 연출용 단차는 46 uu 이상 필요
- 거리↔시간 (등속 400 uu/s 기준, 가감속·대시 미반영): 1,000 uu ≈ 2.5 s · 2,000 uu = 5 s · 4,000 uu = 10 s · 10,000 uu = 25 s
  → 2~3분 분량 중 외부 진입 구간 목표 시간이 정해지면 이 환산으로 거리 상한을 역산한다. 실제 플레이테스트 시간은 별도 실측 필요(가감속·회전·대시 영향).

### 7-4. Unreal MCP 환경 (2026-08-24 구축)

- 플러그인: `kks3800/Unreal_MCP` → `Plugins/UnrealMCP/` (별도 git repo, `.gitignore` 처리)
- 서버: `Plugins/UnrealMCP/Server/.venv` (Python 3.12.10, uv)
- 설정: `P02Dungeon/.mcp.json`, `DYNAMIC_MODE=1` → 19 core + 3 meta + 452 dynamic
- 통신: TCP `127.0.0.1:55557`
- 프로젝트가 C++ 프로젝트로 전환됨 (`Source/`, 엔진 TP_Blank 템플릿 기반, 빈 모듈)

**검증된 기능** — Actor CRUD / Transform / BP 읽기·생성·컴파일 / Material introspection / 뷰포트 카메라 / PIE 시작·정지

**알려진 제약**
1. `add_component_to_blueprint` — 짧은 이름(`StaticMeshComponent`) 실패. **풀 경로(`/Script/Engine.StaticMeshComponent`) 필수.** UE5.1+에서 `FindObject` 단축명 검색이 없어진 것을 플러그인이 반영 못 함
2. `take_editor_screenshot` — `viewport has zero size` 로 실패. 숨겨진 뷰포트 클라이언트를 먼저 잡는 구현 문제. **탑뷰 캡처 자동화는 현재 불가 → High Resolution Screenshot 수동 사용**
3. asset 삭제 명령 없음 — 테스트 에셋은 수동 정리
4. `mcp` 파이썬 패키지는 `<2` 고정 필요 (repo가 상위 버전 파괴 미반영)
5. 머티리얼 C++ 코드에 deprecation 경고 다수 — 다음 엔진 버전에서 컴파일 실패 예상
6. **BP CDO / 상속 컴포넌트 프로퍼티 조회 툴 없음.** `read_blueprint_content` 는 컴포넌트 이름·클래스만 주고 값을 안 줌 → 값을 읽으려면 레벨에 스폰 후 `get_actor_properties`
7. `spawn_blueprint_actor` — 경로가 `/Game/Blueprints/<name>` 로 하드코딩. 다른 폴더의 BP는 그 경로로 복제해야 스폰 가능하고, **디스크에 저장돼 있어야** 인식됨(미저장 복제본은 not found)
8. `get_actor_properties` 응답이 Character 기준 약 59,000자 → 통째로 읽지 말고 파일로 받아 grep

### 7-6. MCP 기능 재검증 (2026-08-24, 세션 2)

임시 오브젝트만 사용하고 전부 정리함. 포트폴리오 공간·Blueprint 는 수정하지 않음.

| 영역 | 테스트 | 결과 |
|---|---|---|
| 환경 | `.mcp.json` `DYNAMIC_MODE=1`, `.uproject` 에 `UnrealMCP` Enabled, venv Python 3.12.10, node v24.16.0, uv 0.11.2 | 정상 |
| Dynamic Mode | core 툴 직접 호출 + `search`/`describe`/`execute_unreal_tool` 3종 메타 툴로 비core 툴 실행 | 정상 |
| Editor | `get_editor_context` (레벨·PIE·활성탭), `get_actors_in_level` (74개) | 정상 |
| Actor CRUD | `spawn_actor` StaticMeshActor → `set_actor_transform` (loc/rot/scale) → 목록 재확인 → `delete_actor` | 정상 |
| Actor 검색 | `find_actors_by_name` — `TMP_*`, `TMP`, 정확한 이름 모두 `[]` 반환 | **실패** |
| Blueprint | `create_blueprint`(Actor) → `add_component_to_blueprint`(풀 경로) → `add_blueprint_variable`(float) → `add_blueprint_event_node`(`ReceiveBeginPlay`) → `compile_blueprint` | 정상 |
| BP 노드 이름 | `event_name="BeginPlay"` 실패, `"ReceiveBeginPlay"` 성공 → **엔진 내부 이름 필요** | 부분 정상 |
| Material | `get_material_instance_parameters` (MI_PrototypeGrid_Gray: scalar 3 / vector 4), `set_actor_material` 스왑 성공 | 정상 |
| PIE | `start_pie` → `is_pie_active` true → `stop_pie` | 정상 |
| Viewport | `get_viewport_camera` — location·fov 정상, **rotation 이 항상 `[0,0,0]`** | 부분 정상 |
| Screenshot | `take_editor_screenshot` — `viewport has zero size` | **실패** (기존 제약 재확인) |
| Unreal Python | `PythonScriptPlugin` 미활성. MCP 플러그인은 C++ TCP 기반이라 불필요 | 해당 없음 |

**추가로 확인된 제약**
9. `find_actors_by_name` 동작 안 함 → 액터 조회는 `get_actors_in_level` 사용 (74개 레벨에서 응답이 큼, 반복 호출 금지)
10. `get_viewport_camera` 의 rotation 값 신뢰 불가 → 카메라 각도는 에디터에서 직접 확인
11. `add_blueprint_event_node` 는 엔진 내부 이벤트 이름 필요 (`ReceiveBeginPlay` / `ReceiveTick`)

**측정 기준 확보** — 7-3 표의 캐릭터 실측값(캡슐 90/35, 400 uu/s, 44.765°, 45 uu). 거리·시간은 레벨이 만들어진 뒤 실측한다.

### 7-7. 쿼터뷰 카메라 1차 시도 — 에디터 크래시 (2026-08-24)

**결과: 중단.** `compile_blueprint` 호출에서 에디터가 죽었다. 프로젝트 손상은 없음(디스크의 `Content/` 무변경, git 상태 문서 3개만 수정).

작업 순서
1. `duplicate_asset` : `BP_CombatCharacter` → `/Game/Blueprints/BP_QVCharacter` (템플릿 원본 보호. 자식 BP 는 상속 컴포넌트에 `set_component_property` 가 안 먹어서 복제 선택)
2. `set_component_property` 로 Camera Boom 고정 세팅 성공 — `bUsePawnControlRotation=false`, `bAbsoluteRotation=true`, `bInheritPitch/Yaw/Roll=false`, `bDoCollisionTest=false`, `bEnableCameraRotationLag=false`, `TargetArmLength=1200`(임시), Camera `FieldOfView=45`(임시)
3. `set_component_property` `RelativeRotation=[-50,0,0]` (임시) — 툴은 success 반환
4. `compile_blueprint` → **EXCEPTION_ACCESS_VIOLATION**

크래시 지점 (`Saved/Logs/P02Dungeon.log`)
```
UnrealMCPBlueprintCommands.cpp:985  FUnrealMCPBlueprintCommands::HandleCompileBlueprint()
  → FKismetEditorUtilities::CompileBlueprint(Blueprint)
EXCEPTION_ACCESS_VIOLATION reading address 0xffffffffffffffff
```
직전 로그 마지막 줄: `SetComponentProperty - Handling struct property RelativeRotation of type Rotator`

**원인 후보 (미확정)** — ① 구조체(Rotator) 프로퍼티 기록 후 BP 가 불완전한 상태로 남아 컴파일이 터짐 ② 복잡한 캐릭터 BP 복제본에 대한 MCP 컴파일 경로 자체가 불안정. 단순 BP(`TMP_TestBP`) 컴파일은 같은 세션에서 정상이었다.

**대응 규칙 (확정)**
- MCP 로는 **bool / 숫자 프로퍼티만** 설정한다.
- **`compile_blueprint` 를 캐릭터급 BP 에 쓰지 않는다.** 컴파일은 에디터 Compile 버튼으로.
- 복제 직후 `save_asset` 으로 먼저 저장한다. 저장 전 크래시는 작업 전체 소실.
- 카메라 Pitch / Yaw / Arm Length / FOV 는 어차피 **설계자가 화면 보고 정할 값**이므로 MCP 로 건드리지 않고 Details 패널에서 직접 조정한다.

**추가로 확인된 제약**
12. `set_component_property` 는 상속 컴포넌트에 접근 불가 → 컴포넌트 세팅을 바꿔야 하면 자식 BP 가 아니라 **복제본**이 필요
13. `set_component_property` 의 `property_value` 는 **JSON 원시 타입**이어야 함. 문자열 `"1200"` / `"false"` 는 실패(`Numeric property requires a number value`), 숫자 `1200` / bool `false` 는 성공
14. **`compile_blueprint` 가 에디터를 죽일 수 있다** (위 크래시). 캐릭터급 BP 에는 사용 금지

### 7-8. 크래시 2회차 — 원인 확정 (2026-08-24)

같은 카메라 작업을 안전 절차(복제 → 즉시 저장 → bool/숫자만 설정 → compile 호출 안 함)로 재시도했으나 **`save_asset` 에서 또 크래시.**

```
UnrealMCPAssetCommands.cpp:790  HandleSaveAsset()
  → UPackage::SavePackage(Package, Asset, ...)
EXCEPTION_ACCESS_VIOLATION reading 0xffffffffffffffff
```

**원인 확정 — `set_component_property` 후에는 BP 를 저장도 컴파일도 할 수 없다.**
- 플러그인 소스 `UnrealMCPAssetCommands.cpp:778` 주석에 이미 적혀 있음: *"in a state where recompile null-derefs. Caller should explicitly call compile_blueprint before save if a recompile is required."*
- 즉 컴포넌트 프로퍼티를 바꾸면 저장 전에 컴파일이 필요한데, **그 `compile_blueprint` 가 7-7 에서 크래시**했다. 순환 함정이라 MCP 만으로는 빠져나갈 수 없다.

**부차 원인 — 부팅 중 명령 수신.** 크래시 로그의 프레임 카운터가 끝까지 `[  0]` 이고 `LogLoad: Took ... to LoadMap` 이 **한 번도 안 찍혔다.** MCP TCP 리스너가 에디터 부팅 완료 전부터 명령을 받는다. 1·2차 크래시 모두 맵 로드가 끝나기 전 상태에서 발생.

**확정 규칙 (추가)**
15. **캐릭터급 BP 의 컴포넌트 프로퍼티는 MCP 로 만지지 않는다.** Details 패널에서 직접 설정. MCP 는 액터/트랜스폼/측정/신규 단순 BP 에만 사용.
16. MCP 명령은 **에디터 부팅 완료 후에만** 보낸다. 판정 기준: `Saved/Logs/P02Dungeon.log` 에 `LogLoad: Took ... to LoadMap` 이 찍히고 프레임 카운터가 0 을 벗어날 때.

**피해** — 없음. `Content/Blueprints/BP_QVCharacter.uasset` 은 복제 직후 저장본(카메라 세팅 미적용, 849KB)이 디스크에 남아 있다. 템플릿 원본·포트폴리오 자산 무변경.

### 7-9. 고정 쿼터뷰 카메라 — 설정 명세 (수동 적용)

`BP_QVCharacter` 는 `BP_CombatCharacter` 의 복제본이다(템플릿 원본 보호). Camera Boom / Camera 에 아래 값을 Details 패널에서 적용한다.

**구조 플래그 — 기술 요구사항, 이 값이어야 카메라가 고정된다**

| 컴포넌트 | 프로퍼티 | 값 | 이유 |
|---|---|---|---|
| Camera Boom | Use Pawn Control Rotation | **false** | 컨트롤러 회전이 카메라를 돌리지 않게 |
| Camera Boom | Absolute Rotation (Rotation 옆 월드/로컬 토글) | **World** | 캐릭터가 돌아도 카메라 각도 고정 |
| Camera Boom | Inherit Pitch / Yaw / Roll | **전부 false** | 폰 회전 상속 차단 |
| Camera Boom | Do Collision Test | **false** | 뒤쪽 지형에 카메라가 끌려오지 않게 |
| Camera Boom | Enable Camera Rotation Lag | **false** | 고정 각도이므로 회전 랙 무의미 |

**설계자가 화면 보고 정할 값 — 아래는 시작점일 뿐 확정값이 아니다**

| 컴포넌트 | 프로퍼티 | 시작값 |
|---|---|---|
| Camera Boom | Rotation (Pitch) | -50° |
| Camera Boom | Rotation (Yaw) | 0° |
| Camera Boom | Target Arm Length | 1200 |
| Camera | Field of View | 45 |
| Camera Boom | Enable Camera Lag / Camera Lag Speed | 취향 (템플릿 기본 true / 10) |

**주의 — Yaw 와 이동 입력 방향의 관계.** Camera Boom Yaw 를 0 이 아닌 값으로 정하면 W 키가 화면 위쪽이 아닌 방향으로 가는 문제가 생길 수 있다(템플릿 `Move` 가 컨트롤 회전을 기준으로 입력을 변환). Yaw 확정 후 PIE 로 확인하고, 어긋나면 컨트롤 회전을 카메라 Yaw 에 맞추는 처리를 추가한다. **Yaw 값 확정 전에는 손대지 않는다.**

### 7-5. Git 버전 관리

- `16bb86f` 템플릿 순정 상태 (C++ 전환 전)
- `0229bdd` C++ 전환 + MCP 연동
- 추적: `Content/` 포함. 제외: `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `*.sln`, `Plugins/UnrealMCP/`

### 7-10. 외부 진입부 Graybox 재구성 v0.1 (2026-08-24)

**상태: TEST VALUE / 미확정.** 새 공간 아이디어를 추가하지 않고 `직진 접근 → 높은 붉은 단서 → 정면 차단 → 우측 우회 → 상승 → 높은 입구` 관계만 복제 맵에 구현했다.

**초기 구현**
- 원본 `/Game/ThirdPerson/Lvl_ThirdPerson`: PlayerStart `(0, 0, 302.01)`, StaticMeshActor 55개인 평면형 Third Person 템플릿.
- 높은 입구, 정면 차단, 우측 단일 우회, 상승 경로가 별도 구조로 존재하지 않았다.
- 수정 전 플레이 화면: `Saved/Screenshots/Portfolio02_Entrance_Before_Play.png`.
- 원본은 수정하지 않고 World Partition 전용 복제로 `/Game/Portfolio02/Levels/L_Entrance_v01`을 생성했다.

**수정 내용 — 모든 수치는 TEST VALUE / 미확정**

| 항목 | 위치 / 크기 / 값 |
|---|---|
| START | `(0, 0, 100)`, Yaw `0°` |
| 하부 지면 | 중심 `(600, 0, -50)`, `2000 × 2000 × 100uu` |
| 정면 고지대/차단 | 중심 `(1600, -250, 250)`, `1000 × 1500 × 500uu` |
| 우측 상승 Ramp | 중심 `(1400, 700, 250)`, 길이 `1118.03uu`, 폭 `400uu`, 두께 `100uu`, Pitch `26.565°` |
| 상부 접근면 | 중심 `(2100, 700, 450)`, `400 × 400 × 100uu` |
| 우측 외곽 경계 | 중심 `(1700, 1050, 125)`, `1600 × 100 × 250uu` |
| 입구 중심 관계 | `X=2300`, `Y=700`, 바닥 높이 `Z=500` |
| 입구 기둥 | `(2300, 525, 900)`, `(2300, 875, 900)`, 각 `150 × 150 × 800uu` |
| 입구 상부 보 | `(2300, 700, 1300)`, `150 × 500 × 200uu` |
| 붉은 단서 | `(1500, 350, 700)`, Emissive Cube `100 × 100 × 200uu` |
| 붉은 보조 Light | Intensity `500`, Attenuation Radius `250uu`, Source Radius `20uu` |
| 테스트 카메라 | Pitch `-50°`, Yaw `0°`, Distance `1700uu`, FOV `45` |

**검증 기록**
- Map Check: `0 Error / 0 Warning`.
- 별도 Game 실행의 START 프레임에서 입구 전체는 노출되지 않았다.
- 직진 600uu 지점에서 정면 고지대와 우측 상승 구조가 같은 화면에 들어왔다.
- 정면 차단 직전 1000uu 지점에서 벽이 실제 공간 구조로 진로를 막고 우측 경사가 열렸다.
- 경사 중간 프레임에서 플레이어와 상승면, 입구 프레임이 함께 보였고 플레이어 완전 가림은 없었다.
- 초기 보조 Light 범위가 넓어 경사 근접 화면이 과노출되는 문제를 확인했고, 최종값을 Radius `250uu`의 작은 한 점 조명으로 축소했다.
- 최종 작은 조명값 저장 후 추가 화면 검증은 사용자 요청에 따라 진행하지 않았다. 에디터 PIE가 아닌 별도 Game 실행 관찰이므로 최종 PIE 판정은 보류한다.

**보존 자료**
- 수정 전: `Saved/Screenshots/Portfolio02_Entrance_Before_Play.png`
- 장거리 실패값: `Saved/Screenshots/Portfolio02_Entrance_v01_LongDistance_Test.png`
- 정면 차단/단서 문제: `Saved/Screenshots/Portfolio02_Entrance_DirectBlock_NoCue.png`
- 조정 후 START: `Saved/Screenshots/Portfolio02_Entrance_After_START_FinalTest.png`
- 조정 후 접근: `Saved/Screenshots/Portfolio02_Entrance_After_Approach600_Final.png`
- 과노출 문제: `Saved/Screenshots/Portfolio02_Entrance_Ramp_Overexposure_Test.png`

**미확정 / 사용자 판단 필요**
- START에서 붉은 단서가 처음 인지되는 정확한 시점.
- 정면 고지대까지의 접근 거리, 우측 경로 폭, 고저차 `500uu`, 경사 `26.565°`.
- 최종 작은 한 점 조명의 밝기와 색감.

### 7-11. 붉은 단서 방향성 변경 v02 (2026-08-25)

**상태: TEST VALUE / 미확정.** 공간 배치는 변경하지 않고 `v01`을 보존한 뒤 `/Game/Portfolio02/Levels/L_Entrance_v02`를 생성했다.

- 기존 Point Light 및 반복 작업 중 남을 수 있는 동일 이름 광원을 정리했다.
- 붉은 단서 위치 `(1500, 350, 700)`에 `L_RedEntranceSpot_TEST` 하나만 배치했다.
- START `(0, 0, 100)`을 향하는 회전: Pitch `-21.28°`, Yaw `-166.87°`, Roll `0°`.
- Spot Light: Intensity `1000`, Attenuation Radius `1800uu`, Inner Cone `3°`, Outer Cone `7.5°`, Source Radius `20uu`.
- 좁은 원뿔 형태로 한 방향만 비추며, 방향은 액터 Rotation으로 사용자가 변경할 수 있다.
- 저장 후 읽기 검증: SpotLight `1개`, 위치·회전·광폭 값 일치, Map Check `0 Error / 0 Warning`.

### 7-12. 붉은 단서 광폭·광량 조정 (2026-08-25)

**상태: TEST VALUE / 미확정.** 사용자의 “좁고 세게” 요청에 따라 공간 배치와 조명 방향은 유지하고 광원 파라미터만 변경했다.

- Intensity: `1000` → `12000 cd`
- Inner Cone: `3°` → `1.5°`
- Outer Cone: `7.5°` → `3.5°`
- Source Radius: `20uu` → `0uu`, Soft Source Radius: `0uu`
- Attenuation Radius: `1800uu` 유지
- Indirect Lighting Intensity: `1` → `0`
- Affect Global Illumination: `true` → `false`
- Volumetric Scattering Intensity: `2` → `0.75`
- START 방향 회전은 변경하지 않았다.
- 광폭과 광량은 PIE 화면 확인 전이므로 최종값으로 확정하지 않는다.

### 7-13. 공기 중 붉은 빔 경로 표시 (2026-08-25)

**상태: TEST VALUE / 미확정.** 사용자가 바닥 투사광이 아니라 빛이 진행하는 경로 자체가 보여야 한다고 요청했다. 공간 배치와 SpotLight의 현재 위치·회전은 변경하지 않았다.

- 기존 `ExponentialHeightFog`의 Volumetric Fog를 활성화했다.
- 기존 Fog Density `0.0436`은 유지했다.
- Volumetric Fog Scattering Distribution: `0.8`
- Volumetric Fog Extinction Scale: `0.05`
- 붉은 SpotLight Volumetric Scattering Intensity: `25`
- 붉은 SpotLight Cast Volumetric Shadow: `true`
- Directional Light Volumetric Scattering Intensity: `0`으로 설정해 전역 안개광을 억제했다.
- 붉은 SpotLight의 Intensity `12000 cd`, Inner Cone `1.5°`, Outer Cone `3.5°`는 유지했다.
- 공기 중 빔의 최종 가시성과 과도한 안개 발생 여부는 플레이 화면 확인이 필요하다.
- 최초 MCP 직접 수정은 World Partition 외부 Actor 패키지를 dirty 상태로 만들지 않아 저장되지 않았다. 별도 프로세스 저장도 열린 에디터의 파일 잠금(Windows Error `32`)으로 실패했다.
- 에디터 종료 후 외부 Actor 패키지 3개를 직접 저장했으며 `saved=True`를 확인했다.
- 새 Unreal 프로세스 재로딩 검증: Intensity `12000`, Inner/Outer `1.5°/3.5°`, Indirect `0`, Volumetric `25`, Cast Volumetric Shadow `true`, Volumetric Fog `true`, Distribution `0.8`, Extinction `0.05` 유지.
- 재실행 후 `L_Entrance_v02` 로드 및 Map Check `0 Error / 0 Warning` 확인.

### 7-14. 붉은 방향광 SpotLight 1차안 (2026-08-25)

**상태: TEST VALUE / 사용자 판단 대기.** 사용자의 새 지시에 따라 기존 붉은 빛 구현을 중단하고 SpotLight만 사용하는 1차 화면을 구성했다.

- 길을 막던 `VFX_RedEntranceBeam_TEST`를 제거했다.
- 직접 시선을 끌던 `GB_RedEntranceCue_TEST` 발광 메시를 제거했다.
- Volumetric Fog와 SpotLight Volumetric Scattering을 비활성화했다.
- 기존 레벨 구조는 수정하지 않았다.
- SpotLight 임시 위치: `(2250, 700, 900)` — 기존 던전 입구 그레이박스 중심을 기준으로 한 TEST VALUE.
- 조사 목표: PlayerStart `(0, 0, 100)`.
- 회전: Pitch `-18.752588°`, Yaw `-162.718502°`, Roll `0°`.
- Intensity `10000 cd`, Attenuation Radius `2800uu`, Inner Cone `2°`, Outer Cone `4°`, Indirect Lighting `0`, Volumetric Scattering `0`.
- PIE 캡처: `Saved/Screenshots/P02_SpotLight_FirstPass.png`.
- 실제 관찰: START 화면에서는 붉은 방향광 또는 붉은 시각 단서가 보이지 않았다. 사용자의 화면 판단 전 추가 조정하지 않는다.

### 7-15. 공기 중 방향성 빔 1차안 (2026-08-25)

**상태: TEST VALUE / 사용자 판단 대기.** SpotLight만으로는 START 화면에서 경로가 보이지 않아, 사용자 지시에 허용된 충돌 없는 Additive/Unlit 테스트 빔을 추가했다. 레벨 구조와 카메라는 변경하지 않았다.

- 광선 연결: 입구 측 `(2250, 700, 900)` → PlayerStart `(0, 0, 100)`.
- 중심 `(1125, 350, 500)`, 회전 Pitch `-18.752588°`, Yaw `-162.718502°`, Roll `0°`.
- 테스트 메시 스케일: `(24.8847, 0.22, 0.22)`.
- 메시 애셋: `/Game/Portfolio02/Graybox/SM_RedBeam_NoCollision_TEST`.
- 머티리얼: `/Game/Portfolio02/Graybox/M_RedBeam_Path_TEST_v04`, Additive / Unlit / Two Sided, Emissive `(25, 0.005, 0)`, Opacity `0.08`.
- 충돌: 메시 Collision Primitives `0`, 액터 컴포넌트 Collision Profile `NoCollision`, Collision Enabled `NoCollision`, Overlap Events `false`, Navigation 영향 `false`.
- SpotLight 보조값: Intensity `15000 cd`, Attenuation Radius `2700uu`, Inner Cone `0.75°`, Outer Cone `2°`, Volumetric Scattering `50`, Cast Shadows `false`.
- 기존 대형 붉은 발광 오브젝트 `GB_RedEntranceCue_TEST`와 이전 테스트 빔 `VFX_RedEntranceBeam_TEST`는 제거했다.
- PIE 캡처: `Saved/Screenshots/P02_DirectionalBeam_PathAndEnd_v04.png`.
- 실제 관찰: 공기 중 빔과 PlayerStart 쪽 종료점은 명확하게 보인다. 입구 쪽 시작점은 고정 쿼터뷰의 START 화면 밖에 있어 상단에서 잘린다. 따라서 양 끝을 포함한 전체 광선이 START 한 화면에 보이는 상태는 아니다.
- 미확정: 고정 카메라 및 입구 위치를 유지하면서 어느 구간까지 한 화면에 노출할지 사용자 판단이 필요하다.
