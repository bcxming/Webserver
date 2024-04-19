set(SRCS
    Channel.cpp
    Epoll.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    EventLoopThreadPool.cpp
    HttpData.cpp
    Main.cpp
    Util.cpp
    Server.cpp
    Timer.cpp
)
include_directories(${PROJECT_SOURCE_DIR}/Log)

add_subdirectory(Log)
add_executable(Webserver ${SRCS})
target_link_libraries(Webserver libserver_base)