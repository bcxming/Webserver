#include <getopt.h>
#include <string>
#include "EventLoop.h"
#include "Server.h"


int main(int argc, char* argv[]) {
  //argc：命令行参数的个数，即 main 函数的参数 argc。
  //argv：命令行参数的数组，即 main 函数的参数 argv。
    int threadNum = 4;
    int port = 80;
    int opt;
    
  //一个 C 语言风格的字符串常量
    const char* str = "t:l:p:";
  //t 是一个选项，表示 -t，它需要一个参数；
  //opt 将会是该选项字符的 ASCII 值，同时 getopt 会把参数值存储在全局变量 optarg 中；
    while ((opt = getopt(argc, argv, str)) != -1) {
        switch (opt) {
        case 't': {
            threadNum = atoi(optarg);
            break;
        }
        case 'p': {
            port = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }
// STL库在多线程上应用
//如果 _PTHREADS 没有被定义，就输出一条日志消息。
#ifndef _PTHREADS
    std::cout << "_PTHREADS is not defined !";
#endif
    //_PTHREADS 是一个宏定义，用于表示是否启用 POSIX 线程库（POSIX Threads，简称 pthreads）。
    EventLoop mainLoop;
    Server myHTTPServer(&mainLoop, threadNum, port);
    myHTTPServer.start();
    mainLoop.loop();
    return 0;
}
