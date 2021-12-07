#include "thread_pool.h"
#include <iostream>
#include <chrono>

std::mutex g_mutex;

void task(int i) {
    std::cout << "正在执行第 " << i << " 个任务" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    std::cout << "执行完成第 " << i << " 个任务" << std::endl;
}

int main(int argc, char** argv) {
    // todo:
    ThreadPool::Ptr thread_pool_ptr = std::make_shared<ThreadPool>(1);
    for (int i = 0; i < 1; ++i) {
        thread_pool_ptr->commit([i](){ task(i); });
    }
}
