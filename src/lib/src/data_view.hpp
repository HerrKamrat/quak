#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include "buffer.hpp"

template <class T>
class DataView {
  public:
    //   Member types
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr size_type npos = size_type(-1);
    static constexpr size_type byte_size = sizeof(T);

    // Constructors
    constexpr DataView():DataView((pointer)nullptr, 0){};
    // DataView(pointer data, size_type size);
    constexpr DataView(pointer data, size_type size) : m_data(data), m_size(size){};

    constexpr DataView(char* data, size_type size)
        : DataView(reinterpret_cast<pointer>(data), size / byte_size){};

    DataView(Buffer& buffer) : DataView(buffer.data(), buffer.size()){};
    // DataView& operator=(DataView&&);

    // DataView(const DataView&) = delete;
    // DataView& operator=(const DataView&) = delete;

    // Element access
    reference at(size_type index) {
        if (index >= size()) {
            throw new std::exception();
        }
        return data()[index];
    };

    const_reference at(size_type index) const {
        if (index >= size()) {
            throw new std::exception();
        }
        return data()[index];
    };

    reference operator[](size_type index) {
        return data()[index];
    };

    const_reference operator[](size_type index) const {
        return data()[index];
    };
    ;

    pointer data() {
        return m_data;
    };

    const_pointer data() const {
        return m_data;
    };

    reference front() {
        return begin()[0];
    };

    const_reference front() const {
        return cbegin()[0];
    };
    ;

    reference back() {
        return end()[-1];
    };
    ;
    const_reference back() const {
        return cend()[-1];
    };

    // Iterators
    iterator begin() {
        return data();
    };
    const_iterator cbegin() const {
        return data();
    };

    iterator end() {
        return data() + size();
    };

    const_iterator cend() const {
        return data() + size();
    };

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    };
    ;
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    };

    reverse_iterator rend() {
        return reverse_iterator(begin());
    };

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    };

    // Capacity
    constexpr size_type size() const {
        return m_size;
    };
    constexpr bool empty() const {
        return size() == 0;
    };

  protected:
  private:
    size_type m_size;
    pointer m_data;
};

// template<class T>
// constexpr DataView<T>::DataView<T>() : m_size(0), m_data(nullptr){};
/*
template <class T>
constexpr DataView< T>::size_type DataView< T>::size() const {
    return m_size;
};

template <class T>
constexpr bool DataView<typename T>::empty() const {
    return size() == 0;
};
*/
using Uint8DataView = DataView<uint8_t>;
using Uint16DataView = DataView<uint16_t>;
using Uint32DataView = DataView<uint32_t>;
using Uint64DataView = DataView<uint64_t>;

using Int8DataView = DataView<int8_t>;
using Int16DataView = DataView<int16_t>;
using Int32DataView = DataView<int32_t>;
using Int64DataView = DataView<int64_t>;

static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

using Float32DataView = DataView<float>;
using Float64DataView = DataView<double>;
