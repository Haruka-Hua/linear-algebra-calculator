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
add_library(lac_math STATIC
    src/math/LacMatrix.cpp
    src/math/LacEngine.cpp
    src/math/LacException.cpp
)
add_library(lac_parser STATIC
    src/parser/Lexer.cpp
    src/parser/Parser.cpp
)
add_library(lac_cli STATIC
    src/cli/cli.cpp
)
target_include_directories(lac_math PUBLIC ${CMAKE_SOURCE_DIR}/include/math)
target_include_directories(lac_parser PUBLIC ${CMAKE_SOURCE_DIR}/include/parser)
target_include_directories(lac_cli PUBLIC ${CMAKE_SOURCE_DIR}/include/cli)
target_link_libraries(lac_math PUBLIC Eigen3::Eigen)
target_link_libraries(lac_parser PUBLIC lac_math Eigen3::Eigen)
target_link_libraries(lac_cli PUBLIC lac_math lac_parser)

add_executable(lac_cli_app src/cli/main.cpp)
target_link_libraries(lac_cli_app PRIVATE lac_math lac_parser lac_cli)

# 4. 单元测试目标（使用 GoogleTest + CTest）
if(BUILD_TESTS)
    # 数学库测试
    add_executable(lac_math_tests 
        tests/test_math/test_eigen.cpp
        tests/test_math/test_det.cpp
    )
    target_link_libraries(lac_math_tests PRIVATE lac_math GTest::gtest_main)
    
    # Parser库测试（新增）
    add_executable(lac_parser_tests  
        tests/test_parser/test_lexer.cpp
        tests/test_parser/test_parser.cpp
        tests/test_parser/test_eval_more.cpp
    )
    target_link_libraries(lac_parser_tests PRIVATE lac_parser lac_math GTest::gtest_main)
    
    include(GoogleTest)
    gtest_discover_tests(lac_math_tests)
    gtest_discover_tests(lac_parser_tests)
endif()

# 5. 根据系统进行微调（跨平台必备）
if(MSVC)
        # Windows 环境下特别处理乱码、内存检查等
        add_compile_options(/utf-8 /W4)
else()
        # Linux/Unix 环境下处理符号链接和警告
        add_compile_options(-Wall -Wextra)
endif()