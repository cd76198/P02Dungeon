# 구성 요소별 라이선스

## 직접 작성한 코드

`Source/` 내 cd76198의 독자적인 구현과 직접 작성한 Markdown 문서는 `LICENSE`의 MIT 조건으로 제공합니다. 기존 Epic Games 저작권 표시와 제3자 부분은 유지합니다. UE API를 사용하므로 빌드·실행에는 별도의 Unreal Engine 설치 및 이용 조건 준수가 필요합니다.

이 저장소 전체나 모든 맵·에셋을 MIT로 배포한다는 뜻은 아닙니다. `Content/` 및 참고 이미지에는 위 MIT 라이선스를 적용하지 않습니다.

## Unreal Engine 템플릿

Third Person Blueprint의 Combat 변형, 캐릭터, 프로토타이핑 메시 및 관련 입력·블루프린트는 Unreal Engine 5.7 템플릿을 기반으로 합니다. Epic Games의 권리는 그대로 유지됩니다. Unreal Engine 본체와 Engine Tools는 포함하지 않습니다.

[Unreal Engine EULA](https://www.unrealengine.com/eula/unreal)는 설치 디렉터리의 Samples 및 Templates를 Examples로 정의하며, 5(b)에서 수정본을 포함한 Examples의 소스·오브젝트 배포를 허용합니다. 이는 해당 자료를 MIT로 재라이선스한다는 의미가 아닙니다. Fab 등에서 별도로 취득한 자료에 이 허용을 자동 적용하지 않습니다.

## ONE Mobile POP — 별도 설치

Copyright (c) 2021 ONESTORE. All rights reserved. Font designed by FONTRIX Inc.

폰트 원본과 폰트 데이터를 포함한 `ONE_Mobile_POP.uasset`은 공개 배포에서 제외합니다. [원스토어 공식 글꼴 페이지](https://www.onestorecorp.com/sv/fordev_font/)에서 사용자가 직접 내려받고 최신 이용 조건을 확인해야 합니다. [산돌구름의 라이선스 원문 안내](https://www.sandollcloud.com/free-font/16954/ONE-Mobile-POP-OTF?lang=en_US)에는 폰트 자체의 수정·재배포 제한이 기재되어 있습니다.

기존 UI와 같은 표시를 재현하려면 공식 TTF를 UE 콘텐츠 브라우저의 `/Game/Portfolio02/UI`에 Font Face로 가져오고 이름을 `ONE_Mobile_POP`으로 설정합니다. 최종 오브젝트 경로는 `/Game/Portfolio02/UI/ONE_Mobile_POP.ONE_Mobile_POP`입니다. 미설치 시 코드의 기본 폰트 대체가 적용될 수 있지만 한글 표시까지 검증된 상태는 아닙니다. 폰트 원본 및 생성한 폰트 애셋을 PR에 포함하지 마세요.

## UnrealMCP — 별도 설치

`Plugins/UnrealMCP`는 별도 프로젝트 [kks3800/Unreal_MCP](https://github.com/kks3800/Unreal_MCP)이며 이 저장소에서 배포하지 않습니다. 해당 저장소의 설치 안내와 라이선스를 따릅니다.

## 포트폴리오 관계

본 프로젝트는 개인 레벨디자인 학습·포트폴리오 작업입니다. LOST ARK 또는 해당 개발사의 공식 프로젝트나 제휴 결과물이 아닙니다. 외부 참가자 원본 로그·영상은 공개 자료에 포함하지 않습니다.
