#include <iostream>
#include <cstddef>
#include <utility>
#include <memory>


template <typename T>
struct RawMemory {
    T* buffer_ = nullptr;
    size_t capacity_ = 0;

    static T* Allocate(size_t n) {
        return static_cast<T*>(operator new (n * sizeof(T)));
    }

    static void Deallocate(T* buffer) {
        operator delete(buffer);
    }

    RawMemory() = default;

    RawMemory(size_t n) {
        buffer_ = Allocate(n);
        capacity_ = n;
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    RawMemory(const RawMemory&) = delete;

    RawMemory& operator = (const RawMemory&) = delete;

    RawMemory& operator = (RawMemory&& other) noexcept {
        Swap(other);
        return *this;
    }

    RawMemory(RawMemory&& other) noexcept {
        Swap(other);
    }

    T* operator + (size_t i) {
        return buffer_ + i;
    }

    const T* operator + (size_t i) const {
        return buffer_ + i;
    }

    T& operator[] (size_t i) {
        return buffer_[i];
    }

    const T& operator[] (size_t i) const {
        return buffer_[i];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }
};

template <typename T>
class Vector {
private:
    size_t size_ = 0;

    RawMemory<T> buffer;

    static void Construct(void* buffer) {
        new (buffer) T();
    }

    static void Construct(void* buffer, const T& elem) {
        new (buffer) T(elem);
    }

    static void Construct(void* buffer, T && elem) {
        new (buffer) T(std::move(elem));
    }

    static void Destroy(T* buffer) {
        buffer->~T();
    }

public:
    Vector() = default;

    explicit Vector(size_t size) : buffer(size) {
        std::uninitialized_value_construct_n(buffer.buffer_, size);
        size_ = size;
    }

    Vector(const Vector& other) : buffer(other.size_) {
        try {
            for (size_t i = 0; i < other.size_; ++i) {
                Construct(buffer + i, other.buffer[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < size_; ++j) {
                Destroy(buffer + j);
            }
            throw;
        }
        size_ = other.size_;
    }

    Vector& operator=(const Vector& other) {
        if (other.size_ > buffer.capacity_) {
            Vector tmp(other);
            swap(tmp);
        } else {
            for (size_t i = 0; i < size_ && i < other.size_; ++i) {
                buffer[i] = other[i];
            }
            if (size_ < other.size_) {
                std::uninitialized_copy_n(
                        other.buffer.buffer_ + size_,
                        other.size_ - size_,
                        buffer.buffer_);
            } else if (size_ > other.size_) {
                std::destroy_n(
                        buffer.buffer_ + other.size_, size_ - other.size_);
            }
            size_ = other.size_;
        }
        return *this;
    }

    Vector& operator=(Vector && other) noexcept {
        swap(other);
        return *this;
    }

    Vector(Vector && other) noexcept {
        swap(other);
    }

    void swap(Vector& other) noexcept {
        buffer.Swap(other.buffer);
        std::swap(size_, other.size_);
    };

    void reserve(size_t n) {
        if (buffer.capacity_ < n) {
            // auto buf2 = Allocate(n);
            RawMemory<T> buf2(n);
            std::uninitialized_move_n(buffer.buffer_, size_, buf2.buffer_);
            std::destroy_n(buffer.buffer_, size_);
            buffer = std::move(buf2);
        }
    }

    void push_back(const T& el) {
        if (size_ == buffer.capacity_) {
            reserve(size_ == 0 ? 1 : size_ * 2);
        }
        new (buffer + size_) T (el);
        ++size_;
    }

    void push_back(T&& el) {
        if (size_ == buffer.capacity_) {
            reserve(size_ == 0 ? 1 : size_ * 2);
        }
        new (buffer + size_) T (std::move(el));
        ++size_;
    }

    void pop_back() {
        std::destroy_at(buffer + size_ - 1);
        --size_;
    }

    void resize(size_t n) {
        reserve(n);
        if (size_ < n) {
            std::uninitialized_value_construct_n(
                    buffer + size_, n - size_);
        } else if (size_ > n) {
            std::destroy_n(buffer + n, size_ - n);
        }
        size_ = n;
    }

    void clear() {
        while (size_ > 0) {
            pop_back();
        }
        // resize(0);
    }

    [[nodiscard]] size_t capacity() const {
        return buffer.capacity_;
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }

    T& operator[] (size_t i) {
        return buffer[i];
    }

    const T& operator[] (size_t i) const {
        return buffer[i];
    }

    [[nodiscard]] const T* begin() const {
        return buffer.buffer_;
    }

    [[nodiscard]] const T* end() const {
        return buffer + size_;
    }

    [[nodiscard]] T* begin() {
        return buffer.buffer_;
    }

    [[nodiscard]] T* end() {
        return buffer + size_;
    }

    ~Vector() {
        std::destroy_n(buffer.buffer_, size_);
    }
};