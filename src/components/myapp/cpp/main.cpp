#include <functional>
#include <iostream>

#include <emscripten.h>
#include <SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>

// https://zhuanlan.zhihu.com/p/664016921
// https://github.com/emscripten-core/emscripten/blob/main/test/browser/test_sdl2_glshader.c

// Function to check OpenGL errors
void checkGLError(const char *operation)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error after " << operation << ": " << error << std::endl;
    }
}

// Function to check if glBufferData was successful
bool checkBufferDataSuccess(GLuint buffer, GLenum target, GLsizeiptr expectedSize)
{
    glBindBuffer(target, buffer);
    GLint bufferSize = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);

    if (bufferSize != expectedSize)
    {
        std::cout << "Buffer data failed: expected size " << expectedSize
                  << ", got " << bufferSize << std::endl;
        return false;
    }

    return true;
}

// Shader sources
const char *vertexShaderSource =
    "attribute vec4 a_position;\n"
    "void main() {\n"
    "    gl_Position = a_position;\n"
    "}\n";

const char *fragmentShaderSource =
    "precision mediump float;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

// an example of something we will control from the javascript side
bool background_is_black = true;

// the function called by the javascript code
extern "C" void EMSCRIPTEN_KEEPALIVE toggle_background_color() { background_is_black = !background_is_black; }

std::function<void()> loop;
void main_loop() { loop(); }

int main()
{
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(400, 300, 0, &window, nullptr);

    // 针对 OpenGL ES，表示要生成几个 vao，后面顶点属性绑定 vbo 时会保存到这里
    GLuint vao;
    glGenVertexArraysOES(1, &vao);
    glBindVertexArrayOES(vao);

    // 1. 顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // 2. 片元着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // 3. 程序对象
    GLuint program = glCreateProgram();
    // 将着色器附加到程序对象上
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);

    // 顶点数据
    GLfloat vertices[] = {0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f};

    GLuint vbo;
    glGenBuffers(1, &vbo);
    // 绑定缓冲区到上下文
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // 将 顶点数据 复制到 缓冲区
    glBufferData(GL_ARRAY_BUFFER, 24, vertices, GL_STATIC_DRAW);

    // 获取顶点着色器中的 position 属性
    GLint position = glGetAttribLocation(program, "a_position");

    // 设置读取方式
    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // 启用顶点属性数组
    glEnableVertexAttribArray(position);

    loop = [&]
    {
        // Clear the screen first
        // checkGLError("glBufferData in loop");

        glClearColor(0.5f, 0.f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle from the 3 vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    };

    // Enable vertical sync by setting frame rate limit to 60 FPS
    // This simulates VSync behavior in WebGL/WebAssembly
    emscripten_set_main_loop(main_loop, 60, true);

    return EXIT_SUCCESS;
}
