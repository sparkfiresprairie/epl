// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <utility>
namespace epl{
    
class invalid_iterator {
public:
    enum SeverityLevel {SEVERE,MODERATE,MILD,WARNING};
    SeverityLevel level;
    
    invalid_iterator(SeverityLevel level = SEVERE){ this->level = level; }
    virtual const char* what() const {
        switch(level){
            case WARNING:   return "Warning"; // not used in Spring 2015
            case MILD:      return "Mild";
            case MODERATE:  return "Moderate";
            case SEVERE:    return "Severe";
            default:        return "ERROR"; // should not be used
        }
    }
};
    
    
template <typename T>
class vector {
private:
    /*
     * The element data is managed using four pointers
     * capacity_begin : storage begin -- address of the start of the allocated storage
     * capacity_end : storage end -- address (one after) the last storage location
     * data_begin : data begin -- address of the first initialized element in the vector
     * data_end : data end -- address (one after) the last initialized element in vector
     */
    T* data_begin;
    T* data_end;
    T* capacity_begin;
    T* capacity_end;
    uint64_t position_version;
    uint64_t memory_version;
    static constexpr uint64_t MINIMUMCAPACITY = 8;
public:
    vector(void) { create(); }

    explicit vector(uint64_t n) { create(n); }
    
    vector(const vector<T>& that) { copy(that); }
    
    vector(vector<T>&& that) { move(std::move(that)); }
    
    vector& operator=(const vector<T>& rhs){
        if (this != &rhs) {
            destroy();
            copy(rhs);
        }
        return *this;
    }
    
    vector& operator=(vector<T>&& rhs) {
        if (this != &rhs) {
            destroy();
            move(std::move(rhs));
        }
        return *this;
    }
    
    template <typename IT>
    vector(IT b, IT e) { construct_from_iterator(b, e, typename std::iterator_traits<IT>::iterator_category{}); }
    
    vector(std::initializer_list<T> il) : vector(il.begin(), il.end()) {}
    
    ~vector(void) { destroy(); }
    
    class iterator;
    
    class const_iterator {
    private:
        const vector<T>* parent;
        uint64_t index;
        uint64_t position_version;
        uint64_t memory_version;
        bool is_inbound;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using difference_type = ptrdiff_t;
        
        friend vector<T>;
        friend vector<T>::iterator;
        
        //  CopyConstructible, CopyAssignable, Destructible
        const_iterator(): parent{nullptr}, index{0}, position_version{0}, memory_version{0}, is_inbound{true} {}
        const_iterator(const const_iterator& that) { if (this != &that ) copy(that); }
        const_iterator& operator=(const const_iterator& that) {
            if (this != &that) copy(that);
            return *this;
        }
        
        // Forward Iterator: dereferenceable, incrementable, EqualityComparable
        const T& operator*() const {
            assert_iterator_is_valid();
            return *(parent->data_begin + index);
        }
        const T* operator->() const {
            assert_iterator_is_valid();
            return parent->data_begin + index;
        }//?????????
        const_iterator& operator++() {
            assert_iterator_is_valid();
            ++index;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator t{*this};
            this -> operator++();
            return t;
        }
        bool operator==(const const_iterator& that) {
            assert_iterator_is_valid();
            return (this->parent == that.parent) && (this->index == that.index);
        }
        bool operator!=(const const_iterator& that) {
            return !(*this == that);
        }
        
        //  Bidirectionary Iterator: decrementable
        const_iterator& operator--() {
            assert_iterator_is_valid();
            --index;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator t{*this};
            this -> operator--();
            return t;
        }
       
        // Random Access Iterator
        const_iterator& operator+=(int64_t k) {
            assert_iterator_is_valid();
            index += k;
            return *this;
        }
        const_iterator& operator-=(int64_t k) {
            this->operator+=(-k);
            return *this;
        }
        const_iterator operator+(int64_t k) const {
            const_iterator temp{*this};
            return temp += k;
        }
        const_iterator operator-(int64_t k) const {
            const_iterator temp{*this};
            return temp -= k;
        }
        const T& operator[](uint64_t k) const { return *(*this + k); }
        int64_t operator-(const const_iterator& that) const {
            assert_iterator_is_valid();
            return this->index - that.index;
        }
        bool operator<(const const_iterator& that ) const { return (*this - that < 0) ? true : false; }
        bool operator>(const const_iterator& that) const { return that < *this; }
        bool operator>=(const const_iterator& that) const { return !( *this < that ); }
        bool operator<=(const const_iterator& that) const { return !( *this > that ); }
    private:
        void copy(const const_iterator& that) {
            parent = that.parent;
            index = that.index;
            position_version = that.position_version;
            memory_version = that.memory_version;
            is_inbound = ( that.index >= 0 && index < that.parent->size() );
        }
        const_iterator(const vector<T>* parent, uint64_t index, uint64_t position_version, uint64_t memory_version) {
            this->parent = parent;
            this->index = index;
            this->position_version = position_version;
            this->memory_version = memory_version;
            is_inbound = ( index >= 0 && index < parent->size() );
        }
        void assert_iterator_is_valid() const {
            uint64_t ipv = (*this).position_version;
            uint64_t imv = (*this).memory_version;
            uint64_t vpv = (*this).parent->position_version;
            uint64_t vmv = (*this).parent->memory_version;
            bool still_inbound = ( (*this).index >= 0 && (*this).index < (*this).parent->size() );
            
            if ( (*this).is_inbound && !still_inbound && ((imv!=vmv) || (ipv!=vpv)) )
                throw invalid_iterator(invalid_iterator::SEVERE);
            else if ( (*this).is_inbound && still_inbound && (imv!=vmv) )
                throw invalid_iterator(invalid_iterator::MODERATE);
            else if ( (ipv != vpv) || (imv != vmv) )
                throw invalid_iterator(invalid_iterator::MILD);
            else
                return;
        }
    };
    
    class iterator : public const_iterator {
    public:
        using pointer = T*;
        using reference = T&;
        
        friend vector<T>;
        
        //  CopyConstructible, CopyAssignable, Destructible
        iterator() : const_iterator() {}
        iterator(const iterator& that) { if (this != &that ) const_iterator::copy(that); }
        iterator& operator=(const iterator& that) {
            if (this != &that) const_iterator::copy(that);
            return *this;
        }
        
        // Forward Iterator: dereferenceable, incrementable, EqualityComparable
        T& operator*() const {
            return const_cast<T&>(const_iterator::operator*());
        }
        T* operator->() const {
            return const_cast<T*>(const_iterator::operator->());
        }
        iterator& operator++() {
            const_iterator::operator++();
            return *this;
        }
        iterator operator++(int) {
            iterator t{*this};
            this->operator++();
            return t;
        }
        
        //  Bidirectionary Iterator: decrementable
        iterator& operator--() {
            const_iterator::operator--();
            return *this;
        }
        
        iterator operator--(int) {
            iterator t{*this};
            this->operator--();
            return t;
        }
        
        // Random Access Iterator
        iterator& operator+=(int64_t k) {
            const_iterator::operator+=(k);
            return *this;
        }
        iterator& operator-=(int64_t k) {
            this->operator+=(-k);
            return *this;
        }
        iterator operator+(int64_t k) const {
            iterator temp{*this};
            return temp += k;
        }
        iterator operator-(int64_t k) const {
            iterator temp{*this};
            return temp -= k;
        }
        T& operator[](uint64_t k) const { return *(*this + k); }
    private:
        iterator(const vector<T>* parent, uint64_t index, uint64_t position_version, uint64_t memory_version) : const_iterator(parent, index, position_version, memory_version) {}
    };
    
    // Member Function Group: Capacity
    uint64_t size(void) const { return length(); }
    
    // Member Function Group: Element Access
    T& operator[](uint64_t k) {
        if (k >= length()) { throw std::out_of_range{"index out of range"}; }
        return data_begin[k];
    }
    
    const T& operator[](uint64_t k) const {
        if (k >= length()) { throw std::out_of_range{"index out of range"}; }
        return data_begin[k];
    }
    
    // Member Function Group: Modifiers
    void push_back(const T& element) {
        // Since ensure_back_capacity will destroy original vector,
        // if it is the element of original vector that is to be push_back
        // a segmentation fault will occur. That's why I need a temp.
        // Example:
        // vector<vector<Foo>> x(8);
        // x[0].push_front(Foo());
        // x.push_back(x[0]);
        T temp{element};
        ensure_back_capacity(1);
        new (data_end) T{ std::move(temp) };
        ++data_end;
        ++position_version;
    }
    
    void push_back(T&& element) {
        T temp{std::move(element)};
        ensure_back_capacity(1);
        new (data_end) T{ std::move(temp) };
        ++data_end;
        ++position_version;
    }
    
    void push_front(const T& element) {
        T temp{element};
        ensure_front_capacity(1);
        --data_begin;
        new (data_begin) T{ std::move(temp) };
        ++position_version;
    }
    
    void push_front(T&& element) {
        T temp{std::move(element)};
        ensure_front_capacity(1);
        --data_begin;
        new (data_begin) T{ std::move(temp) };
        ++position_version;
    }
    
    void pop_back(void) {
        if (!length()) { throw std::out_of_range{"fatal error: popping from an empty vector"}; }
        --data_end;
        data_end -> ~T();
        ++position_version;
    }
    
    void pop_front(void) {
        if (!length()) { throw std::out_of_range{"fatal error: popping from an empty vector"}; }
        data_begin -> ~T();
        ++data_begin;
        ++position_version;
    }
    
    // Member Function Group: Iterators
    const_iterator begin() const { return const_iterator(this, 0, position_version, memory_version); }
    iterator begin() { return iterator(this, 0, position_version, memory_version); }
    const_iterator end() const { return const_iterator(this, size(), position_version, memory_version); }
    iterator end() { return iterator(this, size(), position_version, memory_version); }
private:
    uint64_t length() const { return data_end - data_begin; }
    
    uint64_t capacity() const { return capacity_end - capacity_begin; }
    
    void create() {
        data_begin = data_end = capacity_begin = static_cast<T*>(operator new(MINIMUMCAPACITY * sizeof(T)));
        capacity_end = capacity_begin + MINIMUMCAPACITY;
        position_version = memory_version = 0;
    }
    
    void create(const uint64_t& n) {
        if (n == 0) { create(); }
        else {
            data_begin = data_end = capacity_begin = static_cast<T*>(operator new(n * sizeof(T)));
            capacity_end = capacity_begin + n;
            for (uint64_t k = 0; k < n; ++k) {
                new (data_end) T();
                ++data_end;
            }
            position_version = memory_version = 0;
        }
    }
    
    void destroy() {
        while (data_end != data_begin) {
            --data_end;
            data_end -> ~T();
        }
        operator delete(capacity_begin);
    }
    
    void copy(const vector<T>& that) {
        capacity_begin = static_cast<T*>(operator new(that.capacity() * sizeof(T)));
        capacity_end = capacity_begin + that.capacity();
        data_begin = data_end = capacity_begin + (that.data_begin - that.capacity_begin);
        for (uint64_t k = 0; k < that.length(); ++k) {
            new (data_end) T{ that.data_begin[k] };
            ++data_end;
        }
        ++memory_version;
    }
    
    void move(vector<T>&& that) {
        capacity_begin = that.capacity_begin;
        capacity_end = that.capacity_end;
        data_begin = that.data_begin;
        data_end = that.data_end;
        ++memory_version;
        that.capacity_begin = that.capacity_end = that.data_begin = that.data_end = nullptr;
    }
    
    void ensure_back_capacity(uint64_t capacity_required) {
        uint64_t back_capacity = capacity_end - data_end;
        
        if ( back_capacity >= capacity_required ) return;
        
        uint64_t new_capacity = capacity();
        
        while (back_capacity < capacity_required) {
            back_capacity += capacity();
            new_capacity += capacity();
        }
        
        T* new_capacity_begin = static_cast<T*>(operator new(sizeof(T) * new_capacity));
        T* new_capacity_end = new_capacity_begin + new_capacity;
        T* new_data_begin = new_capacity_begin + (data_begin - capacity_begin);
        T* new_data_end = new_data_begin;

        while (data_begin != data_end) {
            new (new_data_end) T{std::move(*data_begin)};
            data_begin -> ~T();
            ++data_begin;
            ++new_data_end;
        }
        
        operator delete(capacity_begin);
        
        capacity_begin = new_capacity_begin;
        capacity_end = new_capacity_end;
        data_begin = new_data_begin;
        data_end = new_data_end;
        ++memory_version;
    }
    
    void ensure_front_capacity(uint64_t capacity_required) {
        uint64_t front_capacity = data_begin - capacity_begin;
        
        if (front_capacity >= capacity_required) return;
        
        uint64_t new_capacity = capacity();
        
        while (front_capacity < capacity_required) {
            front_capacity += capacity();
            new_capacity += capacity();
        }
        
        T* new_capacity_begin = static_cast<T*>(operator new (sizeof(T) * new_capacity));
        T* new_capacity_end = new_capacity_begin + new_capacity;
        T* new_data_begin = new_capacity_end - (capacity_end - data_end) - length();
        T* new_data_end = new_data_begin;
        
        while (data_begin != data_end) {
            new (new_data_end) T{std::move(*data_begin)};
            data_begin -> ~T();
            ++data_begin;
            ++new_data_end;
        }
        
        operator delete(capacity_begin);
        
        capacity_begin = new_capacity_begin;
        capacity_end = new_capacity_end;
        data_begin = new_data_begin;
        data_end = new_data_end;
        ++memory_version;
    }
    
    template <typename IT>
    void construct_from_iterator(IT b, IT e, std::random_access_iterator_tag) {
        uint64_t capacity = static_cast<uint64_t>(e - b);
        if (capacity < MINIMUMCAPACITY) capacity = MINIMUMCAPACITY;
        capacity_begin = data_begin = data_end = static_cast<T*>(operator new (sizeof(T) * capacity));
        capacity_end = capacity_begin + capacity;
        while (b != e) {
            new (data_end) T(*b);
            ++b;
            ++data_end;
        }
        position_version = memory_version = 0;
    }
    
    template <typename IT>
    void construct_from_iterator(IT b, IT e, std::forward_iterator_tag) {
        uint64_t capacity = MINIMUMCAPACITY;
        capacity_begin = data_begin = data_end = static_cast<T*>(operator new (sizeof(T) * capacity));
        capacity_end = capacity_begin + capacity;
        while (b != e) {
            push_back(*b);
            ++b;
        }
        position_version = memory_version = 0;
    }
};

} //namespace epl

#endif /* _Vector_h */
