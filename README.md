[![License](https://img.shields.io/badge/License-BSD%202--Clause-orange.svg)](https://opensource.org/licenses/BSD-2-Clause)
# lorecpp-myst

[안영기](https://github.com/smgal)님이 개발한 [또 다른 지식의 성전 3부 비전속으로](https://namu.wiki/w/%EB%98%90%EB%8B%A4%EB%A5%B8%20%EC%A7%80%EC%8B%9D%EC%9D%98%20%EC%84%B1%EC%A0%84)의 [파스칼 소스코드](https://github.com/smgal/LoreTrilogy_1993)를 LLM 기반의 코딩 에이전트를 이용해 SDL 과 C++ 로 포팅한 버전의 저장소 입니다.

![screenshot](https://github.com/user-attachments/assets/f56367ca-9594-4bb2-9f7c-164e943c1db3)

## 구현

* 본 저장소의 모든 코드는 LLM을 이용해 생성하였습니다.
* 사용한 LLM 서비스는 모두 무료(Free tier)로 사용 가능한 제품/모델만을 사용했습니다.
* Gemini CLI 를 기본적으로 사용, Gemini CLI로 해결이 안되는 버그 및 Adlib ROL 파일 재생 등 난이도가 높은 작업은 [웹으로 제공되는 Claude Sonnet](https://claude.ai/new)을 이용하였습니다.


## 빌드

### Linux
```bash
cmake -B build -S .
cmake --build build
```

### Windows (Command Prompt for VS)
```bash
vcpkg x-update-baseline --add-initial-baseline
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build build --config Release
```

## 라이선스

아래 파일들을 제외하고 BSD 라이선스를 따릅니다.

* [adlib.h](https://github.com/leejeonghun/lorecpp-myst/blob/main/src/adlib.h) / [adlib.cpp](https://github.com/leejeonghun/lorecpp-myst/blob/main/src/adlib.cpp) : [adplug](https://github.com/adplug/adplug) 코드를 참고하도록 명시적으로 프롬프트 하여 작성되었습니다. 따라서 LGPL 라이선스를 따릅니다.
* [sounddrv.h](https://github.com/leejeonghun/lorecpp-myst/blob/main/src/sounddrv.h) / [sounddrv.cpp](https://github.com/leejeonghun/lorecpp-myst/blob/main/src/sounddrv.cpp) : [adplug](https://github.com/adplug/adplug) woodyopl 코드를 이름만 바꿔 사용했습니다. 따라서 LGPL 라이선스를 따릅니다.
