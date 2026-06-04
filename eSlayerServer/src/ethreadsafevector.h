#ifndef ETHREADSAFEVECTOR_H
#define ETHREADSAFEVECTOR_H

#include <vector>
#include <mutex>
#include <shared_mutex>
#include <optional>

template<typename T>
class eThreadSafeVector {
public:
    eThreadSafeVector() = default;

    // Add an element
    void push_back(const T& value) {
        std::unique_lock lock(mutex_);
        data_.push_back(value);
    }

    void push_back(T&& value) {
        std::unique_lock lock(mutex_);
        data_.push_back(std::move(value));
    }

    // Remove last element
    bool pop_back() {
        std::unique_lock lock(mutex_);
        if (data_.empty()) {
            return false;
        }
        data_.pop_back();
        return true;
    }

    // Get a copy of an element
    std::optional<T> at(std::size_t index) const {
        std::shared_lock lock(mutex_);
        if (index >= data_.size()) {
            return std::nullopt;
        }
        return data_[index];
    }

    // Current size
    std::size_t size() const {
        std::shared_lock lock(mutex_);
        return data_.size();
    }

    bool empty() const {
        std::shared_lock lock(mutex_);
        return data_.empty();
    }

    // Execute arbitrary code while holding the lock
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
    std::vector<T> data_;
};

#endif // ETHREADSAFEVECTOR_H
