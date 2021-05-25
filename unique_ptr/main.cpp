#include <iostream>
#include <cstddef>
#include <memory>

template <typename T>
class UniquePtr {
private:
    T* ptr_ = nullptr;

public:
    UniquePtr() {
    }

    UniquePtr(T* ptr) {
        ptr_ = ptr;
    }

    UniquePtr(UniquePtr<T>&& other) {
        std::swap(ptr_, other.ptr_);
    }

    UniquePtr& operator=(UniquePtr&& other) {
        auto tmp = other.release();
        reset(tmp);
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        delete ptr_;
        ptr_ = nullptr;
        return *this;
    }

    void reset(T * ptr) {
        delete ptr_;
        ptr_ = ptr;
    }

    const T& operator*() const {
        return *ptr_;
    }

    T& operator*() {
        return *ptr_;
    }

    const T* operator->() const {
        return ptr_;
    }

    T* operator->() {
        return ptr_;
    }


    T* get() const {
        return ptr_;
    }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
    }

    operator bool() const {
        return(ptr_ != nullptr);
    }

    ~UniquePtr() {
        delete ptr_;
    }

    UniquePtr(const UniquePtr& other) = delete;

    UniquePtr& operator=(const UniquePtr& other) = delete;
};