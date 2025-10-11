#include "ozz_animation.h"
#include <iostream>
#include <fstream>
#include <cstring>

#include <emscripten.h>
#include <emscripten/html5.h>

// 使用相对路径包含 ozz-animation 头文件
#include "../thirdParty/ozz-animation/include/ozz/animation/runtime/animation.h"
#include "../thirdParty/ozz-animation/include/ozz/animation/runtime/skeleton.h"
#include "../thirdParty/ozz-animation/include/ozz/animation/runtime/sampling_job.h"
#include "../thirdParty/ozz-animation/include/ozz/animation/runtime/local_to_model_job.h"
#include "../thirdParty/ozz-animation/include/ozz/animation/runtime/blending_job.h"
#include "../thirdParty/ozz-animation/include/ozz/base/maths/soa_transform.h"
#include "../thirdParty/ozz-animation/include/ozz/base/maths/vec_float.h"
#include "../thirdParty/ozz-animation/include/ozz/base/maths/simd_math.h"
#include "../thirdParty/ozz-animation/include/ozz/base/io/stream.h"
#include "../thirdParty/ozz-animation/include/ozz/base/io/archive.h"
#include "../thirdParty/ozz-animation/include/ozz/base/containers/vector.h"

using namespace ozz;
using namespace ozz::animation;
using namespace ozz::math;

OzzAnimation::OzzAnimation()
    : blend_ratio_(0.f), manual_mode_(false), threshold_(.1f), initialized_(false)
{
    weights_[0] = 1.f;
    weights_[1] = 0.f;
    weights_[2] = 0.f;
}

OzzAnimation::~OzzAnimation()
{
    // 清理资源
}

bool OzzAnimation::Initialize()
{
    if (initialized_)
    {
        return true;
    }

    // EM_ASM(  //无法在渲染进程使用，只能在node.js使用
    //     FS.mkdir('/data');                         // (1)
    //     FS.mount(NODEFS, {root : '.'}, '/data');); // (2)
    // std::ifstream in(std::string("/data/media/animation1.ozz"));
    // if (in.fail())
    //{
    //     std::cerr << "ERROR, Could not open " << "/data/media/animation1.ozz" << std::endl;
    //     exit(1);
    // }

    // 初始化数据结构
    initialized_ = true;
    return true;
}

bool OzzAnimation::LoadSkeleton(const std::string &filePath)
{
    // 打开文件流
    ozz::io::File file(filePath.c_str(), "rb");
    if (!file.opened())
    {
        std::cerr << "Failed to open skeleton file: " << filePath << std::endl;
        return false;
    }

    // 加载骨架
    ozz::io::IArchive archive(&file);
    if (!archive.TestTag<ozz::animation::Skeleton>())
    {
        std::cerr << "Failed to load skeleton from file: " << filePath << std::endl;
        return false;
    }

    archive >> skeleton_;

    // 初始化本地变换和模型矩阵
    const int num_joints = skeleton_.num_joints();
    const int num_soa_joints = skeleton_.num_soa_joints();

    for (int i = 0; i < 3; ++i)
    {
        locals_[i].resize(num_soa_joints);
        contexts_[i].Resize(num_joints);
    }
    blended_locals_.resize(num_soa_joints);
    models_.resize(num_joints);

    std::cout << "Skeleton loaded successfully: " << num_joints << " joints" << std::endl;
    return true;
}

bool OzzAnimation::LoadAnimation(const std::string &filePath, int index)
{
    if (index < 0 || index >= 3)
    {
        std::cerr << "Invalid animation index: " << index << std::endl;
        return false;
    }

    // 打开文件流
    ozz::io::File file(filePath.c_str(), "rb");
    if (!file.opened())
    {
        std::cerr << "Failed to open animation file: " << filePath << std::endl;
        return false;
    }

    // 加载动画
    ozz::io::IArchive archive(&file);
    if (!archive.TestTag<ozz::animation::Animation>())
    {
        std::cerr << "Failed to load animation from file: " << filePath << std::endl;
        return false;
    }

    archive >> animations_[index];

    std::cout << "Animation " << index << " loaded successfully: duration="
              << animations_[index].duration() << std::endl;
    return true;
}

bool OzzAnimation::LoadAnimationFromMemory(const char *data, size_t size, int index)
{
    if (index < 0 || index >= 3)
    {
        std::cerr << "Invalid animation index: " << index << std::endl;
        return false;
    }

    if (!data || size == 0)
    {
        std::cerr << "Invalid animation data: null pointer or zero size" << std::endl;
        return false;
    }

    // 创建内存流
    ozz::io::MemoryStream stream;
    stream.Write(data, size);
    stream.Seek(0, ozz::io::Stream::kSet);

    // 加载动画
    ozz::io::IArchive archive(&stream);
    if (!archive.TestTag<ozz::animation::Animation>())
    {
        std::cerr << "Failed to load animation from memory" << std::endl;
        return false;
    }

    archive >> animations_[index];

    std::cout << "Animation " << index << " loaded from memory successfully: duration="
              << animations_[index].duration() << std::endl;
    return true;
}

void OzzAnimation::Update(float deltaTime)
{
    if (!initialized_ || skeleton_.num_joints() == 0)
    {
        return;
    }

    // 更新播放控制器
    for (int i = 0; i < 3; ++i)
    {
        if (animations_[i].duration() > 0.f)
        {
            controllers_[i].Update(animations_[i], deltaTime);
        }
    }

    // 更新运行时参数
    UpdateRuntimeParameters(blend_ratio_, weights_);

    // 采样动画
    for (int i = 0; i < 3; ++i)
    {
        if (animations_[i].duration() > 0.f)
        {
            SamplingJob sampling_job;
            sampling_job.animation = &animations_[i];
            sampling_job.context = &contexts_[i];
            sampling_job.ratio = controllers_[i].time_ratio();
            sampling_job.output = make_span(locals_[i]);
            if (!sampling_job.Run())
            {
                std::cerr << "Failed to sample animation " << i << std::endl;
            }
        }
    }

    // 混合动画
    BlendingJob blend_job;
    blend_job.threshold = threshold_;

    // 设置混合层
    BlendingJob::Layer layers[3];
    for (int i = 0; i < 3; ++i)
    {
        layers[i].transform = make_span(locals_[i]);
        layers[i].weight = weights_[i];
    }
    blend_job.layers = layers;
    blend_job.rest_pose = skeleton_.joint_rest_poses();
    blend_job.output = make_span(blended_locals_);

    if (!blend_job.Run())
    {
        std::cerr << "Failed to blend animations" << std::endl;
    }

    // 转换为模型空间矩阵
    LocalToModelJob ltm_job;
    ltm_job.skeleton = &skeleton_;
    ltm_job.input = make_span(blended_locals_);
    ltm_job.output = make_span(models_);
    if (!ltm_job.Run())
    {
        std::cerr << "Failed to convert local to model space" << std::endl;
    }
}

void OzzAnimation::UpdateRuntimeParameters(float blend_ratio, float weights[3])
{
    // 从 sample_blend.cc 复制的逻辑
    if (manual_mode_)
    {
        // 手动模式：直接使用设置的权重
        weights_[0] = weights[0];
        weights_[1] = weights[1];
        weights_[2] = weights[2];
    }
    else
    {
        // 自动模式：基于混合比例计算权重
        weights_[0] = 1.f - blend_ratio;
        weights_[1] = blend_ratio;
        weights_[2] = 0.f;
    }
}

void OzzAnimation::SetBlendRatio(float ratio)
{
    blend_ratio_ = ratio;
}

void OzzAnimation::SetManualMode(bool manual)
{
    manual_mode_ = manual;
}

void OzzAnimation::SetWeights(const float weights[3])
{
    weights_[0] = weights[0];
    weights_[1] = weights[1];
    weights_[2] = weights[2];
}

std::string OzzAnimation::GetStatus() const
{
    std::string status = "OzzAnimation Status:\n";
    status += "  Initialized: " + std::string(initialized_ ? "Yes" : "No") + "\n";
    status += "  Skeleton joints: " + std::to_string(skeleton_.num_joints()) + "\n";
    status += "  Blend ratio: " + std::to_string(blend_ratio_) + "\n";
    status += "  Manual mode: " + std::string(manual_mode_ ? "Yes" : "No") + "\n";
    status += "  Weights: [" + std::to_string(weights_[0]) + ", " + std::to_string(weights_[1]) + ", " + std::to_string(weights_[2]) + "]\n";

    for (int i = 0; i < 3; ++i)
    {
        status += "  Animation " + std::to_string(i) + ": duration=" + std::to_string(animations_[i].duration()) + "\n";
    }

    return status;
}

// C 接口实现
extern "C"
{
    void *ozz_animation_create()
    {
        return new OzzAnimation();
    }

    void ozz_animation_destroy(void *animation)
    {
        delete static_cast<OzzAnimation *>(animation);
    }

    bool ozz_animation_initialize(void *animation)
    {
        return static_cast<OzzAnimation *>(animation)->Initialize();
    }

    bool ozz_animation_load_skeleton(void *animation, const char *filePath)
    {
        return static_cast<OzzAnimation *>(animation)->LoadSkeleton(filePath);
    }

    bool ozz_animation_load_animation(void *animation, const char *filePath, int index)
    {
        return static_cast<OzzAnimation *>(animation)->LoadAnimation(filePath, index);
    }

    bool ozz_animation_load_animation_from_memory(void *animation, const char *data, int size, int index)
    {
        return static_cast<OzzAnimation *>(animation)->LoadAnimationFromMemory(data, size, index);
    }

    void ozz_animation_update(void *animation, float deltaTime)
    {
        static_cast<OzzAnimation *>(animation)->Update(deltaTime);
    }

    void ozz_animation_set_blend_ratio(void *animation, float ratio)
    {
        static_cast<OzzAnimation *>(animation)->SetBlendRatio(ratio);
    }

    void ozz_animation_set_manual_mode(void *animation, bool manual)
    {
        static_cast<OzzAnimation *>(animation)->SetManualMode(manual);
    }

    void ozz_animation_set_weights(void *animation, const float weights[3])
    {
        static_cast<OzzAnimation *>(animation)->SetWeights(weights);
    }

    const char *ozz_animation_get_status(void *animation)
    {
        static std::string status;
        status = static_cast<OzzAnimation *>(animation)->GetStatus();
        return status.c_str();
    }

} // extern "C"

// Electron 绑定接口 - 用于在渲染进程中调用 ozz 动画功能
#ifdef __EMSCRIPTEN__
extern "C"
{

    // 创建新的 ozz 动画实例
    EMSCRIPTEN_KEEPALIVE
    void *create_ozz_animation()
    {
        return ozz_animation_create();
    }

    // 销毁 ozz 动画实例
    EMSCRIPTEN_KEEPALIVE
    void destroy_ozz_animation(void *animation)
    {
        ozz_animation_destroy(animation);
    }

    // 初始化 ozz 动画
    EMSCRIPTEN_KEEPALIVE
    bool initialize_ozz_animation(void *animation)
    {
        return ozz_animation_initialize(animation);
    }

    // 加载骨架
    EMSCRIPTEN_KEEPALIVE
    bool load_skeleton(void *animation, const char *filePath)
    {
        return ozz_animation_load_skeleton(animation, filePath);
    }

    // 加载动画
    EMSCRIPTEN_KEEPALIVE
    bool load_animation(void *animation, const char *filePath, int index)
    {
        return ozz_animation_load_animation(animation, filePath, index);
    }

    // 从内存加载动画
    EMSCRIPTEN_KEEPALIVE
    bool load_animation_from_memory(void *animation, const char *data, int size, int index)
    {
        return ozz_animation_load_animation_from_memory(animation, data, size, index);
    }

    // 更新动画
    EMSCRIPTEN_KEEPALIVE
    void update_ozz_animation(void *animation, float deltaTime)
    {
        ozz_animation_update(animation, deltaTime);
    }

    // 设置混合比例
    EMSCRIPTEN_KEEPALIVE
    void set_blend_ratio(void *animation, float ratio)
    {
        ozz_animation_set_blend_ratio(animation, ratio);
    }

    // 设置手动模式
    EMSCRIPTEN_KEEPALIVE
    void set_manual_mode(void *animation, bool manual)
    {
        ozz_animation_set_manual_mode(animation, manual);
    }

    // 设置权重
    EMSCRIPTEN_KEEPALIVE
    void set_weights(void *animation, float weight0, float weight1, float weight2)
    {
        float weights[3] = {weight0, weight1, weight2};
        ozz_animation_set_weights(animation, weights);
    }

    // 获取状态信息
    EMSCRIPTEN_KEEPALIVE
    const char *get_ozz_status(void *animation)
    {
        return ozz_animation_get_status(animation);
    }

    // 测试函数 - 在渲染进程中调用
    EMSCRIPTEN_KEEPALIVE
    void test_ozz_animation()
    {
        std::cout << "Ozz animation test function called from JavaScript" << std::endl;

        // 创建动画实例
        void *animation = create_ozz_animation();
        if (animation)
        {
            std::cout << "Ozz animation instance created successfully" << std::endl;

            // 获取状态
            const char *status = get_ozz_status(animation);
            std::cout << "Animation status: " << status << std::endl;

            // 销毁实例
            destroy_ozz_animation(animation);
            std::cout << "Ozz animation instance destroyed" << std::endl;
        }
        else
        {
            std::cout << "Failed to create ozz animation instance" << std::endl;
        }
    }

} // extern "C"
#endif
