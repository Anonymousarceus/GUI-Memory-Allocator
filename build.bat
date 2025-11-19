@echo off
echo Building Dynamic Memory Allocator...

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Compile source files
echo Compiling allocator.c...
gcc -Wall -Wextra -std=c99 -g -O2 -Iinclude -c src/allocator.c -o build/allocator.o
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling allocator.c
    exit /b 1
)

echo Compiling test.c...
gcc -Wall -Wextra -std=c99 -g -O2 -Iinclude -c src/test.c -o build/test.o
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling test.c
    exit /b 1
)

echo Compiling demo.c...
gcc -Wall -Wextra -std=c99 -g -O2 -Iinclude -c src/demo.c -o build/demo.o
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling demo.c
    exit /b 1
)

echo Compiling gui.c...
gcc -Wall -Wextra -std=c99 -g -O2 -Iinclude -c src/gui.c -o build/gui.o
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling gui.c
    exit /b 1
)

REM Link executables
echo Linking test executable...
gcc build/allocator.o build/test.o -o build/memory_allocator_test.exe
if %ERRORLEVEL% NEQ 0 (
    echo Error linking test executable
    exit /b 1
)

echo Linking demo executable...
gcc build/allocator.o build/demo.o -o build/demo.exe
if %ERRORLEVEL% NEQ 0 (
    echo Error linking demo executable
    exit /b 1
)

echo Linking GUI executable...
gcc build/allocator.o build/gui.o -o build/gui.exe -lgdi32 -luser32 -lkernel32 -lcomctl32
if %ERRORLEVEL% NEQ 0 (
    echo Error linking GUI executable
    exit /b 1
)

echo Build completed successfully!
echo.
echo Available executables:
echo   build\memory_allocator_test.exe - Run comprehensive tests
echo   build\demo.exe                  - Run demonstration program
echo   build\gui.exe                   - Interactive GUI visualizer
echo.
echo Usage:
echo   .\build\memory_allocator_test.exe
echo   .\build\demo.exe
echo   .\build\gui.exe