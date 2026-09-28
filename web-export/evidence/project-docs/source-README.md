# P02Dungeon — UE5 필드·던전 레벨디자인

고정 쿼터뷰에서의 경로 선택, 오브젝트 배치, 내부 관찰과 던전 연결을 검토하는 개인 포트폴리오 프로젝트입니다. 환경 아트 완성본이 아닌 그레이박싱 프로젝트입니다.

## 현재 저장된 맵

- 최신 필드: `/Game/Portfolio02/Levels/던전외부필드_v09-17_A`
- 외부 플레이테스트 기준 필드: `/Game/Portfolio02/Levels/던전외부필드_v09-1_A`
- 던전 원본: `/Game/Portfolio02/Tests/FinalBossCandidates/던전내부최종`
- 최신 던전 보수본: `/Game/Portfolio02/Tests/FinalBossCandidates/던전내부최종_보수_v02`

필드의 이전 버전과 A/B/C 비교 맵도 보존합니다. 위 최신 맵 표기는 2026-09-16 디스크에 저장된 파일 기준입니다. 에디터의 미저장 변경은 Git에 포함되지 않습니다. 기본 시작 맵 설정은 변경하지 않았으므로 원하는 맵을 콘텐츠 브라우저에서 직접 엽니다.

## 구성

- `Content/Portfolio02`: 필드·던전 맵, 전용 메시·머티리얼·블루프린트 및 관찰 UI 자료
- `Source/P02Dungeon`: 관찰 상호작용, 캐릭터 가림 처리, 플레이테스트 진행 및 좌표·이벤트 기록 기능
- `Config`: 프로젝트 설정
- `Docs/Portfolio02_Entrance_Design_Log.md`: 진입 구간 설계 기록
- `Docs/Portfolio02_Field_Design_Log.md`: 필드 설계 기록

## 실행 환경

Unreal Engine 5.7.4, Windows C++ 개발 환경 및 Git LFS가 필요합니다.

```sh
git clone https://github.com/cd76198/P02Dungeon.git
cd P02Dungeon
git lfs pull
```

`P02Dungeon.uproject`를 통해 프로젝트 파일을 생성하고 `P02DungeonEditor`를 빌드합니다. 빌드 전에 에디터를 종료합니다. 본 백업에서 신규 빌드나 플레이 검증을 수행한 것은 아닙니다.

프로젝트는 별도 저장소인 `kks3800/Unreal_MCP` 플러그인을 사용하며 `Plugins/UnrealMCP`는 이 저장소에 포함하지 않습니다. 기존 환경의 해당 플러그인을 별도로 준비해야 합니다. 엔진 생성물인 `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`도 포함하지 않습니다.

## 라이선스와 공개 범위

홈페이지는 별도 저장소인 https://github.com/cd76198/cd76198.github.io 에 있습니다.

직접 작성한 C++ 구현과 Markdown 문서는 [MIT 라이선스](LICENSE) 범위로 제공합니다. Unreal Engine 템플릿·에셋, 폰트 및 별도 플러그인의 권리는 각각의 권리자에게 있습니다. **모든 UE 콘텐츠가 MIT인 저장소는 아닙니다.** 구성 요소별 조건은 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md), 기여 방법은 [CONTRIBUTING.md](CONTRIBUTING.md)에 정리했습니다.

관찰 UI와 테스트 안내에서 사용하는 ONE Mobile POP 폰트는 재배포하지 않습니다. 같은 한글 UI를 재현하려면 THIRD_PARTY_NOTICES의 안내에 따라 공식 폰트를 직접 설치해야 합니다.

현재 개인 프로젝트의 공개 초기 단계이며, 외부 프로젝트 도입 실적과 월간 다운로드는 확인되지 않았습니다. 레벨 플레이테스트 참가자는 오픈소스 사용자·도입 실적으로 집계하지 않습니다.

로컬 계정 설정, 대화 첨부 파일, 원본 참가자 로그·녹화, 작업노트 사이트의 인증 설정은 이번 백업 대상에 포함하지 않습니다. Git 커밋·푸시는 명시적으로 실행할 때만 이루어지며 자동 백업을 의미하지 않습니다.
