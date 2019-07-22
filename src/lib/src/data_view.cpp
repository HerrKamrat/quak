#include "data_view.hpp"
#ifdef USE_DATA_VIEW_CPP
// Constructors

Buffer::Buffer(const_pointer data, size_type size) : Buffer(data, size, size){};

Buffer::Buffer(Buffer&& other) {
    *this = std::move(other);
};

Buffer& Buffer::operator=(Buffer&& other) {
    auto size = other.size();
    auto capacity = other.capacity();
    auto data = other.release();

    reset(std::move(data), size, capacity);
    return *this;
};

// Element access

Buffer::reference Buffer::at(size_type pos) {
    if (pos >= size()) {
        throw new std::exception();
    }
    return data()[pos];
};

Buffer::const_reference Buffer::at(size_type pos) const {
    if (pos >= size()) {
        throw new std::exception();
    }
    return data()[pos];
};

Buffer::reference Buffer::operator[](size_type index) {
    return data()[index];
};

Buffer::const_reference Buffer::operator[](size_type index) const {
    return data()[index];
};

Buffer::pointer Buffer::data() {
    return m_data.get();
};

Buffer::const_pointer Buffer::data() const {
    return m_data.get();
};

Buffer::reference Buffer::front() {
    return begin()[0];
};

Buffer::const_reference Buffer::front() const {
    return cbegin()[0];
};

Buffer::reference Buffer::back() {
    return end()[-1];
};

Buffer::const_reference Buffer::back() const {
    return cend()[-1];
};

Buffer::view_type Buffer::view() {
    return {data(), size()};
};

Buffer::const_view_type Buffer::view() const {
    return {data(), size()};
};


// Iterators

Buffer::iterator Buffer::begin() {
    return data();
};
Buffer::const_iterator Buffer::cbegin() const {
    return data();
};

Buffer::iterator Buffer::end() {
    return data() + size();
};
Buffer::const_iterator Buffer::cend() const {
    return data() + size();
};

Buffer::reverse_iterator Buffer::rbegin() {
    return reverse_iterator(end());
};
Buffer::const_reverse_iterator Buffer::crbegin() const {
    return const_reverse_iterator(cend());
};
Buffer::reverse_iterator Buffer::rend() {
    return reverse_iterator(begin());
};
Buffer::const_reverse_iterator Buffer::crend() const {
    return const_reverse_iterator(cbegin());
};

// Modifiers

void Buffer::clear() {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
};

void Buffer::resize(size_type size) {
    if (size > capacity()) {
        reserve(size);
    }
    m_size = size;
};

void Buffer::resize(size_type new_size, const_reference value) {
    if (new_size > size()) {
        if (new_size > capacity()) {
            reserve(new_size);
        }
        std::fill(data() + size(), data() + (new_size - size()), value);
    }
};
void Buffer::reserve(size_type count) {
    if (count > capacity()) {
        auto old = std::move(m_data);
        m_data = {allocate(count), default_deleter};
        m_capacity = count;

        if (m_size > 0) {
            memcpy(data(), old.get(), m_size);
        }
    }
};

void Buffer::swap(Buffer& other) {
    auto tmp = std::move(*this);
    *this = std::move(other);
    other = std::move(tmp);
}

void Buffer::append(const Buffer& src) {
    append(src.data(), src.size());
};

void Buffer::append(const_pointer data, size_type count) {
    const auto oldSize = size();
    resize(size() + count);
    memcpy(this->data() + oldSize, data, count);
};

void Buffer::fill(value_type value) {
    std::fill(begin(), end(), value);
};

Buffer::pointer Buffer::allocate(size_type count) {
    return count > 0 ? new value_type[count] : nullptr;
}

Buffer::unique_ptr Buffer::release() {
    m_size = 0;
    m_capacity = 0;
    return std::move(m_data);
}

void Buffer::reset(const_pointer new_data, size_type new_size, size_type new_capacity) {
    reserve(new_capacity);
    resize(new_size);
    memcpy(data(), new_data, new_size);
};

void Buffer::reset(unique_ptr new_data, size_type new_size, size_type new_capacity) {
    m_data = std::move(new_data);
    m_size = new_size;
    m_capacity = new_capacity;
};

#endif
