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

### 7-16. 고정 쿼터뷰 벽 가림 비교 프로토타입 (2026-08-26)

**상태: TEST VALUE / 미확정.** 기존 Portfolio 레벨은 수정하지 않고 `/Game/Portfolio02/Tests/WallOcclusion` 아래 별도 테스트 맵 4개에서 비교했다.

## 문제

고정 쿼터뷰에서 높은 벽이 플레이어와 주변 이동 가능 공간을 가릴 가능성이 있다.

## 가설

벽 높이 조정으로 기본 가림을 줄일 수 있으며, 높은 벽이 필요한 구간은 하부 경계를 남기고 상부 렌더링만 전환하는 방식으로 보완할 수 있다.

## 테스트 조건

- 카메라: Perspective, Pitch `-50°`, Yaw `0°`, Distance `1700uu`, FOV `45`.
- 공통 바닥: 중심 `(400, 0, -50)`, 크기 `3000 × 2400 × 100uu`.
- 벽: 중심 X `0`, 폭 Y `1800uu`, 두께 X `100uu`.
- Low / Mid / High PlayerStart: `(400, 0, 100)`, Yaw `0°`.
- Low: `150uu`, Mid: `350uu`, High: `700uu`.
- 동적 벽: Lower `150uu` + Upper `550uu` = 총 `700uu`.
- 동적 PlayerStart: `(400, -800, 100)`.
- Trigger 중심: `(400, 0, 100)`, Half Extent `(300, 300, 200)` — 전체 범위 `600 × 600 × 400uu`.
- Upper Actor Tag: `MCP_TEST_OCCLUDER_UPPER`.
- 처리: Trigger 진입 시 Upper Actor의 `Hidden In Game`만 `true`, 이탈 시 `false`. Lower와 두 벽 파트의 `BlockAll` 충돌 설정은 변경하지 않는다.
- Fade/투명 머티리얼은 구현하지 않았다.

## 관찰

- Low `150uu`: 화면 하단에 좁은 수평 경계로 보인다. 플레이어 전신과 주변 이동 공간은 보이지만 벽의 세로 덩어리감은 세 조건 중 가장 약하다.
- Mid `350uu`: Low보다 벽 면적과 경계가 크게 읽힌다. 동일 PlayerStart에서 플레이어 전신과 진행 방향 쪽 바닥은 보인다.
- High `700uu`: 벽이 화면 중앙 아래 대부분을 차지하며 플레이어 캐릭터는 완전히 가려지고 HUD 체력바만 보인다. 플레이어 주변 바닥 정보도 크게 차단된다.
- Dynamic 진입 전: Upper가 표시된 총 `700uu` 벽이 플레이어와 주변 공간을 가린다.
- Dynamic Trigger 내부: Upper `550uu`가 숨겨지고 Lower `150uu` 경계는 남는다. 플레이어 전신과 주변 바닥이 다시 보인다.
- 복원 로그: `hidden=false → hidden=true (Y=0) → hidden=false (Y=800)` 순서를 확인했다. Upper Actor 수는 `1`이다.
- 숨김 처리는 렌더링 상태만 바꾸므로 충돌 설정은 유지된다. 실제 벽 충돌에 대한 별도 이동 실측은 이번 비교 범위에서 하지 않았다.

## 기술 문제

- 최초 자동 캡처에서 게임 창 포커스 입력 때문에 Mid의 이동거리 `151uu`가 기록되어 비교 조건에서 제외했다.
- Low / Mid / High를 게임 입력 없이 다시 캡처했고, 최종 3장 모두 HUD `Distance 0uu`를 확인했다.
- Dynamic 캡처는 Trigger 상태를 재현하기 위해 1회성 CVar `p02.OccluderTest.PlayerY`로 플레이어를 Y `0`과 Y `800`에 이동시켰다.

## 보존 자료

- Low: `Saved/Screenshots/WallOcclusion_TEST_20260826/01_WallHeight_Low_150uu.png`
- Mid: `Saved/Screenshots/WallOcclusion_TEST_20260826/02_WallHeight_Mid_350uu.png`
- High: `Saved/Screenshots/WallOcclusion_TEST_20260826/03_WallHeight_High_700uu.png`
- Dynamic 진입 전: `Saved/Screenshots/WallOcclusion_TEST_20260826/04_Dynamic_Before_UpperVisible.png`
- Dynamic Trigger 내부: `Saved/Screenshots/WallOcclusion_TEST_20260826/05_Dynamic_InsideTrigger_UpperHidden.png`

## 사용자 결정 필요

- Portfolio 실제 공간에서 사용할 벽 높이 또는 구간별 높이.
- 높은 벽이 필요한 구간에 Upper Hide 방식을 적용할지 여부.
- Trigger 위치와 범위를 실제 레벨 좌표에 어떻게 배치할지.
- Hide 대신 Fade를 추가로 비교할 필요가 있는지.

### 7-17. Interior v01 벽 높이 / Upper Hide 시험 적용 (2026-08-26)

**상태: TEST VALUE / 사용자 판단 대기.** 원본 `/Game/Portfolio02/Levels/L_Interior_v01`은 변경하지 않고, 복제 맵 `/Game/Portfolio02/Tests/InteriorWallTrial/L_Interior_v01_WallTrial_TEST`에서만 시험했다.

## 기본 벽 적용

- 적용 구간: Mid Boss 공간의 기존 일반 벽.
- 사용 Height: 기존 South / North / East 벽 `350uu` 유지. 별도 높이 변경 없음.
- 비교 참고: Final Boss 기존 일반 벽은 `360uu`로 동일 Mid 후보 범위에 있으나, 이번 대표 캡처는 Mid Boss 한 곳만 사용했다.
- PIE 기준 위치: PlayerStart 임시 위치 `(2800, -1200, 100)`, Yaw `0°`.
- 공간 경계: 화면 상단에서 방 외곽 경계가 연속된 벽으로 보인다.
- 플레이어 가림: 플레이어 전신과 주변 바닥이 보였고, 해당 위치에서 350uu 벽에 의한 캐릭터 가림은 관찰되지 않았다.
- 관찰 한계: 이 한 지점만으로 Interior 전체의 기본 벽 높이를 확정하지 않는다.

## 높은 벽 Upper Hide

- 적용 구간: Transition 상승 동선 왼쪽의 기존 100uu 난간 6개 구간. 위치와 하부 난간은 유지하고 복제 맵에만 Upper를 추가했다.
- Lower Height: 기존 `100uu`.
- Upper Height: 추가 `600uu`; 시험 총높이 `700uu`.
- Upper 수: `6`, Actor Tag `MCP_TEST_OCCLUDER_UPPER`.
- Trigger 중심: `(2600, 500, 300)`.
- Trigger Half Extent: `(300, 300, 200)` — 전체 `600 × 600 × 400uu`.
- Hide 전: PlayerStart 임시 위치 `(2600, 100, 340)`. 700uu 벽이 화면 중앙 대부분과 플레이어를 가렸고 HUD 체력바만 보였다.
- Hide 후: 플레이어 Y를 `500`으로 이동해 Trigger 내부 상태를 재현했다. Upper 600uu가 숨겨지면서 플레이어 전신과 Transition 진행면이 보였고 Lower 100uu 난간은 남았다.
- 복원: Y `900` 이동 후 `upper hidden=false, sections=6` 로그 확인.
- 충돌: Upper StaticMeshComponent는 `BlockAll` 유지. Trigger는 `SetActorHiddenInGame`만 변경하므로 숨김 후에도 충돌 설정은 유지된다. 벽에 직접 부딪히는 별도 이동 실측은 하지 않았다.
- 전환 방식: Fade 없이 즉시 Hide. 캡처상 Before/After 화면 변화량이 크다.
- 관찰 한계: 자동 캡처는 좌표 이동으로 Trigger 상태를 재현했기 때문에, 연속 이동 중 Hide 순간이 체감상 자연스러운지는 이번 결과만으로 확정하지 않는다.

## 기술 문제

- 첫 Save As 시도는 활성 World가 원본 패키지에 남는 것을 확인해 저장을 중단했다.
- 이후 원본을 별도 asset으로 복제하고, 활성 패키지가 시험 맵인지 확인한 뒤 시험 맵만 명시 저장했다.
- 원본 `L_Interior_v01.umap`의 수정 시각과 파일 크기는 작업 전후 동일하게 유지됐다.

## 스크린샷

- 기본 벽: `Saved/Screenshots/InteriorWallTrial_20260826/01_BasicWall_MidBoss_350uu_TEST.png`
- Upper Hide Before: `Saved/Screenshots/InteriorWallTrial_20260826/02_TransitionUpperHide_Before_TEST.png`
- Upper Hide After: `Saved/Screenshots/InteriorWallTrial_20260826/03_TransitionUpperHide_After_TEST.png`

## 사용자 판단 필요

- 실제 Interior 기본 벽으로 `350~360uu` 계열을 채택할지.
- Transition에 이번 700uu 시험 벽 자체가 필요한지. 현재 원본 난간은 100uu이며, 700uu 상부는 가림 검증용으로만 추가했다.
- Upper Hide의 큰 즉시 전환을 채택할지, 연속 이동 테스트 또는 Fade 비교가 필요한지.
- 다른 실제 높은 벽 후보가 정해진 뒤 동일 처리를 추가 시험할지.

### 7-18. Transition Upper Hide 연속 이동 검증 (2026-08-26)

**상태: 검증 완료 / Trigger 조정 필요.** 구조, 카메라, Lower `100uu`, Upper `600uu` × 6, 총높이 `700uu`, 기존 Trigger, 즉시 Hide/Show 조건을 변경하지 않았다. Fade는 구현하지 않았다.

## 검증 조건

- 시험 맵: `/Game/Portfolio02/Tests/InteriorWallTrial/L_Interior_v01_WallTrial_TEST`.
- PlayerStart 임시 시작점: `(2600, -500, 100)`, Trigger 진입 전 Mid Boss 북측 진행면. 맵에는 저장하지 않았다.
- 이동: Yaw `0°` 상태에서 `D` 입력을 한 번 계속 눌러 Trigger 진입 → 통과 → 이탈을 좌표 이동 없이 연속 수행했다.
- 카메라: Perspective, Pitch `-50°`, Yaw `0°`, Distance `1700uu`, FOV `45` 유지.
- 로그: `upper hidden=true, sections=6` 이후 `upper hidden=false, sections=6` 복원 확인.

## 관찰

- Hide 직전: HUD 이동거리 `684uu` 프레임에서 Upper가 아직 표시되어 플레이어 전신이 완전히 가려지고 체력바만 보였다.
- Hide 직후: HUD 이동거리 `782uu` 프레임에서 Upper가 숨겨져 플레이어 전신과 진행면이 다시 보였다.
- Hide 상태: Lower `100uu` 경계는 남았고, 플레이어 및 계단 진행면 가독성은 개선됐다.
- Show 직전: HUD 이동거리 `1437uu` 프레임에서 플레이어가 보였다.
- Show 직후: HUD 이동거리 `1512uu` 프레임에서 Upper가 즉시 복원되며 플레이어 전신이 다시 완전히 가려졌다.
- 플레이 이동 자체는 중단되지 않았지만, 화면 정보는 Hide 직전과 Show 직후에 끊겼다.

## 판단

- `Upper Hide 방식은 적합하지만 Trigger 위치/범위 조정 필요`.
- 근거: 현재 진입 경계는 플레이어가 가려진 뒤에 Hide되고, 이탈 경계는 Upper가 복원되자 플레이어를 즉시 덮는다. 이번 결과의 우선 문제는 Fade 유무보다 Trigger 진입/이탈 시점이다.

## 보존 자료

- 전체 연속 캡처: `Saved/Screenshots/InteriorWallTrial_Continuous_20260826/Run02/`.
- Hide 직전: `Saved/Screenshots/InteriorWallTrial_Continuous_20260826/Selected/01_Hide_ImmediatelyBefore_PlayerOccluded.png`.
- Hide 직후: `Saved/Screenshots/InteriorWallTrial_Continuous_20260826/Selected/02_Hide_ImmediatelyAfter_PlayerVisible.png`.
- Show 직전: `Saved/Screenshots/InteriorWallTrial_Continuous_20260826/Selected/03_Show_ImmediatelyBefore_PlayerVisible.png`.
- Show 직후: `Saved/Screenshots/InteriorWallTrial_Continuous_20260826/Selected/04_Show_ImmediatelyAfter_PlayerOccluded.png`.

## 추가 수정

- 이번 검증에서는 수정하지 않았다. 다음 단계가 필요하면 Trigger 진입/이탈 경계만 사용자 판단으로 조정하고 동일 연속 이동을 재검증한다.

### 7-19. 실제 Interior Graybox v01 동선 재검증 (2026-08-26)

**상태: 실제 플레이 가능 1차안 유지 / 공간 판단 대기.** 기존 `/Game/Portfolio02/Levels/L_Interior_v01`이 확정 흐름을 이미 포함하므로 중복 맵이나 새 공간을 만들지 않았다.

## 관계 확정

- Interior Entrance는 외부 진입부의 붉은 방향성 빛이 나오는 높은 던전 입구를 통과한 직후의 내부 시작점이다.
- 외부 진입부 구조는 보호하고 변경하지 않았다.
- 외부 → 내부 Level Transition 로직은 이번 검증에서 추가하지 않았다.

## TEST VALUE / 미확정

- Entrance `800 × 900uu`, 상단 Z `160uu`; `40uu × 4단` 하강.
- Mid Boss `2700 × 2000uu`, 바닥 Z `0uu`, 일반 벽 `350uu`.
- Transition `40uu × 6단`, 총 상승 `240uu`; 높은 경로 `2600 × 800uu`; Mid Boss 방향 낮은 난간 `100uu`; 외벽 `260uu`.
- Final Boss 압축 진입 `700 × 600uu`; 공간 `3200 × 3000uu`, 바닥 Z `240uu`, 벽 `360uu`.
- 중앙 매스 `900 × 700 × 220uu`, 중심 `(7900, 1100, 350)`.
- 모든 값은 기존 v01의 TEST VALUE이며 최종값으로 확정하지 않는다.

## 실제 이동 검증

- 고정 카메라: Perspective / Pitch `-50°` / Yaw `0°` / Distance `1700uu` / FOV `45`.
- PlayerStart에서 텔레포트와 대시 없이 실제 이동 입력으로 Mid Boss → Transition → Final Boss까지 연속 통과했다.
- HUD 실측 이동속도 약 `400uu/s`.
- Mid Boss: 진입 가능. 넓은 평지는 보이나 방 경계와 개방 형상은 화면에서 약하다.
- Transition: 상승과 전방 진행면은 보이나 지나온 Mid Boss 공간은 같은 화면에서 식별되지 않는다.
- Final Boss: 압축 뒤 넓은 바닥으로 열리지만 진입 직후 중앙 매스는 화면에 보이지 않는다.
- 위 세 결과에 대해 공간을 자동 수정하지 않았다.

## 보존 자료

- Mid Boss 진입: `Saved/Screenshots/InteriorGraybox_v01_20260826/01_MidBoss_Entry_ActualRoute.png`.
- Transition 핵심: `Saved/Screenshots/InteriorGraybox_v01_20260826/02_Transition_Core_ActualRoute.png`.
- Final Boss 진입: `Saved/Screenshots/InteriorGraybox_v01_20260826/03_FinalBoss_Entry_ActualRoute.png`.
- 첫 이동시간 산정 오류 자료는 `Saved/Screenshots/InteriorGraybox_v01_20260826/Attempt01_InvalidRoute/`에 분리 보존했다.

## 사용자 판단 필요

- Mid Boss 진입 시 경계/개방감 노출 정도.
- Transition 재노출을 위해 조정할 공간 변수.
- Final Boss 중앙 매스의 최초 노출 시점.
- 외부 입구와 Interior 맵의 실제 전환 연결 시점.
- Mid Boss Encounter Clear 문 연결 여부.

### 7-20. LOST ARK Mobile CBT 기준 카메라 Distance 최소 재검증 (2026-08-27)

**상태: 3안 비교 완료 / 사용자 최종 선택 대기.** `L_Interior_v01`의 구조와 오브젝트는 수정하지 않았고, Pitch `-50°`, Yaw `0°`, FOV `45`를 유지한 채 Distance만 비교했다.

## 검증 조건

- 레벨: `/Game/Portfolio02/Levels/L_Interior_v01`.
- 임시 테스트 위치: PlayerStart `(1500, -1850, 100)`, Rotation `(0, 0, 0)`. Mid Boss 공간의 바닥과 350uu 외벽이 함께 보이는 위치이며 맵에는 저장하지 않았다.
- 해상도: 세 조건 모두 `1286 × 760`, 동일 화면비.
- 비교값: Distance `1700 / 2000 / 2200uu`.
- 각 캡처의 HUD에서 실제 카메라 값 `1700/1700`, `2000/2000`, `2200/2200`을 확인했다.

## 관찰

- `1700uu`: 캐릭터 점유가 가장 크고 전방 바닥 및 외벽 노출 범위가 가장 좁다. 벽 높이와 캐릭터 식별은 명확하지만 CBT 화면과 비교하면 근접한 시야다.
- `2000uu`: 캐릭터 식별을 유지하면서 주변 바닥과 외벽의 연속 노출이 증가했다. 1700보다 전투 공간과 진행 방향을 함께 읽기 쉽다.
- `2200uu`: 세 안 중 캐릭터 점유가 가장 작고 주변 공간 및 경계 노출이 가장 많다. 350uu 벽의 경계는 계속 읽히며 캐릭터도 식별 가능했다.
- 세 안 중 CBT 레퍼런스의 캐릭터 대비 공간 노출 관계에 가장 가까워 보이는 후보는 `2200uu`다. 최종 확정은 사용자 판단 항목으로 남긴다.
- 세 안 모두 명확히 실패한 상태는 아니므로 Pitch `-55°` 추가 테스트는 수행하지 않았다.

## 보존 자료

- `Saved/Screenshots/CameraDistance_CBT_20260827/Camera_Distance_1700.png`.
- `Saved/Screenshots/CameraDistance_CBT_20260827/Camera_Distance_2000.png`.
- `Saved/Screenshots/CameraDistance_CBT_20260827/Camera_Distance_2200.png`.

## 복원 및 다음 단계

- 테스트 종료 후 CVar Distance를 기존 `1700uu`로 복원했다.
- PlayerStart를 기존 `(0, -1200, 260)`으로 복원했고 PIE를 종료했다.
- 최종 카메라는 적용하지 않았다.
- 사용자가 최종 카메라를 선택하기 전이므로 350uu / 700uu / Upper Hide 대표 벽 재확인은 아직 수행하지 않았다.

### 7-21. 최종 Camera 2000uu 채택 및 대표 벽 재확인 (2026-08-27)

**상태: 최종 카메라 확정 / 대표 벽 재확인 완료 / 카메라·벽 비교 종료.** 사용자가 Perspective, Pitch `-50°`, Yaw `0°`, Distance `2000uu`, FOV `45`를 최종 기준으로 선택했다.

## 적용

- `p02.Camera.Distance` 기본값과 `AP02QuarterViewCameraRig::CameraDistance` 초기값을 `2000uu`로 변경했다.
- 에디터를 종료한 상태에서 `P02DungeonEditor Win64 Development` 빌드 성공을 확인했다.
- 재실행 후 CVar `p02.Camera.Distance = 2000`을 확인했다.

## 대표 벽 재확인 조건

- 시험 맵: `/Game/Portfolio02/Tests/InteriorWallTrial/L_Interior_v01_WallTrial_TEST`.
- 카메라: Perspective / Pitch `-50°` / Yaw `0°` / Distance `2000uu` / FOV `45`.
- 레벨 구조, 벽 높이, Trigger, Fade 조건은 변경하지 않았다.

## 350uu 기본벽

- 임시 PlayerStart: `(2800, -1200, 100)`, Rotation `(0, 0, 0)`.
- Mid Boss 외곽의 `350uu` 벽이 화면 상단에서 연속된 공간 경계로 읽혔다.
- 플레이어 전신과 주변 바닥이 동시에 보였으며 대표 위치에서 캐릭터 가림은 관찰되지 않았다.

## 700uu 고벽

- 임시 PlayerStart: `(2600, 100, 340)`, Rotation `(0, 0, 0)`.
- Lower `100uu` + Upper `600uu`, 총 `700uu`가 표시된 상태를 사용했다.
- 높은 벽이 화면 전면을 크게 차지하며 플레이어 전신과 진행면을 가렸다. HUD 체력바만 보였다.

## 700uu + Upper Hide

- 기존 Trigger와 Upper Section 6개를 그대로 사용했다.
- Trigger 내부 Y `500`에서 로그 `upper hidden=true, sections=6`을 확인했다.
- Upper `600uu`가 숨겨진 뒤 Lower `100uu` 경계는 남았고, 플레이어 전신과 Transition 진행면이 다시 보였다.
- Y `900` 복원 후 로그 `upper hidden=false, sections=6`을 확인했다.
- 이번 재확인은 대표 상태 비교만 수행했다. 이전 연속 이동 검증의 `Trigger 위치/범위 조정 필요` 판단은 변경하지 않았다.

## 보존 자료

- `Saved/Screenshots/WallRecheck_Camera2000_20260827/01_BasicWall_350uu_Cam2000.png`.
- `Saved/Screenshots/WallRecheck_Camera2000_20260827/02_HighWall_700uu_Cam2000.png`.
- `Saved/Screenshots/WallRecheck_Camera2000_20260827/03_HighWall_700uu_UpperHide_Cam2000.png`.

## 종료

- `2000uu`에서도 350uu 기본벽은 대표 위치에서 경계와 가독성을 함께 유지했다.
- 700uu 고벽은 가림 문제가 남았고, Upper Hide는 가독성 회복에 유효했다.
- 추가 카메라 Distance/Pitch/FOV 비교, 새 벽 높이 후보, Fade 테스트는 진행하지 않는다.
- 테스트 후 원본 `/Game/Portfolio02/Levels/L_Interior_v01`을 다시 열었고 PIE는 종료 상태다.


---

## 외부 구간 개편 — 짧은 분기 + 빛으로 메인 루트 확정 (2026-08-27)

기존 확정 구조(START → 거의 직진 → 우측 우회)를 **분기 구조로 개편**. 사용자 설계 보드 기준. 이전 버전은 디스크의 `L_Entrance_v03`(분기 이전 상태)에 보존, 신규 구조는 `L_Entrance_v04`.

### 확정 (사용자 결정)

- **첫 화면에는 아무것도 보이지 않는다.** 광장에서 전진하며 빛을 발견하는 방식. 후보 입구가 첫 프레임에 안 들어오는 것은 의도된 것.
- 시작 광장 X `-1700 ~ -300` (**1600 × 1400**). 통로 폭 `1600uu` (Y ±800) = 400uu/s 로 4초 거리.
- 후보 3개 중 C(우측)만 던전 입구로 연결.
- 빛은 던전 입구에서 **바깥으로 길게 쏘는 세계 단서**. 플레이어를 겨냥하지 않는다.
- 입구·빛·트리거는 원래 기준 **오른쪽 45°**(UE yaw `+45`) 회전.

### 후보 구성 (실측)

| 후보 | 구성 | 막는 것 | 판정 |
|---|---|---|---|
| A 좌측 포켓 | X `-300~300`, Y `-800~-1540` (폭 600 / 깊이 740) | 티어 C 바위 상단 `780~900` | 절벽, 진행 불가 |
| B 정면 | 광장에서 직진 | `GB_BlockB_Step` 상단 `130` + 뒤 던전 매스 `180` | 높이 차, 진행 불가 |
| C 우측 (메인) | X `-300` 부터 개방 → 상승 램프 → 입구 | — | 연결 |

Max Step Height `45` 기준으로 `130` / `780~900` 전부 초과 → 오르기 불가 확정.

### 빛 (수평 세계 단서)

- 긴 빔: 문 `(2210, 700)` → 끝 `(407, -1103)`, 길이 **2550uu**, Z `500`, yaw `45`
- 문 앞 발광 스텁 `(2132.2, 622.2, 500)` / SpotLight `(2202.9, 692.9, 500)` yaw `225`
- Z `500` 은 문 개구부(`180~870`) 중앙. 경로상 장애물(`GB_BlockB_Step` 130 / 던전 매스 180 / 좌 티어A 바위 250·340 / 좌 테라스 120) 전부 빔보다 낮아 묻히지 않음.

**측정값 — 빛을 처음 보는 시점**: 통로 중앙(Y 0)을 걸을 때 플레이어 X `-460` 에서 빔 끝단이 화면에 들어온다. START(`-1650`)로부터 **1190uu = 3.0초** (등속 400uu/s, 가감속·대시 미반영).

### 왼쪽 45° 를 쓰지 않은 이유 (검증 결과)

빔 Z `500` 기준 방향별 최대 길이 — 왼쪽 45°(yaw 135) **622uu**(우측 티어B 바위 상단 500에 막힘) / 오른쪽 45°(yaw 225) **2602uu** / 회전 전 수평 서쪽 4360uu. 왼쪽은 622uu 스텁이 되어 "길게 쏘는 단서" 가 성립하지 않음.

### 회전 부작용 처리

- 회전으로 기둥 하나가 입구 패드(X `1900~2300`) 밖으로 나가 89uu 떠서, **패드를 X `1900~2450` / Y `450~950` 로 확장**(위치 `2175,700,130`, 스케일 `5.5,5,1`). 원래 값은 `2100,700,130` / `4,4,1`.
- 입구 패드 자체는 회전하지 않음(상승 플랫폼과 어긋나므로).

---

## 4단계 — Bridge 파괴 (2026-08-28)

**상태: 구현 완료 / PIE 초기화 검증 완료 / 실제 플레이 검증 대기.**
대상 맵 `/Game/Portfolio02/Tests/FinalBossCandidates/L_Interior_FinalBossCandidates_TEST`.

### 설계 의도

3단계에서 Mid Boss Door 는 **홀드만으로는 절대 열리지 않는** 상태로 만들었다.
4단계는 그 문제를 전투가 아니라 **공간 변경**으로 풀게 한다.

### 다리 분할 (실측)

| 액터 | 범위 | 비고 |
|---|---|---|
| `StaticMeshActor_20` (기존 데크, 축소) | X `3000~4900` | 서쪽 잔존부 |
| `StaticMeshActor_32` / `_22` (기존 난간, 축소) | X `3000~4900` | Y `650` / `1550` |
| `P02_BridgeWeakSection_Deck` | X `4900~5600`, Y `700~1500` | 파괴 대상 |
| `P02_BridgeWeakSection_RailS` / `_RailN` | X `4900~5600` | 데크와 함께 낙하 |

파괴 시 생기는 간극 **700uu**. 대시 거리가 `450uu` 이므로 건널 수 없다.

### 파괴 규칙

- 이름이 `P02_BridgeWeakSection` 으로 시작하는 액터 전부가 함께 낙하한다. 데크/난간 분리는 코드가 아니라 레벨에서 한다.
- 피격은 **동쪽에서만** 받는다 (`플레이어 X ≥ 구간 동쪽 끝 5600`). 발밑에서 무너뜨려 갇히는 것을 막기 위함.
- 히트 판정은 액터 원점이 아니라 **바운딩 박스와 공격 구체의 거리**로 한다. 원점 기준으로는 구간이 길어 닿지 않는다.
- 피격 피드백: 0.12초 15uu 하강 + HUD 카운터 + 로그. VFX/사운드는 후순위.

### 접근 경로 차단

`AP02PlaytestEnemy` 는 중력이 없어 수평 이동만 한다. **간극이 생겨도 허공을 그대로 건너온다.**
따라서 지오메트리가 아니라 코드로 차단한다 — 파괴 시 `AP02MidBossDoorGate` 가 **스폰을 중단하고 현재 추격자를 제거**한다.

적 스폰 지점(X `4400~4800`)은 서쪽 잔존부에 있어, 파괴 후 스폰 위치 자체가 플레이어와 단절된다.

### 조정값 (CVar — 빌드 불필요)

| CVar | 기본값 |
|---|---|
| `p02.Bridge.HitCount` | `3` (TEST VALUE, 범위 2~4) |
| `p02.Bridge.DropDistance` | `1400` |
| `p02.Bridge.DropDuration` | `0.6` |

### 3단계 확정값 (실측 기반)

로그 실측 `spawn->first hit 6.62s over 1799uu at speed 250` 에서 역산.

```
필요 속도 = (1799 − AttackRange 150) / 4.0초 = 412
```

`EnemyMoveSpeed = 410` 확정 (도달 약 4.0초, 홀드 5초).

추가로 `TryBasicAttack` 에 **`E` 홀드 중 공격 차단**을 넣었다. 이것이 없으면 `E` 를 누른 채 다가오는 적을 처치해 홀드를 완주할 수 있어, "홀드만으로는 불가능" 이 성립하지 않는다.

### PIE 초기화 검증 (2026-08-28 02:16)

```
Bridge breaker ready: sections=3 span X 4900..5600 hits=3
Mid Boss door gate ready: door=yes spawn=yes bridge=yes hold=5.0s
Color lock ready: pads=3 door=yes end=yes answer=Blue,Red,Green
Wall fade: 152 fadeable walls found
```

### 미검증 — 실제 플레이 필요

- 다리 피격이 실제로 들어가는지 (동쪽 끝에서 서쪽을 향해 공격)
- 파괴 후 추격자 제거·스폰 중단이 체감되는지
- 파괴 후 문 홀드 5초 완주가 실제로 가능한지
- 간극 700uu 가 대시로 안 넘어가는지
- 파괴 후 힌트 재확인 동선이 실제로 끊기는지 (8단계 Lever 의 전제)

---

## 4단계 보정 — 약한 구간 색 / 파괴 후 스폰 / 레버 복구 (2026-08-28)

### 확정 — 약한 구간은 전용 색을 쓰지 않는다

기존: 약한 구간에 `MI_Grid_Weak`(황갈색)를 상시 적용해 부술 곳을 미리 알렸다.
변경: 다리 나머지와 **같은 `MI_Grid_1Second_400uu_TEST`** 를 쓰고, **피격당했을 때만** 색이 바뀐다.

- 평상시 색 = 다리 나머지와 동일 (사전 표식 없음)
- 피격 시 `SurfaceColor` 를 `HitsTaken / 필요타수` 비율로 손상색 `(0.50, 0.13, 0.05)` 쪽으로 보간
- 기존 0.12초 하강 모션과 **같은 프레임**에 색이 바뀐다 — 모션이 타격감, 색이 누적 진행도
- 기준 색은 하드코딩하지 않고 `BeginPlay` 에서 `GetVectorParameterValue(SurfaceColor)` 로 애셋에서 읽는다. 다리 색을 바꿔도 그대로 동작한다.

`MI_Grid_Weak` 는 어느 액터에서도 참조하지 않게 됐다.

### 수정 — 파괴 직후 추격자가 다시 스폰되던 버그

`Tick` 안에서 arm 판정이 파괴 판정보다 먼저 돌았다. 파괴가 `bArmed = false` 로 되돌리면 **다음 틱에 arm 조건(`플레이어 X ≥ ArmLineX`)이 다시 참**이 되어 매 틱 스폰 → 제거가 반복됐다.

`bChaseRetired` 를 도입해 첫 파괴에서 **영구 래치**한다. arm 조건과 리스폰 조건 양쪽에 걸었다.

### 확정 — 다리는 중간보스 레버로만 복구된다

| | |
|---|---|
| 복구 주체 | `MCP_TEST_MidBossInteraction` (중간보스 처치 후 나타나는 레버) |
| 조작 | 레버 반경 안에서 `E` **누름** (계단 자동 하강과 별개) |
| 복구 대상 | 다리 위치·콜리전·피격 누적·색 전부 초기화 |
| 복구되지 않는 것 | **추격자 스폰** — `bChaseRetired` 는 래치라 다시 켜지지 않는다 |

계단은 기존대로 처치 2초 후 자동 하강하므로, 레버의 실제 역할은 **다리 복구**가 됐다. 그래서 E 입력 분기에서 `!bActivated` 조건을 제거해 계단이 이미 내려간 뒤에도 레버를 쓸 수 있게 했다. 레버 반경 안에서 다리가 끊긴 상태면 `LEVER  press E to rebuild the bridge` 를 표시한다.

이로써 5단계(계단 역할 정리)의 "레버가 계단 하강과 중복된다" 문제도 해소됐다.

### PIE 초기화 검증 (2026-08-28 09:29, 빌드 후)

```
Bridge breaker ready: sections=3 span X 4900..5600 hits=3
Mid Boss door gate ready: door=yes spawn=yes bridge=yes hold=5.0s
Color lock ready: pads=3 door=yes end=yes answer=Blue,Red,Green
Candidate mechanism ready: enemies=1 stairs=11 blockers=1
Wall fade: 152 fadeable walls found
```

레버의 다리 참조는 별도 로그가 없다. 다만 스폰 순서가 BridgeBreaker → DoorGate → Interaction 이고 DoorGate 가 같은 `Cast` 로 `bridge=yes` 를 얻었으므로, 나중에 스폰되는 레버도 반드시 찾는다.

### 미검증 — 실제 플레이 필요

- 피격 시 색 변화가 실제로 눈에 들어오는지 (1/3, 2/3 단계 구분)
- 파괴 후 추격자가 정말 한 번도 다시 나오지 않는지
- 중간보스 처치 → 레버 `E` → 다리 복구 → 서쪽 복귀가 이어지는지

### 수정 — 문이 내려가도 사라지지 않던 문제 (2026-08-28)

**관찰:** 중간보스 문이 `DoorOpenDistance 620` 만큼 내려가지만 화면에서 사라지지 않는다. 계단이 꺾인 뒤에야 사라진다.

**원인:** 두 가지가 겹쳤다.

1. **쿼터뷰는 아래를 내려다본다** (Pitch −50°). 바닥 아래로 내린 문은 위에서 그대로 보인다. 즉 "내리는 것" 자체는 이 카메라에서 은폐 수단이 되지 못한다.
2. 실제로 문을 지우고 있던 건 벽 투명화였는데, 그 판정이 `Mesh->Bounds.Min.Z > PawnZ + HeadRoom(100)` — **문이 플레이어보다 위에 있을 때만** 투명해진다. 문이 620 내려가면 플레이어와 같은 높이가 되어 조건에서 빠진다. 플레이어가 계단 꺾임을 지나 아래로 내려가야 다시 조건이 성립하고, 그때서야 사라진다.

**수정:** 문을 내리는 1초 동안 `WallFadeOut` 을 0→1 로 같이 올리고, 다 내려가면 `SetActorHiddenInGame(true)`. 계단 앞 차단판(Blocker)에 이미 쓰던 것과 같은 처리다.

**최종보스 문(색 순서 자물쇠)에도 같은 결함이 있어 함께 고쳤다.** 같은 코드 패턴이었다.

### 변경 — 다리 필요 타수 3 → 8

`p02.Bridge.HitCount` 기본값을 `8` 로 확정. 레벨디자이너 결정.

파괴에 시간이 더 걸리므로 추격 압박을 받는 시간도 그만큼 길어진다. 실제 체감은 플레이테스트 필요.

### PIE 초기화 검증 (2026-08-28 09:37, 빌드 후)

```
Bridge breaker ready: sections=3 span X 4900..5600 hits=8
Mid Boss door gate ready: door=yes spawn=yes bridge=yes hold=5.0s
Color lock ready: pads=3 door=yes end=yes answer=Blue,Red,Green
Candidate mechanism ready: enemies=1 stairs=11 blockers=1
```

### 수정 — 파괴된 다리가 이미지만 남아 되살아나던 버그 (2026-08-28)

**관찰:** 다리를 부수면 낙하 후 다리가 원래 자리에 다시 나타난다. 콜리전은 없어서 그 위로 가면 캐릭터가 떨어진다.

**원인:** 피격 흔들림(`ShakeRemaining`)이 파괴 시점에 남아 있었다.

1. 마지막 타격이 `ShakeRemaining = 0.12` 를 설정하고 동시에 `bBroken = true`, `DropElapsed = 0`
2. 낙하 0.6초 동안 `Tick` 이 낙하 블록에서 **early return** — `ShakeRemaining` 이 감소하지 않는다
3. 낙하가 끝난 다음 프레임, 낙하 블록 조건이 거짓이 되어 흔들림 블록으로 넘어간다
4. 흔들림 블록은 위치를 **`SectionStartLocations` 기준으로 다시 쓴다** → 다리 전체가 원래 높이로 순간이동
5. 콜리전은 계속 꺼져 있으므로 **보이지만 없는 다리**가 된다

**수정:** 파괴 시 `ShakeRemaining = 0`, 그리고 `bBroken` 이면 흔들림 블록에 도달하기 전에 반환한다. 흔들림 블록은 살아 있는 다리에만 유효하다.

### 확정 — 벌어진 틈으로 떨어지면 사망, 중간보스 문 앞에서 부활

틈 아래는 하층 구역이라, 방치하면 낙하가 **실수가 아니라 하층으로 가는 지름길**이 된다. 다리 위에서 다리를 부수는 경우도 있으므로 안전망이 필요하다.

| | |
|---|---|
| 판정 | 파괴 완료 후, 플레이어가 다리 XY 범위(여유 200uu) 안이면서 `SectionBounds.Min.Z − 250` 아래 |
| 여유 200uu | 낙하 중 수평 관성으로 옆으로 밀리기 때문 |
| 결과 | 즉사 처리 → 부활 |
| 부활 위치 | 중간보스 문 접근면 — 문 바운딩박스 중심에서 `X −200`, `Min.Z + 100` |
| 조정 | `p02.Bridge.FallKillDrop` (기본 250) |

부활 지점은 하드코딩하지 않는다. 문 액터의 실제 바운딩박스에서 계산하므로 문을 옮기면 부활 지점도 따라간다. 접근면이 `−X` 인 것은 플레이어가 다리 쪽에서 오기 때문.

`UP02PlaytestSubsystem` 에 `RespawnLocation` 을 추가했다. 기본값은 기존과 동일한 `StartLocation` 이고, **다리가 파괴되는 순간에만** 문 앞으로 옮긴다. 그 이전 죽음은 종전대로 시작 지점에서 부활한다.

### PIE 초기화 검증 (2026-08-28 09:44, 빌드 후)

```
Bridge breaker ready: sections=3 span X 4900..5600 hits=8
Mid Boss door gate ready: door=yes spawn=yes bridge=yes hold=5.0s
Candidate mechanism ready: enemies=1 stairs=11 blockers=1
```

### 미검증 — 실제 플레이 필요

- 파괴 후 다리가 정말 안 돌아오는지
- 틈으로 떨어졌을 때 사망 판정이 잡히는지 (하층에 착지해버리지 않는지)
- 문 앞 부활 위치가 바닥에 제대로 서는지 (공중이나 바닥 아래가 아닌지)

---

## 4단계 완료 확인 (2026-08-28)

플레이테스트 결과 사용자 확인:

- 파괴 후 다리가 돌아오지 않는다 ✔
- 틈으로 떨어지면 사망 판정이 잡힌다 ✔
- 문 앞 부활 위치는 살짝 공중이지만 허용 범위 ✔

`p02.Bridge.FallKillDrop` 및 부활 Z 오프셋(`Min.Z + 100`)은 현재값 유지.

## 5단계 — 계단 역할 정리

### 확정 — 레버는 한 가지 일만 한다

| 장치 | 역할 | 발동 |
|---|---|---|
| 계단 하강 | 최종보스 구역으로 내려가는 길 | 중간보스 처치 **2초 후 자동** |
| 레버 | **다리 복구** | 중간보스 처치 후 등장, 반경 안에서 `E` |

레버의 `E` 입력이 `StartMechanism()`(계단 하강)까지 호출하고 있었다. 계단은 이미 자동으로 내려가므로 `bActivated` 가 참이라 실제로는 아무 일도 하지 않는 **죽은 두 번째 경로**였다. 제거했다.

이제 계단과 레버는 발동 조건도 대상도 겹치지 않는다.

---

## 최종 제작 진행 순서 (사용자 확정 원문 요약, 2026-08-28)

새로운 설계 아이디어를 추가하지 않는다. 사용자가 공간 구조·레벨디자인을 결정하고, Claude Code 는 구현·수정·검증만 한다.

**카메라 확정 — 더 이상 변경하지 않는다:** Perspective / Pitch −50° / Yaw 0° / Distance 2000uu / FOV 45

**확정된 공간 역할**

```
Exterior → Dungeon Entrance → Interior → Bridge → Mid Boss Door → Mid Boss
→ 자동 하강 계단 → Lower Route → Candidate 1 / Final Boss Area → Final Boss Door → END
```

- 왼쪽 Candidate 1 = **Final Boss Area**
- 오른쪽 Candidate 2 = **Hint Area**
- Candidate 비교는 종료. 역할을 다시 바꾸지 않는다.

| 단계 | 내용 | 상태 |
|---|---|---|
| 1 | Final Boss 구간 완주 가능화 (색 입력 장치·힌트·END) | 완료 |
| 2 | 치명적 이동/시야 문제 수정 | 완료 |
| 3 | Mid Boss Door 압박 | 완료 |
| 4 | Bridge 파괴 | 완료 (2026-08-28 확인) |
| 5 | Mid Boss 이후 계단 역할 정리 | 3/4 완료 — 보행 확인 미실시 |
| 6 | **Hint Area 와 정보 활용 구조 검증** | **진행 중** |
| 7 | 오답 실패 복구 (반대편 끝 Enemy) | 대기 |
| 8 | 선택적 Bridge 복구 Lever | 구현 완료 (순서보다 먼저 요청됨), 검증 대기 |
| 9 | 전체 연속 플레이 3경로 테스트 | 대기 |
| 10 | 포트폴리오용 대표 캡처 16종 | 대기 |
| 11 | 레벨 데이터 정리 | 대기 |

**최종 금지:** 새 방 / 새 퍼즐 / 새 Enemy 종류 / Final Boss 실제 전투 / 복잡한 AI / 환경 아트 확장 / 고급 VFX / 복잡한 파괴 물리 / 새 카메라 실험 / 새 벽 실험 / Candidate 비교 재개 / 새 플레이 시스템

**작업 방식:** 단계별로 상태 확인 → 필요한 기능만 구현 → Compile/Save → PIE 검증 → 값·문제 보고 → 사용자 판단 대기. 승인 없이 다음 단계 선행 구현 금지.

## 6단계 — Hint Area 검증 완료 (2026-08-28)

### 확정 배치

**상층 색 블럭** `Z 80~240`, 다리 북쪽 허공, X5200

| 액터 | Y | 색 | 순서 |
|---|---|---|---|
| `P02_ColorHint_1` | 1870 | Blue | 1 |
| `P02_ColorHint_3` | 2070 | Green | 3 |
| `P02_ColorHint_2` | 2270 | Red | 2 |

**하층 개수 표식** `Z −950`, Candidate 2 통로 바닥, X6060 — 기본 회색(무채색)

| 그룹 | Y | 개수 |
|---|---|---|
| `HintMark_1a` | 2090 | 1 |
| `HintMark_3a/b/c` | 2230~2350 | 3 |
| `HintMark_2a/b` | 2460~2520 | 2 |

정답 `Blue → Red → Green` 과 인코딩 일치 확인.

### 확정 — 블럭과 표식의 좌표 어긋남은 의도된 것

블럭과 표식이 Y `+220uu` 오프셋에 Z `1110uu`, X `860uu` 차이가 나서 화면상 정렬이 쌍마다 다르다(Blue 0.04 / Green 0.12 / Red 0.22 차이). **위에서 볼 때의 시점을 기준으로 배치한 것이며 수정하지 않는다.**

### 확정 — 하층에서 색을 얻을 수 없는 것은 의도된 것

표식은 순서(개수)만 주고 색이 없다. 색 블럭은 하층에서 Fade 로 사라진다. 따라서 정보 획득 경로는:

```
다리 위에서 확인   → 색 + 순서 동시 획득
Candidate 2 하층  → 순서만 획득, 색 불가
레버 → 다리 복구  → 다시 올라가서 색 확인
```

**하층 힌트는 완결된 정보원이 아니라 "다시 올라가야 한다"는 것을 알려주는 부분 정보다.** 이것이 8단계 Lever 의 존재 이유가 된다.

### 확인된 동작

| 항목 | 결과 | 근거 |
|---|---|---|
| 아래에서 색 블럭 안 보임 | ✔ | Fade 가 처리. 하층 플레이어는 `P02_WallFade_Lower` 안, 블럭 `Min.Z 80` > `PawnZ+100 = −764` → overhead → 100% 투명. 지오메트리 차폐가 아니므로 `p02.WallFade.Enabled` 에 의존 |
| 다리 위에서 안 사라짐 | ✔ | 다리(Z 336)는 모든 Fade 볼륨 밖 |
| 난간이 시야 차단 안 함 | ✔ | 카메라가 Z 1868, 시선이 난간 상단(Z 500) 위를 통과 |

### 기록 — 다리 중앙선에서는 3번째 블럭이 화면 밖

화면 좌표(16:9, `|값| ≤ 1` 이 화면 안), Pitch −50 / FOV 45 / Distance 2000 기준:

```
플레이어         Blue      Green     Red
X4600 Y1100     +0.74     +0.93     +1.12 ✗
X4600 Y1300     +0.55     +0.74     +0.93
X5000 Y1100     +0.82     +1.03 ✗   +1.25 ✗
X5000 Y1458     +0.44     +0.65     +0.87
X4200 (전부)     세로 +1.01 ✗ (위로 벗어남)
```

세 개를 모두 보려면 북쪽 난간에 붙어 `X4600~5400` 구간을 지나야 한다. 사용자 판단으로 **현 상태 유지**.

## 5단계 완료 (2026-08-28)

계단 하강 후 보행·Collision 사용자 확인 완료. 5단계 4개 항목 전부 종료.

## 7단계 — 오답 실패 복구 (2026-08-28)

### 확정 값

| 항목 | 값 | 결정 |
|---|---|---|
| 적 수 | 3 (스폰 지점당 1) | 사용자 |
| 이동 속도 | 250 | 사용자 |
| 체력 | 34 (기존 테스트 Enemy 기준) | 유지 |
| 공격력 | 10 | 유지 |

### 스폰 지점 — 레벨 액터, 이동 가능

Candidate 1 동쪽 벽(입력장치 반대편)을 따라 배치. 이름으로 찾으므로 에디터에서 드래그하면 그대로 반영된다.

| 액터 | 좌표 | → 입력장치 | 250 도달 |
|---|---|---|---|
| `P02_WrongAnswerSpawn_NE` | 5600, 500, −860 | 2827uu | 11.3s |
| `P02_WrongAnswerSpawn_E` | 5600, −500, −860 | 2333uu | 9.3s |
| `P02_WrongAnswerSpawn_SE` | 5600, −1500, −860 | 2211uu | 8.8s |

방 크기 X 2800~5800 × Y −1700~700. 입력장치 → 문 475uu. 입구(5800, 300) → 입력장치 2871uu.

### 동작

```
오답 입력 → Entered 초기화 → 3마리 스폰 → 패드 입력 잠금
→ 전부 처치 → 잠금 해제 → 재입력 가능
```

패드가 잠기는 동안 HUD 는 `WRONG - CLEAR THE ROOM`.

### 하드락 방지

`p02.WrongAnswer.MaxSeconds` (기본 60). 도달 불가 위치에 적이 끼면 패드가 영구히 잠기므로, 초과 시 남은 적을 제거하고 강제 해제한다. 로그에 `(TIMED OUT)` 이 찍히면 스폰 지점이나 경로에 문제가 있다는 뜻이다.

### 자동 측정 항목

`SpawnPunishers` / `Tick` 이 로그로 남긴다.

```
Wrong answer #N: 3 enemies at speed 250, A..Buu away (C..Ds)
Wrong answer MEASURED: first contact X.XXs after the wrong input
Wrong answer MEASURED: retry available Y.YYs after the wrong input
```

`retry available` 이 곧 **오답 1회의 전체 추가 비용**이다.

### PIE 초기화 검증 (2026-08-28 10:12)

```
Color lock ready: pads=3 door=yes end=yes spawns=3 answer=Blue,Red,Green
```

### 미검증 — 실제 플레이 필요

- 무작정 찍기가 힌트 확인보다 빠르지 않은가 (3! = 6가지, 오답 1회 비용 × 기대 시도 횟수로 판단)
- 오답 전투가 지나치게 길지 않은가
- 적 도달 대기(8.8~11.3s)가 지루하지 않은가
- 반복 오답 후 하드락 없는가

### 7단계 플레이테스트 결과 (2026-08-28)

| 검증 항목 | 결과 |
|---|---|
| 반복 오답 후 하드락 | **없음** — 5회 오답까지 확인 |
| 오답 전투 길이 | **짧음** — 플레이어 공격이 다중 타격이라 2~3회로 3마리 전부 처치 |
| 적 도달 대기 | **지루함** — 속도 250에서 가장 가까운 적 실측 약 7초 |
| 무작정 찍기 vs 힌트 확인 | **찍기가 더 빠름** — 미해결, 아래 참조 |

### 확정 — 이동 속도 250 → 410 (추격자와 통일)

대기 시간이 지루하다는 판단. 중간보스 추격자와 같은 값으로 통일한다.

| 스폰 | 거리 | 250 | **410** |
|---|---|---|---|
| SE | 2211uu | 8.8s | **5.4s** |
| E | 2333uu | 9.3s | **5.7s** |
| NE | 2827uu | 11.3s | **6.9s** |

### 미해결 — 찍기가 힌트 확인보다 빠르다

현재 색 3개. 자물쇠가 틀린 단계를 알려주므로 전수 탐색이 아니라 단계별 결정이 된다.

```
색 3개 / 3단계 → 최악 오답 3+2 = 5회, 기댓값 약 2회
오답 1회 비용 = 도달 5.4~6.9s + 전투 약 2s ≒ 8s
→ 기대 총 비용 약 16s
```

다리로 되돌아가 힌트를 확인하는 비용보다 싸다. **해결 방향은 색을 5개로 늘리는 것으로 기획서에 기록만 하고, 이번 회차에서는 구현하지 않는다.**

```
색 5개 / 5단계 → 최악 오답 4+3+2+1 = 10회, 기댓값 약 5회
→ 기대 총 비용 약 40s (2.5배)
```

7단계 종료.

### 수정 — 낙하 사망 판정이 Candidate 2 바닥과 겹치던 문제 (2026-08-28)

**관찰:** 하층에서 Candidate 2 남쪽 벽 쪽으로 붙으면 갑자기 사망한다.

**원인:** 판정 영역을 다리 바운딩박스 + 여유 200uu 로 잡았는데, 그 박스가 **난간까지 포함**해 Y 로 부풀어 있었고 아래로는 무한히 열려 있었다.

```
Deck   X4900..5600  Y 700..1500
RailS                Y 600.. 700
RailN                Y1500..1600   → SectionBounds Y 600..1600
+ 여유 200uu        → 사망영역 X4700..5800  Y 400..1800  Z < −110

Candidate2 바닥      X4445..5795  Y1590..2800  상단 Z −960
겹침                 X4700..5795  Y1590..1800  → 즉사
```

**수정:** XY 판정을 **완전히 제거**하고 **높이 하나로만** 판정한다.

```
p02.Player.KillZ = -1200
```

맵의 모든 보행 가능 바닥 하단이 `Z −1060` 이고 그보다 아래로 갈 방법은 구멍으로 떨어지는 것뿐이다. 따라서 바닥 위에 서 있는 플레이어는 **구조적으로 판정에 걸릴 수 없다.**

판정을 `AP02BridgeBreaker` 에서 `UP02PlaytestSubsystem::Tick` 으로 옮겼다. 다리 전용이 아니라 맵 전체에 적용되므로, 다른 구멍으로 떨어져도 동일하게 사망 처리된다. `p02.Bridge.FallKillDrop` 는 제거.

부작용: 사망까지 약 1000uu 더 떨어진다(약 1초). 오히려 "떨어졌다"는 것이 읽히므로 그대로 둔다.

## 8단계 — 선택적 Bridge 복구 Lever (2026-08-28)

구현은 4단계 보정 회차에 완료(순서보다 먼저 사용자 요청). 계획서의 최종 역할과 대조:

| 요건 | 상태 |
|---|---|
| 필수 진행 X | ✔ 찍기로 완주 가능 — 5회 오답 하드락 없음 실측 확인 |
| 계단 작동 X | ✔ 5단계에서 수동 경로 제거 |
| 파괴된 Bridge 복구 O | ✔ |
| 과거 공간·힌트 재확인 O | ✔ 다리 위(Z 336)는 모든 Fade 볼륨 밖이라 색 블럭이 보인다 |

### 확정 — A/B 비교 조항은 해당 없음

계획서 8단계 검증은 A(Candidate 2 Hint Area)와 B(Lever → 다리 복구 → 상층 재확인)를 비교해 한쪽이 항상 우월하면 삭제·통합 후보로 보고하라고 되어 있다.

그러나 6단계에서 **하층 표식은 순서만 주고 색이 없는 것이 의도**로 확정됐다. 따라서 A 는 자물쇠를 풀 수 있는 완결된 정보원이 아니다.

```
A (하층 표식)   → 부분 정보. "순서는 1-3-2, 색은 모름"
B (레버 → 다리) → A 가 만든 결핍을 메우는 유일한 경로
```

**두 방식은 경쟁 관계가 아니라 직렬로 이어진 하나의 흐름이다.** 삭제·통합 대상이 아니므로 해당 조항은 종료 처리한다.

---

## 9단계 — 전체 연속 플레이 테스트

### 측정 장치 구축 (2026-08-28)

기존에는 총 시간·총 거리만 있어 **구간별 비교가 불가능**했다. 마일스톤 기록기를 추가했다.

기록 시점:

| 라벨 | 발생 |
|---|---|
| `START` | 플레이어 초기화 |
| `BRIDGE BROKEN (N hits)` | 약한 구간 파괴 |
| `MID BOSS DOOR OPEN (N chasers)` | 홀드 완료 |
| `MID BOSS DOWN (lever appears)` | 중간보스 처치 |
| `STAIRS DOWN` | 계단 하강 시작 |
| `BRIDGE RESTORED (lever)` | 레버 사용 |
| `WRONG ANSWER #N` | 오답 입력 |
| `RETRY AVAILABLE (after wrong #N)` | 오답 적 전멸 |
| `FINAL BOSS DOOR OPEN` | 정답 입력 |
| `END` | END Trigger 도달 |

각 항목에 **경과 시간 + 누적 이동 거리**가 함께 찍힌다.

### 출력

END 도달 시 자동 출력. 중도 확인은 `p02.Run.Report 1` (자동으로 0 으로 복귀).

```
===== RUN REPORT =====
     0.33s         0uu  (+  0.33s)  START
     ...
  total 9.74s  0uu  (in progress)
======================
```

`(+X.XXs)` 는 직전 마일스톤과의 간격이다. 계획서가 요구하는 값이 이 표에서 직접 나온다.

| 요구 측정 | 산출 방법 |
|---|---|
| 총 플레이 시간 | `total` |
| 총 이동 거리 | `total` |
| 경로별 Final Boss Door 개방까지 시간 | `FINAL BOSS DOOR OPEN` 의 시각 |
| Wrong Answer 추가 비용 | `WRONG ANSWER #N` → `RETRY AVAILABLE` 간격 |
| Lever 복구 추가 비용 | `BRIDGE RESTORED` 전후 구간 간격 |

### PIE 검증 (2026-08-28 11:01)

`START` 기록 및 `p02.Run.Report` 출력 동작 확인.

### 미실시 — 사용자 플레이 필요

3경로 각각 1회 이상 연속 플레이 후 RUN REPORT 수집.

```
A 정보 확인 성공 : 다리에서 힌트 확인 → 정답 → END
B 정보 미확인    : 오답 → 적 → 재시도 → END
C 정보 복구      : 미확인 → 레버 → 다리 복구 → 재확인 → 정답 → END
```

### 확정 — 9단계 측정 범위는 Interior 한정 (2026-08-28)

계획서 9단계는 "START → END 연속 플레이"라고 되어 있으나, Exterior(`L_Entrance_v03`)와 Interior 테스트 맵은 별도 맵이고 연결 작업은 계획서에 없다.

**9단계의 START 는 Interior PlayerStart, END 는 `P02_EndTrigger` 로 확정한다.** Exterior 는 측정 대상에서 제외한다. (10단계 캡처 1~3번은 여전히 Exterior 에서 촬영한다 — 캡처와 측정의 범위가 다르다.)

### 수정 — END 에서 런이 닫히지 않던 버그

`Color lock: END reached` 는 찍히는데 `CompleteRun()` 호출이 빠져 있었다. 옛 테스트 코스용 `AP02EndTrigger` 와 색 자물쇠의 END 판정이 별개 경로라 연결되지 않은 것. `END` 마일스톤과 RUN REPORT 자동 출력이 둘 다 누락됐다. 수정 완료.

### A 경로 실측 (2026-08-28 11:09) — END 행 미기록

```
  0.01s       0uu            START
 26.37s    8972uu  (+26.4s)  BRIDGE BROKEN (8 hits)
 32.85s    9444uu  (+ 6.5s)  MID BOSS DOOR OPEN (3 chasers)
 35.09s   10011uu  (+ 2.2s)  MID BOSS DOWN (lever appears)
 37.09s   10401uu  (+ 2.0s)  STAIRS DOWN
 62.88s   20400uu  (+25.8s)  FINAL BOSS DOOR OPEN
 약 64.6s      —             END 도달 (위 버그로 미기록, 거리 불명)
```

오답 0회, 레버 미사용. 정답을 아는 플레이어의 기준선.

**관찰 1 — 다리 파괴가 압박을 실제로 제거했다.**
파괴 `26.37s` → 문 개방 `32.85s` = **6.5초**. 홀드가 5초이므로 이동 약 1.5초 + 홀드 5초, 즉 **한 번도 끊기지 않았다.** 3단계에서 만든 "홀드만으로는 열 수 없다"가 4단계 파괴로 해소되는 것이 수치로 확인된다.

**관찰 2 — 전체의 40%가 하층 구간이다.**
계단 하강 `37.09s` → 정답 `62.88s` = 25.8초, 이동 약 10000uu. 하층 이동 비중이 상층 전체와 비슷하다.

### B / C 경로 실측 (2026-08-28 11:15, 11:16)

**B — 정보 미확인, 오답 복구**

```
  0.01s        0uu            START
 22.70s     7513uu  (+22.69s)  BRIDGE BROKEN (8 hits)
 29.11s     7963uu  (+ 6.41s)  MID BOSS DOOR OPEN (3 chasers)
 31.42s     8689uu  (+ 2.31s)  MID BOSS DOWN (lever appears)
 33.42s     9069uu  (+ 2.00s)  STAIRS DOWN
 57.99s    18774uu  (+24.57s)  WRONG ANSWER #1
 63.35s    19509uu  (+ 5.35s)  RETRY AVAILABLE
 67.10s    20285uu  (+ 3.75s)  FINAL BOSS DOOR OPEN
 70.07s    21402uu  (+ 2.98s)  END
```

**C — 정보 복구, 레버 사용**

```
  0.01s        0uu            START
 22.63s     7543uu  (+22.62s)  BRIDGE BROKEN (8 hits)
 28.99s     8017uu  (+ 6.36s)  MID BOSS DOOR OPEN (3 chasers)
 31.30s     8583uu  (+ 2.31s)  MID BOSS DOWN (lever appears)
 33.30s     9281uu  (+ 2.00s)  STAIRS DOWN
 38.55s    11316uu  (+ 5.25s)  BRIDGE RESTORED (lever)
 83.97s    29412uu  (+45.42s)  FINAL BOSS DOOR OPEN
 86.97s    30545uu  (+ 3.00s)  END
```

### 3경로 요약

| | A 정답 | B 오답 | C 레버 |
|---|---|---|---|
| 총 시간 | 65.87s **(가정)** | **70.07s** | **86.97s** |
| 총 거리 | 21525uu **(가정)** | **21402uu** | **30545uu** |
| 오답 | 0 | 1 | 0 |

A 의 END 행은 `CompleteRun()` 누락 버그로 기록되지 않았다. 사용자 판단으로 **문 개방 → END 구간을 B·C 평균(2.98s / 1125uu)으로 가정**해 채웠다. **실측이 아니라 가정값이므로 포트폴리오 제시 시 그대로 쓰지 않는다.** 참고로 로그 타임스탬프상 A 의 실제 END 도달은 런 시작 후 약 64.6초이며, 거리만 불명이다.

### 학습곡선 보정 — 총 시간으로 비교하지 말 것

상층 구간(START → STAIRS DOWN)이 회차마다 다르다.

```
A 37.09s / 10401uu   ← 첫 회차
B 33.42s /  9069uu
C 33.30s /  9281uu
```

A 가 약 3.7초 느리다. 조작 숙련 차이지 설계 차이가 아니다. **경로 비교는 하층 구간(STAIRS DOWN → FINAL BOSS DOOR OPEN)으로 한다.**

```
A 25.79s /  9999uu
B 33.68s / 11216uu
C 50.67s / 20131uu
```

### 확정 측정값

| 항목 | 값 |
|---|---|
| **오답 1회 추가 비용** | **5.36s / 735uu** |
| **레버 복구 추가 비용** | **+24.88s / +10132uu** (C−A, 하층 구간 기준) |
| 문 개방 → END | 2.98s / 1125uu |

### 관찰 — 찍기가 레버보다 싼 것이 수치로 확인됐다

자물쇠가 틀린 단계를 알려주므로 전수 탐색이 아니라 단계별 결정이 된다. 색 N개일 때 최악 오답 `N(N−1)/2`, 기댓값은 그 절반.

| 색 개수 | 기대 오답 | 기대 비용 | 판정 |
|---|---|---|---|
| **3 (현재)** | 1.5회 | **8.0s** | 찍기 유리 |
| 4 | 3.0회 | 16.1s | 찍기 유리 |
| **5** | 5.0회 | **26.8s** | 힌트 유리 (레버 24.9s 를 근소하게 초과) |
| 6 | 7.5회 | 40.2s | 힌트 유리 |

**7단계에서 기록한 "색 5개로 늘린다"가 실측 기준으로도 최소선이다.** 5개에서 겨우 역전되므로 여유를 두려면 6개가 안전하다. 이번 회차 구현 대상은 아니며 기록만 한다.

9단계 종료.

---

## 하층 구조 변경 — 입구/보스문 위치 반전 (2026-08-28, 착수)

### 백업

작업 전 스냅샷 보존. 폴더의 기존 `Before*` 규칙을 따랐다.

```
L_Interior_FinalBossCandidates_BeforeEntranceFlip_TEST.umap   304,228 bytes
```

작업은 기존 `L_Interior_FinalBossCandidates_TEST` 에서 계속한다.

### 변경 의도 (사용자)

계단을 내려오면 바로 좌우로 갈라지는 현 구조를, **직진 복도 → 추가 복도 → 방** 순서로 바꾼다. 바닥 공간 자체는 그대로 두고 **입구를 최저 X, 보스문을 최고 X 로 반전**한다. 보스문 자리는 처음에 입구 옆이라 폐기했던 위치.

### 현재 구조 (변경 전 실측)

```
계단 착지  X6109~7300, Y 930~1230        (Z −220 → −940)
Junction   X5900~8700, Y 700~1500        ← 착지 직후 바로 분기
  ├ 남 스퍼 X5802~6677, Y−500~1100  → Candidate 1 동쪽(고 X) 진입
  └ 북 스퍼 X5800~6700, Y1100~2800  → Candidate 2 동쪽(고 X) 진입

Candidate 1  X2800~5800, Y−1700~700   보스문 X2900~3900 (저 X, 남벽)
Candidate 2  X4445~5795, Y1590~2800
```

맞춰 쓸 치수: 복도 폭 `800~900uu` / 바닥 상단 `Z −960` / 벽·난간 높이 `350uu`

### 연쇄 변경 목록

**이동만 하면 되는 것** (이름으로 찾으므로 빌드 불필요)
`MCP_TEST_Candidate1_BigDoor` · `P02_ColorPad_Red/Green/Blue` · `P02_EndTrigger` · `P02_EndPad`+난간 3 · `P02_WrongAnswerSpawn_NE/E/SE` · `P02_MidBossDoor`

**반드시 다시 잡아야 하는 것**

| 볼륨 | 현재 범위 | 놓치면 |
|---|---|---|
| `P02_WallFade_Lower` | X3030~8070, Y−1750~3450, Z−1000~−200 | 새 복도에서 천장이 안 사라짐 |
| `P02_FadeTarget_All` | X520~8680, Y−1630~4530, Z−920~520 | 벗어난 지오메트리가 페이드 대상에서 빠짐 |
| `P02_WallFade_LowerRoute` / `_StairShaft` | 구 동선 기준 | 동선 변경으로 무의미 |

**재검증 필요**
- 오답 적 스폰 — 입력장치가 고 X 로 가면 "반대편"이 **저 X 로 뒤집힌다.** 거리 재측정 필수
- 6단계 힌트 가시성 — 하층 동선 변경분
- `p02.Player.KillZ` — 새 바닥이 `Z −1060` 보다 낮으면 조정

**무효화**
9단계 3경로 실측 전부. 특히 레버 복구 비용 `+24.88s / +10132uu` 는 하층 이동 거리에 직결되므로, 복도가 길어지면 **"색 5개면 힌트 유리"** 결론도 재계산 대상이다.

### 기록 — 10단계 전제 위반

계획서 10단계는 "공간과 기능이 잠긴 뒤에만 촬영". 이 변경은 공간을 다시 여는 것이므로, 2·6·9단계를 부분적으로 다시 밟은 뒤 10단계로 간다. 사용자 판단으로 진행.

### 계단 직선화 + 하층 통로 (2026-08-28)

**계단** — 180° 꺾임 폐기, 19칸 직선. 기존 18칸 + Turn 발판 액터를 **삭제 없이 재배치**했다.

```
X5229~7300  Y800~1490  Z-960~-200
19칸 x (진행 109 / 상승 40) = 20.2도   ← 기존 경사 유지
착지 X5283, 윗면 Z-920
```

**통로** — `P02_LowerCorridor_Floor` X2700~5229, Y800~1490, 윗면 Z-960. 폭 690uu (사용자 결정).

**삭제** — `P02_LowerRail_Route_West`(계단이 관통), 구 계단 난간 8개(중앙 분리 난간·꺾임부 난간 포함)

**Fade 볼륨 확장** — `P02_WallFade_Lower` 를 X2650~8070 / Z-1000~0 으로. 안 넓히면 새 통로에서 천장이 안 사라지고, 계단 꼭대기(Z-200)도 볼륨 밖이었다.

### 사용자 수동 수정 반영 (좌표 대조)

| 액터 | 변경 |
|---|---|
| `Candidate1_Floor` | 북쪽 Y700 → Y802 |
| `Candidate1_Wall_North` | X2800~5800 → X3620~6620 (→ **X2800~3620 개방 = Cand1 최저 X 입구**) |
| `Candidate2_Floor` | 남쪽 Y1590 → Y1490 |
| `Candidate2_Wall_South` | 통로 북벽으로 연장 |
| `Candidate1_Wall_WestNorth` | 통로 서벽까지 연장 |
| `Shaft_Wall_South/North` | X2765~8515 로 연장 → 통로 상부벽 |

사용자가 `P02_LowerCorridor_Wall_North/_West` 와 계단 난간 4개를 삭제하고 위 확장 벽으로 대체했다.

### Candidate 2 최저 X 입구

`Candidate2_Wall_South` 를 X3568~4445 로 축소하고 `P02_Corridor_Wall_North_East`(X4745~7592) 를 신설해 **X4445~4745, 폭 300uu** 개방. 통로 바닥(Y~1490)과 Cand2 바닥(Y1490~)이 맞물려 바로 걸어 들어간다.

### 변경 — 힌트 블럭은 사라지지 않는다

`P02_ColorHint*` 는 어떤 FadeTarget 박스 안에 있든 **페이드 대상에서 제외**한다. 코드에 `P02NeverFadePrefix` 추가.

```
Wall fade: 142 fadeable walls found (P02_ColorHint excluded)   ← 152 에서 감소
```

**이 변경은 6단계 정보 구조를 뒤집는다.** 기존에는 하층에서 색 블럭이 Fade 로 사라져 "순서만 알고 색은 모른다"였고, 그것이 8단계 레버(다리 복구 → 재확인)의 존재 이유였다. 이제 하층에서도 색이 보이므로 **C 경로(레버 복구)의 필요성이 사라진다.**

9단계에서 측정한 레버 복구 비용 `+24.88s` 와, 그에 기반한 "색 5개면 힌트가 유리" 결론도 전제가 바뀌었다. 하층 구조 확정 후 재검토 대상.

### 되돌림 — 힌트 블럭은 다시 사라진다 (2026-08-28)

시야각이 안 나온다는 판단으로 `P02NeverFadePrefix` 제거, Fade 대상으로 복귀.

```
Wall fade: 145 fadeable walls found
```

6단계 정보 구조(하층=순서만, 색은 다리 위에서만)와 8단계 레버의 존재 이유가 그대로 유지된다.

### 검토 결과 — 하층 Z 를 낮춰도 블럭은 안 가려진다

"카메라에 안 보이게 하층 Z 를 낮추면 되지 않나"에 대한 계산. 블럭 Z160 고정, 수직 half-FOV 13.1°(16:9).

| 하층 바닥 윗면 | 플레이어 Z | 블럭이 화면에 잡히는 플레이어 X 구간 |
|---|---|---|
| Z−960 (현재) | −864 | X5809 ~ X6228 |
| Z−1060 | −964 | X5942 ~ X6279 |
| Z−1160 | −1064 | X6075 ~ X6330 |
| Z−1260 | −1164 | X6209 ~ X6380 |
| Z−1460 | −1364 | X6475 ~ X6482 |

**낮춰도 "안 보이는" 것이 아니라 보이는 구간이 동쪽으로 밀릴 뿐이다.** 그 구간에 걸을 수 있는 바닥이 있으면 거기서 다시 보인다. 블럭을 위로 올리는 것도 수학적으로 동일한 효과라 같은 한계를 갖는다.

구간을 완전히 없애려면 그 X 대역에 바닥이 아예 없어야 하는데, 하층 구조가 바뀔 때마다 다시 성립 여부를 확인해야 하는 조건이다. **렌더링(Fade)으로 처리하는 것이 구조 변경에 영향받지 않는 유일한 방법이다.**

## Candidate 2 하강 — 힌트 블럭 기하학적 차폐 (2026-08-28)

### 결정 근거

"카메라에 안 보이게 하층 Z 를 낮출 수 있나"에 대해, 걸을 수 있는 모든 지점 × 블럭 8개 꼭짓점으로 프러스텀 판정을 돌린 결과:

| 조건 | 안 보이는 최초 바닥 Z | 하강 | 계단 |
|---|---|---|---|
| 힌트 스퍼 유지 | **Z−1560** | 600uu | 15칸 |
| 힌트 스퍼 삭제 | Z−1160 | 200uu | 5칸 |

블럭이 보이는 자리가 X5595~6700 인데 힌트 표식이 얹힌 `LowerRoute_Candidate2`(X5800~6700)가 정확히 그 한가운데다. **표식 바닥을 그대로 두기로 한 결정이 하강량을 3배로 키웠다.** 사용자 판단으로 스퍼 유지 + 600uu 하강.

**Candidate 2 구역만 내린다.** 통로와 Candidate 1 은 원래 블럭이 안 보이므로 Z−960 유지.

### 신설 — `P02_StairD` 15칸

```
X4745~6380  Y1490~1990 (폭 500)  Z-1560~-960
15칸 x (진행 109 / 상승 40) = 20.2도   ← 기존 계단과 동일
```

폭 500 은 힌트 표식(Y2090~)을 피하기 위한 값. Candidate 2(X4445~5795)만으로는 1635uu 가 안 나와 **힌트 스퍼까지 관통**한다.

| 신설 액터 | 범위 |
|---|---|
| `P02_StairD_01~15` | X4745~6380 |
| `P02_StairD_Landing` | X4445~4745, Y1490~1990, Z−960 (진입 발판) |
| `P02_StairD_Landing_Wall_West` | X4395~4445 (발판 서쪽 낙하 방지) |
| `P02_StairD_Rail_North_01/02` | Y1990, 계단 북쪽 (아래 바닥까지 최대 500uu 낙차) |

### Z −600 이동

`Candidate2_Floor` · `LowerRoute_Candidate2` · `Candidate2_Wall_North/West` · `LowerRail_SpurN_East/North` · `HintMark_*` 6개 · `LowerLight_Cand2_W/E` · `LowerLight_Route_N`

표식은 **XY 그대로, Z 만** 내렸다(−950 → −1550). 바닥과 함께 내려가므로 상대 배치는 유지된다.

### 볼륨 / CVar

```
P02_WallFade_Lower   Z-1000~0  ->  Z-1700~0
P02_FadeTarget_All   Z-920~520 ->  Z-2150~1350
p02.Player.KillZ     -1200     ->  -1750   (새 바닥 하단 -1660 보다 아래)
```

### 검증 — 걸을 수 있는 전 지점 프러스텀 판정

Cand2 / 힌트 스퍼 / 신설 계단 / 통로 / Cand1 **전부 16:9·16:10·4:3 에서 블럭 안 보임.**

### 남은 노출 — 기존 직선 계단(StairB)

메인 하강 계단 `X5409~6209` 구간에서는 여전히 블럭이 프레임에 들어온다(Z−857~−622 지점). 이 구간은 `P02_WallFade_Lower` 안이라 **Fade 가 처리한다.**

6단계 규칙은 "하층에서 안 보일 것"이고 StairB 는 하층이 아니라 전이 구간이므로 일단 그대로 둔다. Fade 팝이 거슬리면 별도 판단 필요.

### PIE 초기화 검증 (2026-08-28 12:18)

```
Wall fade: 164 fadeable walls found   (145 -> 164, 신설 지오메트리 반영)
Color lock ready: pads=3 door=yes end=yes spawns=3
```

### 계단 난간 (2026-08-28)

기준 규격은 사용자가 지정한 `P02_StairD_Landing_Wall_West` — **50 두께 × 350 높이**. 바닥 대비 위치 관계(윗면에서 220 위, 130 묻힘)까지 동일하게 맞췄다.

`P02_StairD_Rail_S_01~07` (Y1325) / `_N_01~07` (Y1775) / `Landing_Rail_S·N`.

계단이 520uu 하강하므로 **2칸씩 7단으로 분할**했다. 한 판으로 만들면 위쪽 끝이 350uu 넘게 뜬다. 구간 내 최대 오차 ±40uu(계단 한 칸).

### 사용자 재배치 반영

계단이 통로 서쪽 끝으로 이동. `P02_StairD_01` 삭제, 구 난간 2개와 `P02_Corridor_Wall_North_East` 삭제됨.

```
P02_StairD_02~15   14칸  X3068.5~4485.5  Y1300~1800  Z-1020~-1540
P02_StairD_Landing       X2605~2905  Z-960
```

---

## 보스문 이설 — Candidate 1 최고 X (2026-08-28)

### 확정 — 동벽을 일자로 정리하고 그 안에 문을 낸다

중복 벽 2개 삭제. `Wall_EastMid`(Y−740~−500)와 `Wall_EastFarSouth`(Y−1700~−1440)는 `Wall_EastSouth`(Y−1777~−402) 안에 완전히 포함돼 있었다.

| 액터 | 범위 |
|---|---|
| `MCP_TEST_Candidate1_Wall_EastSouth` | Y−1777~−350 (확장) |
| **`MCP_TEST_Candidate1_BigDoor`** | **X5775~5925, Y−350~650, Z−960~−360** |
| `P02_Candidate1_Wall_EastNorth` | Y650~800 (신설) |

**문 Y 위치는 기하학적으로 강제된다.** 문 바깥에 END 를 놓으려면 기존 `LowerRoute_Candidate1`(X5802~6677, **Y−500~1100**) 바닥 위여야 하고, Candidate 1 바닥은 Y802 까지다. 겹치는 구간 Y−500~802 안에 폭 1000 문을 넣으면 Y−350~650 이 된다.

문 scale 을 `(10,1.5,6)` → `(1.5,10,6)` 으로 바꿔 90° 방향 전환했다.

### 딸려 이동한 것

| 대상 | 새 위치 |
|---|---|
| `P02_ColorPad_Red/Green/Blue` | X5375, Y−50 / 150 / 350 — **문 서쪽면에서 400uu** |
| `P02_EndPad` | X5950~6450, Y−350~650 (문 너머 스퍼 위) |
| `P02_EndTrigger` | 중심 (6350, 150, −860) |
| `P02_EndPad_Rail_North/South/West` | 새 EndPad 둘레 |
| `P02_WrongAnswerSpawn_*` | **서쪽으로 반전** X3000, Y500 / −500 / −1500 |

### 오답 스폰 거리 — 반전 후에도 기존과 동등

| 스폰 | → 입력장치 | 410 기준 |
|---|---|---|
| (3000, 500) | 2401uu | 5.9s |
| (3000, −500) | 2462uu | 6.0s |
| (3000, −1500) | 2892uu | 7.1s |

기존 동쪽 배치는 2211~2827uu / 5.4~6.9s 였다. **7단계 실측값(오답 1회 5.36s)이 거의 그대로 유효하다.**

### PIE 초기화 검증 (2026-08-28 13:24)

```
Color lock ready: pads=3 door=yes end=yes spawns=3 answer=Blue,Red,Green
Wall fade: 169 fadeable walls found
```

### 보스문 형태 — 아치 성문 (2026-08-28)

박스 하나로는 "문"으로 읽히지 않아 아치형으로 교체.

**선행 코드 변경:** 색 자물쇠가 문을 `Name.Equals("MCP_TEST_Candidate1_BigDoor")` 단일 액터로 찾고 있어 여러 조각으로 만들 수 없었다. **`StartsWith` 프리픽스 매칭 + `TArray<DoorParts>`** 로 바꿔, 이름이 같은 접두어면 전부 함께 내려가고 함께 페이드하고 함께 숨겨진다. `P02_BridgeWeakSection*` 과 같은 방식이다.

| 액터 | 구성 | 이동 |
|---|---|---|
| `MCP_TEST_Candidate1_BigDoor` | 슬래브 Y−250~550, Z−960~−460 | O |
| `MCP_TEST_Candidate1_BigDoor_Arch` | `SM_Cylinder` 반지름 400, 축을 X로 회전 (0,90,0), 중심 Z−460 | O |
| `P02_BossGate_Jamb_South/North` | 기둥 Y±100, Z−960~−100 | X (프레임) |
| `P02_BossGate_Keystone` | 아치 위 마감 | X (프레임) |

개구부가 Y−350~650 → **Y−250~550(폭 800)** 으로 좁아지고 아치 꼭대기가 Z−60 까지 올라간다. 총 높이 900 — 주변 벽(350)보다 훨씬 높아 멀리서도 문으로 읽힌다.

벽 재조정: `Wall_EastSouth` Y−1777~−450, `Wall_EastNorth` Y750~800.

```
Color lock ready: pads=3 door parts=2 end=yes spawns=3
```

---

## 보스문 최종 형태 — 네모 + 반원 (2026-08-28)

### 폐기 — 원통 테두리(`P02_BossGate_ArchSurround`)

바깥 아치를 **온전한 원통(반지름 550)** 으로 만들었더니 두 가지 문제가 났다.

1. 아래쪽 절반이 통로를 가로막아, 문이 내려가도 **지나갈 수 없다**
2. 실루엣이 "네모 위 반원"이 아니라 큰 동그라미

삭제하고 네모 기둥으로 대체.

### 확정 구조

| 액터 | 범위 | 문 열릴 때 |
|---|---|---|
| `MCP_TEST_Candidate1_BigDoor` | 슬래브 Y−250~550(폭 800), Z−960~−460 | **하강** |
| `MCP_TEST_Candidate1_BigDoor_Arch` | `SM_Cylinder` 반지름 400, 중심 Z−460, 축 X | **하강** |
| `P02_BossGate_Jamb_South` | Y−400~−250, Z−960~−460 | 고정 |
| `P02_BossGate_Jamb_North` | Y550~700, Z−960~−460 | 고정 |

**반지름(400) = 슬래브 폭(800)의 절반**이라 반원이 윗변에 정확히 얹힌다. 원통 아래 절반은 슬래브 안에 묻혀 실루엣에 안 나온다.

기둥은 슬래브 높이(Z−460)에서 끊었다. 반원이 벽 위로 솟아 실루엣이 살고, 열렸을 때 통로에 걸리는 것이 없다.

### 회전 축 함정 — `[Pitch, Yaw, Roll]`

MCP `spawn_actor` / `set_actor_transform` 의 `rotation` 배열은 **`[Pitch, Yaw, Roll]`** 이다. 처음에 `[0, 90, 0]` 을 넣어 Yaw 로 들어갔고, 세로 원통이 제자리에서 돌기만 해 둥근 면이 위를 향했다. 축을 눕히려면 **`[90, 0, 0]`**.

판정 근거: 레벨의 `DirectionalLight_0` 회전이 `[-55, -35, 0]` — 지향성 라이트에 Roll −55 는 의미가 없으므로 첫 값이 Pitch다.

---

## 9단계 재측정 — 하층 개편 후 (2026-08-28, B→A→C 순 플레이)

### A — 정답

```
  0.01s        0uu            START
 17.39s     7825uu  (+17.38s)  BRIDGE BROKEN (8 hits)
 23.50s     8296uu  (+ 6.11s)  MID BOSS DOOR OPEN (3 chasers)
 25.73s     9030uu  (+ 2.23s)  MID BOSS DOWN
 27.73s     9399uu  (+ 2.01s)  STAIRS DOWN
 41.57s    17067uu  (+13.84s)  FINAL BOSS DOOR OPEN
 44.52s    18333uu  (+ 2.95s)  END
```

### B — 오답 복구

```
 16.92s     7571uu            BRIDGE BROKEN (8 hits)
 23.16s     7962uu  (+ 6.24s)  MID BOSS DOOR OPEN (2 chasers)
 27.78s     9063uu            STAIRS DOWN
 43.34s    17735uu  (+15.56s)  WRONG ANSWER #1
 49.35s    19044uu  (+ 6.01s)  RETRY AVAILABLE
 53.86s    20236uu  (+ 4.52s)  FINAL BOSS DOOR OPEN
 57.04s    21579uu  (+ 3.17s)  END
```

### C — 레버 복구

```
 15.84s     7700uu            BRIDGE BROKEN (8 hits)
 22.32s     8145uu  (+ 6.48s)  MID BOSS DOOR OPEN (2 chasers)
 26.52s    10010uu            STAIRS DOWN
 29.25s    11530uu  (+ 2.73s)  BRIDGE RESTORED (lever)
 61.52s    28617uu  (+32.28s)  FINAL BOSS DOOR OPEN
 64.08s    29894uu  (+ 2.56s)  END
```

### 3경로 요약

| | A 정답 | B 오답 | C 레버 |
|---|---|---|---|
| **총 시간** | **44.52s** | **57.04s** | **64.08s** |
| **총 거리** | **18333uu** | **21579uu** | **29894uu** |
| 오답 | 0 | 1 | 0 |

**세 경로 모두 END 행까지 완전 기록.** 이전 회차와 달리 가정값이 없다.

### 학습곡선 — 이번엔 무시해도 된다

상층 구간(START → STAIRS DOWN)이 `27.73 / 27.78 / 26.52s` 로 **편차 1.3초 이내**다. 이전 회차(A가 3.7초 느렸음)와 달리 숙련도가 안정됐으므로 **총 시간 비교가 유효하다.**

### 확정 측정값

| 항목 | 값 |
|---|---|
| **오답 1회 추가 비용** | **6.01s / 1309uu** |
| **레버 복구 추가 비용** | **+21.16s / +10939uu** (하층 구간 C−A) |
| 오답 경로 전체 비용 (B−A) | +12.24s / +3505uu |
| 문 개방 → END | 2.56 ~ 3.18s |

하층 구간(STAIRS DOWN → 문 개방): A `13.84s` / B `26.08s` / C `35.00s`

### 개편 전후 비교 — 하층이 크게 짧아졌다

| | 개편 전 | 개편 후 |
|---|---|---|
| A 총 시간 | 약 64.6s | **44.52s** |
| A 하층 구간 | 25.79s | **13.84s** |
| 레버 복구 비용 | +24.88s | **+21.16s** |
| 오답 1회 비용 | 5.36s | **6.01s** |

계단 직선화 + 통로 직결로 **하층 이동이 거의 절반으로 줄었다.** 다리 파괴도 26.37s → 17.39s 로 빨라졌는데, 이는 숙련도 상승분이다.

### 결론 — 색 5개 필요성이 재확인됐다

| 색 개수 | 기대 오답 | 기대 비용 | 판정 |
|---|---|---|---|
| **3 (현재)** | 1.5회 | **9.0s** | 찍기 유리 |
| 4 | 3.0회 | 18.0s | 찍기 유리 |
| **5** | 5.0회 | **30.0s** | 힌트 유리 (레버 21.2s 초과) |
| 6 | 7.5회 | 45.1s | 힌트 유리 |

레버 비용이 24.9s → 21.2s 로 줄었지만 **5개에서 역전되는 결론은 그대로**다. 이번엔 30.0s vs 21.2s 로 여유가 더 생겼다.

9단계 재측정 완료.

### 캡처용 HUD 스위치 `p02.HUD` (2026-08-28)

```
p02.HUD 0   끔
p02.HUD 1   켬 (기본)
```

끄면 사라지는 것:

- 플레이테스트 HUD 텍스트 (시간·거리·대시·체력)
- 다리 타수 / 중간보스 문 홀드 / 색 자물쇠 진행도 / 레버 안내
- **템플릿 체력바 (`UI_LifeBar`) 와 적 체력바**

체력바는 `AddOnScreenDebugMessage` 가 아니라 **UMG 위젯**이라 `DisableAllScreenMessages` 로는 안 없어진다. `ApplyHudVisibility()` 가 해당 월드의 모든 `UUserWidget` 을 `Collapsed` 로, 모든 `UWidgetComponent` 를 숨김 처리한다.

**0.5초마다 재적용한다.** 오답 적·추격자가 런 도중 계속 스폰되면서 자기 체력바를 달고 나오기 때문에, 한 번만 적용하면 새 적의 바가 다시 보인다.

`P02Dungeon.Build.cs` 에 `UMG` 모듈 의존성을 추가했다.

**주의:** `P02HudVisible()` 은 `P02PlaytestCVars` 네임스페이스 **뒤에** 정의해야 한다. 앞에 두면 `C2653: 'P02PlaytestCVars' is not a class or namespace` 로 컴파일 실패한다.

### 상호작용 프롬프트 + 홀드 카운트다운 (2026-08-28)

**`p02.Prompt`** (기본 1) — `p02.HUD` 와 **분리**했다. 캡처 때 디버그 HUD 는 꺼도 상호작용 안내는 남아야 하기 때문.

| 대상 | 조건 | 표시 |
|---|---|---|
| 중간보스 문 | `InteractRadius 250` 안 | `E` (문 위 400uu) |
| 중간보스 문 | `E` 홀드 중 | **`5.0 → 0.0` 카운트다운** |
| 레버 | `InteractionRadius` 안 + 등장 후 | `E` (레버 위 250uu) |
| 색 패드 | `PadRadius 220` 안 | `E` (밟은 패드 위 200uu) |

`AddOnScreenDebugMessage` 대신 **`DrawDebugString`** 을 쓴다. 이유 두 가지:

1. 화면 구석이 아니라 **해당 오브젝트 위**에 뜬다 — 어느 물체가 E 에 반응하는지 명확해진다
2. `FontScale` 인자가 있다. 온스크린 디버그 메시지는 글자 크기를 못 바꾼다

### 카운트다운은 올라가지 않고 내려간다

기존 HUD 는 `hold 2.3/5.0s` 로 **올라가는** 표시였다. 피격으로 `HoldElapsed` 가 0 이 되면 숫자가 줄어드는데, 이건 "진행이 날아갔다"로 읽히지 않는다.

`HoldSeconds - HoldElapsed` 로 바꿔 **5.0 에서 내려가게** 했다. 피격 순간 숫자가 5.0 으로 튀어 올라가므로 **손실이 즉시 읽힌다.** 3단계에서 만든 압박 구조를 캡처로 보여줄 때 이게 핵심이 된다.

### 프롬프트 폰트 — ONE Mobile POP (2026-08-28)

**함정 1 — `DrawDebugString` 은 폰트를 못 받는다.** 엔진 디버그용 비트맵 폰트를 6배로 확대하던 것이라 화질이 나빴다. 확대할수록 뭉개진다.

→ `AP02PlaytestHUD : AHUD` 를 만들어 **Canvas 로 직접 그린다.** 프롬프트를 프레임마다 큐에 넣고 `DrawHUD()` 에서 소비, `Project()` 로 월드→화면 변환, `bCentreX/Y` 로 중앙 정렬, 검은 그림자.

HUD 는 `PC->ClientSetHUD(AP02PlaytestHUD::StaticClass())` 로 **런타임 설치**한다. `BP_CombatGameMode` 를 편집하지 않아도 된다.

**함정 2 — UE5 는 TTF 를 `UFont` 가 아니라 `UFontFace` 로 임포트한다.** `LoadObject<UFont>` 가 null 을 반환해 엔진 폰트로 폴백됐다.

→ `LoadObject<UObject>` 로 받아 **`FSlateFontInfo(Object, Size)`** 에 넘긴다. `FSlateFontInfo` 는 Face 와 Font 를 모두 받는다. `FCanvasTextItem` 에 `FSlateFontInfo` 를 받는 오버로드가 있다.

`Build.cs` 에 `Slate`, `SlateCore` 추가. (`UMG` 는 앞서 체력바 숨김용으로 추가됨)

```
Content/Portfolio02/UI/ONE_Mobile_POP.uasset   (FontFace)
로그 확인: "Prompt font: FontFace"
```

크기는 **28pt × `p02.Prompt.Scale`**(기본 2.5). E 는 배율 1.0, 카운트다운은 0.8. 폴백 경로(엔진 폰트)는 기존 Scale 방식을 유지한다.

### 함정 3 — `UFontFace` 를 `FSlateFontInfo` 에 넘기면 조용히 아무것도 안 그려진다

`FSlateFontInfo` 는 넘겨받은 폰트에서 **composite font** 를 읽어 글리프를 찾는다. `UFont` 에는 있지만 **`UFontFace` 에는 없다.** Face 를 그대로 넘기면 경고도 에러도 없이 텍스트가 통째로 사라진다.

일반적인 해결은 에디터에서 Font 애셋을 따로 만들어 Face 를 연결하는 것이지만, 수동 단계가 늘어나므로 **런타임에 감싼다.**

```cpp
UFont* Wrapper = NewObject<UFont>(this);
Wrapper->FontCacheType = EFontCacheType::Runtime;
FTypefaceEntry& Entry = Wrapper->CompositeFont.DefaultTypeface.Fonts.AddDefaulted_GetRef();
Entry.Name = TEXT("Regular");
Entry.Font = FFontData(Face);
```

애셋은 임포트한 `ONE_Mobile_POP`(FontFace) 하나면 된다. 로그 판정: `Prompt font: ready (source FontFace)`.

**진단용 `p02.Prompt.Test`** 를 남겨뒀다. 1 이면 플레이어 위에 상시 `E` 를 그린다. "안 보인다"가 그리기 문제인지 상호작용 범위 문제인지 한 번에 가른다.

### 확정 크기

```
p02.Prompt.Scale 2.0    (28pt x 2.0 = 56pt)
```

E 는 배율 1.0, 중간보스 문 카운트다운은 0.8. 코드 기본값도 2.0 으로 반영해 에디터를 다시 켜도 유지된다.

### 확정 — 힌트 블럭은 Fade 예외 (2026-08-28)

**관찰:** 다리에서 계단으로 발을 딛는 순간 힌트 블럭이 사라진다.

**원인:** Candidate 2 하강 작업 때 `P02_WallFade_Lower` 의 Z 상한을 `-200 → 0` 으로 올렸다. 계단 꼭대기(Z−200, 플레이어 Z−104)에서 위층 천장이 안 사라지는 문제를 고치려던 것인데, 부작용으로 **계단에 올라서는 순간 블럭도 페이드 대상이 됐다.**

```
힌트 블럭            Z 80~240
P02_WallFade_Lower   Z -1700~0

다리 위  Z 336   -> 볼륨 밖 -> 정상
계단 상단 Z -104  -> 볼륨 안 -> 사라짐   <- 문제
하층 통로 Z -864  -> 볼륨 안 -> 사라짐
```

**수정:** `P02NeverFadePrefix = "P02_ColorHint"` 를 다시 넣어 **페이드 대상에서 완전히 제외**한다.

```
Wall fade: 168 fadeable walls found (P02_ColorHint excluded)
```

### 이전 회차와 조건이 달라졌다

같은 예외 처리를 앞서 한 번 넣었다가 "시야각이 안 나온다"고 되돌린 적이 있다. **그때는 Candidate 2 를 아직 안 내린 상태였다.**

지금은 Candidate 2 와 힌트 스퍼를 600uu 내려 **걸을 수 있는 전 지점에서 프러스텀 밖**인 것을 16:9·16:10·4:3 모두에서 검증했다. 따라서 하층 은폐를 Fade 가 아니라 **기하학적 차폐**가 담당하고, Fade 예외는 안전하다.

**남은 노출:** 기존 직선 계단 StairB 의 `X5409~6209` 구간에서는 여전히 블럭이 프레임에 들어온다. 하층이 아니라 전이 구간이므로 허용한다. 6단계 규칙("하층에서 안 보일 것")은 유지된다.


---

## 2026-09-07 — 야외 순례길 별도 1차 검토 맵

### 확정 — 이번 사용자 요청

- 기존 맵 보존, 새 맵에서 진행. 공통 길 → 좌우 분기 → 신전 정면 합류 → 단일 정면 입구.
- 오른쪽 짧은 전투/통과, 왼쪽 긴 무전투 순례길. 왼쪽 옛 접근축만 쓰러진 기둥으로 차단.
- 기둥 너머 계단, 바깥 우회 → 후면 → 왼쪽 측면의 내부 관찰 → 정면 복귀.
- 두 경로의 높이 차별화 금지. 카메라 −50° / 기존 Yaw 0° / 2000uu / FOV45.
- 연결 내부 맵은 사용자 답변으로 `L_Interior_FinalBossCandidates_TEST` 확정.
- 이전 보호 진입 구조는 기존 맵에 보존. 새 맵에만 이번 사용자 조건을 적용.

### 제안 — 새 맵 배치

- `/Game/Portfolio02/Levels/L_OutdoorPilgrimage_Review_v01` 생성 및 저장.
- 바닥 Z1560, 정면 접속 Z1800. 일반 바닥 폭800 / 오른쪽900 / 경계벽320uu는 검토용 제안값.
- 천막2 / 기존 테스트 적2. 새 맵 전용 전투 반경750uu와 단일 정면 맵 전환 보조 액터.
- 내부 이미지 원본은 미확보. 게임 속 판에 한글로 미촬영 임시 영역 표시.

### 검증

- C++ Editor Development 빌드 성공. 런타임 카메라 −50/0/2000/45 확인.
- 오른쪽 무처치 자동 연속 이동: 최초 이동부터 전환 트리거까지16.627초, 누적6781.7uu. 지정 내부 맵 전환 성공.
- 왼쪽은 후면과 측면까지 이동했으나 정면 복귀부에서 막힘. 완주 시간 없음.
- 실제 편집기 렌더9장과 설명용 한글 배치도 작성. 실제 PIE 캡처로 오인하지 않도록 구분.
- 기존 애셋692개 SHA256 대조: 변경0. 기존 맵 저장·덮어쓰기 없음.

### 미정 / 미충족

- 2~4분 목표에 미달. 전투 포함 오른쪽 시간 우위는 미검증.
- 공통 길에서 신전 최초 인지 불가.
- 고정 시점에서 기둥·계단·우회로 동시 판독 미흡.
- 왼쪽 복귀 길과 옛 계단 상부 접속부 충돌: 폰이(21435,-1200,1652)에서 정지.
- 관찰 틈의 기둥·벽 겹침으로 이미지 판독 불가. 보상과 평면성 검증은 미완료.
- 공간 수정 방향은 사용자가 결정할 대상. 관찰/가능한 문제/근거/수정 가능 변수는 상세 보고서에 기록.

### 삭제 / 미채택

- 히든 보상, 퍼즐, 추가 입구, 높이로 차별화한 전망대, 신규 이야기·경로 장치 미추가.
- 제작 중 회전 인수 바인딩 오류와 정면 기단의 계단 겹침은 선언한 배치로 복구. 기단 수정 전 새 맵 보존본을 Saved/OutdoorReview에 저장.

### 결과물

- `Docs/OutdoorReview_v01/검토보고서.md`
- `Docs/OutdoorReview_v01/01_전체공간배치도.png`
- `Docs/OutdoorReview_v01/Engine/` 및 `Evidence/`

## 2026-09-07 — v02 길이 확대 / 양쪽 상승 / 접합부 정리

### 확정 — 사용자 최신 요청

- 현재 형태를 유지하며 길이를 늘린다. 왼쪽은 계단으로 점점 올라가며 오른쪽도 함께 상승한다.
- 최종 신전의 높이를 올릴 수 있다. 이어지는 부분의 틈과 돌출을 정리한다.
- 이번 요청은 v02에서 이전 평면 조건을 대체한다. v01과 기존 던전은 보존한다.

### 제안 — 구현값

- 새 맵 `L_OutdoorPilgrimage_Review_v02`. XY 중심점 간격 1.6배, 폭은 공통/왼쪽800 및 오른쪽900uu.
- 계단 단차30uu. 시작Z1560 → 분기1800 → 옛길차단3000 → 후면/정면합류3600 → 입구3840.
- 입구 XY(21500,0), +X 진입 유지. 선택된 내부 던전 애셋은 수정 없이 맵 전환으로 연결.
- 바닥과 경계벽의 윤곽을 합친 새 메시로 겹친 상자의 접합부를 교체했다.

### 검증

- 실제 PIE 연속 이동 입력, 점프/대시/공격/순간이동 없음.
- 오른쪽: 27.174초 / 11385.7uu 누적 이동, 지정 던전 전환 성공.
- 왼쪽: 88.436초 / 36277.3uu 누적 이동, 지정 던전 전환 성공.
- 생성 메시의 뒤집힌 면 방향으로 발생한 충돌과 음영 오류를 수정 후 양쪽 완주 확인.
- 기존 애셋 및 v01 총693개 SHA256 비교, 변경0개.
- 최종 편집기 렌더에서 계단참과 갈림길 연결 상태 확인. PIE 화면과 구분해 보고서에 기록.

### 미정 / 남은 검증

- 제안 치수의 사용자 확정, 2~4분 목표, 전투 포함 시간 우위, 고정 카메라의 목표/차단/우회 판독.
- 중간보스 이미지는 미촬영 임시 영역. 관찰 틈의 가독성과 평면성은 검증 미완료.
- 자동 완주 결과는 사람의 탐색 시간이나 모든 가장자리 충돌 검증을 대신하지 않는다.

### 결과물

- `Docs/OutdoorReview_v02/검토보고서.md`, `Engine/`, `보존검사.json`
- `Saved/OutdoorReviewV02/play_right.json`, `play_left.json`, `geometry.json`

## 2026-09-07 — v03 왼쪽 담 부분 파손 예시

- 확정: 사용자가 단순한 담 파손과 회색 블록 2~3개 예시 적용에 동의. 불꽃·그을음 생략.
- 적용: v02 복제 맵 `L_OutdoorPilgrimage_Review_v03`. 왼쪽 계단참 옆 담 상단 일부를 큰 면으로 낮추고 잔해 블록3개 배치. 바닥·계단·전체 동선은 수정하지 않음.
- 보존: v02 맵 SHA256 전후 일치. 경계벽은 v03 전용 메시를 복제해 수정했고 v02 공유 메시에는 기록하지 않음.
- 확인: 실제 UE 편집기 렌더로 단순 파손 및 잔해 위치 확인. 이번 변경 후 PIE 이동 검사는 미실시.
- 자료: `Docs/OutdoorReview_v03/Engine/02_파손담과단순잔해.png`, `Saved/OutdoorReviewV03/result.json`.

## 2026-09-07 — v03 왼쪽 다리 전체 파손 확장

- 확정: 사용자가 현재 단순 파손 컨셉을 왼쪽 다리 전체에 확대 요청. 불꽃·그을음 없이 큰 파손 면과 회색 블록으로 표현.
- 보존: 확대 전 맵을 `L_OutdoorPilgrimage_Review_v03_PartialBackup`으로 복제. 기존 부분 파손 메시 보존, 확대 전용 `SM_Wall_LeftRuined` 생성.
- 적용: 왼쪽 계단길·바깥 우회·측면 복귀의 담 상단에 비대칭 파손을 분산하고 회색 잔해66개 추가. 온전한 담 구간과 중앙 통행 영역 유지. 바닥/계단 메시와 오른쪽 길, 신전 액터는 수정하지 않음.
- 저장: 다른 에디터의 파일 잠금으로 첫 저장 실패. 사용자 맵 전환 후 v03 저장 성공,139액터 확인.
- 화면: `Docs/OutdoorReview_v03/Expanded/`는 실제 UE 편집기 렌더이며 PIE 스크린샷은 아님.
- 검증: 잔해 배치 후 왼쪽 경로 연속 입력 PIE 완주 및 지정 던전 전환 성공(89.204초). 점프·대시·공격 없이 검사. 모든 가장자리 이동과 파손 담 월경 가능성까지 검증한 것은 아님. 원자료 `Saved/OutdoorReviewV03/play_left.json`.

## 2026-09-08 — v04 양쪽 길의 배치물 제안

### 확정 — 요청 범위

- 최신 v03 탑뷰 촬영. 이번 것을 복사해 양쪽 길에 어울리는 배치안을 제작하라는 사용자 요청.
- 배치안은 복사본 `L_OutdoorPilgrimage_Review_v04`에 제작. v01/v02/v03 보존.

### 제안 — 추가 배치

- 왼쪽 담 위 부서진 기둥 밑동8묶음(기단·원기둥·상단 각1), 오른쪽 기존 천막 옆 보급 더미4묶음(상자2·통1), 주 이동로 경계석2. 총38액터.
- 순례길의 석조 형태와 경계, 천막의 사용 공간을 구분하는 제안. 단순 도형·기존 회색 머티리얼 사용.
- 위치·높이·간격은 검토용 제안값. 새 기믹·입구·경로·적·보상 기능 없음.
- 기둥 받침은 파손 담의 실제 메시 높이를 재계산해 접합. 기존139액터 위치·회전·크기 변경0.

### 검증과 남은 판단

- v01/v02/v03 SHA256 전후 일치. 검사 파일 `Saved/OutdoorReviewV04/final_audit.json`.
- 편집기 화면 확인: 오른쪽 보급 더미 일부는 천막 지붕·담·프레임에 가려짐. 왼쪽 기둥 상단도 가까운 시점에서 화면 밖으로 나감. 자동으로 천막이나 카메라를 변경하지 않음.
- 자세한 관찰/가능한 문제/근거/수정 가능 변수는 v04 검토보고서에 기록.
- 기존 목표 시간·초기 목표 인지·관찰 보상의 미검증 항목을 해결한 버전으로 간주하지 않음.

### 결과물

- `Docs/OutdoorReview_v03/Topview/00_v03_전체탑뷰.png`
- `Docs/OutdoorReview_v04/Engine/`, `Topview/`, `검토보고서.md`
- 양쪽 PIE 자동 연속 이동 후 지정 던전 전환 성공: 오른쪽27.170초 / 왼쪽89.175초. 점프·대시·공격 없음. 사람의 탐색 시간과 전투 포함 시간 아님.

## 2026-09-08 — 기둥밑동02를 넘어뜨려 오른쪽 틈만 남김

- 확정: 사용자가 `제안_왼쪽_기둥밑동_02`를 바닥에 눕혀 왼쪽을 막고 오른쪽으로만 통과하게 요청.
- 보존: `L_OutdoorPilgrimage_Review_v04_BeforeColumn02` 복제. 기존 메시와 일체형 바닥/벽은 수정하지 않음.
- 제안 치수: 기둥 길이560uu·지름180uu. 진행 방향 기준 횡축−400~+160을 차지하고, +160~+400에 명목240uu 틈을 남김. 원래120uu 길이로는 차단 요구를 표현할 수 없어 액터 크기 조절.
- 기둥 위치(17705.663,-4036.057,2760), Pitch90°, Yaw66.038°. 상단 조각을 왼쪽 끝으로 함께 이동, 기존 기단 유지.
- PIE 검증: 중앙 직진 시 기둥 앞(17642.465,-3870.236,2728.838)에서 정지. 오른쪽 틈 경유 시 다음 계단참까지 연속 이동 성공. 점프·대시·공격 없음. 이번 검사는 국소 통과 검사이며 던전 전체 재완주 아님.
- 자료: `Saved/Column02Blocker/placement.json`, `play_center.json`, `play_left.json`; `Docs/Column02Blocker/Engine/` 실제 편집기 렌더.
- 최초 저장은 다른 에디터의 파일 잠금으로 실패했으나 재시도에서 v04 저장 성공. `Saved/OutdoorReview/column02_final_save.json`에 기록.

## 2026-09-08 — v05 열린 마당 복사본

- 확정: 사용자의 최신 지시에 따라 v04를 보존하고 `L_OutdoorPilgrimage_Review_v05_OpenCourt`를 제작. 야외 구간만 수정. 짧은 진입로 → 열린 마당 → 오른쪽 빠른 접근 / 왼쪽 긴 순례길 → 정면 입구 구성.
- 확정: 오른쪽은 포장 밖 이동 가능한 면으로 구성하고 낮은 폐허를 분산 배치. 천막 옆 빈 공간만 확보하며 이벤트·몬스터·강제 전투는 추가하지 않음. 기존 신전 및 던전 내부, 정면 입구 유지.
- 확정: 왼쪽 기존 계단 높이·외곽 우회 동선 보존. 옛길 차단 기둥과 너머의 계단 유지, 붕괴 확인 마당과 신전 왼쪽 관찰 확장부 배치. 관찰 면은 실제 던전 이미지 미연결.
- 제안: 바닥 전체 실측 범위 X13,520 × Y12,170uu. 임시 작업 사각형14,200 ×12,800uu는 게임 숨김·충돌 없음. 천막 옆 빈 중심 영역1,000 ×1,000uu. 이 수치들을 향후 확정 치수로 간주하지 않음.
- 실측: PIE 실제 속도400uu/s, 단차 허용45uu, 실제 카메라 Pitch−50°, Yaw0°, 거리 설정2,000uu, FOV45°. 속도·카메라 수정 없음.
- 실측: 점프·대시·전투 없이 연속 이동하여 오른쪽17.914초 / 포장 밖 우회20.804초 / 왼쪽89.658초에 같은 지정 던전 전환 성공. 각1회 검사이며 사람의 탐색 시간·전투 시간 아님. 초기화2초 및 촬영 정지 별도.
- 기술 수정: 새 왼쪽 연결 계단과 기존 계단참의 겹침으로 생긴 턱을 정리. 범위·끝점 높이 및30uu 단차 유지. 기단과 겹친 자동 경유점은 옆으로 이동시켜 검사했으며 그 이유로 레벨의 기단을 옮기지는 않음.
- 실측: 중앙 Y100 직진은 (17041.37,100,2225.55) 부근에서 낮은 지형 덩어리에 정지. 다른 오른쪽 이동선들은 통과 확인.
- 편집성: 바닥68개·외곽 경계45개 독립 액터와 로컬 피벗. 전체 단일 메시보다 부분 선택이 가능하나 파라미터형 프랍 키트는 아님.
- 미해결 관찰: 진입로 끝에서 신전이 화면 밖. 붕괴 기둥 너머 계단의 동시 인지 부족. 관찰 임시 면 일부가 화면 상단과 신전 기둥에 가림. 사용자 판단 없이 이 문제의 공간 배치를 재수정하지 않음.
- 미정·검증 항목: 첫 방문자의 유도 해석, 관찰 발견성·이미지 평면성·전체 난간 통과 불가, 여러 화면비 가림, 이벤트 포함40~50초. 현재 가독성 검증 완료로 표현하지 않음.
- 보존 확인: v04 및 선택 던전 파일 SHA-256 일치, 신전 액터 변환값 변경 없음. v05 저장 완료.
- 결과물: `Docs/OutdoorReview_v05/검토보고서.md`, `Editor/` 전체 탑뷰·사선, `PIE/` 실제 플레이 카메라 렌더8장, `01_실제쿼터뷰_모음.png`, `00_설명용배치도.png`. 설명용 도식과 실제 엔진 렌더를 구분.

## 2026-09-08 — v06 기둥 너머 계단 시야

- 확정: 사용자 승인에 따라 쓰러진 기둥 몸통 지름520→200uu. X/Y·회전·길이1,150uu 유지. 바닥 접촉을 유지하기 위한 중심 높이만3270→3110으로 조정.
- 보존: v05 파일이 다른 에디터에 잠겨 있어 `L_OutdoorPilgrimage_Review_v06_ColumnSight` 복사본에서 진행. 정적 메시 액터 비교 시 기둥 몸통 한 개만 변경됨.
- 검사: 동일 위치 PIE 전후 촬영 및 정면 이동 입력으로 차단 유지 확인. 완주시간 재측정이나 모든 끝단 틈 검사 아님.
- 관찰: 차단 앞에서 계단 일부 노출 개선. 먼 접근 지점에서 계단이 화면 밖인 문제는 남음. 계단·우회로·카메라를 추가 수정하지 않음.
- 자료: `Docs/ColumnVisibility/검토.md`, `전후비교.png`, `before/`, `after/`, `Saved/ColumnVisibility/`.

### 후속 수정 — 끝 받침 크기 일치

- 사용자 지시: 기둥 받침도 몸통과 함께 축소.
- v06의 `쓰러진기둥_주두` 단면620×550→238.46×211.54uu로 축소. 몸통과 동일한200/520 비율 적용, 축 방향 두께160uu 및 바닥 접촉Z2985 유지.
- v06 저장 성공. 비교 화면은 받침 축소까지 반영해 갱신. `Saved/ColumnVisibility/cap_change.json`.

## 2026-09-08 — 야외 맵 한글 이름 및 v07 관찰 보강

- 사용자 확정 범위: 최근 순례길 야외 맵8개만 이름 변경. `던전외부필드_v01`~`v06`, v03 부분 백업→`v03-1`, v04 기둥 변경 전→`v04-1`. 예전 Entrance·Field·내부 맵 이름 유지.
- UE AssetTools 이름 변경 및8개 맵 로드 확인. 에디터 뷰포트 기록의 참조로 인한 자동 알림 때문에 초기 호출은 중단됐으며, 별도 UE 커맨드릿에서 정상 처리. 리디렉터와 변경 전 백업 보존. `Docs/야외맵_이름변경.md`.
- 최신 사용자 지시로 v06 복제→`던전외부필드_v07`. 왼쪽 관찰 틈과 바깥쪽 유도 기둥만 보강. 기존 옛길 차단 및 던전 정면 입구·내부 보존.
- 관찰 정보 변경 확정: 중간보스 사전 발견 대신 최종 색 입력 장치의 존재를 알림. 정답 순서·힌트 노출 금지. E로 선택 열람, 자동 실행 없음.
- 공간 배치 저장 및 임시 텍스처 준비 완료. 관찰 액터/UI C++는 작성했으나 에디터 종료 후 빌드·연결·플레이 검증 대기. 동작 검증 완료로 표현하지 않음.
- 진행 상태와 남은 검사: `Docs/OutdoorObservation_v07/작업현황.md`.

## 2026-09-09 v08 중앙 호수 및 천막 접근
확정: v07 복제 후 중앙 선택 덩어리 주변 절삭, 수면/옹벽 추가, 오른쪽 열린 마당 유지. 사용자 허용에 따라 천막 하나 이동 범위 확대 적용. 기존 내부·정면 입구·카메라·이동·왼쪽 우회·관찰 보존.
검증: 일반 이동 빠른 경로17.92초(400uu/s, 반사 없이), 왼쪽89.82초, 현재 이름의 던전 내부 연결. 원본파일 해시 동일, 보호액터69개 Transform 동일.
미달/미정: 간단한 수면에서 성당 반사 식별 실패. 수면 높이·단차 및 별도 반사 방식은 제안만 기록. 외부 사람의 첫 탐색 검증과 전체 경계 점프/대시 검증 없음. 자세한 캡처·근거는 Docs/OutdoorLake_v08/검토결과.md.

## 2026-09-10 — 호수 반사 옹벽·지붕 개별 비교

- 사용자 승인: 왼쪽250uu 위치에서 기존 / 옹벽만 숨김 / 지붕만 상승 비교. 복사본v08-1의 PIE에서만 변형하고 저장하지 않음.
- 검증: 실제 카메라·플레이어 위치 동일. 옹벽3개 숨김 상태 및 지붕2개 실제Z4730→5730→6730→4730을 읽어 확인. 최초 Static 이동 실패 시도는 무효 처리하고 재촬영.
- 관찰: 옹벽 숨김은 수면 노출을 늘림. 지붕 상승은 반사상의 어두운 면과 테두리를 바꿈. 어느 개별 조건에서도 성당으로 식별할 만한 반사상은 확보하지 못함. 외부 테스터 인지 검증 아님.
- 미적용: 지붕 상승·옹벽 삭제를 최종 설계에 반영하지 않음. 다른 지형 가림과 화면 잘림, 변수 조합은 아직 원인 분리하지 않음.
- 근거: `Docs/ReflectionTrial_v08_1/Occlusion/검토결과.md`, `비교보기.html`, `results.json`, 이번 비교 전후 보존 확인 `restoration.json`.

## 2026-09-10 — v08-2 사각 종탑 시안

- 사용자 승인: 기존 지붕을 유지하고 사각 종탑 하나를 임시로 붙여 비교. v08 복사본 `던전외부필드_v08-2`에만 추가.
- 제안 배치: 정면 쪽 지붕 위 XY(22220,0), 몸통900×900uu, 작은 지붕까지 최대Z약6905.55. 몸통·처마·상부 개구부·작은 지붕을 독립 부품9개로 구성. 치수·위치는 최종 확정 아님.
- 검증: 기존 정적 메시216개 위치·회전·크기·메시·재질 일치. v08/v08-1/DefaultEngine.ini 해시 유지. 기존 본체·지붕·동선 변경 없음.
- 관찰: 외형 검토 렌더2장, 기억한 기준의 전체 탑뷰, 실제 고정 카메라6장 촬영. 종탑 숨김/표시 시 수면 반사 변화는 있지만, 작은 지붕과 개구부를 읽을 만한 반사 실루엣은 미확보. 기존 반사 설정에서는 더 흐릿하게 보임. 외부 테스터 인지 검증 아님.
- 미적용: 반사에 맞춘 옹벽·카메라·본체 추가 변경. 신규 종·소리·상호작용·입구 없음.
- 보존: 시험 후 기존 통계 수집 설정 복원, 사용자 로그 유지. v08-2를 에디터에서 열어 둠. 평면 반사 클립 옵션은 이번 실행에만 적용.
- 자료: `Docs/BellTowerTrial_v08_2/검토결과.md`, `비교보기.html`, `verification.json`. 재현 실행 스크립트 `Tools/launch_belltower_trial.ps1`.

## 2026-09-10 — v09 A/B/C 오른쪽 마당 유도 비교

- 사용자 승인: 원본 보존, 같은 기본 맵에서 포장 / 포장+천막 / 포장+천막+낮은 구조 단서의 세 복사본 비교. `던전외부필드_v09_A/B/C` 저장. 최종안 미확정.
- 공통 종탑 수정: 왼쪽 앞 XY(21970,-1280), 기존 벽체와 연결되는 몸통, 단순 종 윤곽4부품 추가. 종 소리·상호작용·새 입구 없음. 기존 성당 지붕 유지. 세 안 공통 동일.
- A: 굽어지는 포장9조각, 기존7개 조정+2개 추가. 포장 밖 이동 가능. B: A 유지+아래 천막(+250,-272)/Yaw−32°, 위 천막(+100,-80)/Yaw−25° 조정. C: B 유지+접근부 기단2개·물가 흔적3개. 치수·위치는 이번 시안값.
- 검증: 기존 레벨21개 및 DefaultEngine.ini 해시 유지. 승인 대상 외 기존 액터 값 동일. 공통 포장·종탑, B/C 천막 일치. 대응 실제 카메라·플레이어 좌표/회전0.1 이내, 400uu/s/−50°/Yaw0/거리2000/FOV45 유지.
- 이동 실측: 알려진 빠른 경로 A17.925/B17.937/C17.922초, 아래 천막 쪽 별도 경로 A20.842/B20.814/C20.935초. 각1회 일반 이동, 기존 정면 입구→던전내부최종 전환. 외부 테스터 첫 탐색 시간이 아님.
- 경계: C 물가3곳·왼쪽 교차1곳의 입력 검사에서 수면 내부 진입 없음/종료 시 지지 유지. C 옆 이동 양방향 약1164.6/1163.1uu. 전수 점프·대시 검사는 미실시.
- 관찰: C의 물가와 접근부 구분 강화. B/C 아래 천막 노출은 늘지만 계단 노출까지 연결되지 않음. B/C 위 천막 지붕이 동일 촬영점에서 캐릭터 일부를 가림. 종탑 반사 식별 미달.
- 추천: 현재 저장된 세 안 그대로라면 A 우선. 낮은 비용·공간 여유·위 천막의 추가 가림 부재를 고려. C의 구조 단서는 후속 가치 있음. 인지 성공률은 미측정, 공간 문제를 이유로 임의 제4안/추가 보정 미적용.
- 결과물: `Docs/FieldGuideABC_v09/비교보기.html`, `비교보고서.md`, A/B/C 원본 PNG37장, `verification.json`. 기존 탑뷰 기준 보존. 통계 수집 설정 복원, 사용자 원본 로그 유지. 에디터는 C 비교본을 열어 둠.

## 2026-09-10 — 사용자 최종 결정: v09_A 기준안 채택

- 확정: 이후 개발·외부 플레이테스트 기준은 v09_A. 포장9개·기존 천막·열린 마당·호수와 왼쪽 비연결 경계·성당 접근로/정면 입구 유지.
- 미채택: B 천막 위치/회전, C 접근 기단2개/물가 조각3개. 비교 맵·이미지·데이터는 보존하며 A에 병합하지 않음.
- 삭제 확정: 성당 반사의 이동 유도 역할. 호수는 이동 경계와 공간 분리 역할만 유지. A의 시험용 PlanarReflection 액터1개 제거·저장. 나머지 액터 값 동일, 기존 천막 유지, C 추가물 부재, B/C 맵과 비교 PNG37장 보존 확인.
- 사용자 판단: 초입 길찾기 성공은 어느 안도 입증하지 못함. B/C 천막의 계단 노출 개선 없음·캐릭터 가림 발생. C의 구간 구분은 강화되나 인공 통로 인상 및 추가 충돌/가림 비용 발생. A가 자유도와 쿼터뷰 가독성을 가장 적게 훼손한다고 결정.
- 후속 원칙: A는 외부 테스트 후보안. 외부 테스트에서 반복 혼란이 확인될 때만 해당 문제 위치에 대해 별도 최소 수정안 제작. 제작자 이동시간을 첫 탐색 성공이나 발견률로 사용하지 않음.
- 보존: 정리 전 A 파일은 `Docs/FieldGuideABC_v09/Decision/던전외부필드_v09_A_채택정리전.umap`. `adoption_verification.json`에 저장·불변값 기록. 사용자 에디터의 내부 맵 미저장 변경 보호를 위해 맵 전환 없이 별도 커맨드릿에서 처리. 내부 맵 유지.
- 기술 기록: 첫 커맨드릿은 GUI용 LevelEditorSubsystem 호출에서 실패해 저장 전 종료됨. LevelEditorSubsystem 호출을 제거한 별도 처리에서 0 errors/0 warnings 저장 완료. `Saved/Logs/AdoptV09A_Commandlet2.log`.
- 최신 결정 문서: `Docs/FieldGuideABC_v09/최종결정.md`. 후속 작업은 AGENTS.md와 `development_baseline.json`의 v09_A 기준을 따른다. 외부 테스트는 이번에 실행하지 않음.

## 2026-09-10 — v09_A 포장 접지 보정 (사용자 요청)

- 확정: 기존 포장 9개의 배치 흐름을 유지하고 공중 부양·지면 매몰·경사 연결부 잘림을 정리한다.
- 구현: 기존 바닥/계단 메시의 실제 면 경계로 포장 형상을 맞춤. 개별 액터 9개, 위치/회전/스케일/재질/NoCollision 유지. 다른 레벨 액터와 B/C 비교 기록 보존.
- 검증: 바닥 충돌 표면 106개 표본 대조, PIE 고정 카메라 3개 위치 캡처 확인. 전체 경로 주행이나 외부 길찾기 테스트는 수행하지 않음.
- 상세 기록 및 수정 전 백업: `Docs/FieldGuideABC_v09/PavingFit/바닥정리.md`.

## 2026-09-10 — v09_A 파손 관찰 틈 (사용자 요청)

- 확정: 기존 E 관찰 위치의 신전 벽에 각진 파손 개구부를 먼저 제작. 사용자가 이후 아래쪽 색 3개 구간만 담은 이미지를 같은 틈 형태로 제작할 예정.
- 구현: `관찰틈_파손벽체` 1개 추가. 기존 차폐면·난간·관찰 이미지·기능·다른 액터 보존. 수정 전 A 맵 백업.
- 확인: 실제 구멍/주변 고형 벽의 충돌 구분, 기존 API 열기·닫기와 이동 복귀, 통과 차단 확인. 물리 E 키 재시험과 HUD 시각 검증은 미완료.
- 관찰: 기존 주두가 고정 쿼터뷰에서 틈을 가림. 기둥·주두·카메라 자동 수정하지 않고 사용자 판단 대상으로 보고.
- 자료: `Docs/ObservationCrack_v09A/관찰틈_작업기록.md`, 정면 캡처 및 동일 윤곽 SVG.

### 2026-09-10 관찰 틈 기존 벽선 비교
- 사용자 요청: 돌출된 관찰면을 기존 안쪽 벽선에 맞춰 실제 플레이 시야 확인.
- 적용: v09_A의 작은 틈은 유지하고 벽 전면 Y=-1872로 복귀. 이전 돌출 상태 별도 보존.
- 검증: 실제 고정 카메라에서 E 안내 표시 및 기둥 통과 후 활성화 확인. 틈 자체는 주두·처마로 가림. 가시성 성공으로 판단하지 않음.
- 미정: 가리는 주두/처마 조정 여부는 사용자 결정. 임의 수정하지 않음.
- 상세: Docs/ObservationCrack_v09A/FlushCheck/검토결과.md


### 2026-09-10 신전 기둥 붕괴 비교본
- 사용자 결정: 원본을 남기고 복제본에서 관찰 틈 앞 신전 기둥 제거. 기존 쓰러진 기둥과의 관계를 비교.
- 구현: 던전외부필드_v09-1_A 생성, 신전기둥_22600_-1250 및 해당 주두 제거. 원본 v09_A 파일 해시 일치 확인.
- 검증: 실제 동일 고정 카메라에서 E 안내와 기둥 통과 후 활성화 유지. 틈은 처마 때문에 계속 가림. 처마 추가 변경은 미정.
- 비교 이미지: Docs/ObservationCrack_v09A/ColumnComparison/비교보기.html


### 2026-09-10 쓰러진 기둥 주두·크기 통일
- 사용자 확정: 관찰 기둥 왼쪽 끝에 주두 연결, 가능하면 옛길 차단 기둥도 같은 크기로 통일.
- v09-1_A 적용: 관찰 주두 추가. 옛길 몸통 지름280/길이720uu, 주두384×384×100uu. 원본 v09_A 보존.
- 제작자 검사: 옛길7개 선 일반 이동 모두 차단, 낙하 없음. 관찰 구간 통과 후 E 활성화 유지. 외부 길찾기 검증 아님.
- 상세 및 이미지: Docs/ObservationCrack_v09A/ColumnCaps/작업기록.md


### 2026-09-10 관찰 틈 대상 난간으로 변경
- 사용자 결정: 관찰마당_낮은이동차단난간에 구멍 배치.
- v09-1_A 적용: 작은 파손 틈을 난간으로 이동, 뒤쪽 신전 벽의 기존 틈 복구. E 범위는 유지하고 시선 판정점만 이동.
- 실제 확인: 고정 쿼터뷰에서 틈과 E 안내 표시. 5개 접근선 모두 이동 차단·낙하 없음. 외부 발견 검증은 미실시, 이미지 임시 상태 유지.
- 상세: Docs/ObservationCrack_v09A/RailingOpening/작업기록.md


### 2026-09-10 관찰 화면 실제 던전 캡처 연결
- 사용자 승인: 기존 던전의 아래 색 입력 장치3개를 촬영하고 난간 틈 모양으로 제한하여 E 관찰 화면에 연결.
- v09-1_A에 실제 캡처 텍스처 연결, 임시 이미지 플래그 해제. 던전 원본 해시 불변 및 사용자 필드 변경 보존.
- 실제 PIE 화면 표시·닫기 후 입력 복구 확인. 외부 이해도 검증 미실시. 촬영 각도·마스크 윤곽·원본 PNG 보존.
- 자료: Docs/ObservationCrack_v09A/InteriorPhoto/촬영및연결기록.md


### 2026-09-10 관찰 이미지 거리·대각선 구도 수정
- 사용자 요청: 가까운 촬영을 완화하고 대각선으로 보이도록 변경.
- 정지 이미지 촬영 거리1250→1800uu, 수평각0→30도. 실제 플레이 카메라와 던전 구조 유지.
- v09-1_A에 새 캡처 연결, 이전 이미지 보존. 실제 UI 표시 및 닫기 후 이동 복구 확인.
- 자료: Docs/ObservationCrack_v09A/InteriorPhoto/Diagonal/수정기록.md


### 2026-09-10 v09-1_A 두 판 외부 테스트 세팅
- 사용자 확정: 최근 관찰 구간·이미지가 포함된 v09-1_A에 두 판 테스트 환경 설정.
- 진행자 메뉴: 새 참가자/1판 보존 후 반대 경로2판/테스트 종료. 던전 진입 확인 시 기록 보존 후 PIE 종료, 같은 시작점 재시작.
- 원래 v09_A 기준 및 B/C 비교자료 보존. 공간·이동·카메라 변경 없음. 테스트 대상은 Docs/FieldTelemetry/active_test_profile.json으로 별도 지정.
- 제작자 T9901의 두 회차로 기록 분리·묶음·진입 및 재시작 기능 확인. 위치 강제 이동 검사이므로 플레이 시간·길찾기 근거로 사용 금지.
- 사용법: Docs/FieldTelemetry/두판플레이테스트_진행법.md


### 2026-09-10 — v09-1_A 정면 입구 가장자리 전환 누락 수정
- 사용자 관찰: 입구 양옆에 붙어서 들어가면 던전 전환 없이 외형 건물 내부로 들어감.
- 확인: 양쪽 정면벽 안쪽 경계 Y=-480/+480으로 실제 개구부 폭 960uu. 기존 전환 판정은 중앙 ±240uu만 검사.
- 수정: EntranceHalfWidth를 인스턴스 편집 값으로 노출하고 현재 v09-1_A에서 480uu로 설정. 다른 맵의 기본값은 240uu 유지. 입구 기준 좌표 (21500,0,3840), 높이 판정, +X 진입 및 목적지 던전 유지.
- 보존: Saved/EntranceEdgeFix/던전외부필드_v09-1_A_before_entrance_width.umap.
- 검증 완료: 실제 PIE 캐릭터를 입구 중앙 및 Y=-435/+435 가장자리에 배치한 뒤 일반 +X 이동 입력으로 세 경우 모두 던전내부최종 로드 확인. MaxWalkSpeed=400, 캡슐 반경=35. 전체 경로 시간 측정은 아님.
- 참가자 로그 수집을 끄고 검사했으며 기존 참가자 run 폴더 목록이 그대로임을 확인. 실제 검증 자료 Saved/EntranceEdgeFix/play_verification.json.

### 2026-09-10 — v09-1_A 초입 계단 끝 곡선 연결 및 왼쪽 벽 높이 보강
- 사용자 지정: 표시한 초입 계단 옆끝을 벽까지 둥글게 연결하고, 왼쪽 벽을 타고 올라가지 못하도록 높임.
- 적용: 기존 시작 계단 00 및 사용자가 추가한 1/2/3의 높이·직선부를 유지하고 왼쪽 끝에 곡선 연결부 추가. V05_외곽경계_15000_-1600 상단 +200uu, 인접 -3200 구간은 접합부에서 400uu에 걸쳐 기존 높이로 연결. 벽 바닥/XY 경계 유지.
- 해당 맵에만 별도 Static Mesh 6개 연결. 원본 공용 Mesh 및 다른 레벨 보존. 백업 Saved/StartStairJoin/던전외부필드_v09-1_A_before_stair_join.umap.
- 실제 PIE 검사: 중앙 계단 상승·곡선 끝부분 상승 통과. 아래 벽 밀기·계단 옆 벽 밀기·벽을 향한 대각선 상승 입력 3개 검사에서 플레이어 중심이 벽의 평면 영역에 진입하지 않음. 전체 경계와 모든 입력 조합 검증은 아님. 이동속도400uu/s, MaxStepHeight45uu.
- 참가자 기록 수집을 끄고 기술 검증. 기존 run 목록 유지 확인. 실행 증거 Saved/StartStairJoin/checked_movement_verification.json.
- 이미지 Docs/StartStairJoin/before, after, close: UE 편집기 SceneCapture 렌더이며 실제 플레이 카메라 캡처와 구분. 참가자 테스트 전후 공간 조건을 비교할 때 본 수정 시점을 구분할 것.

### 2026-09-10 — 캐릭터 가림 자동 벽 흐림
- 사용자 요청: 캐릭터가 벽에 약70% 가려지면 반투명/뿌연 표현으로 보이도록 하는 시스템.
- v09-1_A 연결 액터에서만 활성화. 캐릭터 컴포넌트 10개 신체 표본 중7개 이상 차단 시 시작,5개 이하 시 복구. 정확한 화면 면적 측정은 아님.
- 동일 회색 재질의 벽·난간85개 태그 지정. 캐릭터 주변 국소 디더 페이드, 충돌·고정 카메라·이동·원본 머티리얼 보존. 필드 외 기존 던전 시스템 미변경.
- 실제 PIE 초입벽3곳에서 캐릭터 노출 확인, 열린 마당 비활성 확인, 연속 이동 진입/이탈에 따른 자동 시작/복구 확인. 기존 참가자 로그에 기술 검사 기록 추가하지 않음.
- 상세와 이미지: Docs/CharacterOcclusion/구현과검증.md. 이후 플레이테스트에서는 본 렌더 조건 변경 시점을 구분할 것.


## 2026-09-10 — AI 전용 기술 검사 구성

- 확정 요청: AI용 테스트로 이동·기능 문제를 확인. 공간 수정은 자동 수행하지 않음.
- 구현: Tools/ai_field_tests.py, ai_field_cases.json, AI-Field-Test.cmd. 3개 기존 전체 경로, 계단 접합부 2개, 입구 중앙/양끝 3개, 관찰 UI API, 캐릭터 가림/복구 1개씩 총 10개.
- 분리: Saved/AIFieldTests 실행별 폴더. 참가자 설정과 원본 로그 해시 보존 확인. 참가자 로그 수집이 켜져 있으면 실행 거부.
- 한계: 좌표 경유 방식의 기술 회귀 검사이며 시각적 첫 탐색 AI가 아님. 도달을 발견으로 표현하지 않음. 정체는 원인 확인 전 REVIEW로 분류.
- 검증 결과: 해당 실행 폴더 result.json 및 결과.md 참조. 범위 밖의 맵 결함 없음 또는 길찾기 성공을 주장하지 않음.


## 2026-09-10 — 100개 이동 조건 자동 검사

- 사용자 요청: 낮은 토큰 사용으로 100개 이동 조건 검사, 히트맵/검사 방식/문제 위치 보존. 다음 별도 100회는 화면 노출 면적률 측정 구상.
- 완료: 20개 실제 통과 구간 × 횡변위 5종(-140,-70,0,70,140uu). 99 PASS, 1 REVIEW. 100회 전체 맵 완주나 사용자 성공률이 아님.
- M081: 관찰 구간 기존 외곽 경계벽 방향 입력에서 정체 재현. 바깥쪽으로 방향 전환 후 통과. 영구 끼임 미확인. 맵 수정 없음.
- 결과: Docs/AIFieldTests100_20260910_224529/검사보기.html. 같은 탑뷰 촬영 기준으로 검사 이동선/체류 히트맵/문제 후보 위치 생성. 참가자 로그/설정 동일 확인.
- 미정: 두 번째 화면 노출 검사는 대상 및 기준 구역 답변 대기. 면적률은 균등 면적 표본의 추정치이며 실제 사람의 발견 확률로 표현하지 않음.

## 2026-09-10 — 지정 대상 화면 노출 100개 위치 검사 완료

- 확정 대상: 호수 반사 랜드마크, 기둥 너머 옛길 계단, 성당 옆 힌트 균열, 초입 좌측 계단/우측 바닥 순서, 오른쪽 천막/지형/천막2/상부 접근로.
- 방법: 관련 영역 5곳 × 20개 균등 셀 무작위 표본. 실제 PIE 캐릭터를 세워 고정 카메라 1099×862, pitch -50, yaw 0, FOV45, 이동속도400uu/s를 기록. 150uu 후보 셀 근사이며 전체 면적 전수 조사나 100회 완주가 아님.
- 완료: 정상 플레이 렌더 100장, 표면 광선 노출 원본, 대상별 탑뷰 지도9장, 별도 연속 이동선3개, 초기 노출 위치 재촬영, 반사 원본 숨김/복구 진단. 지형지물은 천막 사이 낮은지형덩어리_2/폐허벽잔존_2로 가정.
- 결과: 관련 영역별 기둥 너머 계단8/20, 균열10/20, 초입 왼쪽계단6/20·오른쪽바닥13/20, 아래천막5/20·지형4/20·위천막0/20·상부접근0/20. 0/20을 해당 대상의 전체 노출 면적0으로 해석하지 않음. 오른쪽 연속 참고 이동 후반에 위천막/상부접근 노출 확인.
- 초입 직진: 3개 표면점·20px 비교 기준 최초 노출 왼쪽0.86초, 오른쪽1.48초. 샘플 간격 약0.2초. 먼저 발견/선택했다고 해석하지 않음.
- 반사: V010/V017에서 실제 반사 원본 기여 확인. V018 수면 가장자리 진단은 건물 형태 기여 미확인. 사람의 성당 식별 미검증. 기존 반사 유도 제외 결정 유지.
- 보존: 참가자 원본 로그/설정 동일. 맵·카메라·속도 변경 저장 없음, 공간 수정 없음. 종료 시 필드 v09-1_A / PIE 꺼짐 확인.
- 근거: Docs/FieldVisibility100_20260910_225925/결과보고.md, plan.json, results.json, sequences.json, frames/*.png. HTML 자바스크립트 구문·정적 산출물 확인 완료, 로컬 URL 접근 정책으로 브라우저 UI 동작은 미검증.

## 2026-09-10 — T01~T08 실제 동선 기반 영상 검토 표본

- 사용자 요청: 실제 참가자 기록으로 화면 검사 표본 추출, 필요시 녹화 대조.
- 원본 확인: 8명·15판(자유 탐색8 / 지시7). T01 연습1건 제외, T08 재시작 보정 적용. T06 R2는 현재 로그에서 미확인.
- 추출: 다섯 대상 구간에서 실제 좌표100개, 참가자/회차/run/sequence/시각 연결. 참가자 균형을 고려한 검토용 선택이며 면적률·시간 노출률·발견률 추정용 확률 표본 아님.
- 추가 자료: 사건 주변 영상 검토58구간, 참가자별 도달/관찰 입력/옛길 이후 이벤트 표, 현재 탑뷰 위 과거 좌표 지도. 현재 맵 화면을 당시 화면으로 재현했다고 주장하지 않음.
- 미확인: 당시 로그에 카메라/화면 노출 정보 없음. 실제 노출 순서·반사 식별·계단 노출 후 행동 기준점은 영상 동기화 필요. 영상 경로/회차 대응 요청 상태.
- 보존: 원본45개 파일의 SHA256 전후 동일. 맵·참가자 원본 변경 없음. 결과 Docs/FieldTelemetry/T01_T08_화면검토표본/검토표본_설명.md.

### 사용자 후속 확인 — T06 단일 회차 종료 / T01 포함
- T06은 한 판에서 양쪽 루트를 모두 진행해 추가 회차가 불필요하다고 사용자가 판단하고 종료. 2판 기록 누락으로 분류하지 않으며 자유 탐색1판으로 유지. 가상2판 생성 금지.
- T01 본 테스트2판은 계속 포함. 1판 대시는 시간 비교의 조건 차이이며 제외 사유 아님. 현재 표본 수100개와 참가자8명·15판 구성은 동일.

### 실제 녹화 자료 연결
- 사용자 지정 C:/Users/user/Videos에서 T01.mp4~T08.mp4 확보. 원본8개 모두1920×1080/60fps,15판에 대응. T00 및 다른 날짜 영상은 분석 제외.
- 완료: 실제 녹화의 표본 프레임100장, 확대 썸네일100장, 대상별 모아보기5장, 회차 동기화 근거8장,58구간 영상 파일/후보 시각 연결. 원본 로그 SHA256 동일 확인. 영상 원본 수정 없음.
- 영상/로그 시작 시각 차이는 파일시각으로 후보 오프셋 계산 후 시작·종료·관찰 이미지 전후 장면으로 초기 대조. 정확한 프레임 동기화는 미확정. T01 녹화 이전 약5초 구간은 복원하지 않음, 해당 참가자는 포함 유지.
- 분리: 표본 장면 확인과 전체 영상의 최초 노출/노출 시간 비율/인지 판단은 다름. 후자는 이번 표본 추출 완료를 근거로 검증 완료라 표현하지 않음.
- 결과: Docs/FieldTelemetry/T01_T08_화면검토표본/실제영상_표본보기.html 및 영상연결_확인결과.md.

### T01~T08 형광펜 방식 동선 3장
- 사용자 요청: 전체/왼쪽/오른쪽 동선을 같은 탑뷰에 옅게 겹치고 재통과할수록 진하게 표현.
- 본 테스트8명·15판 사용. 붓 폭120uu,1회 농도10%,세 장 동일한 곱셈형 색 누적. 픽셀을 붓이 벗어났다 재진입할 때 추가하며 정지·연속 중복 표본으로 색을 누적하지 않음. 참가자 수/체류시간 히트맵과 구분.
- 왼쪽/오른쪽은 한 판의 실제 이동 위치를 분석용 공간 경계로 나눔. 공통 초입·입구는 전체 이미지에만 표시. T06/T08 혼합 경로를 단일 경로로 강제 분류하지 않음.
- 결과: Docs/FieldTelemetry/T01_T08_형광펜동선/*.png 3장 및 제작기준.json. 동일 좌표·배율의 현재 참고 탑뷰 사용, 과거 좌표 오버레이임을 표기. 실제 PNG 3장 시각 확인 및 정지 시 무누적/재진입 누적 규칙 확인. 원본 로그·맵 보존.

### 2026-09-11 — 사용자 요청: 가림 벽을 현재보다 20% 더 투명하게
- 확정/적용: 현재 열린 `던전외부필드_v09-1_A`의 CharacterOcclusionFadeAmount를 0.80 → 0.84로 변경. ‘20% 더 투명’을 남은 불투명도의 상대 20% 감소(0.20 → 0.16)로 해석하여 안내 후 적용했다.
- 유지: 가림 시작 임계값 0.7, 대상 벽 태그, 충돌, 카메라, 액터 배치. 동결 기준 `v09_A`와 기존 캡처는 변경하지 않았다.
- 보존: `Saved/CharacterOcclusion/던전외부필드_v09-1_A_before_opacity20_20260911_092624.umap`.
- 검증 완료: 맵 저장 후 재로드하여 0.84 유지 및 전체 액터 Transform 동일 확인. 상세 `Saved/CharacterOcclusion/opacity20_20260911_092624.json`.
- 미검증: 변경 후 실제 가림 장면의 시각 비교와 외부 플레이테스트 효과.

### 2026-09-11 — 사용자 추가 왼쪽 진입계단 _4 접합 정리
- 확정 요청: 사용자가 벽 밀착 상승 보완을 위해 추가한 `V05_왼쪽진입계단_4`를 주변 계단과 자연스럽게 연결.
- 변경: 해당 액터만 전용 메시 `SM_Curve_04_Fit`로 교체. 위 `_3`의 곡선 중심을 따라 한 단 바깥으로 연장하고 XY를 정렬. 사용자가 배치한 Z=1502 및 재질 유지. 위 단과 윗면 높이 차 43uu.
- 보존: 사용자 미저장 추가분을 먼저 저장하고 `Saved/StartStairJoin/Step04Fit/before_step04_fit.umap`에 수정 전 사본 보관. 다른 액터·공유 메시·카메라·이동 규칙 불변.
- 검증: 실제 캐릭터를 각 검사 시작점에 배치한 후 일반 이동 입력으로 벽 근접·곡선 끝·중앙 상승 3경로 통과. 이동속도 400uu/s, 최대 단차 45uu 유지. 외부 사용자 길찾기 시험 아님. 기존 참가자 기록 불변.
- 근거: `Saved/StartStairJoin/Step04Fit/movement_verification.json`, 외형 검토용 UE 편집기 렌더 `Docs/StartStairJoin/Step04Fit/curve_inside.png` 및 `curve_top.png`.
- 후속 요청 반영: `_4` 끝단과 벽 사이 틈 제거. 꺾이는 벽의 직선 구간(y=−300)에 맞춰 끝단만 최대 21.43uu 연장하고 벽 내부로 3uu 겹침. 외곽 곡선·높이·액터 변환 불변. 기존 `SM_Curve_04_Fit` 보존, 해당 액터에만 `SM_Curve_04_WallJoined` 적용. 접합부 외형 재촬영: `Docs/StartStairJoin/Step04WallJoined/curve_inside.png`. 이번 끝단 연장 후 이동 시험은 재실행하지 않았음.

### 2026-09-11 — 사용자 요청: 진입 곡선 계단 아래 한 단 추가
- 사용자 보고: 벽 접합 후 벽 가까운 위치에서 여전히 상승이 막힘. 확정 요청에 따라 아래 한 단 추가.
- 구현: `V05_왼쪽진입계단_5` 추가. `_4`보다 35uu 낮은 윗면 1647uu, 기존 곡선 한 단 연장 및 벽에 3uu 겹침. 기존 액터 상태 불변. 수정 전 사본 `Saved/StartStairJoin/Step05Add/before_step05_add.umap`.
- 실제 검사: 더 낮은 바닥에서 벽을 따라 접근, 벽 앞에서 직각 접근 후 상승, 곡선 끝 상승, 중앙 상승 4경로 모두 통과. 각 시작 위치로 검사 캐릭터를 배치한 후 일반 이동 입력 사용. 속도 400uu/s·최대 단차 45uu 불변. 모든 벽 밀기 각도를 검증했다는 뜻은 아님.
- 증거: `Saved/StartStairJoin/Step05Add/movement_verification.json`; 외형 검토용 편집기 렌더 `Docs/StartStairJoin/Step05Add/curve_inside.png`.

### 2026-09-12 — 외부 테스트 분석 후 호수·위 천막 최소 수정 후보
- 사용자 승인: 호수를 조금 밝게 하고 선명한 반사를 시험, 위 천막을 정면 계단 아래 오른편으로 이동. 원본 v09-1_A 보존, 새 `/Game/Portfolio02/Levels/던전외부필드_v09-2_A` 제작.
- 변경: 위 천막 중심 XY (20070,1400) → (20300,1050), +90도 회전. 바닥 실측으로 기둥 높이 정리. 아래 천막 유지. 수면 전용 복제 재질 RGB(.16,.25,.30), roughness .04. 나머지 액터 스냅샷 일치.
- 가설: 천막의 열린 면 앞에서 정면 계단이 함께 보이면 관심점에서 다음 경로로 연결될 수 있다. 열린 면 앞 한 지점에서 계단·천막·캐릭터 동시 노출 확인. 과거 T05의 바깥쪽 진입 좌표에서는 계단 미노출이 남아 있음.
- 반사 검증: 물가 2지점에서 밝기 변화 확인, 성당 식별 가능한 반사는 확인되지 않음. 반사를 이동 유도 기능으로 재채택하지 않음.
- 실제 이동 검사: 일반 입력 3조건 통과. 물가 경로는 실제 시작점 출발 후 던전 전환, 400uu/s, 약17.93초(초기 대기 제외). 나머지 2조건은 천막 근처 부분 이동 검사. 외부 탐색 성공률이 아님.
- 보존 확인: 원본 맵·참가자 로그·세션 설정 해시 일치. 사용자 에디터의 미저장 내부 맵은 유지한 채 별도 작업 에디터로 진행.
- 미정: 외부 테스트 재채택 여부와 이동된 천막에 맞춘 통계 구역 설정. 현재 기준 수집 필터를 새 버전으로 전환하지 않음.
- 근거: `Docs/FieldTentLake_v09-2_A/비교보기.html`, `검토결과.md`, `verification.json`. 쿼터뷰 6지점 전후12장, 기준 탑뷰와 마당 탑뷰2장 보존.
### 2026-09-12 — 사용자 승인: 왼쪽 큰 우회 고리 단축
- 요청: 기존 구성을 최대한 유지하며 왼쪽 연속 이동을 약 40~50초로 줄이고, 항상 새 버전에서 작업. 실제 신전 뒤까지 가지 않고 관찰 구간 바로 뒤에서 돌아오는 방향을 사용자에게 확인한 뒤 승인받음.
- 보존/구현: v09-2_A를 복제한 `/Game/Portfolio02/Levels/던전외부필드_v09-3_A`. 관찰 뒤 회전부와 정면 복귀부를 단축하고 연결 계단·바닥·낙차 난간 정리. 시작 및 기존 왼쪽 진입 계단, 차단 기둥과 너머 옛 계단, 관찰 위치·기능, 신전·입구·내부, 오른쪽 마당·천막·호수 유지.
- 실제 연속 이동 검사: 왼쪽 87.39초 → 49.30초, 오른쪽 17.93초 → 17.92초. 실제 PlayerStart 출발, 400uu/s, 일반 이동 입력, 지정 경로, 관찰·대시·정지 없음. 초기 안정화와 도착 맵 로딩 제외. 수정 후 각 최종 경로 1회 측정. 사람의 첫 탐색시간이나 수학적 최단시간 아님.
- 관찰: 정상 통과 중 관찰 가능 범위 진입 약 36.26~37.04초. 기존 관찰 API로 열기·닫기·이동 복귀 확인. 물리 E 키 및 실제 사용자의 발견률을 새로 검증한 것은 아님.
- 경계 확인: 옛길 차단 정면, 새 길에서 옛 계단으로 넘어가려는 3지점, 외곽 난간, 관찰 난간 6개 지정 입력 조건에서 차단 유지. 개발 중 접점 턱·옛길 우회 생략 틈·오른쪽 합류부 난간 간섭을 수정하고 최종 재검사.
- 보호 검증: 관련 없는 액터 상태 동일, 기존 맵·참가자 원본 등 114개 파일 해시 일치. 참가자 수집 설정 변경 없음. 수정 후 외부 테스트 결과는 아직 없음.
- 근거: `Docs/FieldShortLeft_v09-3_A/검토결과.md`, `verification.json`, `boundary_final.json`, `비교보기.html`; 최종 연속 이동 `Saved/AIFieldTests/20260912_225155_756359/result.json`. 같은 기준 탑뷰 및 실제 PIE 고정 카메라 장면 촬영.
### 2026-09-12 — 사용자 승인: 관찰 기둥 아래 잔여 통로 차단
- 사용자 확인: 탑뷰에서 기둥 바로 아래와 안쪽 난간 사이에 남은 바닥을 막는 범위 확정. 49초 실측은 기둥 위로 돌아 신전 벽 쪽으로 진행한 경로였음을 좌표 로그로 설명한 뒤 승인받음.
- 새 버전 `/Game/Portfolio02/Levels/던전외부필드_v09-4_A` 제작. v09-3_A 보존. `관찰유도_기둥아래틈_차단벽` 1개 추가, 기존267개 액터 상태 유지. 기둥·안쪽 난간에 각각20uu 겹치도록 연결하고 난간 윗면3760uu에 맞춤.
- 실제 검사: 동일 경로·동일400uu/s·일반 이동 입력으로49.30초 →49.29초. 정상 진행 중 관찰 범위 진입, 정면 입구 던전 전환, 관찰 API 열기·닫기·이동 복귀 확인. 관찰·정지·대시·초기 대기·도착 맵 로딩 제외. 미세 시간 차이를 개선 효과로 해석하지 않음.
- 차단: 바깥/힌트 양쪽 접근, 기둥 접점, 안쪽 난간 접점4개 지정 입력 조건 통과. 전체 경계 전수검사나 외부 참가자의 길찾기 시험 아님.
- 보존: 기존 맵과 참가자 원본 등115개 파일 해시 동일, 수집 설정 변경 없음. 근거 `Docs/FieldHintClosure_v09-4_A/검토결과.md`, `verification.json`, `boundary_result.json`, 실제 탑뷰·PIE 쿼터뷰.
### 2026-09-13 — 사용자 정정 반영: 박스 대신 빈 공간·난간을 기둥까지 연장
- 사용자 확정: 네모 차단벽 형태를 채택하지 않고, 아래쪽 검은 빈 공간을 기둥까지 확장해 난간이 기둥에 닿도록 변경.
- v09-4_A 보존 후 새 `/Game/Portfolio02/Levels/던전외부필드_v09-5_A` 제작. 박스 삭제, 해당 구간의 겹친 실제 바닥 메시를 제거, 기존 두께85uu·윗면3760uu의 난간을 빈 공간 가장자리에서 기둥까지 연장. 기존 난간 접합선을 맞춤. 원본 공유 메시 대신 새 버전 전용 메시 사용.
- 실측: 빈 공간 안3곳에서 바닥 충돌 없음, 양옆 이동 바닥3600uu 및 난간 윗면3760uu 확인. 양쪽 접근·기둥 끝·기존 난간 접점4개 지정 입력 조건에서 낙하 없이 경계 유지.
- 동일 PlayerStart·동일 지정 경로·400uu/s 일반 이동으로49.31초에 기존 던전 전환. 이전49.30/49.29초와 사실상 같은 수준. 정상 진행 중 관찰 범위 진입, 기존 관찰 API 열기·닫기·이동 복귀 성공. 대시·관찰·정지·초기 대기·도착 맵 로딩 제외. 외부 참가자 첫 탐색 시간 아님.
- 보존: 허용 범위 밖 액터 상태 및 기존 맵·참가자 원본 등116개 파일 해시 동일. 수집 설정·기둥·관찰 기능·입구·이동·카메라 변경 없음.
- 근거: `Docs/FieldHintVoid_v09-5_A/검토결과.md`, `verification.json`, `surface_traces.json`, `boundary_result.json`, 실제 UE 탑뷰·PIE 쿼터뷰. 이동 검사 `Saved/AIFieldTests/20260913_000331_635172/result.json`.


## 2026-09-13 — v09-6_A 오른쪽 마당 단서 연결 후보
- 확정 범위: 사용자가 승인한 위 천막 현재 위치 유지, 아래 천막·중간 오브젝트 이동. v09-5_A 보존 후 별도 복제.
- 적용: 아래 천막 안쪽/상부 이동 및 90° 회전, 기존 중간 지형·폐허 Y -450uu, 포장 4개 위치·방향과 지면 맞춤. 신규 액터 없음.
- 가설: 포장을 따르다 천막으로 관심이 옮겨도 다음 포장과 폐허를 함께 보며 상부 접근을 계속할 수 있다.
- 실제 검증: 동일 좌표 고정 쿼터뷰 7쌍, 탑뷰 2쌍, 기존 캐릭터 자동 이동 3경로. 상세 수치와 결과는 Docs/FieldYardLink_v09-6_A/검토결과.md.
- 미확인: 외부 사용자의 수정 후 길찾기·천막 이탈 행동 개선. 중간 일부는 여전히 포장 중심이며 천막 간 연속 시야를 확보했다고 주장하지 않는다.
- 보존: 기존 맵과 참가자 원본 117파일 동일. 기존 위 천막·왼쪽 구조·호수·입구·카메라·이동 유지.


## 2026-09-13 — v09-7_A 중간 폐허의 천막 사이 배치
- 사용자 승인: 두 천막 유지, 중간 오브젝트를 두 천막 사이 포장 옆으로 이동. 탑뷰 요청.
- 적용: v09-6_A 보존 후 복제, 중간 지형·폐허 2개만 ΔXYZ(200,-300,168)uu 이동. 기존 경사에 맞춤.
- 검증: 실제 편집기 탑뷰, 고정 플레이 카메라 3지점, 지정 경로 일반 이동 2건. 세부 결과 Docs/FieldMiddleLink_v09-7_A/검토결과.md.
- 미확인: 수정 후 외부 사용자 유도력. 원본 118개 보호 파일 및 참가자 기록 동일.


## 2026-09-14 — v09-8_A 재합류 분수·호수 초입 노출 시험
- 사용자 승인: 분수 형태 시안, 기존 입구 강조, 호수 시작점 방향 이동 및 호수 바닥·수면만 하강. v09-7_A 보존 후 새 버전.
- 적용 제안값: 호수 −X 1000uu, 수면 Z1480uu, 기존 진입부 보존; 지름460uu 낮은 분수; 기존 개구부 바깥 석조 테두리3개.
- 검증: 같은 기준 전후 탑뷰, 실제 고정 쿼터뷰, 캐릭터400uu/s 일반 입력 이동. 오른쪽19.35초/왼쪽48.83초에 기존 던전 전환. 참가자 첫 탐색시간 아님.
- 초입 직진550uu 위치에서 수면 화면 점유율 약8.1%,650uu에서 약14.1%(색상 영역 근사 측정). 발견/시선 유도 성공률과 구분.
- 제한: 출발직후 호수미노출, 합류 원거리에서 분수일부만노출, 입구 상부 화면잘림, 기존 합류 난간 가림. 구체 결과 Docs/FieldReunion_v09-8_A/검토결과.md.
- 보존: 기존1143개 맵/에셋SHA동일, 참가자원본/수집설정동일. 사용자 에디터맵은 전환하지 않음. 최종 기준안 채택 미정.


## 2026-09-14 — v09-9_A 계단·호수 접합 정리
- 사용자 지시: 계단 보존 최우선, 호수와 겹친 부분 자연스럽게 연결, 랜드마크 반사 최대한 유지.
- v09-8_A 보존 후 복제. 계단/난간34개 액터 원형 유지, 수면/호수옹벽 절삭 및 계단 하부·외측 석조 연결. 수면Z/재질/랜드마크/카메라 그대로.
- 검증: 계단 중앙/물가쪽 상행, 하행끝 낙하차단, 기존 시작과 계단의 왕복 접속. 참가자 첫 탐색검사 아님.
- 비교: 같은 플레이어좌표에서 반사구조물 유지 확인. 계단경계로 초입수면 화면점유율8.1→6.3%,14.1→11.2%(색상근사). 반사인식률은 미확인.
- 근거: Docs/FieldLakeStair_v09-9_A/검토결과.md 및 최종캡처·충돌검사. 최종기준안 채택은 사용자판단.


## 2026-09-14 — v09-10_A 난간 등반 차단·중복 경계 통합
- 사용자 확인 문제: v09-9_A 난간에 올라갈 수 있고 두 겹 경계가 어색함. 사용자 요청 범위로 수정.
- v09-9_A 보존 후 복제. 기존 안쪽 난간과 물가 측벽을 한 줄 석조 난간으로 통합. 난간 단 올라서기와 상면 보행 불허.
- 검증: 방향별 난간 접근 5건, 계단 상행/하행 등 4건, 시작 마당과 계단 왕복 2건. 점프는 기존 설정상 실행 불가로 별도 미검증. 실제 고정 카메라와 편집기 상세 캡처 확인.
- 보존: 계단·수면·랜드마크·카메라·이동 유지, 기존 1,251개 에셋/맵 SHA 동일. 재로딩 후 충돌 설정 확인.
- 근거: Docs/FieldRail_v09-10_A/검토결과.md 및 실제 검사 JSON/PNG. 외부 사용자 재검증 미실시.


## 2026-09-14 — v09-11_A 분수 재합류 마당 확장·난간 정리
- 사용자 승인: 분수 구간을 아래쪽의 옛 호수 쪽 여유 공간으로 확장하고 난간 구멍·낙하 경계 정리. v09-10_A 보존 후 복제.
- 적용: 분수/정면 계단/입구 유지, Z3600 평탄 바닥 확장, 기존 겹친 난간 정리 후 연속 외곽 난간. 현재 수면과 겹침 없음.
- 실측: 분수 중앙 단면의 아래쪽 수조–난간 여유 145.3→522.3uu. 바닥 추가 면적 약1,218,785uu². 새 난간은 올라서기·상단 보행 불허, 기존 가림 기능 적용.
- 검증: 난간7구간과 기존 접속3구간 캐릭터 이동, 바닥81개/난간276개 선분 검사. 분수 마당 순회 및 양쪽 정면 접근 후 기존 던전 전환 확인. 오른쪽 지정 경로19.37초(최초 탐색시간 아님).
- 기존 현상: 계단 옆면으로 곧바로 진입하려는 초기2건은 단차에서 정체. 기존 정면 접근경로로는 통과. 계단 측면은 변경하지 않고 상세 보고에 기록.
- 보존: 기존1,253개 에셋/맵 해시 동일, 참가자 기록/수집설정 유지. 저장 후 재로딩 검사 완료.
- 근거: Docs/FieldMerge_v09-11_A/검토결과.md, 실제 UE 탑뷰/고정 카메라 캡처 및 검사 JSON. 외부 사용자 재검증 미실시.


## 2026-09-14 — v09-12_A 호수 오른쪽 난간 높이 보정
- 사용자 지적: 오른쪽 호수 난간이 낙하 위험으로 읽힐 만큼 낮음. v09-11_A 보존 후 복제.
- 원인 확인: v09-10 난간 통합의 계단 높이 분기가 오른쪽 물가까지 적용. 일부 상면이 인근 바닥보다 낮아짐(15지점 높이 실측).
- 수정: 오른쪽 윗면135정점을 실제 지면+220uu 기준으로 올리고 끝 연결 보간. XY/하부/삼각형 구성 보존. 수면·계단·분수·입구·카메라·이동 유지.
- 검증: 물가7지점 캐릭터 접근, 난간45선분 검사, 물가 통행/시작→입구 지정경로 성공. 오른쪽19.37초는 자동 알려진경로 입력 시간.
- 실제 고정 쿼터뷰3쌍에서 난간 경계와 반사 일부 유지 확인. 근거리 수면 일부 더 가려짐, 반사 발견률 미측정.
- 기존1257개 맵/에셋 해시 동일. 상세 Docs/FieldLakeRail_v09-12_A/검토결과.md.


## 2026-09-14 — v09-13_A 난간 높이 통일

- 사용자 확정: 인접 바닥 높이를 기준으로 공통 최소 난간 높이 적용. v09-12_A 보존 후 새 버전 작업.
- 적용: 현재 이동 규칙의 1uu 단위 시험에서 89uu 하행 실패, 90uu 31회 실패 없음. 90uu를 공통 기준으로 난간·외곽 경계 51개 정리. 계단 접속은 높은 디딤판 기준, 관찰 균열·기둥 접속 난간 2개 기능상 보존.
- 검증: 대표50위치 및 최종 변경부 재검사, 계단 왕복·좌우 던전 전환 통과. 지정 동선 왼쪽49.38초/오른쪽19.49초. 외부 첫 탐색 지표가 아님.
- 보존: 기존 애셋1,259개 SHA256 동일, 비대상 액터 변경0, 참가자 원본 로그 유지.
- 한계·예외·원본 결과·실제 캡처: [검토결과](FieldRailStandard_v09-13_A/검토결과.md).


## 2026-09-14 실제 최대 오르막 경사 기준 재검증

사용자 요청에 따라 v09-13_A의 실제 이동 바닥을 비교하고 시험 복제본에서 충돌·이동을 검사했다. 최대 연속 경사는 오른쪽 접근부 X19000~20000, 수평1000uu/상승840uu의40.03°. 실제 바닥 충돌5곳과 상하행2회로 확인했다. 해당 경사에서90uu 난간은 정면·상행사선·하행사선3회 모두 차단했다. 더 낮은30uu도 이 구간의 제한된3입력을 막았지만, 기존 계단 시험에서89uu 실패가 있어 공통90uu를 유지한다. 제작용 맵 변경 없음. 전 조건의 수학적 최소 높이로 표현하지 않는다. 상세: Docs/FieldMaxSlope_v09-13_A/측정결과.md.


## 2026-09-14 — v09-14_A 바닥 타일 음영 복구

사용자 요청으로 v09-13_A를 보존한 새 버전에서 끊어진포장_5/6만 수정. 기존 재질은 모두 동일했으며 두 메시 변환을 정점에 반영하고 노멀·탄젠트를 다시 계산하여 어두운 음영을 바로잡음. 탑뷰에서 주변 타일과 밝기 일치 확인. 월드 형상 최대오차0.000014uu 미만, 기존 NoCollision 및 재질 유지. 다른 액터 변경 없음. 기존 애셋1313개 보존. 자세한 내용 Docs/FieldPavingColor_v09-14_A/수정기록.md.

## 2026-09-14 — v09-15_A 난간·관찰 범위 정리 (사용자 요청 반영)
- 확정: v09-14_A 보존, 새 v09-15_A 제작. 난간·외곽벽 51개 상단 연결/하단 정리, 시작 계단 접합부 돌출 축소. 옛길/호수 왼쪽 누락 난간 2곳 연결.
- 확정: 기둥 이후 관찰 범위를 실제 구멍 방향으로 150uu 확장. X·Z 제한 및 기존 이미지 유지.
- 검증: 최종 55개 난간 컴포넌트, 1,320개 경계 표본, 실제 캐릭터 132회+보수부 16회, 경로/계단 9개 통과, 관찰 위치 17개 기대 결과 일치. 정상 양쪽 경로 던전 전환 확인.
- 보존: 기존 1,316개 애셋 동일, 참가자 로그·이동·카메라·던전 내부 유지.
- 한계: 모든 입력의 인간 전수 플레이가 아님. 성당 내부 플랫폼 쪽 하단 선 비차단 4개는 야외 검사 범위 밖이며 미수정. 일부 접합부 면 음영 남음. 자세한 근거/원시 주의 표시는 Docs/FieldRailCleanup_v09-15_A/수정결과.md 참조.

## 2026-09-14 — v09-16_A 시작 계단 옆 돌출 바닥 낮춤
- 사용자 지적: 계단 옆 높은 바닥이 낮은 단을 가리고 내려갈 때 턱이 생김.
- 확정: v15 보존. 새 v16에서 인접 바닥 두 조각의 접점만 최대133.33uu 낮추고 주변 바닥으로 연결. 계단/난간/기타 액터 유지.
- 검증: 기존 캐릭터 이동13개 통과, 접점 정·역/좌·우4개 추가 검사에서 공중 이동 상태 표본0. 캐릭터·카메라·참가자 기록 유지.
- 기록: Docs/FieldStairLanding_v09-16_A/수정결과.md. 모든 자유 입력의 전수 검사는 아님.

## 2026-09-14 — v09-17_A 옛길 차단부 오른쪽 돌출 공간 제거
- 사용자 결정: 기둥 오른쪽 잔존 기단과 돌출 마당이 불필요한 탐색을 유발하므로 제거, 경계 연속 연결.
- 적용: v16 보존. 기단1개 삭제, 바닥1개 돌출부 절삭, 난간2개 국소 정리. 기둥/너머 계단/왼쪽 우회로 유지.
- 검증: 새 경계8곳+기둥 정면3곳 실제 이동 차단 PASS, 낙하0. 좌우 던전 전환 및 기둥 앞→왼쪽 우회복귀3개 PASS.
- 가설: 불필요한 오른쪽 관심·탐색 감소. 새 외부 사용자 행동으로는 미검증.
- 자료: Docs/FieldOldPathEdge_v09-17_A/수정결과.md 및 원본 캡처/검사 로그.
