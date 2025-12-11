#ifndef UTILS_THREADSAFESINGLETONBASE_H
#define UTILS_THREADSAFESINGLETONBASE_H

#include <mutex>

namespace utils {
/**
 * Base class for singletons using CRTP
 * Provides both locked and unlocked access to the singleton instance
 * @tparam T Derived class type
 */
template <typename T>
class ThreadSafeSingletonBase {
public:
    ThreadSafeSingletonBase(const ThreadSafeSingletonBase&)            = delete;
    ThreadSafeSingletonBase& operator=(const ThreadSafeSingletonBase&) = delete;
    ThreadSafeSingletonBase(ThreadSafeSingletonBase&&)                 = delete;
    ThreadSafeSingletonBase& operator=(ThreadSafeSingletonBase&&)      = delete;

    /**
     * Proxy class that automatically locks on every method call
     * Lock is held for the duration of the method call and released immediately after
     */
    class LockedProxy {
    public:
        LockedProxy(T& instance, std::mutex& mutex) : instance_(instance), lock_(mutex) {}

        T* operator->() { return &instance_; }

    private:
        T&                          instance_;
        std::lock_guard<std::mutex> lock_;
    };

    /**
     * Get locked access to the singleton instance
     * Each method call will automatically acquire and release the lock
     *
     * @return LockedProxy that provides thread-safe access
     *
     * @example
     * MyClass::getInstance()->doSomething();  // Locked during doSomething()
     */
    static LockedProxy getInstance() { return LockedProxy(getInstanceUnsafe(), getMutex()); }

    /**
     * Get direct (unlocked) access to the singleton instance
     *
     * WARNING: This bypasses thread safety! Use only when:
     * - You know you're in a single-threaded context
     * - The method you're calling is internally thread-safe
     * - You're manually managing locks
     *
     * @return Reference to the singleton instance
     *
     * @example
     * MyClass::getInstanceUnsafe().doSomething();  // No automatic locking!
     */
    static T& getInstanceUnsafe() {
        static T instance;
        return instance;
    }

    /**
     * Get the mutex for manual locking if needed
     * Useful for locking multiple operations together
     *
     * @return Reference to the singleton's mutex
     *
     * @example
     * std::lock_guard lock(MyClass::getMutex());
     * auto& instance = MyClass::getInstanceUnsafe();
     * instance.operation1();
     * instance.operation2();
     */
    static std::mutex& getMutex() {
        static std::mutex mutex;
        return mutex;
    }

protected:
    ThreadSafeSingletonBase()          = default;
    virtual ~ThreadSafeSingletonBase() = default;
};

}  // namespace utils

#endif  // UTILS_THREADSAFESINGLETONBASE_H
