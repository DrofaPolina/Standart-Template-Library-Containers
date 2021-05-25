#include <utility>

struct BadOptionalAccess {
};

template <typename T>
class Optional {
private:
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) unsigned char data[sizeof(T)];
    bool defined = false;

public:
    Optional() = default;

    Optional(const T& elem) : defined(true) {
        new (data) T(elem);
    }

    Optional(T && elem) : defined(true) {
        new (data) T(std::move(elem));
    }

    Optional(const Optional& other) : defined(other.defined) {
        if (other.defined) {
            new (data) T(*other);
        }
    }

    Optional& operator=(const Optional& other) {
        if (this != &other) {
            if (other.has_value()) {
                if (!has_value()) {
                    new (data) T(*other);
                } else {
                    **this = *other;
                }
            } else {
                reset();
            }
            defined = other.defined;
        }
        return *this;
    }

    Optional& operator=(const T& elem) {
        if (defined) {
            **this = elem;
        } else {
            // defined = true;
            // new (data) T(elem);
            *this(elem);
        }
        return *this;
    }

    Optional& operator=(T&& elem) {
        if (defined) {
            **this = std::move(elem);
        } else {
            defined = true;
            new (data) T(std::move(elem));
            // Optional(elem);
        }
        return *this;
    }

    bool has_value() const {
        return defined;
    }

    T& operator*() {
        return *(*this).operator->();
    }

    const T& operator*() const {
        return *(*this).operator->();
    }

    T* operator->() {
        return reinterpret_cast<T*> (data);
    }

    const T* operator->() const {
        return reinterpret_cast<const T*> (data);
    }

    T& value() {
        if (has_value()) {
            return **this;
        }
        throw BadOptionalAccess();
    }

    const T& value() const {
        if (defined) {
            return **this;
        }
        throw BadOptionalAccess();
    }

    void reset() {
        if (defined) {
            (*this)->~T();
        }
        defined = false;
    }

    ~Optional() {
        reset();
    }
};