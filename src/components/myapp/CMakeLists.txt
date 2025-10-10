cmake_minimum_required(VERSION 3.10.0)
project(OpenglWebTest VERSION 0.1.0 LANGUAGES C CXX)

#https://github.com/emscripten-core/emscripten/blob/main/cmake/Modules/Platform/Emscripten.cmake

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置头文件包含路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cpp)

# 添加第三方库
add_subdirectory(thirdParty)

# 检查是否使用 Emscripten 工具链
if(EMSCRIPTEN)
    message(STATUS "Building with Emscripten")
    
    # 检查EMSDK环境变量
    if(NOT DEFINED ENV{EMSDK})
        message(WARNING "EMSDK environment variable is not set")
    else()
        message(STATUS "EMSDK path: $ENV{EMSDK}")
    endif()

    # 设置Emscripten编译选项
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g -s USE_WEBGL2=1 -s USE_GLFW=3")
        message(STATUS "Building in Debug mode")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -s USE_WEBGL2=1 -s USE_GLFW=3 -s FORCE_FILESYSTEM=1")
        message(STATUS "Building in Release mode")
    endif()
    
    # 创建WebAssembly目标
    add_executable(OpenglWebTest 
        cpp/main.cpp
        cpp/Shader.cpp
        cpp/vertexarrayobject.cpp
        cpp/bufferobject.cpp
        cpp/material.cpp
        cpp/texture.cpp
        cpp/mesh.cpp
        cpp/gameobject.cpp
        cpp/scene.cpp
        cpp/scenemanager.cpp
        cpp/renderpass.cpp
        cpp/rendercommand.cpp
        cpp/renderpipeline.cpp
    )
    
    # 链接第三方库
    target_link_libraries(OpenglWebTest PRIVATE ${THIRD_PARTY_LIBS})
        
    # 设置链接选项 - 所有-s选项应该在链接阶段设置
    target_link_options(OpenglWebTest PRIVATE 
        "-s WASM=1"
        "-s USE_WEBGL2=1"
        "-s USE_GLFW=3"
        # 输出到public目录，确保Vite可以访问
        "-o ${CMAKE_CURRENT_SOURCE_DIR}/public/OpenglWebTest.html"
    )

    # 在Emscripten环境中，OpenGL ES 2.0通过GLFW提供
    # 不需要额外的OpenGL库链接
    set_target_properties(OpenglWebTest PROPERTIES SUFFIX ".html")
endif()
