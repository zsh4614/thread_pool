#include "thread_pool.h"
#include <iostream>
#include <chrono>

std::mutex g_mutex;

void task(int i) {
    printf("正在执行第 %d 个任务， 线程id为 %d\n", i, std::this_thread::get_id());
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    printf("执行完成第 %d 个任务， 线程id为 %d\n", i, std::this_thread::get_id());
}

int main(int argc, char** argv) {
    std::cout << "主线程id为 " << std::this_thread::get_id() << std::endl;
    ThreadPool::Ptr thread_pool_ptr = std::make_shared<ThreadPool>(3);
    for (int i = 0; i < 3; ++i) {
        thread_pool_ptr->commit([i](){task(i);});
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}
