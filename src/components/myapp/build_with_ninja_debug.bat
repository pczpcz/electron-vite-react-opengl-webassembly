@echo off
echo ========================================
echo OpenGL Web Test - Ninja Debug Build Script
echo ========================================

REM 检查Emscripten环境变量
if "%EMSDK%"=="" (
    echo Error: EMSDK environment variable is not set!
    echo Please set EMSDK environment variable first.
    pause
    exit /b 1
)

REM 创建调试构建目录
if not exist "build_emscripten_debug" mkdir build_emscripten_debug

echo.
echo [1/3] Configuring CMake with Emscripten toolchain (Debug mode)...
cd build_emscripten_debug
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=%EMSDK%/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Debug ..

if %errorlevel% neq 0 (
    echo Error: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [2/3] Building with Ninja (Debug mode)...
ninja

if %errorlevel% neq 0 (
    echo Error: Ninja build failed!
    pause
    exit /b 1
)

echo.
echo [3/3] Debug build completed successfully!
echo.
echo Generated debug files:
echo   - OpenglWebTest.html (Debug Web page)
echo   - OpenglWebTest.js   (Debug JavaScript glue code)
echo   - OpenglWebTest.wasm (Debug WebAssembly binary)
echo.
echo Copying debug build to main build directory...
cd ..
if exist "build_emscripten" rmdir /s /q build_emscripten
xcopy build_emscripten_debug build_emscripten /E /I /Y

echo.
echo [4/4] Copying WebAssembly files to public directory...
REM 确保public目录存在
xcopy "build_emscripten\OpenglWebTest.wasm" "..\..\..\public\" /Y
xcopy "build_emscripten\OpenglWebTest.js" "..\..\..\public\" /Y

echo.
echo Debug build ready for debugging!
echo You can now use VSCode debug configurations or run: npm run debug
echo.
echo WebAssembly files copied to public directory for Electron app usage.
echo.
pause
