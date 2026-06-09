#ifndef ETHREADSAFE_H
#define ETHREADSAFE_H

#include <mutex>
#include <shared_mutex>

template<typename T>
class eThreadSafe {
public:
    template<typename Func>
    decltype(auto) with_lock(Func&& func) {
        std::unique_lock lock(mutex_);
        return std::forward<Func>(func)(data_);
    }

    template<typename Func>
    decltype(auto) with_lock(Func&& func) const {
        std::shared_lock lock(mutex_);
        return std::forward<Func>(func)(data_);
    }
private:
    mutable std::shared_mutex mutex_;
    T data_;
};

#endif // ETHREADSAFE_H
