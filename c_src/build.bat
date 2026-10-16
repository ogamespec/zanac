@echo off
rem ===========================================================================
rem  Zanac C port build script (Windows, MSVC + CMake/Ninja).
rem
rem  Requirements
rem    * Visual Studio 2022 or newer (2026 works the same way) with the
rem      "Desktop development with C++" workload - it also provides CMake and
rem      Ninja - or an equivalent MSVC toolchain plus CMake and Ninja on PATH.
rem    * Python 3 on PATH: the port's headers, the extracted ROM banks, the
rem      trampolines, the stubs and the address dispatcher are generated from
rem      the original assembly by c_src\tools\*.py on every build.
rem    * SDL3 for the windowed build.  It is looked up in this order:
rem        1. -DSDL3_DIR=<path> passed on the command line,
rem        2. ..\Nespeculator\third_party\SDL3 (the vendored copy),
rem        3. a system SDL3 found by CMake.
rem      Without SDL3 the port still builds, but headless only
rem      (zanac.exe --frames N / --shot / --hashes / --input).
rem
rem  Usage
rem    build.bat                 configure (first run) and build into build-msvc\
rem    build.bat clean           delete build-msvc\ and rebuild from scratch
rem    build.bat nosdl           force the headless build (no SDL3)
rem
rem  Products
rem    build-msvc\zanac.exe      the C port (windowed when SDL3 was found)
rem    build-msvc\zanac_rom.exe  the reference runner: the ORIGINAL 6502 ROM in
rem                              the same engine, for the frame-by-frame
rem                              comparison (zanac.exe --hashes vs zanac_rom.exe)
rem
rem  The script locates vcvars64.bat itself, so it can be double-clicked from
rem  Explorer; inside a Developer Command Prompt the existing environment is
rem  used as-is.
rem ===========================================================================
setlocal enabledelayedexpansion
set "ROOT=%~dp0"
set "BUILD=%ROOT%build-msvc"
set "NOSDL="

if /i "%~1"=="clean" (
    echo Removing "%BUILD%" ...
    if exist "%BUILD%" rmdir /s /q "%BUILD%"
    if exist "%BUILD%" (
        echo ERROR: could not remove "%BUILD%".
        echo        Close any running zanac.exe and try again.
        exit /b 1
    )
)
if /i "%~1"=="nosdl" set "NOSDL=1"
if /i "%~2"=="nosdl" set "NOSDL=1"

rem ---------------------------------------------------------- toolchain -----
rem If cl.exe is already on PATH we are inside a Developer Command Prompt and
rem vcvars must not be called a second time.
set "VCVARS="
where cl >nul 2>&1 && set "VCVARS=already"

if not defined VCVARS (
    set "VSPATH="
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "!VSWHERE!" (
        for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * ^
            -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
            -property installationPath 2^>nul`) do set "VSPATH=%%i"
    )
    if not defined VSPATH (
        for %%r in ("%ProgramFiles%\Microsoft Visual Studio"
                    "%ProgramFiles(x86)%\Microsoft Visual Studio") do (
            if exist "%%~r" for /d %%y in ("%%~r\*") do (
                if exist "%%~y\VC\Auxiliary\Build\vcvars64.bat" set "VSPATH=%%~y"
            )
        )
    )
    if defined VSPATH set "VCVARS=!VSPATH!\VC\Auxiliary\Build\vcvars64.bat"
)

if not defined VCVARS (
    echo ERROR: no Visual Studio C++ toolchain found.
    echo        Install the "Desktop development with C++" workload, or run
    echo        this script from a Developer Command Prompt.
    exit /b 1
)
if /i not "!VCVARS!"=="already" (
    call "!VCVARS!" >nul 2>&1
    if errorlevel 1 (
        echo VCVARS_FAIL
        exit /b 1
    )
)

rem --------------------------------------------------------------- python ---
set "PYTHON=python"
where python >nul 2>&1
if errorlevel 1 (
    where py >nul 2>&1
    if errorlevel 1 (
        echo ERROR: Python 3 was not found on PATH: the port's generated
        echo        sources are produced by c_src\tools\*.py.
        exit /b 1
    )
    set "PYTHON=py -3"
)

rem --------------------------------------------------------------- cmake ----
set "CMAKE=cmake"
where cmake >nul 2>&1
if errorlevel 1 (
    set "CMAKE=!VCINSTALLDIR!Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    if not exist "!CMAKE!" (
        echo ERROR: cmake was not found on PATH and is not bundled with Visual Studio.
        exit /b 1
    )
)

rem --------------------------------------------------------------- build ----
if not exist "%BUILD%\CMakeCache.txt" (
    set "EXTRA="
    if defined NOSDL set "EXTRA=-DZANAC_NO_SDL=ON"
    "%CMAKE%" -S "%ROOT%." -B "%BUILD%" -G Ninja -DCMAKE_BUILD_TYPE=Release !EXTRA!
    if errorlevel 1 (
        echo CONFIGURE_FAIL
        exit /b 1
    )
)

"%CMAKE%" --build "%BUILD%"
if errorlevel 1 (
    echo BUILD_FAIL
    exit /b 1
)

if not exist "%BUILD%\zanac.exe" (
    echo NO_EXE
    exit /b 1
)

echo BUILD_OK
echo   %BUILD%\zanac.exe        the C port
echo   %BUILD%\zanac_rom.exe    the original ROM in the same engine (reference)
echo.
echo Example: run 900 frames with the scripted input and dump a frame:
echo   "%BUILD%\zanac.exe" --frames 900 --input tests\play.txt --shot frame.ppm
