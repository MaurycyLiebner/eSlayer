#ifndef ETHREADPOOL_H
#define ETHREADPOOL_H

#include <eSlayerHelpers/eexceptions.h>

#include <condition_variable>
#include <cstring>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

using eFunc = std::function<void()>;
using eFinish = std::function<void(const std::exception_ptr&)>;

class eThreadPool {
public:
    eThreadPool(std::size_t threadCount =
                std::thread::hardware_concurrency());
    ~eThreadPool();

    void submit(const eFunc& work, const eFinish& finish);

    void update();
    void shutdown();
private:
    void workerLoop();
    void enqueueFinish(const eFunc& callback);

    std::mutex mWorkMutex;
    std::condition_variable mWorkCondition;
    std::queue<eFunc> mWorkQueue;

    std::mutex mFinishMutex;
    std::queue<eFunc> mFinishQueue;

    std::vector<std::thread> mWorkers;

    bool mStopping = false;
};

#endif // ETHREADPOOL_H
