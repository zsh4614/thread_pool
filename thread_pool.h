#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <future>
#include <functional>


class ThreadPool {
public:
    using Ptr = std::shared_ptr<ThreadPool>;

    ThreadPool(int idl_thr_num) : run_(true), idl_thr_num_(idl_thr_num) {
        for (int i = 0; i < idl_thr_num_; ++i) {
            pool_.emplace_back([this]{  // 工作线程函数
                while (run_) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock{ mutex_ };
                        cv_.wait(lock, [this]{ return !run_ || !task_.empty(); });  // 等待直到任务队列有任务或者线程池停止工作
                        if (!run_ && task_.empty()) {
                            return;
                        }
                        task = std::move(task_.front());  // 从任务队列首取出一个任务
                        task_.pop();
                    }
                    idl_thr_num_--;
                    task();  // 执行任务
                    idl_thr_num_++;
                }
            });
        }
    }

    ~ThreadPool() {
        run_ = false;
        cv_.notify_all();
        for (std::thread& thread : pool_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

public:
    int idlCount() {
        return idl_thr_num_;
    }

    template <class F, class... Args>
    auto commit(F&& f, Args&&... args)->std::future<decltype(f(args...))> {
        if (!run_) {
            throw std::runtime_error("commit on ThreadPool is stopped.");
        }
        using RetType = decltype(f(args...));  // 函数f的返回值类型
        auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<RetType> future = task->get_future();
        {
            std::lock_guard<std::mutex> lock{mutex_ };
            task_.emplace([task](){ (*task)(); });  // 添加任务到任务队列
        }
        cv_.notify_one();  // 唤醒一个线程
        return future;
    }

private:
    using Task = std::function<void()>;
    std::vector<std::thread> pool_;             // 线程池
    std::queue<Task> task_;                     // 任务队列
    std::mutex mutex_;                          // 线程锁
    std::condition_variable cv_;                // 条件阻塞
    std::atomic<bool> run_;                     // 线程池是否执行标志
    std::atomic<int> idl_thr_num_;              // 空闲线程数量
};