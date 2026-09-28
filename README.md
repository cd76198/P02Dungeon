# P02Dungeon — UE5 필드·던전 레벨디자인

고정 쿼터뷰에서의 경로 선택, 오브젝트 배치, 내부 관찰과 던전 연결을 검토하는 개인 포트폴리오 프로젝트입니다. 환경 아트 완성본이 아닌 그레이박싱 프로젝트입니다.

## 현재 저장된 맵

- 최신 필드: `/Game/Portfolio02/Levels/던전외부필드_v09-17_A`
- 외부 플레이테스트 기준 필드: `/Game/Portfolio02/Levels/던전외부필드_v09-1_A`
- 던전 원본: `/Game/Portfolio02/Tests/FinalBossCandidates/던전내부최종`
- 최신 던전 보수본: `/Game/Portfolio02/Tests/FinalBossCandidates/던전내부최종_보수_v02`

필드의 이전 버전과 A/B/C 비교 맵도 보존합니다. 2026-09-27 제출 기준은 **필드 v09-17_A → 던전내부최종_보수_v02**입니다. 에디터·게임 기본 시작 맵을 선택한 필드로 지정하고, 필드 컨트롤러의 던전 연결을 보수_v02로 변경했습니다. 두 맵을 패키징 대상 목록에도 명시했습니다. 원래 연결·시작 설정은 이전 커밋 `f315d8c`에 보존됩니다.

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

`P02Dungeon.uproject`를 통해 프로젝트 파일을 생성하고 `P02DungeonEditor`를 빌드합니다. 빌드 전에 에디터를 종료합니다. 2026-09-27 Windows Editor Development 빌드와 PIE에서 두 필드 경로의 동일 시작점 전환, 던전 진행, 최종문 이후 END·이동 정지를 확인했습니다. 패키징 EXE 빌드·실행은 아직 검증하지 않았습니다.

개발용 `kks3800/Unreal_MCP` 플러그인은 이 저장소에 포함하지 않으며 제출 설정에서 비활성화했습니다. 기본 플레이에 해당 플러그인을 따로 설치할 필요는 없습니다. 원 제작 PC 경로를 참조하던 자동 실행 스크립트도 제출 설정에서 비활성화했습니다. 엔진 생성물인 `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`도 포함하지 않습니다.

## 라이선스와 공개 범위

홈페이지는 별도 저장소인 https://github.com/cd76198/cd76198.github.io 에 있습니다.

직접 작성한 C++ 구현과 Markdown 문서는 [MIT 라이선스](LICENSE) 범위로 제공합니다. Unreal Engine 템플릿·에셋, 폰트 및 별도 플러그인의 권리는 각각의 권리자에게 있습니다. **모든 UE 콘텐츠가 MIT인 저장소는 아닙니다.** 구성 요소별 조건은 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md), 기여 방법은 [CONTRIBUTING.md](CONTRIBUTING.md)에 정리했습니다.

관찰 UI와 테스트 안내에서 사용하는 ONE Mobile POP 폰트는 재배포하지 않습니다. 같은 한글 UI를 재현하려면 THIRD_PARTY_NOTICES의 안내에 따라 공식 폰트를 직접 설치해야 합니다.

현재 개인 프로젝트의 공개 초기 단계이며, 외부 프로젝트 도입 실적과 월간 다운로드는 확인되지 않았습니다. 레벨 플레이테스트 참가자는 오픈소스 사용자·도입 실적으로 집계하지 않습니다.

로컬 계정 설정, 대화 첨부 파일, 원본 참가자 로그·녹화, 작업노트 사이트의 인증 설정은 이번 백업 대상에 포함하지 않습니다. Git 커밋·푸시는 명시적으로 실행할 때만 이루어지며 자동 백업을 의미하지 않습니다.

## 이번 작업 자료

- [제출 설정 변경과 실행 검증](Docs/Submission/README.md): 변경 파일, 양쪽 필드 경로 전환, 던전 기믹·END와 이동 정지의 근거.
- [포트폴리오 검수 보고서](Docs/PortfolioAudit/README.md), [검수 PDF](Docs/PortfolioAudit/UE5_포트폴리오_검수결과.pdf): 설정·계산·실행 확인과 미확인 통계 구분. 제출 연결 변경 전 조사이며 이후 상태는 제출 검증 보고서 참조.
- [던전 보수본 비교](Docs/PortfolioAudit/dungeon-repair-comparison/README.md): 원본·v01·v02의 실제 배치 차이.
- [웹 내보내기 자료](web-export/README.md): 필드·던전 GLB, 충돌 메시, 카메라·좌표, 오브젝트와 기믹 규칙. 모델 검사 뷰어이며 웹 게임 구현·웹사이트 배포 완료를 뜻하지 않습니다.
- [필드 추가 검토 사항](Docs/Field_Review_Observations.md): 경계벽과 계단 측면 정체의 기술 검사 근거. 참가자 통계와 구분합니다.

몬스터 스폰·속도·게임플레이 소스 및 문서의 기존 설계 계산 **1,860 uu / 약 4.17초**는 변경하지 않았습니다. 파생 생성물과 중복 배포 ZIP은 커밋하지 않습니다. 웹 GLB는 Git LFS로 관리합니다.
