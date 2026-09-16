# Portfolio02 — Field Graybox 설계 로그

대상 레벨: `/Game/Portfolio02/Levels/L_Field_v01`
생성일: 2026-09-05
생성 도구: `Tools/build_field_v01.py` (파라미터 → 액터 배치 전량 재생성, 멱등)

기존 `L_Entrance_v01~v04` / `L_FieldGraybox_v01` 은 보존. 이 레벨은 필드 리메이크 v01.

---

## 확정 (사용자 결정)

| 항목 | 값 | 근거 |
|---|---|---|
| 스케일 | 표준 — 경로 폭 800uu | 사용자 선택. 추후 `PARAMS["SCALE"]`, `PATH_W` 로 일괄 조정 |
| 고저차 | V1 = 0 / V4 = 1200 / V5 = 1800 | 사용자 선택 |
| Main : Optional 길이비 | 1 : 2 | 사용자 선택 |
| 진행 축 | +X | 사용자 선택 |
| 카메라 | Perspective / Pitch −50° / Distance 2000uu / FOV 45 | 사용자 고정 조건 |
| 중간 직결 | 끊긴 다리로 차단 | 브리프 고정 조건 |

## 노드 좌표 (walkable surface Z)

| 노드 | 좌표 | 역할 |
|---|---|---|
| V1 START | (0, 0, 0) | 시작 광장 r1600 |
| V2 LANDMARK | (4200, 0, 200) | 랜드마크 인지 구간 |
| V3 BRANCH | (7800, 0, 400) | 분기 광장 r1900 |
| V4 CONVERGENCE | (14500, 0, 1200) | 재합류 광장 r2100 |
| V5 DUNGEON APPROACH | (18200, 0, 1800) | 정면 접근부 |
| **PORTAL (던전 입구)** | **(21500, 0, 1800)** | **진입 방향 +X. 내부 `L_Interior_v01` 접속 앵커** |
| VISTA (Optional 관찰점) | (16687, −2585, 1470) | V4 대비 +270uu |

## 실측값 (스크립트 산출, 가정 아님)

| 항목 | 값 (uu) |
|---|---|
| 접근 구간 (V1→V3) | 7,810 |
| Main Route (V3→V4) | 9,869 |
| Optional Route (V3→V4) | 19,738 |
| **Optional / Main** | **2.00** |
| 최종 접근 (V4→V5) | 3,748 |
| 총 보행 — Main 경유 | 21,428 |
| 총 보행 — Optional 경유 | 31,297 |
| V1→PORTAL 직선거리 | 21,575 |
| 끊긴 다리 간격 | 2,215 |
| 최대 경사 | 9.5° (보행 한계 44.5°) |
| 신전 첨탑 최상단 Z | 5,200 |

Optional 이탈 계수 `k = 0.70` — 기준 웨이포인트를 V3→V4 코드선 기준으로 축소해 1:2 를 맞춘 값.
Y 폭만 줄이는 초기 방식은 하한(0.4)에서 2.23 으로 멈췄다. Optional 길이를 지배하는 것은
**측면 폭이 아니라 신전 후면까지 갔다 오는 X 왕복**이었기 때문. 노브를 "코드선 기준 이탈량"
(V4 를 지나친 점은 V4 를 앵커로 X 도 함께 축소)으로 바꿔 해결.

## 미검증 — 사용자 확인 필요

- **경과 시간**: 캐릭터 이동 속도를 실측하지 않았으므로 위 거리에서 시간을 환산하지 않았다.
  2~3분 분량 판정은 PIE 실측 후.
- **메시 피벗**: LevelPrototyping 메시 피벗을 중심(center)으로 가정하고 배치했다
  (`PARAMS["PIVOT_CENTERED"] = True`). 뷰포트에서 경로 상면이 설계 Z 와 일치하는지 확인 필요.
  어긋나면 이 값을 `False` 로 바꾸고 재실행하면 전체가 한 번에 보정된다.
- **가독성 4항목** (랜드마크 인지 / Main·Optional 구분 / Optional 리턴 / 수렴 구조):
  MCP `take_editor_screenshot` 이 CLAUDE.md 제약 #2 로 실패 → 수동 캡처로 판정.

## 검증 항목 — 카메라 프러스텀 (계산됨, 미해결)

Pitch −50° / Distance 2000uu / FOV 45(수평) / 16:9 기준 화면에 들어오는 지면:

```
수평 폭 (조준점)        1,657 uu
시선축 방향 깊이        1,265 uu   (지면거리 777 ~ 2,043)
```

→ 카메라에서 지면거리 **2,043uu 를 넘는 것은 화면에 존재하지 않는다.**
자세한 관찰·수정 가능 변수는 아래 "관찰" 절.

## 삭제 / 미채택

- 히든박스·수집 요소·퍼즐 — CLAUDE.md 삭제 확정 사항, 이번 필드에도 미적용.
- Main / Optional 머티리얼 구분 — 그레이박스 단계에서는 기하 구조만으로 읽혀야 하므로 미적용.
  광장(V1/V3/V4)만 `MI_PrototypeGrid_TopDark` 로 구분.

## 변경 이력

- 2026-09-05 — `L_Field_v01` 최초 생성. 액터 48개 (`FLD_*`). 실패 호출 0.
  카메라 FOV 는 `set_actor_property` 로 도달 불가(90 유지) → 수동 설정 필요.

---

## 2026-09-06 — 플레이어 무한 낙하 원인과 수정

**관찰.** PIE 시작 즉시 플레이어가 계속 떨어짐. 로그가 1.7초 주기로 반복:

```
LogGameMode: FindPlayerStart: PATHS NOT DEFINED or NO PLAYERSTART with positive rating
Player fell out of the level at Z=-1762 (kill below -1750)
Player defeated: respawned at V(Z=-240.03)
```

**원인.** `FLD_Node_V1` 마커 실린더(반경 30, Z 0~300)가 `FLD_PlayerStart`(0,0,120) 캡슐 안에
박혀 있었다. `AGameModeBase::ChoosePlayerStart` 는 encroaching 인 PlayerStart 를 후보에서
빼고, `FindTeleportSpot` 도 실패하면 어느 목록에도 넣지 않는다 → 후보 0개 → 폰이 월드 원점
(0,0,0)에 폴백 스폰. 원점은 시작 광장 슬래브(Z −300~0) **내부**라 그대로 관통해 낙하했다.

killZ −1750 과 리스폰 Z −240 은 증상이다. `RespawnLocation` 은 `TryInitializePlayer` 가
잡은 최초 폰 위치를 그대로 복사하므로(P02PlaytestSystem.cpp:1896), 지하에서 시작하면
지하로 리스폰한다. 이전 레벨 좌표가 하드코딩된 것이 아니었다.

**수정.** 노드 마커 6개를 `NoCollision` 로 변경(에디터 참조용 기즈모이므로 애초에 막을 이유 없음).
`FLD_PlayerStart` Z 를 120 → 200 으로 올려 바닥에서 캡슐 반높이 이상 여유 확보.
재검증: PIE 8초간 낙하·피격·리스폰 로그 0건, `FindPlayerStart` 경고 소멸.

## 신규 MCP 제약 (이번 세션 확인)

23. **`spawn_actor` 는 콜리전 프로파일을 설정하지만 `RecreatePhysicsState()` 를 호출하지 않는다**
    (`UnrealMCPEditorCommands.cpp:475`). 확정하려면 `set_actor_collision`
    (`name` / `profile` / `save`) 을 이어서 호출한다. 이쪽만 `Modify()` +
    `RecreatePhysicsState()` + `MarkPackageDirty()` 를 한다. **Python 툴 래퍼가 없어
    MCP 툴 목록에는 안 보인다** — raw TCP 로만 호출 가능.
24. **`delete_actor` 직후 같은 이름으로 재스폰 불가.** UE 가 GC 전까지 이름을 예약하고
    `spawn_actor` 는 `Required_ErrorAndReturnNull` 을 쓴다 → "name may already exist" 로
    전량 실패하며 **레벨이 빈 채로 남는다.** 해결: 삭제 후
    `set_cvar gc.CollectGarbageEveryFrame 1` → 3초 대기 → `0`. 빌드 스크립트에 반영됨.

## 카메라 — C++ 에 이미 구현되어 있음

`P02PlaytestSystem` 이 `AP02QuarterViewCameraRig` 를 런타임 스폰해 뷰타깃으로 잡는다.
전부 CVar 구동이며 기본값이 사용자 지정값과 일치한다:

```
p02.Camera.Pitch  -50     p02.Camera.Distance  2000
p02.Camera.Yaw      0     p02.Camera.FOV         45     <- Yaw 만 45 로 바꿔야 함
```

`FLD_Cam_*` 액터는 **에디터 뷰포트 미리보기 전용**이고 게임 카메라가 아니다.
CVar 는 에디터 재시작 시 초기화된다. 영구화하려면 `Config/DefaultEngine.ini` 의
`[SystemSettings]` 에 넣어야 하는데, 이는 `L_Interior_v01` 을 포함한 **모든 레벨에 적용**되므로
내부 레벨을 Yaw 0 기준으로 만들었다면 그쪽이 함께 바뀐다. 미결정.

## 캡처 — MCP 로 가능 (CLAUDE.md 제약 #2 정정)

제약 #2 는 **절반만 맞다.** 커맨드가 두 개다.

| 커맨드 | 결과 |
|---|---|
| `take_editor_screenshot` (base64 반환) | **항상 실패.** `GetLevelViewportClients()` 의 **첫 번째** 클라이언트를 집는데(`UnrealMCPEditorCommands.cpp:1256`) 숨겨진 크기 0 뷰포트가 먼저 걸린다 |
| `take_screenshot` (PNG 파일 저장) | **정상 동작.** `GetActiveViewport()` 사용. 단 `filepath` 가 **프로젝트 아래 절대경로**여야 함 (상대경로는 "must be under the project directory" 로 거부) |

캡처 스크립트: `Tools/capture_field.py`

추가로 확인된 것:

- `focus_viewport` 는 FOV 를 못 바꾼다. 에디터 뷰포트는 FOV 90, 게임 카메라는 FOV 45 @ 2000uu.
  같은 가로 화각 범위는 `2000·tan22.5° / tan45° =` **828uu**. 노드 샷은 이 거리에서 찍는다.
  뷰포트가 16:9 가 아니라 세로로는 게임보다 더 보인다 — 여기서 안 읽히면 게임에서도 안 읽힌다.
- `focus_viewport` 는 `Invalidate()` 만 하고 리드로우는 다음 틱이다. **focus 를 두 번 걸어야**
  올바른 프레임이 잡힌다. 한 번만 걸면 직전 지점 이미지가 그대로 나온다(바이트 단위 동일로 확인).
- 탑뷰는 **Z 10,000 이 상한.** 그 위는 화면이 백지가 된다(`r.Fog 0` 으로도 안 고쳐짐).
  전체 도면은 Z 10,000 에서 X 6,500 / X 18,500 두 타일로 나눠 찍는다.
- **에디터 창이 포그라운드가 아니면 뷰포트가 리드로우를 멈춘다.** 이때 캡처는 전부
  같은 프레임(동일 바이트 수)이 나온다. MCP 응답은 정상이라 실패로 안 보인다 — 판정은 파일 크기.
