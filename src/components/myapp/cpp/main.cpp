#include <functional>
#include <iostream>

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

#include <emscripten.h>
#include <emscripten/html5.h>

// define GL_GLEXT_PROTOTYPES 1
// #include <GLES3/gl3.h>

// https://zhuanlan.zhihu.com/p/664016921
// https://github.com/emscripten-core/emscripten/blob/main/test/browser/test_sdl2_glshader.c
// https://github.com/tcoppex/gl-wasm-template/blob/main/src/cpp/framework/base_app.cc

EM_BOOL onKeyPress(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onKeyDown(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onKeyUp(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onMouseMove(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onMouseUp(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onMouseDown(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onMouseEnter(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onMouseLeave(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onTouchStart(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onTouchEnd(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onTouchMove(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onTouchCancel(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    std::cerr << __FUNCTION__ << std::endl;
    return EM_TRUE;
}

EM_BOOL onResize(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
{
    int newWidth = (int)uiEvent->windowInnerWidth;
    int newHeight = (int)uiEvent->windowInnerHeight /* * hconstants::MAX_DIM_RATIO*/;
    glViewport(0, 0, newWidth, newHeight);
    std::cout << newWidth << "x" << newHeight << std::endl;
    return EM_TRUE;
}

void setupEvents();

// settings
struct WebData_t
{
    int width;
    int height;
};

static WebData_t sWEB;
GLFWwindow *window = nullptr;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 300 es\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\n";

const char *fragmentShaderSource = "#version 300 es\n"
                                   "precision mediump float;\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n";

std::function<void()> frame;
void main_loop() { frame(); }
/**
 * 主函数 - OpenGL应用程序的入口点
 * 初始化GLFW，创建窗口，编译着色器，设置顶点数据，并进入渲染循环
 */
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Render context.
    glfwWindowHint(GLFW_SAMPLES, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    setupEvents();

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 跟踪窗口大小变化
    static int lastWidth = SCR_WIDTH;
    static int lastHeight = SCR_HEIGHT;

    // render loop
    // -----------
    frame = [&]
    {
        if (!glfwWindowShouldClose(window))
        {
            // 处理GLFW事件
            // glfwPollEvents();

            // 检查窗口大小是否变化
            // int currentWidth, currentHeight;
            // glfwGetWindowSize(window, &currentWidth, &currentHeight);

            // emscripten_get_canvas_element_size("#canvas", &currentWidth, &currentHeight);
            // std::cout << "enter glfwPollEvents: " << currentWidth << "x" << currentHeight << std::endl;
            // if (currentWidth != lastWidth || currentHeight != lastHeight)
            //{
            //     std::cout << "Window resized via glfwPollEvents: " << currentWidth << "x" << currentHeight << std::endl;
            //     lastWidth = currentWidth;
            //     lastHeight = currentHeight;

            // 更新视口
            //    glViewport(0, 0, currentWidth, currentHeight);
            //    glfwSetWindowSize(window, currentWidth, currentHeight);
            //}

            // render
            // ------
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw our first triangle
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            // glDrawArrays(GL_TRIANGLES, 0, 6);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            // glBindVertexArray(0); // no need to unbind it every time

            // glfw: swap buffers
            // ------------------
            glfwSwapBuffers(window);
        }
    };
    emscripten_set_main_loop(main_loop, 60, true);

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void setupEvents()
{
#if defined(EMSCRIPTEN)
    {
        EMSCRIPTEN_RESULT res = EMSCRIPTEN_RESULT_SUCCESS;

        constexpr void *userData = nullptr;
        constexpr bool useCapture = true;
        constexpr char const *id = "#canvas"; // [todo : provide in the constructor]

        // Keys callbacks.
        res |= emscripten_set_keypress_callback(id, userData, !useCapture, onKeyPress);
        res |= emscripten_set_keydown_callback(id, userData, !useCapture, onKeyDown);
        res |= emscripten_set_keyup_callback(id, userData, !useCapture, onKeyUp);

        // Mouse callbacks.
        res |= emscripten_set_mousemove_callback(id, userData, !useCapture, onMouseMove);
        res |= emscripten_set_mouseup_callback(id, userData, useCapture, onMouseUp);
        res |= emscripten_set_mousedown_callback(id, userData, useCapture, onMouseDown);
        // res |= emscripten_set_click_callback(id, userData, useCapture, em_mouse_callback_func callback);
        // res |= emscripten_set_dblclick_callback(id, userData, useCapture, em_mouse_callback_func callback);
        res |= emscripten_set_mouseenter_callback(id, userData, !useCapture, onMouseEnter);
        res |= emscripten_set_mouseleave_callback(id, userData, !useCapture, onMouseLeave);

        // Touch callbacks.
        res |= emscripten_set_touchstart_callback(id, userData, useCapture, onTouchStart);
        res |= emscripten_set_touchmove_callback(id, userData, useCapture, onTouchMove);
        res |= emscripten_set_touchend_callback(id, userData, useCapture, onTouchEnd);
        res |= emscripten_set_touchcancel_callback(id, userData, useCapture, onTouchCancel);

        // Surface size callbacks.
        // https://stackoverflow.com/questions/63987317/proper-way-to-handle-sdl2-resizing-in-emscripten
        res |= emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, !useCapture, onResize);

        if (res != EMSCRIPTEN_RESULT_SUCCESS)
        {
            std::cerr << "Some callbacks initialization failed." << std::endl;
        }

        double w, h;
        emscripten_get_element_css_size(id, &w, &h);

        sWEB.width = static_cast<int>(w);
        sWEB.height = static_cast<int>(h);
        emscripten_set_canvas_element_size(id, sWEB.width, sWEB.height);
    }
#endif

    // https://stackoverflow.com/questions/63987317/proper-way-to-handle-sdl2-resizing-in-emscripten
    // glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height)
    //                          { std::cout << "glfwSetWindowSizeCallback" << std::endl;
    //                                    glViewport(0, 0, width, height); });

    // glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
    //                    {
    // if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE)) {
    //   glfwSetWindowShouldClose(window, GLFW_TRUE);
    // } });
}
