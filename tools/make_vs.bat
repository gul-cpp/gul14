@REM This batch file should be run from the root directory of the GUL folder,
@REM e.g. via "tools\make_vs debug x86".

@REM Edit the following 3 lines to reflect your Visual Studio and Meson installation paths.
@set VCVARS_x86="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat"
@set VCVARS_x64="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
@set MESON="C:\Program Files\Meson\meson.exe"



@IF NOT "%~1%"=="mrproper" GOTO after_mrproper
rmdir /S /Q build
@GOTO end
:after_mrproper

@IF NOT "%~1%"=="test" GOTO after_test
@set BUILDTYPE=%~2%
@set PLATFORM=%~3%
@set TARGET=test
@GOTO prepare_build_dir
:after_test

@IF NOT "%~1%"=="release" GOTO after_release
@set BUILDTYPE=release
@set PLATFORM=%~2%
@set TARGET=
@GOTO prepare_build_dir
:after_release

@IF NOT "%~1%"=="debug" GOTO after_debug
@set BUILDTYPE=debug
@set PLATFORM=%~2%
@set TARGET=
@GOTO prepare_build_dir
:after_debug

:show_usage
@echo INVALID ARGUMENTS: %*
@echo.
@echo Usage:
@echo %0 mrproper                    - Remove the entire build directory
@echo %0 release ^<platform^>          - Build the release version of the library for the specified platform
@echo %0 debug ^<platform^>            - Build the debug version of the library for the specified platform
@echo %0 test ^<buildtype^> ^<platform^> - Run the unit tests for the specified build type and platform
@echo.
@echo ^<buildtype^> is one of "release" or "debug"
@echo ^<platform^> is one of "x86" or "x64"

@GOTO end


:prepare_build_dir
@IF NOT EXIST build\ md build

@IF "%PLATFORM%"=="" GOTO show_usage

:x86
@IF NOT "%PLATFORM%"=="x86" GOTO after_x86
@CALL %VCVARS_x86%
@GOTO prepare_platform_dir
:after_x86

:x64
@IF NOT "%PLATFORM%"=="x64" GOTO after_x64
@CALL %VCVARS_x64%
@GOTO prepare_platform_dir
:after_x64

@GOTO show_usage

:prepare_platform_dir
set FOLDER=build\%PLATFORM%-windows
IF NOT EXIST %FOLDER%\ md %FOLDER%

@IF "%BUILDTYPE%" == "" GOTO show_usage

@set FOLDER=%FOLDER%\%BUILDTYPE%

@IF EXIST %FOLDER%\ GOTO after_meson
%MESON% --buildtype=%BUILDTYPE% %FOLDER%
:after_meson

:ninja
ninja -C %FOLDER% %TARGET%

@echo Finished. Build directory: %FOLDER%

:end
