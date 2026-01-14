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

include_directories(include)

# 2. 定义我们的核心计算器程序（现在只有 main.cpp，以后可以把库拆出去）
add_executable(lac_cli 
    src/main.cpp
    src/LacMatrix.cpp
    src/LacEngine.cpp
)

# 3. 关联 Eigen (Eigen 是 header-only 的，非常轻量)
target_link_libraries(lac_cli PRIVATE Eigen3::Eigen)

# 4. 根据系统进行微调（跨平台必备）
if(MSVC)
    # Windows 环境下特别处理乱码、内存检查等
    add_compile_options(/utf-8 /W4)
else()
    # Linux/Unix 环境下处理符号链接和警告
    add_compile_options(-Wall -Wextra)
endif()