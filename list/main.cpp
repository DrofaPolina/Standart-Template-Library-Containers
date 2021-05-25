#include <iostream>
#include <map>

template <typename T>
class List {
private:
    struct Element {
        T* value = nullptr;
        Element* prev = nullptr;
        Element* next = nullptr;

        Element() = default;

        Element(const T& _value) : value(new T(_value)) {}

        ~Element() {
            delete value;
        }
    };

    size_t size_ = 0;
    Element* first = nullptr;
    Element* last = nullptr;

public:
    class iterator {
        Element* ptr = nullptr;

    public:
        iterator(Element* _ptr):
                ptr(_ptr) {
        }

        iterator(const iterator& other):
                ptr(other.ptr) {
        }

        iterator& operator=(const iterator& other) {
            ptr = other.ptr;
            return *this;
        }

        T& operator*() {
            return *ptr->value;
        }

        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }

        iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        iterator& operator--() {
            ptr = ptr->prev;
            return *this;
        }

        iterator operator++(int) {
            auto old(*this);
            ptr = ptr->next;
            return old;
        }

        iterator operator--(int) {
            auto old(*this);
            ptr = ptr->prev;
            return old;
        }
    };

    List() : last(new Element()) {
        first = last;
    }

    List& operator=(const List& other) {
        while (size_ > 0) {
            pop_front();
        }
        for (Element* ptr = other.first; ptr != other.last; ptr = ptr->next) {
            push_back(*ptr->value);
        }
        return *this;
    }

    List(const List& other): List() {
        for (Element* ptr = other.first; ptr != other.last; ptr = ptr->next) {
            push_back(*ptr->value);
        }
    }

    void push_back(const T& el) {
        Element* tmp = new Element();
        if (last->value == nullptr) {
            last->value = new T(el);
        } else {
            *last->value = el;
        }
        last->next = tmp;
        tmp->prev = last;
        last = tmp;
        ++size_;
    }

    void push_front(const T& el) {
        Element* tmp = new Element(el);
        first->prev = tmp;
        tmp->next = first;
        first = tmp;
        ++size_;
    }

    void pop_back() {
        Element* old_last = last;
        last = last->prev;
        last->next = nullptr;
        delete last->value;
        last->value = nullptr;
        delete old_last;
        --size_;
    }

    void pop_front() {
        Element* old = first;
        first = first->next;
        first->prev = nullptr;
        delete old;
        --size_;
    }

    [[nodiscard]] size_t size() {
        return size_;
    }

    iterator begin() {
        return iterator(first);
    }

    iterator end() {
        return iterator(last);
    }

    ~List() {
        size_ = 0;
        while (last != nullptr) {
            Element* prev = last->prev;
            delete last;
            last = prev;
        }
        first = nullptr;
        last = nullptr;
    }
};