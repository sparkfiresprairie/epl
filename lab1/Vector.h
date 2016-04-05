#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <iterator>

/*
 * Utility gives std::rel_ops which will fill in relational
 * iterator operations so long as you provide the
 * operators discussed in class.  In any case, ensure that
 * all operations listed in this website are legal for your
 * iterators:
 * http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
 */

using namespace std::rel_ops;

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
        /* constant */
        static constexpr  uint64_t MIN_CAPACITY = 8;

        /* data member */
        T* cap;                 // start address of the allocated storage
        T* len;                 // start address of constructed elements in the vector
        uint64_t capacity;      // storage size
        uint64_t length;        // vector size (number of elements)
        uint64_t modification;  // version number for vector modification
        uint64_t reallocation;   // version number for memory reallocation

        /* nested class: iterator */
        template <bool is_const_iterator = true>
        class iterator_base : public std::iterator<std::random_access_iterator_tag, T> {
        private:
            /* type alias */
            using data_structure_pointer_type = typename std::conditional<is_const_iterator, vector const*, vector*>::type;
            using value_reference_type = typename std::conditional<is_const_iterator, T const&, T&>::type;
            using value_pointer_type = typename std::conditional<is_const_iterator, T const*, T*>::type;

            /* data member */
            data_structure_pointer_type ds;
            int64_t offset;
            uint64_t modification_snapshot;
            uint64_t reallocation_snapshot;
        public:
            /* type alias */
            using typename std::iterator<std::random_access_iterator_tag, T>::iterator_category;
            using typename std::iterator<std::random_access_iterator_tag, T>::value_type;
            using typename std::iterator<std::random_access_iterator_tag, T>::difference_type;
            using typename std::iterator<std::random_access_iterator_tag, T>::pointer;
            using typename std::iterator<std::random_access_iterator_tag, T>::reference;

            /* copy-constructible, copy-assignable and destructible */
            iterator_base()
                    : ds{ nullptr },
                      offset{ 0 },
                      modification_snapshot{ 0 },
                      reallocation_snapshot{ 0 }
                      {}

            iterator_base(data_structure_pointer_type ds, int64_t offset, uint64_t modification, uint64_t reallocation)
                    : ds{ ds },
                      offset{ offset },
                      modification_snapshot{ modification },
                      reallocation_snapshot{ reallocation }
                      {}

            iterator_base(iterator_base const& that) { iterator_base::copy(that); }

            iterator_base& operator=(iterator_base const& that) {
                if (this != &that) {
                    copy(that);
                }
                return *this;
            }

            ~iterator_base() {}

            /*
             * an iterator can be converted (without warnings or type
             * casts) to const_iterator
             */
            operator iterator_base<true>(void) {
                return iterator_base<true>(ds, offset, modification_snapshot, reallocation_snapshot);
            }

            /* Can be incremented */
            iterator_base& operator++(void) {   // pre-increment operator, e.g. ++it
                assert_valid();
                ++offset;
                return *this;
            }

            iterator_base operator++(int) {  // post-increment operator, e.g. it++
                iterator_base t{ *this };
                operator++();
                return t;
            }

            /* Supports equality/inequality comparisons */
            bool operator==(iterator_base const& that) const {
                assert_valid();
                that.assert_valid();
                return ds == that.ds && offset == that.offset;
            }

            /* Can be dereferenced */
            value_reference_type operator*(void) const {
                assert_valid();
                return (*ds)[offset];
            }

            value_pointer_type operator->(void) const {
                return &(operator*());
            }

            /* Can be decremented */
            iterator_base& operator--(void) {	// pre-decrement operator, e.g. --it
                assert_valid();
                --offset;
                return *this;
            }

            iterator_base operator--(int) {	// post-decrement operator, e.g. it--;
                iterator_base t{ *this };
                operator--();
                return t;
            }

            /* Supports compound assignment operations += and -= */
            iterator_base& operator+=(int64_t k) {
                assert_valid();
                offset += k;
                return *this;
            }

            iterator_base& operator-=(int64_t k) {
                operator+=(-k);
                return *this;
            }

            /* Supports arithmetic operators + and - */
            iterator_base operator+(int64_t k) const {
                iterator_base t{ *this };
                return t += k;
            }

            iterator_base operator-(int64_t k) const {
                iterator_base t{ *this };
                return t -= k;
            }

            difference_type operator-(iterator_base const& that) const {
                assert_valid();
                that.assert_valid();
                return offset - that.offset;
            }

            /* Supports inequality comparisons (<, >, <= and >=) between iterators */
            bool operator<(iterator_base const& that) const {
                return *this - that < 0;
            }

            /* Supports offset dereference operator ([]) */
            value_reference_type operator[](uint64_t k) const {
                return *(*this + k);
            }
        private:
            void copy(iterator_base const& that) {
                ds = that.ds;
                offset = that.offset;
                modification_snapshot = that.modification_snapshot;
                reallocation_snapshot = that.reallocation_snapshot;
            }

            void assert_valid(void) const {
                bool is_modified = (modification_snapshot != ds->modification);
                bool is_reallocated = (reallocation_snapshot != ds->reallocation);
                bool is_inbound = (offset >= 0 && offset < ds->length);

                if (is_modified || is_reallocated) {
                    if (!is_inbound)
                        throw invalid_iterator(invalid_iterator::SEVERE);
                    else if (is_reallocated)
                        throw invalid_iterator(invalid_iterator::MODERATE);
                    else
                        throw invalid_iterator(invalid_iterator::MILD);
                }
            }
        };
    public:
        using const_iterator = iterator_base<true>;
        using iterator = iterator_base<false>;

        vector(void) {
            init(MIN_CAPACITY);
        }

        explicit vector(uint64_t n) {
            if (n) {
                init(n);
                length = n;
                for (uint64_t i = 0; i < n; i += 1) {
                    new (len + i) T{};
                }
            }
            else {
                vector();
            }
        }

        vector(vector const& that) {
            copy(that);
        }

        vector(vector&& that) {
            move(std::move(that));
        }

        template <typename IT>
        vector(IT b, IT e) : vector(b, e, typename std::iterator_traits<IT>::iterator_category{}) {}

        template <typename IT>
        vector(IT b, IT e, std::random_access_iterator_tag) {
            init(e - b);
            length = 0;
            while (b != e) {
                new (len + length) T{ *b };
                ++b;
                ++length;
            }
        }

        template <typename IT>
        vector(IT b, IT e, std::input_iterator_tag) : vector() {
            while (b != e) {
                push_back(*b);
                ++b;
            }
        }

        vector(std::initializer_list<T> list): vector(list.begin(), list.end()) {}

        vector& operator=(vector const& that) {
            if (this != &that) {
                destroy();
                copy(that);
            }
            return *this;
        }

        vector& operator=(vector&& that) {
            if (this != &that) {
                destroy();
                move(std::move(that));
            }
            return *this;
        }

        ~vector() {
            destroy();
        }

        /* Member Function Group: Capacity */
        uint64_t size(void) const {
            return length;
        }

        /* Member Function Group: Element Access */
        T const& operator[](uint64_t n) const {
            if (n >= length) {
                throw std::out_of_range{ "index out of range" };
            }
            return len[n];
        }

        T& operator[](uint64_t n) {
            return const_cast<T&>(static_cast<vector const&>(*this)[n]);
        }


        /* Member Function Group: Modifiers */
        void pop_back(void) {
            if (!length) {
                throw std::out_of_range{ "fatal error: popping from an empty vector" };
            }
            (len + length - 1) -> ~T();
            length -= 1;
            modification += 1;
        }

        void pop_front(void) {
            if (!length) {
                throw std::out_of_range{ "fatal error: popping from an empty vector" };
            }
            len -> ~T();
            len++;
            length -= 1;
            modification += 1;
        }

        /*
         * Since ensure_back_capacity will destroy original vector,
         * if it is the element of original vector that is to be push_back
         * a segmentation fault will occur. That's why I need a temp.
         * Example:
         * vector<vector<Foo>> x(8);
         * x[0].push_front(Foo());
         * x.push_back(x[0]);
         */
        void push_back(T const& elem) {
            T temp{ elem };
            ensure_back_capacity(1);
            new (len + length) T{ std::move(temp) };
            length += 1;
            modification += 1;
        }

        void push_back(T&& elem) {
            T temp{ std::move(elem) };
            ensure_back_capacity(1);
            new (len + length) T{ std::move(temp) };
            length += 1;
            modification += 1;
        }

        void push_front(T const& elem) {
            T temp{ elem };
            ensure_front_capacity(1);
            new (len - 1) T{ std::move(temp) };
            len--;
            length += 1;
            modification += 1;
        }

        void push_front(T&& elem) {
            T temp{ std::move(elem) };
            ensure_front_capacity(1);
            new(len - 1) T{ std::move(temp) };
            len--;
            length += 1;
            modification += 1;
        }

        template<typename... Args>
        void emplace_back(Args... args) {
            ensure_back_capacity(1);
            new (len + length) T{ std::forward<Args>(args)... };
            length += 1;
            modification += 1;
        }

        /* Member Function Group: Iterators */
        const_iterator begin(void) const {
            return const_iterator(this, 0, modification, reallocation);
        }

        const_iterator end(void) const {
            return const_iterator(this, length, modification, reallocation);
        }

        iterator begin(void) {
            return iterator(this, 0, modification, reallocation);
        }

        iterator end(void) {
            return iterator(this, length, modification, reallocation);
        }
    private:
        void init(uint64_t n) {
            cap = static_cast<T*>(::operator new(n * sizeof(T)));
            len = cap;
            capacity = n;
            length = 0;
            modification = 0;
            reallocation = 0;
        }

        void copy(vector const& that) {
            capacity = that.capacity;
            length = that.length;
            cap = static_cast<T*>(::operator new(capacity * sizeof(T)));
            len = cap + (that.len - that.cap);
            for (uint64_t i = 0; i < length; i += 1) {
                new (len + i) T{ that[i] };
            }
            reallocation += 1;
        }

        void move(vector&& that) {
            capacity = that.capacity;
            length = that.length;
            cap = that.cap;
            len = that.len;
            that.cap = nullptr;
            that.len = nullptr;
            /*
             * the storage cap points to is different,
             * it can be considered as a kind of
             * "reallocation"
             */
            reallocation += 1;
            that.reallocation += 1;
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
                new (new_len + i) T{ std::move(len[i]) };
                (len + i) -> ~T();
            }

            ::operator delete(cap);

            cap = new_cap;
            len = new_len;
            capacity = new_capacity;
            reallocation += 1;
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
                new (new_len + i) T{ std::move(len[i]) };
                (len + i) -> ~T();
            }
            ::operator delete(cap);

            cap = new_cap;
            len = new_len;
            capacity = new_capacity;
            reallocation += 1;
        }
    };
} //namespace epl

#endif
