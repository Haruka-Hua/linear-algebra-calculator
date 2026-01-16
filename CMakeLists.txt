cmake_minimum_required(VERSION 3.14)
project(LAC_Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 核心任务：自动化获取 Eigen 库
include(FetchContent)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_PROGRESS TRUE
)

# 下载并让项目可见
FetchContent_MakeAvailable(eigen)

# 开关：是否构建单元测试
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    # 2. 使用 FetchContent 获取 GoogleTest
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )

    # 在 MSVC 上使用共享 CRT（与常见的项目配置保持一致）
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)
    enable_testing()
endif()

include_directories(include)

# 3. 将核心实现拆成库，方便主程序与测试共用
add_library(lac_core STATIC
    src/LacMatrix.cpp
    src/LacEngine.cpp
    src/LacException.cpp
)
target_include_directories(lac_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(lac_core PUBLIC Eigen3::Eigen)

add_executable(lac_cli src/main.cpp)
target_link_libraries(lac_cli PRIVATE lac_core)

# 4. 单元测试目标（使用 GoogleTest + CTest）
if(BUILD_TESTS)
    add_executable(lac_tests tests/test_core/test_eigen.cpp)
    target_link_libraries(lac_tests PRIVATE lac_core GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(lac_tests)
endif()

# 5. 根据系统进行微调（跨平台必备）
if(MSVC)
        # Windows 环境下特别处理乱码、内存检查等
        add_compile_options(/utf-8 /W4)
else()
        # Linux/Unix 环境下处理符号链接和警告
        add_compile_options(-Wall -Wextra)
endif()