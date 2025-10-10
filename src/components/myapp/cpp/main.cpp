#include <functional>
#include <iostream>

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

#include <emscripten.h>
#include <emscripten/html5.h>

// 封装的渲染类
#include "vertexarrayobject.h"
#include "bufferobject.h"
#include "material.h"
#include "mesh.h"
#include "gameobject.h"
#include "scene.h"
#include "scenemanager.h"
#include "renderpass.h"
#include "renderpipeline.h"

// ozz-animation includes
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"

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

// settings
struct WebData_t
{
    int width;
    int height;
};

static WebData_t sWEB;

bool bResize = false;
EM_BOOL onResize(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
{
    int newWidth = (int)uiEvent->windowInnerWidth;
    int newHeight = (int)uiEvent->windowInnerHeight /* * hconstants::MAX_DIM_RATIO*/;
    // glViewport(0, 0, newWidth, newHeight);
    sWEB.width = newWidth;
    sWEB.height = newHeight;
    bResize = true;
    return EM_TRUE;
}

void setupEvents();

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

    ozz::animation::Animation anima;

    double w, h;
    emscripten_get_element_css_size("#canvas", &w, &h);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow((int)w, (int)h, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    setupEvents();

    // 使用新的渲染架构
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

    // 创建着色器对象 - 从内存源码创建
    auto shader = std::make_shared<Shader>(vertexShaderSource, fragmentShaderSource, true);

    // 创建材质
    auto material = std::make_shared<Material>(shader);
    // material->setColor("color", 1.0f, 0.5f, 0.2f, 1.0f);

    // 创建网格
    auto mesh = std::make_shared<Mesh>();
    mesh->setVertices(vertices, 4, 3 * sizeof(float)); // 4个顶点，每个顶点3个float
    mesh->setIndices(indices, 6);                      // 6个索引
    mesh->setMaterial(material);

    // 创建游戏对象
    auto gameObject = std::make_shared<GameObject>("Triangle");
    gameObject->setPosition(0.0f, 0.0f, 0.0f);
    gameObject->addMesh(mesh);

    // 创建场景并添加游戏对象
    auto scene = std::make_shared<Scene>("MainScene");
    scene->addGameObject(gameObject);

    // 创建场景管理器并添加场景
    auto sceneManager = std::make_shared<SceneManager>();
    sceneManager->addScene("main", scene);
    sceneManager->initializeCurrentScene();

    // 创建渲染过程
    auto renderPass = std::make_shared<RenderPass>();
    renderPass->setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    renderPass->setClearMask(GL_COLOR_BUFFER_BIT);

    // 从场景中获取所有游戏对象添加到渲染过程
    auto currentScene = sceneManager->getCurrentScene();
    if (currentScene)
    {
        auto gameObjects = currentScene->getGameObjects();
        for (auto &obj : gameObjects)
        {
            renderPass->addGameObject(obj);
        }
    }

    // 创建渲染管线并添加渲染过程
    auto renderPipeline = std::make_shared<RenderPipeline>();
    renderPipeline->addRenderPass("main", renderPass);

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

            if (bResize)
            {
                double w, h;
                emscripten_get_element_css_size("#canvas", &w, &h);
                // SDL_SetWindowSize(sdl_window, (int)w, (int)h);
                // glfwSetWindowSize(window, (int)w, (int)h);
                emscripten_set_element_css_size("#canvas", (double)sWEB.width, (double)sWEB.height);
                // glViewport(0, 0, sWEB.width, sWEB.height);

                std::cout << sWEB.width << "," << sWEB.height << "," << w << "," << h << std::endl;
                bResize = false;
            }

            // render
            // ------
            // 使用渲染管线执行所有渲染过程
            renderPipeline->render();

            // glfw: swap buffers
            // ------------------
            glfwSwapBuffers(window);
        }
    };
    // 使用requestAnimationFrame而不是固定帧率
    emscripten_set_main_loop(main_loop, 0, true);
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

        // Get the actual CSS size of the canvas element
        double w, h;
        emscripten_get_element_css_size("#canvas", &w, &h);
        sWEB.width = static_cast<int>(w);
        sWEB.height = static_cast<int>(h);
        emscripten_set_element_css_size("#canvas", w, h);

        // Set the canvas size to match the CSS size for proper rendering
        // emscripten_set_canvas_element_size(id, sWEB.width, sWEB.height);

        // Update the viewport to match the new canvas size
        // glViewport(0, 0, sWEB.width, sWEB.height);
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
