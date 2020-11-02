REM BoostPro Binary Installer for Visual C++ - http://www.boostpro.com/download
@set PATH=%PATH%;"%~dp0\bin"
@set PARAMS=threading=multi link=static runtime-link=static
REM bjam toolset=msvc-9.0 variant=debug %PARAMS% address-model=32  --stagedir=./stage cxxflags=/MP
REM bjam toolset=msvc-9.0 variant=release %PARAMS% address-model=32 --stagedir=./stage cxxflags=/MP define=_SECURE_SCL=0
REM bjam toolset=msvc-9.0 variant=debug %PARAMS% address-model=64 --stagedir=./stage64 cxxflags=/MP
REM bjam toolset=msvc-9.0 variant=release %PARAMS% address-model=64 --stagedir=./stage64 cxxflags=/MP define=_SECURE_SCL=0

bjam toolset=msvc-10.0 variant=debug %PARAMS% address-model=32 --stagedir=./stage cxxflags=/MP
bjam toolset=msvc-10.0 variant=release %PARAMS% address-model=32 --stagedir=./stage cxxflags=/MP define=_SECURE_SCL=0
bjam toolset=msvc-10.0 variant=debug %PARAMS% address-model=64 --stagedir=./stage64 cxxflags=/MP
bjam toolset=msvc-10.0 variant=release %PARAMS% address-model=64 --stagedir=./stage64 cxxflags=/MP define=_SECURE_SCL=0
