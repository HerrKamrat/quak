#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <functional>

class Buffer {
  public:
    //   Member types
    using value_type = char;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

	using view_type = std::string_view;
    using const_view_type = const view_type;

    using deleter = void (*)(pointer);
    using unique_ptr = std::unique_ptr<value_type[], std::function<void(pointer)> >;

    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // using deleter = std::default_delete<value_type[]>;
    using deleter_type = unique_ptr::deleter_type;


    static constexpr size_type npos = size_type(-1);

    // Constructors
    constexpr Buffer();
    explicit Buffer(size_type size);
    Buffer(size_type size, size_type capacity);
    Buffer(const_pointer data, size_type size);
    Buffer(const_pointer data, size_type size, size_type capacity);
    Buffer(unique_ptr data, size_type size, size_type capacity);
    Buffer(Buffer&&);
    Buffer& operator=(Buffer&&);

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // Element access
    reference at(size_type);
    const_reference at(size_type) const;

    reference operator[](size_type);
    const_reference operator[](size_type) const;

    pointer data();
    const_pointer data() const;

    reference front();
    const_reference front() const;

    reference back();
    const_reference back() const;

	view_type view();
    const_view_type view() const;

    // Iterators
    iterator begin();
    const_iterator cbegin() const;

    iterator end();
    const_iterator cend() const;

    reverse_iterator rbegin();
    const_reverse_iterator crbegin() const;
    reverse_iterator rend();
    const_reverse_iterator crend() const;

    // Capacity
    constexpr size_type size() const;
    constexpr size_type capacity() const;
    constexpr bool empty() const;

    // Modifiers
    void clear();
    void resize(size_type size);
    void resize(size_type size, const_reference value);
    void reserve(size_type new_cap);

    void swap(Buffer&);

    void append(const Buffer&);
    void append(const_pointer data, size_type size);
    void fill(value_type value = 0);

    unique_ptr release();
    void reset(const_pointer new_data, size_type new_size, size_type new_capacity);
    void reset(unique_ptr new_data, size_type new_size, size_type new_capacity);

	template<class deleter>
    void inline reset(pointer new_data,
                      size_type new_size,
                      size_type new_capacity,
                      deleter new_deleter) {
        release();
        m_size = new_size;
        m_capacity = new_capacity;
        m_data = {new_data, new_deleter};
    };

  protected:
    pointer allocate(size_type capacity);

  private:
    size_type m_size;
    size_type m_capacity;
    unique_ptr m_data;
};

constexpr Buffer::Buffer() : m_size(0), m_capacity(0), m_data(nullptr){};

constexpr Buffer::size_type Buffer::size() const {
    return m_size;
};

constexpr Buffer::size_type Buffer::capacity() const {
    return m_capacity;
};

constexpr bool Buffer::empty() const {
    return size() == 0;
};
