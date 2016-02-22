//
// Created by Xingyuan Wang on 2/18/16.
//

#pragma once
#ifndef VECTOR_VECTOR_H
#define VECTOR_VECTOR_H

#include <cstdint>
#include <iostream>

namespace epl {
    template <typename T>
    class vector {
        static constexpr  uint64_t MIN_CAPACITY = 8;
    private:
        T* cap;
        T* len;
        uint64_t capacity;
        uint64_t length;
    public:
        vector(void) {
            cap = static_cast<T*>(::operator new(MIN_CAPACITY * sizeof(T)));
            len = cap;
            capacity = MIN_CAPACITY;
            length = 0;
        }

        explicit vector(uint64_t n) {
            if (n) {
                cap = static_cast<T*>(::operator new(n * sizeof(T)));
                len = cap;
                capacity = n;
                length = n;
                for (uint64_t i = 0; i < n; i += 1) {
                    new (len + i) T{};
                }
            } else {
                vector();
            }

        }

        vector(vector<T> const& that) {
            copy(that);
        }

        vector(vector<T>&& that) {
            move(std::move(that));
        }

        vector<T>& operator=(vector<T> const& rhs) {
            if (this != &rhs) {
                destroy();
                copy(rhs);
            }
            return *this;
        }

        vector<T>& operator=(vector<T>&& rhs) {
            if (this != &rhs) {
                destroy();
                move(std::move(rhs));
            }
            return *this;
        }

        ~vector() {
            destroy();
        }

        uint64_t size(void) const {
            return length;
        }

        T& operator[](uint64_t n) {
            if (n > length) {
                throw std::out_of_range{"index out of range"};
            }
            return len[n];
        }

        T const& operator[](uint64_t n) const {
            if (n > length) {
                throw std::out_of_range{"index out of range"};
            }
            return len[n];
        }

        void pop_back(void) {
            if (!length) {
                throw std::out_of_range{"fatal error: popping from an empty vector"};
            }
            (len + length - 1) -> ~T();
            length -= 1;
        }

        void pop_front(void) {
            if (!length) {
                throw std::out_of_range{"fatal error: popping from an empty vector"};
            }
            len -> ~T();
            len++;
            length -= 1;

        }

        void push_back(T const& elem) {
            T temp{elem};
            ensure_back_capacity(1);
            new (len + length) T{std::move(temp)};
            length += 1;
        }

        void push_back(T&& elem) {
            T temp{std::move(elem)};
            ensure_back_capacity(1);
            new (len + length) T{std::move(temp)};
            length += 1;
        }

        void push_front(T const& elem) {
            T temp{elem};
            ensure_front_capacity(1);
            new (len - 1) T{std::move(temp)};
            len--;
            length += 1;
        }

        void push_front(T&& elem) {
            T temp{std::move(elem)};
            ensure_front_capacity(1);
            new(len - 1) T{std::move(temp)};
            len--;
            length += 1;
        }

    private:
        void copy(vector<T> const& that) {
            capacity = that.capacity;
            length = that.length;
            cap = static_cast<T*>(::operator new(capacity * sizeof(T)));
            len = cap + (that.len - that.cap);
            for (uint64_t i = 0; i < length; i += 1) {
                new (len + i) T{ that[i] };
            }
        }

        void move(vector<T>&& that) {
            capacity = that.capacity;
            length = that.length;
            cap = that.cap;
            len = that.len;
            that.cap = nullptr;
            that.len = nullptr;
        }

        void destroy(void) {
            if (len) {
                for (uint64_t i = 0; i < length; i += 1) {
                    (len + i) -> ~T();
                }
            }
            if (cap) {
                ::operator delete(cap);
            }
        }

        void ensure_back_capacity(uint64_t capacity_required) {
            uint64_t back_capacity = (cap + capacity) - (len + length);

            if (back_capacity >= capacity_required) return;

            uint64_t new_capacity = capacity;
            while (back_capacity < capacity_required) {
                new_capacity += capacity;
                back_capacity += capacity;
            }

            T* new_cap = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
            T* new_len = new_cap + (len - cap);

            for (uint64_t i = 0; i < length; i += 1) {
                new (new_len + i) T{std::move(len[i])};
                (len + i) -> ~T();
            }

            ::operator delete(cap);

            cap = new_cap;
            len = new_len;
            capacity = new_capacity;
        }

        void ensure_front_capacity(uint64_t capacity_required) {
            uint64_t front_capacity = len - cap;
            if (front_capacity >= capacity_required) return;

            uint64_t new_capacity = capacity;
            while (front_capacity < capacity_required) {
                new_capacity += capacity;
                front_capacity += capacity;
            }

            T* new_cap = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
            T* new_len = new_cap + new_capacity - length - ((cap + capacity) - (len + length));

            for (uint64_t i = 0; i < length; i += 1) {
                new (new_len + i) T{std::move(len[i])};
                (len + i) -> ~T();
            }
            ::operator delete(cap);

            cap = new_cap;
            len = new_len;
            capacity = new_capacity;
        }
    };

} //namespace epl

#endif //VECTOR_VECTOR_H
