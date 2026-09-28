# 이동과 충돌

`field-collision.glb`, `dungeon-collision.glb`는 UE에서 **Pawn을 Block하는 Query 충돌**만 추출했다. 원본 simple collision은 삼각형으로 변환했고, UseComplexAsSimple 메시에는 실제 complex 삼각형을 사용했다. 임의의 평면 높이 지도를 만들거나 상하층을 합치지 않았다. 에디터 전용/NoCollision/Overlap 오브젝트는 보행 장벽에 포함하지 않는다.

각 노드 이름은 원본 GetName, 컴포넌트는 extras. 시각 GLB와 같은 원점·축·단위다. JSON/CSV의 `role_hint`는 이름 기반 분류이며, 실제 통행 가능성 판정이 아니다. 위쪽 법선 Z가 0.71 이상인 면을 녹색 material `upward_support_candidates`, 나머지를 빨간색 `blocking_sides_and_undersides`로 구분했다. 벽/난간의 윗면도 이론상 지지면 후보이므로 녹색 전체를 내비게이션 영역으로 취급하면 안 된다.

실행값: 보행 속도 400 cm/s, 캡슐 반경 35 cm·반높이 90 cm, 최대 턱 45 cm, 보행 법선 Z ≥ 0.710000038 (경사 약 44.765°). 점프와 대시는 비활성화되어 있다. 웹 단위는 각각 4 m/s, 반경 0.35 m·반높이 0.9 m, 턱 0.45 m다. UE의 캡슐 스윕, 바닥 탐색, 계단 오르기와 동등한 처리가 필요하다. 단일 Y 높이 샘플이나 XZ 평면 충돌만으로 다층 통로를 구현하지 말 것.

다리 3개 메시의 충돌은 파괴 즉시 끄고 복구 즉시 켠다. 중간보스 문과 최종문은 열리기 시작할 때 끈다. `MCP_TEST_TransformStair_Blocker`는 중간보스 처치 후 자동 장치가 작동할 때 끈다. 벽 가림 효과는 시각 효과이며 충돌은 유지한다. 시각 노드 이동/숨김과 충돌 노드 상태를 별도로 동기화해야 한다.

정적 충돌 GLB에는 PIE에서 생성되는 적이 없다. `P02PlaytestEnemy`는 Cube 기반 BlockAll, 크기 80×80×160 cm이며 위치와 회전에 맞춰 동적 박스를 추가한다. 레버는 NoCollision이다. 트리거는 interactive-objects.json에 별도로 보존했다.

확인 범위: 충돌 형상 추출, 컴포넌트 수·삼각형 수, 좌표 변환, 바닥 큐브의 위/아래 법선 분류. 미확인: 모든 길의 캡슐 완주, 난간별 새는 틈, 모서리 스텝 처리, 브라우저 물리엔진과 UE CharacterMovement의 동등성. 별도 NavMesh 또는 확정된 보행 경로를 생성한 것은 아니다.
