#ifndef UTILS_THREADSAFERIGNBUFFER_H
#define UTILS_THREADSAFERIGNBUFFER_H

#include <mutex>
#include <span>
#include <vector>

namespace utils {

/**
 * @brief A Simple thread-safe ring buffer implementation
 *
 * This class provides a fixed-size, thread-safe circular buffer which starts overwriting the oldest entries when full.
 * The thread safety is implemented with locking and does not provide any lock-free or wait-free functionality.
 * Supports single and bulk push/pop/peek operations. The buffer allocates its underlying buffer in heap so that it's
 * capacity is settable at runtime.
 *
 * @tparam T Type of element stored in the buffer
 */
template <typename T>
class ThreadSafeRingBuffer {
public:
    /**
     * @brief Construct a ring buffer with the given capacity.
     * @param capacity The maximum number of elements the buffer can hold before overwriting starts.
     */
    explicit ThreadSafeRingBuffer(std::size_t capacity) : capacity_(capacity), buffer_(new T[capacity]) {
        static_assert(!std::is_pointer_v<T>, "Pointers not supported");
    }
    /**
     * @brief Destructor. Frees the dynamically allocated buffer.
     */
    ~ThreadSafeRingBuffer() { delete[] buffer_; }
    // --------------------------------------------------------------------------------------------------
    /**
     * @brief Push a single item into the buffer.
     *
     * If the buffer is full, the oldest item is overwritten.
     *
     * @param item The item to push.
     */
    void push(const T& item);
    /**
     * @brief Push multiple items into the buffer from a span.
     *
     * Items are pushed in order. If the buffer overflows, older items are overwritten.
     *
     * @param items The span of items to push.
     */
    void push(std::span<T> items);
    /**
     * @brief Push multiple items into the buffer from a vector.
     *
     * Items are pushed in order. If the buffer overflows, older items are overwritten.
     *
     * @param items The vector of items to push.
     */
    void push(std::vector<T> items);
    // --------------------------------------------------------------------------------------------------
    /**
     * @brief Pop a single item from the buffer.
     *
     * @param out Reference where the popped item will be stored.
     * @return `true` if an item was successfully popped, `false` if the buffer was empty.
     */
    bool pop(T& out);
    /**
     * @brief Pop multiple items into a span.
     *
     * Removes up to `out.size()` elements from the buffer and writes them into `out`.
     *
     * @param out The span to store popped items.
     * @return A subspan containing the actual number of elements popped.
     */
    std::span<T> pop(std::span<T> out);
    /**
     * @brief Pop up to `count` items and return them in a vector.
     *
     * @param count The number of items to pop.
     * @return A vector containing up to `count` popped items (fewer if the buffer has less data).
     */
    std::vector<T> pop(std::size_t count);
    // --------------------------------------------------------------------------------------------------
    /**
     * @brief Peek the next (oldest) item without removing it.
     *
     * @param out Reference where the peeked item will be written.
     * @return `true` if an item was available, `false` if the buffer was empty.
     */
    bool peek(T& out) const;
    /**
     * @brief Peek multiple items into a span without removing them.
     *
     * Reads up to `out.size()` items starting from the oldest element,
     * but does not modify the buffer.
     *
     * @param out The span to store peeked items.
     * @return A subspan containing the number of elements actually peeked.
     */
    std::span<T> peek(std::span<T> out);
    /**
     * @brief Peek up to `count` items without removing them.
     *
     * @param count The number of items to peek.
     * @return A vector containing up to `count` peeked items.
     */
    std::vector<T> peek(std::size_t count);
    // --------------------------------------------------------------------------------------------------
    /**
     * @brief Get the number of elements currently in the buffer.
     * @return The number of valid elements in the buffer.
     */
    std::size_t size() const;
    /**
     * @brief Check if the buffer is empty.
     * @return `true` if the buffer contains no items, `false` otherwise.
     */
    bool isEmpty() const;
    /**
     * @brief Check if the buffer is full.
     * @return `true` if the buffer is full, `false` otherwise.
     */
    bool isFull() const;
    /**
     * @brief Get the buffer's total capacity.
     * @return The maximum number of elements the buffer can store.
     */
    std::size_t capacity() const;

private:
    const std::size_t  capacity_;
    T*                 buffer_;
    std::size_t        head_ = 0;
    std::size_t        tail_ = 0;
    mutable std::mutex mutex_;

    /// The non-locking interface for public interface functions to use

    void pushImpl(const T& item) {
        buffer_[head_] = item;
        head_          = (head_ + 1) % capacity_;

        // If full, move tail forward (overwrite oldest)
        if (head_ == tail_) {
            tail_ = (tail_ + 1) % capacity_;
        }
    }

    bool popImpl(T& out) {
        if (isEmptyImpl()) return false;

        out = buffer_[tail_];
        tail_++;
        // Wrap around in the end
        if (tail_ == capacity_) {
            tail_ = 0;
        }

        return true;
    }

    std::size_t sizeImpl() const {
        if (head_ >= tail_) return head_ - tail_;

        return capacity_ - tail_ + head_;
    }

    bool isEmptyImpl() const { return sizeImpl() == 0; }
};

// ------------------------------ Template implementations ------------------------------
template <typename T>
void ThreadSafeRingBuffer<T>::push(const T& item) {
    std::lock_guard lock(mutex_);
    pushImpl(item);
}

template <typename T>
void ThreadSafeRingBuffer<T>::push(std::span<T> items) {
    std::lock_guard lock(mutex_);

    for (const auto& item : items) {
        pushImpl(item);
    }
}

template <typename T>
void ThreadSafeRingBuffer<T>::push(std::vector<T> items) {
    std::lock_guard lock(mutex_);

    for (const auto& item : items) {
        pushImpl(item);
    }
}

template <typename T>
bool ThreadSafeRingBuffer<T>::pop(T& out) {
    std::lock_guard lock(mutex_);

    return popImpl(out);
}
template <typename T>
std::span<T> ThreadSafeRingBuffer<T>::pop(std::span<T> out) {
    std::lock_guard lock(mutex_);

    std::size_t idx = 0;
    for (; idx < out.size(); idx++) {
        if (!popImpl(out[idx])) {
            break;
        }
    }

    return out.subspan(0, idx);
}

template <typename T>
std::vector<T> ThreadSafeRingBuffer<T>::pop(std::size_t count) {
    std::lock_guard lock(mutex_);

    std::vector<T> vec;
    vec.reserve(count);
    for (std::size_t i = 0; i < count; i++) {
        T item;
        if (!popImpl(item)) break;
        vec.push_back(item);
    }
    return vec;
}

template <typename T>
bool ThreadSafeRingBuffer<T>::peek(T& out) const {
    std::lock_guard lock(mutex_);

    if (isEmpty()) return false;
    out = buffer_[tail_];
    return true;
}

template <typename T>
std::span<T> ThreadSafeRingBuffer<T>::peek(std::span<T> out) {
    std::lock_guard lock(mutex_);

    std::size_t idx       = 0;
    size_t      temp_tail = tail_;

    for (; idx < out.size(); idx++) {
        out[idx]  = buffer_[temp_tail];
        temp_tail = (temp_tail + 1) % capacity_;

        // There was less data to peek at than out buffer would have fit
        if (temp_tail == head_) {
            break;
        }
    }
    return out.subspan(0, idx);
}

template <typename T>
std::vector<T> ThreadSafeRingBuffer<T>::peek(std::size_t count) {
    std::lock_guard lock(mutex_);

    std::vector<T> vec;
    vec.reserve(count);
    size_t temp_tail = tail_;

    for (std::size_t i = 0; i < count; i++) {
        vec.push_back(buffer_[temp_tail]);
        temp_tail = (temp_tail + 1) % capacity_;

        // There was less data to peek at than requested
        if (temp_tail == head_) {
            break;
        }
    }
    return vec;
}

template <typename T>
std::size_t ThreadSafeRingBuffer<T>::size() const {
    std::lock_guard lock(mutex_);

    return sizeImpl();
}

template <typename T>
bool ThreadSafeRingBuffer<T>::isEmpty() const {
    std::lock_guard lock(mutex_);

    return isEmptyImpl();
}

template <typename T>
bool ThreadSafeRingBuffer<T>::isFull() const {
    std::lock_guard lock(mutex_);

    return sizeImpl() == capacity_;
}

template <typename T>
std::size_t ThreadSafeRingBuffer<T>::capacity() const {
    std::lock_guard lock(mutex_);

    return capacity_;
}

}  // namespace utils

#endif  // UTILS_THREADSAFERIGNBUFFER_H
