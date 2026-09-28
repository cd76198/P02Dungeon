# 필드 플레이테스트 자료 확인

**테스트 기능과 과거 설계·디버깅 기록은 있다. 참가자별 실제 결과 로그·녹화는 현재 저장소에 없다.** 원본 README는 참가자 로그·녹화를 백업 제외 대상으로 명시한다. 따라서 참여 인원, 평균/중앙 완주 시간, 성공률, 경로 선택률, 설문 결과는 확인 불가다.

| 자료 | 확인 내용 | 적용 범위 |
|---|---|---|
| evidence/project-docs/source-README.md | 외부 플레이테스트 기준은 `던전외부필드_v09-1_A`, 최신 필드는 `던전외부필드_v09-17_A` | 두 버전을 혼동하면 안 됨 |
| evidence/source/P02FieldTelemetry.cpp/.h | 익명 참가자 T숫자 ID, 버전·시도·노출 이력, 위치 CSV, 이벤트 CSV, run.json 기록 | 수집 기능의 존재. 실제 참가 결과라는 뜻은 아님 |
| evidence/source/P02OutdoorReviewController.cpp/.h | 이동 시작·거리, 좌/우 경로, 캠프 진입, 필드 완료 시간과 던전 전환 로그 | 최신 필드의 Enemies 배열은 비어 있음 |
| evidence/project-docs/Portfolio02_Field_Design_Log.md | 2026-09-05 L_Field_v01 설계 거리·경사, 09-06 시작점 충돌로 무한 낙하한 원인과 수정, PIE 8초 재검증 기록 | 이전 L_Field_v01 문서. 최신 v09-17의 플레이테스트 결과로 인용하면 안 됨 |
| evidence/project-docs/Portfolio02_LevelData.csv / Portfolio02_UsedMetrics.csv | 설계 파라미터 및 산출·근거 표 | 참가자 원시 데이터가 아님 |

Telemetry는 `Saved/FieldTelemetry/session.json`이 존재하고 enabled=true이며 map_filter가 일치해야 작동한다. 실행 결과는 `Saved/FieldTelemetry/runs/<run_id>/run.json`, `positions.csv`, `events.csv`에 기록된다. 현재 원본 체크아웃에는 세션 설정·runs 원시 결과가 없어 구간 박스, 샘플 주기 등의 실제 테스트 세션 값도 확정할 수 없다.

수집 가능한 항목: elapsed_s, 위치 XYZ, 구간·route, 이동 입력·거리, 이동 가능/관찰창/일시정지/위치 불연속, 구간 진입·재방문, 경로 변경·분기 복귀, 관찰 프롬프트·이미지 열기/닫기, 장애물 접촉·막힌 이동 시도, 정체 후보, 낙하 후 재접근, 입구 최초 도달, 던전 로드 확인, 진행자 메모. 정체 후보는 코드에도 `not_proven_collision`로 표시되어 있어 충돌 발생률로 곧바로 해석하면 안 된다.

최신 필드에는 추가 퍼즐/전투 기믹은 없지만 `P02ObservationPoint_0`의 선택형 ‘틈새 살펴보기’ UI가 남아 있다. 범위 안·시선 차단 없음·E 입력으로 이미지를 열고 게임/입력을 정지하며 E/Esc로 닫아 복구한다. 실제 위치·범위는 interactive-objects.json, 원본 연결 이미지는 field-observation.png에 포함했다. 웹에서 관찰 UI를 살릴지 여부는 별도 구현 선택이다.

이번 작업에서 확인한 실행은 최신 필드의 초기 스폰·카메라·이동 설정이다. 이번 실행을 사람 참가자의 플레이테스트 결과나 필드 완주 기록으로 계산하지 않았다.
