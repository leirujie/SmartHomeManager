#ifndef THREADPOOLMANAGER_H
#define THREADPOOLMANAGER_H

#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>

class ThreadPool
{
public:
    ThreadPool(size_t numThreads);
    void enqueueTask(std::function<void()> task);
    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

#endif //THREADPOOLMANAGER_H