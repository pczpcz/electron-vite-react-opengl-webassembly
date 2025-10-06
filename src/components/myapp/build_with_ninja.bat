@echo off
echo ========================================
echo OpenGL Web Test - Ninja Build Script
echo ========================================

REM 检查Emscripten环境变量
if "%EMSDK%"=="" (
    echo Error: EMSDK environment variable is not set!
    echo Please set EMSDK environment variable first.
    pause
    exit /b 1
)

REM 创建构建目录
if not exist "build_emscripten" mkdir build_emscripten

echo.
echo [1/3] Configuring CMake with Emscripten toolchain...
cd build_emscripten
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=%EMSDK%/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake ..

if %errorlevel% neq 0 (
    echo Error: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [2/3] Building with Ninja...
ninja

if %errorlevel% neq 0 (
    echo Error: Ninja build failed!
    pause
    exit /b 1
)

echo.
echo [3/4] Build completed successfully!
echo.
echo Generated files:
echo   - OpenglWebTest.html (Web page)
echo   - OpenglWebTest.js   (JavaScript glue code)
echo   - OpenglWebTest.wasm (WebAssembly binary)
echo.
echo [4/4] Copying WebAssembly files to public directory...
REM 确保public目录存在

xcopy "build_emscripten\OpenglWebTest.wasm" "..\..\..\public\" /Y
xcopy "build_emscripten\OpenglWebTest.js" "..\..\..\public\" /Y

echo.
echo Build completed successfully!
echo You can open build_emscripten\OpenglWebTest.html in a web browser to test the application.
echo.
echo WebAssembly files copied to public directory for Electron app usage.
echo.
pause
