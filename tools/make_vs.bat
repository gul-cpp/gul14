@REM Setting up the ninja build system with meson on Windows

@REM Uncomment and edit the following 3 lines to reflect your Visual Studio and Meson installation paths.
@REM If nothing is set some defaults will be used

@REM @set MESON="C:\Program Files\Meson\meson.exe"
@REM @set VCVARS_x86="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars32.bat"
@REM @set VCVARS_x64="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"

@call :find_default_studio
@IF ERRORLEVEL 1 (
    echo Can not find Visual Studio installation at default location
    exit /B 1
)

@IF NOT DEFINED MESON set MESON="meson.exe"

@set TOOLDIR=%~p0
@call :get_repo_root "%TOOLDIR:~0,-1%"

@IF NOT "%~1%"=="mrproper" GOTO after_mrproper
@IF EXIST "%REPO_ROOT%build" rmdir /S /Q "%REPO_ROOT%build"
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
@echo %~n0 mrproper                    - Remove the entire build directory
@echo %~n0 release ^<platform^>          - Build the release version of the library for the specified platform
@echo %~n0 debug ^<platform^>            - Build the debug version of the library for the specified platform
@echo %~n0 test ^<buildtype^> ^<platform^> - Run the unit tests for the specified build type and platform
@echo.
@echo ^<buildtype^> is one of "release" or "debug"
@echo ^<platform^> is one of "x86" or "x64"

@GOTO end


:prepare_build_dir
@IF NOT EXIST "%REPO_ROOT%build" md "%REPO_ROOT%build"

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
IF NOT EXIST "%REPO_ROOT%%FOLDER%" md "%REPO_ROOT%%FOLDER%"

@IF "%BUILDTYPE%" == "" GOTO show_usage

@set FOLDER=%FOLDER%\%BUILDTYPE%

@IF EXIST %FOLDER% GOTO after_meson
%MESON% --buildtype=%BUILDTYPE% "%REPO_ROOT%%FOLDER%" "%REPO_ROOT:~0,-1%"
:after_meson

:ninja
ninja -C "%REPO_ROOT%%FOLDER%" %TARGET%

@echo Finished. Build directory: %FOLDER%
@GOTO end

@REM Subroutines following

:get_repo_root
    @set REPO_ROOT=%~dp1
    @exit /B

:find_default_studio
    @IF DEFINED VCVARS_x86 IF DEFINED VCVARS_x64 exit /B 0

    @REM Trying to find the newest Visual Studio at the default install path
    @set VC_PREFIX=C:\Program Files (x86)\Microsoft Visual Studio\
    @IF EXIST "%VC_PREFIX%2019" (
        set VC_YEAR=2019\
    ) ELSE IF EXIST "%VC_PREFIX%2017" (
        set VC_YEAR=2017\
    ) ELSE exit /B 1

    @IF EXIST "%VC_PREFIX%%VC_YEAR%Enterprise" (
        set VC_VERSION=Enterprise\
    ) ELSE IF EXIST "%VC_PREFIX%%VC_YEAR%Professional" (
        set VC_VERSION=Professional\
    ) ELSE IF EXIST "%VC_PREFIX%%VC_YEAR%Community" (
        set VC_VERSION=Community\
    ) ELSE exit /B 2

    @IF NOT DEFINED VCVARS_x86 set VCVARS_x86="%VC_PREFIX%%VC_YEAR%%VC_VERSION%VC\Auxiliary\Build\vcvars32.bat"
    @IF NOT DEFINED VCVARS_x64 set VCVARS_x64="%VC_PREFIX%%VC_YEAR%%VC_VERSION%VC\Auxiliary\Build\vcvars64.bat"
    @exit /B 0

:end
