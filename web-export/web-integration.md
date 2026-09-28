# 웹 적용 메모

이 폴더는 최종 저장 맵에서 추출한 모델·충돌·규칙 자료다. GitHub Pages 배포나 전체 플레이 시스템 구현은 하지 않았다. 기존 사이트와 테스트 dungeon.glb는 수정하지 않았다.

1. GLB의 `extras.ue_actor_name` 또는 node-map.json으로 객체를 찾는다. 에디터 ActorLabel은 중복된 `StaticMeshActor`가 많고 코드가 사용하는 GetName과 다르다. Three.js GLTFLoader가 표시 이름을 정리할 수 있으므로 extras를 안정된 식별자로 사용한다.
2. 최상위 씬을 중앙으로 옮기거나 바닥 높이를 0으로 맞추지 않는다. 각 레벨의 로컬 원점을 그대로 두고 필드 전환 조건에 도달하면 던전 씬과 스폰 좌표로 전환한다. 물리적으로 붙어 있는 두 월드의 오프셋은 원본에 없다.
3. `extras.initially_hidden`을 적용한다. 특히 레버의 본체와 손잡이는 내보내기 때 보존하기 위해 포함했지만 처음에는 표시하지 않는다. GLB 자체는 게임 상태 머신이나 가시성 애니메이션을 실행하지 않는다.
4. 충돌 GLB는 렌더링용으로 표시하지 말고 물리에 사용한다. 다리·문·Blocker의 충돌도 상태에 맞춰 갱신한다. 층마다 높이를 따로 유지한다.
5. 카메라는 고정 회전(-50°,0°,0°), 폰 추적 거리 20 m다. UE FOV 45°는 수평 화각이다. Three PerspectiveCamera의 fov는 수직이므로 `2*atan(tan(45°/2)/aspect)`로 변환한다. 실제 초기 카메라 좌표와 추적 오프셋은 coordinates-camera.json을 사용한다.
6. 기존 viewer.js에는 grid 색을 교체하고 magenta 재질을 회색으로 덮는 `repairedMaterialFor`가 있다. 새 파일의 실제 검사를 할 때 해당 보정을 적용하지 않는다. 원본 procedural grid를 텍스처로 구운 결과이며 웹 조명·톤매핑은 UE와 다를 수 있다. WallFadeOut은 웹 셰이더/투명도 상태로 별도 구현한다.
7. 필드의 원본 OpenLevel 목적지는 보수 전 던전이다. 이 폴더의 dungeon.glb는 README 기준 최신 보수 v02이므로 웹 전환은 v02를 사용한다는 선택을 명시해야 한다.

`preview.html`은 모델·충돌·이름·카메라를 확인하는 검사 뷰어다. WASD 이동·기믹 전체 플레이를 구현한 게임은 아니다. 폴더를 HTTP로 서비스해서 실행한다. 예: `python -m http.server 8765` 후 `http://localhost:8765/preview.html`. Three.js 0.180.0 및 MIT 라이선스를 vendor/에 포함해 외부 CDN 없이 실행된다.

