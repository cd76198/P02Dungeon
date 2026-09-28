# 던전 보수본 비교

원본 및 보수_v01/v02를 읽기용 복사본에서 로드하여 비교했다. 프로젝트 저장·수정·PIE 실행 없음.

- 3개 맵 모두 액터194개, 추가/삭제 없음.
- 원본→v01: 조회한 배치·메시·재질·충돌 속성 차이 없음. 파일 전체 동일 또는 모든 속성 동일을 뜻하지 않음.
- 원본/v01→v02: 12개 액터의 위치·스케일 변경. 조회한 메시 참조·재질·충돌 모드 차이 없음.
- 보수 목적의 당시 원작업 기록은 현재 저장소에서 확인하지 못함. 접합부/경계를 보완한 성격이라는 설명은 아래 형상 차이의 해석. 낙하·끼임 해결 여부를 새로 실행 검증하지 않음.

|오브젝트|에디터 이름|전체 Bounds 크기 전→후(uu)|
|---|---|---|
|MCP_TEST_Candidate1_Floor|StaticMeshActor|[3000.0, 2525.0, 100.0] → [3000.0, 2552.5, 100.0]|
|MCP_TEST_Candidate1_Wall_South|StaticMeshActor|[1900.0, 100.0, 350.0] → [3000.0, 100.0, 350.0]|
|MCP_TEST_Candidate2_Floor|StaticMeshActor|[1350.0, 1330.0, 100.0] → [1355.0, 1330.0, 100.0]|
|MCP_TEST_Candidate2_Wall_North|StaticMeshActor|[1350.0, 100.0, 350.0] → [1355.0, 100.0, 350.0]|
|P02_Rail_Landing_North|StaticMeshActor|[800.0, 100.0, 260.0] → [850.0, 100.0, 260.0]|
|P02_Rail_Start_North|StaticMeshActor|[800.0, 100.0, 350.0] → [1000.0, 100.0, 350.0]|
|P02_Rail_Start_South|StaticMeshActor|[800.0, 100.0, 350.0] → [1000.0, 100.0, 350.0]|
|P02_StairD_0|StaticMeshActor16|[109.0, 500.0, 40.0] → [120.0, 500.0, 40.0]|
|P02_StairD_Landing|StaticMeshActor|[300.0, 500.0, 100.0] → [313.5, 500.0, 100.0]|
|StaticMeshActor_27|MCP_TEST_Interior_FinalBossWall_South|[3200.0, 100.0, 360.0] → [3300.0, 100.0, 360.0]|
|StaticMeshActor_28|MCP_TEST_Interior_FinalBossWall_North|[3200.0, 100.0, 360.0] → [3300.0, 100.0, 360.0]|
|StaticMeshActor_6|MCP_TEST_Interior_MidBossWall_South|[2700.0, 100.0, 350.0] → [2800.0, 100.0, 350.0]|

## 복사본과 원본 맵 동일성

- 던전내부최종.umap: True
- 던전내부최종_보수_v01.umap: True
- 던전내부최종_보수_v02.umap: True