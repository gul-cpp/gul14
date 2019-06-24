@WHERE cl >nul 2>nul
@IF %ERRORLEVEL%==1 GOTO vcvars
@WHERE ninja >nul 2>nul
@IF %ERRORLEVEL%==1 GOTO vcvars

GOTO after_vcvars

:vcvars
@CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
:after_vcvars

@IF NOT EXIST build\ md build

@IF NOT "%~1%"=="release" GOTO after_release
set BUILDTYPE=release
set FOLDER=build\release
set TARGET=
GOTO meson
:after_release

@IF NOT "%~1%"=="debug" GOTO after_debug
set BUILDTYPE=debug
set FOLDER=build\debug
set TARGET=
GOTO meson
:after_debug

@IF NOT "%~1%"=="test" GOTO after_test
set BUILDTYPE=debug
set FOLDER=build\debug
set TARGET=test
GOTO meson
:after_test

@echo UNKNOWN ARGUMENT (must be "release", "debug", or "test")
@GOTO end

:meson
"C:\Program Files\Meson\meson.exe" --buildtype=%BUILDTYPE% %FOLDER%

:ninja
ninja -C %FOLDER% %TARGET%

:end
