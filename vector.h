#ifndef VECTOR__VECTOR_H_
#define VECTOR__VECTOR_H_

#include <cstdio>
#include <algorithm>
#include <cassert>

template<typename T>
struct vector {
    typedef T *iterator;
    typedef T const *const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const &);                  // O(N) strong
    vector &operator=(vector const &other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T &operator[](size_t i);                // O(1) nothrow
    T const &operator[](size_t i) const;    // O(1) nothrow

    T *data();                              // O(1) nothrow
    T const *data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T &front();                             // O(1) nothrow
    T const &front() const;                 // O(1) nothrow

    T &back();                              // O(1) nothrow
    T const &back() const;                  // O(1) nothrow
    void push_back(T const &);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector &);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    iterator insert(const_iterator pos, T const &); // O(N) weak

    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(const_iterator first, const_iterator last); // O(N) weak

 private:
    T *data_;
    size_t size_;
    size_t capacity_;

    static void remove_elements(T *data_container, size_t starting_point);
    void ensure_capacity();
    static void copy_elements(T *from, T *to, const size_t upper_limit);
    static T *new_buffer(size_t size);
};

template<typename T>
vector<T>::vector() : data_(nullptr), size_(0), capacity_(0) {}

template<typename T>
vector<T>::vector(vector const &that) {
    data_ = new_buffer(that.size_);
    copy_elements(that.data_, data_, that.size_);
    size_ = that.size_;
    capacity_ = size_;
}

template<typename T>
vector<T> &vector<T>::operator=(vector const &that) {
    vector tmp(that);
    swap(tmp);
    return *this;
}

template<typename T>
vector<T>::~vector() {
    clear();
    operator delete(data_);
}

template<typename T>
T &vector<T>::operator[](size_t i) {
    return data_[i];
}

template<typename T>
T const &vector<T>::operator[](size_t i) const {
    return data_[i];
}

template<typename T>
typename vector<T>::iterator vector<T>::data() {
    return data_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::data() const {
    return data_;
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
T &vector<T>::front() {
    return data_[0];
}

template<typename T>
T const &vector<T>::front() const {
    return data_[0];
}

template<typename T>
T &vector<T>::back() {
    return data_[size_ - 1];
}

template<typename T>
T const &vector<T>::back() const {
    return data_[size_ - 1];
}

template<typename T>
void vector<T>::pop_back() {
    data_[--size_].~T();
}

template<typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}
template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::reserve(size_t s) {
    if (s > capacity_) {
        T *data_new = new_buffer(s);
        copy_elements(data_, data_new, size_);
        remove_elements(data_, size_);
        operator delete(data_);
        data_ = data_new;
        capacity_ = s;
    }
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (size_ == capacity_) return;
    vector<T> tmp(*this);
    swap(tmp);
}

template<typename T>
void vector<T>::clear() {
    remove_elements(data_, size_);
    size_ = 0;
}

template<typename T>
void vector<T>::swap(vector &that) {
    std::swap(data_, that.data_);
    std::swap(capacity_, that.capacity_);
    std::swap(size_, that.size_);
}

template<typename T>
typename vector<T>::iterator vector<T>::begin() {
    return data_;
}
template<typename T>
typename vector<T>::iterator vector<T>::end() {
    return data_ + size_;
}
template<typename T>
typename vector<T>::const_iterator vector<T>::begin() const {
    return data_;
}
template<typename T>
typename vector<T>::const_iterator vector<T>::end() const {
    return data_ + size_;
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector::const_iterator pos, const T &element) {
    ptrdiff_t index = pos - data_;

    push_back(element);

    for (size_t i = size_ - 1; i > index; i--) {
        std::swap(data_[i], data_[i - 1]);
    }

    return data_ + index;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator first, vector::const_iterator last) {
    ptrdiff_t delta = last - first;
    ptrdiff_t start = first - data_;

    if (delta > 0) {
        for (size_t i = start; i < size_ - delta; i++) {
            data_[i] = data_[i + delta];
        }
        for (size_t i = 0; i < delta; i++) {
            pop_back();
        }
    }

    return start + data_;
}

template<typename T>
void vector<T>::push_back(const T &element) {
    if (size_ == capacity_) {
        const T element_copy = element;
        ensure_capacity();
        new(data_ + size_) T(element_copy);
    } else {
        new(data_ + size_) T(element);
    }
    ++size_;
}

template<typename T>
void vector<T>::remove_elements(T *data_container, size_t starting_point) {
    while (starting_point > 0) {
        data_container[--starting_point].~T();
    }
}
template<typename T>
void vector<T>::ensure_capacity() {
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : 2 * capacity_);
    }
}
template<typename T>
void vector<T>::copy_elements(T *from, T *to, const size_t upper_limit) {
    size_t loop = 0;
    try {
        for (; loop < upper_limit; loop++) {
            new(to + loop) T(from[loop]);
        }
    }
    catch (...) {
        remove_elements(to, loop);
        operator delete(to);
        throw;
    }
}
template<typename T>
T *vector<T>::new_buffer(size_t size) {
    return size ? static_cast<T *>(operator new(size * sizeof(T))) : nullptr;
}

#endif //VECTOR__VECTOR_H_
