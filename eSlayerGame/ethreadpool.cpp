#include "ethreadpool.h"

eThreadPool::eThreadPool(std::size_t threadCount) {
    if(threadCount == 0) threadCount = 1;
    for(std::size_t i = 0; i < threadCount; i++) {
        mWorkers.emplace_back([this] {
            workerLoop();
        });
    }
}

eThreadPool::~eThreadPool() {
    shutdown();
}

void eThreadPool::submit(const eFunc& work, const eFinish& finish) {
    {
        std::lock_guard lock(mWorkMutex);

        if(mStopping) {
            eRuntimeThrow("ThreadPool is shutting down");
        }

        mWorkQueue.emplace([this, work, finish] {
            std::exception_ptr error;

            try {
                if(work) work();
            } catch (...) {
                error = std::current_exception();
            }

            enqueueFinish([this, finish, error]() {
                if(finish) finish(error);
                mNTasks--;
            });
        });
        mNTasks++;
    }

    mWorkCondition.notify_one();
}

void eThreadPool::update() {
    std::queue<eFunc> localQueue;

    {
        std::lock_guard lock(mFinishMutex);
        std::swap(localQueue, mFinishQueue);
    }

    while(!localQueue.empty()) {
        const auto callback = std::move(localQueue.front());
        localQueue.pop();

        callback();
    }
}

void eThreadPool::shutdown() {
    {
        std::lock_guard lock(mWorkMutex);

        if(mStopping) return;
        mStopping = true;
    }

    mWorkCondition.notify_all();

    for(auto& worker : mWorkers) {
        if(worker.joinable()) {
            worker.join();
        }
    }

    mWorkers.clear();
}

void eThreadPool::workerLoop() {
    while (true) {
        eFunc job;

        {
            std::unique_lock lock(mWorkMutex);

            mWorkCondition.wait(lock, [this] {
                return mStopping || !mWorkQueue.empty();
            });

            if(mStopping && mWorkQueue.empty()) {
                return;
            }

            job = std::move(mWorkQueue.front());
            mWorkQueue.pop();
        }

        job();
    }
}

void eThreadPool::enqueueFinish(const eFunc& callback) {
    std::lock_guard lock(mFinishMutex);
    mFinishQueue.push(callback);
}

void eThreadPool::wait() {
    while(mNTasks > 0) {
        update();

        if(mNTasks > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
