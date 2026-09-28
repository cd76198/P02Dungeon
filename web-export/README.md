> 이 자료는 f315d8c에서 추출한 모델과 조사 기록입니다. 이후 제출 설정에서는 시작 필드와 보수_v02 연결을 확정했습니다. GLB 형상은 그대로이며 이전 연결값이 든 JSON은 추출 당시 증거로 보존합니다. [현재 제출 설정·검증](../Docs/Submission/README.md)을 함께 확인하세요.

# UE5 포트폴리오 웹 내보내기

최신 저장 필드 `던전외부필드_v09-17_A`와 최신 던전 보수본 `던전내부최종_보수_v02`에서 생성했다. 원본 프로젝트 및 기존 웹사이트 파일은 수정하지 않았다. 원본 SHA-256 대조 결과는 validation.json에 있다.

## 파일 목록

| 파일 | 용도 |
|---|---|
| field.glb / dungeon.glb | 최종 시각 모델. 원래 원점·크기·높이 보존, 상호작용 액터 분리 |
| field-collision.glb / dungeon-collision.glb | UE Pawn blocking 충돌 메시. 바닥 후보/차단 면 분리 |
| *-collision-index.json / .csv | 충돌 오브젝트 원본 이름, 분류 근거, bounds, 삼각형 수 |
| *-node-map.json | GLB 노드 인덱스 ↔ 실제 GetName ↔ ActorLabel |
| coordinates-camera.json | 시작·전환·종료 위치, 좌표계, 이동 방향, 실제 카메라 설정 |
| interactive-objects.json / .csv | 동적 액터·장치·트리거의 정확한 이름·좌표·컴포넌트·초기 상태 |
| gimmick-rules.md | 소스와 실행을 구분한 시작→조작→변화→완료/실패/초기화 규칙 |
| video-comparison.md | 제공 영상의 확인 시점과 현재 구현의 일치·차이·미확인 |
| collision-rules.md | 캡슐 이동 기준, 다층/동적 충돌 적용 및 검증 한계 |
| field-playtest.md | 필드 테스트 코드·기록의 존재와 참가자 데이터 미확보 사실 |
| field-observation.png | 최신 필드의 관찰 UI에 실제 연결된 원본 이미지 |
| material-fallbacks.json | 레버 손잡이 1개 재질 대체 내역·제한 |
| preview.html + vendor/ | 외부 CDN 없이 실행하는 모델·충돌 검사 뷰어(게임 플레이 미구현) |
| web-integration.md | 기존 사이트에 모델·상태·카메라·충돌을 연결할 때의 적용 사항 |
| validation.json | 구조·좌표·이름·재질·원본 무변경 검사 결과 |
| source-hashes.json / files.json | 원본 파일 및 결과물 무결성 확인용 SHA-256 |
| evidence/ | UE actor 스냅샷, T3D 블루프린트·레벨, 충돌 원자료, 소스/기존 문서 사본, PIE 기믹 시험, 재질·GLB validator 보고서 |

## 보존 방식과 확인된 차이

UE [X,Y,Z] cm를 glTF [X,Z,Y] m로 변환했다. 맵을 중앙 정렬하거나 높이를 맞추지 않았다. 두 레벨은 OpenLevel로 전환되는 별도 월드여서 연속 공간에 붙이는 오프셋은 원본에 정의되어 있지 않다. 웹에서는 기록된 필드 입구 조건에서 던전 씬/스폰으로 전환할 수 있다. 원본 목적지는 **보수 전 던전**이며 이 파일은 **최신 보수 v02**이므로 그 차이를 coordinates-camera.json에 표시했다.

원본 UObject GetName을 GLB 노드 이름으로 보존했다. UE 기본 exporter가 중복 ActorLabel을 사용하는 문제를 바로잡았으며, 임의로 새 이름을 붙이거나 액터를 병합하지 않았다. ActorLabel도 extras에 보존했다. 레버 본체와 손잡이는 각 컴포넌트의 실제 월드 변환으로 분리해 비균일 부모 스케일 경고를 보정했다. initially_hidden을 웹 로더에서 적용해야 초기 숨김 상태가 재현된다.

필드는 원래 단색 재질 4개로 텍스처가 0개다. 던전은 512px 기준 재질 베이크를 사용한다. 최초 내보내기의 분홍색 상수 재질 54개 중 53개는 복사본에서 400cm 격자 주기의 임시 Z 이동 후 재베이크하고, 최종 GLB 좌표는 원래 값으로 되돌렸다. 남은 레버 손잡이 1개는 원본 SurfaceColor(선형 RGB 0.18)·Roughness(1)를 사용했다. **손잡이의 절차적 격자 무늬는 재현하지 못했다.** 원본 프로젝트/배치는 저장·변경하지 않았다. UE의 조명·동적 셰이더와 픽셀 단위 일치는 보장하지 않는다.

최종 4개 GLB의 표준 검사 오류/경고 0건. 최종 모델에서 분홍색 상수/텍스처 픽셀과 외부 이미지 참조 0건. 브라우저에서 두 모델, 충돌 구분, 레버를 확인했다. 모든 길의 완주, 모든 기믹 키 입력, 웹 물리와 UE의 동등성은 미검증이다. 중간보스 초기 체력 등 코드와 실제 실행의 차이는 gimmick-rules.md에 기록했다.

필드 플레이테스트 기능과 이전 설계·검증 기록은 존재한다. 외부 테스트 기준 맵 v09-1_A는 최신 필드 v09-17_A와 다르다. 참가자 원시 로그·영상·통계는 현재 저장소에 없으며 추정하지 않았다.

## 크기와 실행

- `field.glb`: 3,570,448 bytes (3.41 MiB)
- `dungeon.glb`: 4,152,536 bytes (3.96 MiB)
- `field-collision.glb`: 1,637,680 bytes (1.56 MiB)
- `dungeon-collision.glb`: 208,684 bytes (0.20 MiB)

각 모델이 5 MiB 미만이어서 용량 때문에 형상·오브젝트를 줄이는 추가 경량화본은 만들지 않았다. 충돌 메시의 크기는 실제 UE complex collision 삼각형을 보존한 결과다. evidence/는 개발 근거 자료이며 웹 배포에는 전부 올릴 필요가 없다.

이 폴더에서 `python -m http.server 8765`를 실행한 후 `http://localhost:8765/preview.html`을 연다. 운영 웹사이트에 배포한 것은 아니다.

출처: https://github.com/cd76198/P02Dungeon (커밋 f315d8c0d245fa74fc335042fa302002d88f76a0), https://cd76198.github.io/ , https://youtu.be/d1TWflW1VhE . 생성에 사용한 도구: UE 5.7.4 GLTFExporter/GeometryScript, Khronos glTF Validator, Three.js 0.180.0(MIT).
