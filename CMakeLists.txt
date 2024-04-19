cmake_minimum_required(VERSION 2.8)
# 定义项目名称为WebServer，并使用C++编译器
project(Webserver CXX)

# 如果未定义构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")# 设置构建类型为Debug
endif()

set(CXX_FLAGS# 定义C++编译器的选项
    -g# 生成调试信息
    -Wall# 启用所有警告
    -std=c++11# 使用C++11标准
    -D_PTHREADS # 定义_PTHREADS宏
    -Wno-unused-parameter# 禁用未使用参数的警告
)


set(CMAKE_CXX_COMPILER "g++")# 设置C++编译器为g++
set(CMAKE_CXX_FLAGS_DEBUG "-O0")# 设置Debug模式下的编译选项为不优化
set(CMAKE_CXX_FLAGS_RELEASE "-O0") # 设置Release模式下的编译选项为不优化

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")# 将CXX_FLAGS中的分号替换为空格，生成C++编译器的选项


string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)# 将构建类型转换为大写
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}}) # 输出C++编译器的选项

add_subdirectory(Main)# 添加子目录WebServer到构建过程中