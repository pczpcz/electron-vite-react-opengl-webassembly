# Third-party libraries configuration

# 添加ozz-animation库
add_subdirectory(ozz-animation)

# 设置第三方库的头文件路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ozz-animation/include)

# 定义第三方库的链接目标
set(THIRD_PARTY_LIBS
    ozz_animation
    ozz_animation_offline
    ozz_base
    ozz_options
    ozz_geometry
    PARENT_SCOPE
)
