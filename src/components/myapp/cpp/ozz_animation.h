#ifndef OZZ_ANIMATION_H
#define OZZ_ANIMATION_H

#include <string>
#include <vector>
#include <memory>

// ozz-animation includes
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/blending_job.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/containers/vector.h"

// PlaybackController 类的实现
class PlaybackController
{
public:
    PlaybackController() : time_ratio_(0.f), previous_time_ratio_(0.f), playback_speed_(1.f), play_(true), loop_(true) {}

    int Update(const ozz::animation::Animation &_animation, float _dt)
    {
        float new_ratio = time_ratio_;

        if (play_)
        {
            new_ratio = time_ratio_ + _dt * playback_speed_ / _animation.duration();
        }

        return set_time_ratio(new_ratio);
    }

    int set_time_ratio(float _ratio)
    {
        previous_time_ratio_ = time_ratio_;
        if (loop_)
        {
            const float loops = floorf(_ratio);
            time_ratio_ = _ratio - loops;
            return static_cast<int>(loops);
        }
        else
        {
            time_ratio_ = ozz::math::Clamp(0.f, _ratio, 1.f);
            return 0;
        }
    }

    float time_ratio() const { return time_ratio_; }
    float previous_time_ratio() const { return previous_time_ratio_; }
    void set_playback_speed(float _speed) { playback_speed_ = _speed; }
    float playback_speed() const { return playback_speed_; }
    void set_loop(bool _loop) { loop_ = _loop; }
    bool loop() const { return loop_; }
    bool playing() const { return play_; }
    void Reset()
    {
        previous_time_ratio_ = time_ratio_ = 0.f;
        playback_speed_ = 1.f;
        play_ = true;
    }

private:
    float time_ratio_;
    float previous_time_ratio_;
    float playback_speed_;
    bool play_;
    bool loop_;
};

// OzzAnimation 类 - 管理 ozz 动画系统
class OzzAnimation
{
public:
    OzzAnimation();
    ~OzzAnimation();

    // 初始化函数
    bool Initialize();

    // 加载骨架和动画
    bool LoadSkeleton(const std::string &filePath);
    bool LoadAnimation(const std::string &filePath, int index);
    bool LoadAnimationFromMemory(const char *data, size_t size, int index);

    // 更新动画
    void Update(float deltaTime);

    // 获取变换矩阵
    const std::vector<ozz::math::Float4x4> &GetModelMatrices() const { return models_; }

    // 设置混合参数
    void SetBlendRatio(float ratio);
    void SetManualMode(bool manual);
    void SetWeights(const float weights[3]);

    // 获取状态信息
    std::string GetStatus() const;

private:
    // 从 sample_blend.cc 复制的函数，用于更新运行时参数
    void UpdateRuntimeParameters(float blend_ratio, float weights[3]);

private:
    ozz::animation::Skeleton skeleton_;
    ozz::animation::Animation animations_[3];
    PlaybackController controllers_[3];
    ozz::animation::SamplingJob::Context contexts_[3];

    ozz::vector<ozz::math::SoaTransform> locals_[3];
    ozz::vector<ozz::math::SoaTransform> blended_locals_;
    std::vector<ozz::math::Float4x4> models_;

    float blend_ratio_;
    float weights_[3];
    bool manual_mode_;
    float threshold_;
    bool initialized_;
};

// C 接口函数 - 用于与 JavaScript 交互
#ifdef __cplusplus
extern "C"
{
#endif

    // 创建和销毁 OzzAnimation 实例
    void *ozz_animation_create();
    void ozz_animation_destroy(void *animation);

    // 初始化
    bool ozz_animation_initialize(void *animation);

    // 加载文件
    bool ozz_animation_load_skeleton(void *animation, const char *filePath);
    bool ozz_animation_load_animation(void *animation, const char *filePath, int index);

    // 更新
    void ozz_animation_update(void *animation, float deltaTime);

    // 设置参数
    void ozz_animation_set_blend_ratio(void *animation, float ratio);
    void ozz_animation_set_manual_mode(void *animation, bool manual);
    void ozz_animation_set_weights(void *animation, const float weights[3]);

    // 获取状态
    const char *ozz_animation_get_status(void *animation);

#ifdef __cplusplus
}
#endif

#endif // OZZ_ANIMATION_H
