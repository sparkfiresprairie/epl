#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <iterator>

//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
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
        using Same = vector<T>;
        static constexpr uint64_t MINIMUMCAPACITY = 8;
    private:
        T* dataBegin;
        T* dataEnd;
        T* capacityBegin;
        T* capacityEnd;
        
        uint64_t length() const {
            return (((uint64_t)dataEnd - (uint64_t)dataBegin)/sizeof(T));
        }
        
        uint64_t capacity() const {
            return (((uint64_t)capacityEnd - (uint64_t)capacityBegin)/sizeof(T));
        }
        
        void create() {
            dataBegin = dataEnd = capacityBegin = (T*) ::operator new( MINIMUMCAPACITY * sizeof(T) );
            capacityEnd = capacityBegin + MINIMUMCAPACITY;
        }
        
        void create(const uint64_t& n) {
            if (n == 0) { create(); }
            else {
                dataBegin = dataEnd = capacityBegin = (T*) ::operator new( n * sizeof(T) );
                capacityEnd = capacityBegin + n;
                for (uint64_t k = 0; k < n; ++k) {
                    new (dataEnd) T();
                    ++dataEnd;
                }
            }
        }
        
        void destroy() {
            while (dataEnd != dataBegin) {
                --dataEnd;
                dataEnd -> ~T();
            }
            ::operator delete(capacityBegin);
        }
        
        void copy(const Same &that) {
            capacityBegin = (T*) ::operator new( (that.capacity()) * sizeof(T) );
            capacityEnd = capacityBegin + (that.capacity());
            dataBegin = dataEnd = capacityBegin + (that.dataBegin - that.capacityBegin);
            for (uint64_t k = 0; k < (that.length()); ++k) {
                new (dataEnd) T{ that.dataBegin[k] };
                ++dataEnd;
            }
        }
        
        void move(Same &&that) {
            capacityBegin = that.capacityBegin;
            capacityEnd = that.capacityEnd;
            dataBegin = that.dataBegin;
            dataEnd = that.dataEnd;
            that.capacityBegin = that.capacityEnd = that.dataBegin = that.dataEnd = nullptr;
        }
    public:
        /* (A) creates an array with some minimum capacity (8 is fine) and length equal to zero */
        vector(void) { create(); initVersionNumber(); }
        
        /* (A) create an array with capacity and length exactly equal to n */
        explicit vector(uint64_t n) { create(n); initVersionNumber(); }
        
        /* (A) copy constructors for vector<T> arguments (same type) */
        vector(const Same &that) { copy(that); initVersionNumber(); }
        
        /* (B) move constructors for vector<T> arguments (same type) */
        vector(Same &&that) {
            this -> move(std::move(that));
            initVersionNumber();
        }
        
        /* (A) copy assignment operators for vector<T> arguments (same type) */
        vector& operator=(const Same &rhs){
            if (this != &rhs) {
                destroy();
                copy(rhs);
                versionNumber[VECTOR_ASSIGNMENT] += 1;
            }
            return *this;
        }
        
        vector& operator=(Same &&rhs) {
            if (this != &rhs) {
                destroy();
                this -> move(std::move(rhs));
                versionNumber[VECTOR_ASSIGNMENT] += 1;
            }
            return *this;
        }
        
        /* (A) destructor */
        ~vector(void) { destroy(); }
        
        /* (A) return the number of constructed objects in the vector */
        uint64_t size(void) const { return length(); }
        
        /* (A) if k is out of bounds (equal to or larger than length), then you must throw std::out_of_range(“subscript out of range”); or similar error message (the string is arbitrary). If k is in bounds, then you must return a reference to the element at position k */
        T& operator[](uint64_t k) {
            if (k >= length()) {
                throw std::out_of_range{"index out of range"};
            }
            return dataBegin[k];
        }
        
        /* (A*) same as above, except for constants */
        const T& operator[](uint64_t k) const {
            if (k >= length()) {
                throw std::out_of_range{"index out of range"};
            }
            return dataBegin[k];
        }
        
        /* (A) add a new value to the end of the array, using amortized doubling if the array has to be resized. Copy construct the argument. */
        void push_back(const T& element) {
            if (dataEnd == capacityEnd) {
                T* newCapacityBegin = (T*) ::operator new( 2 * capacity() * sizeof(T) );
                T* newCapacityEnd = newCapacityBegin + 2 * capacity();
                T* newDataBegin = newCapacityBegin + capacity();
                T* newDataEnd = newDataBegin;
                
                new (newDataEnd) T{ element };
                ++newDataEnd;
                
                for (uint64_t k = 0; k < length(); ++k) {
                    --newDataBegin;
                    new (newDataBegin) T{ std::move(dataBegin[length()-k-1]) };
                }
                
                destroy();
                
                dataBegin = newDataBegin;
                dataEnd = newDataEnd;
                capacityBegin = newCapacityBegin;
                capacityEnd = newCapacityEnd;
                
                versionNumber[MEMORY_ALLOCATION] += 1;
            }
            else {
                new (dataEnd) T{ element };
                ++dataEnd;
                
                versionNumber[NORMAL_PUSH_OR_POP] += 1;
            }
        }
        
        /* (B) same as above, but move construct the argument. */
        void push_back(T&& element) {
            if (dataEnd == capacityEnd) {
                T* newCapacityBegin = (T*) ::operator new( 2 * capacity() * sizeof(T) );
                T* newCapacityEnd = newCapacityBegin + 2 * capacity();
                T* newDataBegin = newCapacityBegin + capacity();
                T* newDataEnd = newDataBegin;
                
                new (newDataEnd) T{ std::move(element) };
                ++newDataEnd;
                
                for (uint64_t k = 0; k < length(); ++k) {
                    --newDataBegin;
                    new (newDataBegin) T{ std::move(dataBegin[length()-k-1]) };
                }
                
                destroy();
                
                dataBegin = newDataBegin;
                dataEnd = newDataEnd;
                capacityBegin = newCapacityBegin;
                capacityEnd = newCapacityEnd;
                
                versionNumber[MEMORY_ALLOCATION] += 1;
            }
            else {
                new (dataEnd) T{ std::move(element) };
                ++dataEnd;
                versionNumber[NORMAL_PUSH_OR_POP] += 1;
            }
        }
        
        /* (A* or B) similar to push_back, but add the element to the front of the Vector */
        void push_front(const T& element) {
            if (dataBegin == capacityBegin) {
                T* newCapacityBegin = (T*) ::operator new( 2 * capacity() * sizeof(T) );
                T* newCapacityEnd = newCapacityBegin + 2 * capacity();
                T* newDataBegin = newCapacityBegin + capacity();
                T* newDataEnd = newDataBegin;
                
                --newDataBegin;
                new (newDataBegin) T{ element };
                
                for (uint64_t k = 0; k < length(); ++k) {
                    new (newDataEnd) T{ std::move(dataBegin[k]) };
                    ++newDataEnd;
                }
                
                destroy();
                
                dataBegin = newDataBegin;
                dataEnd = newDataEnd;
                capacityBegin = newCapacityBegin;
                capacityEnd = newCapacityEnd;
                versionNumber[MEMORY_ALLOCATION] += 1;
            }
            else {
                --dataBegin;
                new (dataBegin) T{ element };
                versionNumber[NORMAL_PUSH_OR_POP] += 1;
            }
            
        }
        
        /* (B) same as above, but move construct the argument. */
        void push_front(T&& element) {
            if (dataBegin == capacityBegin) {
                T* newCapacityBegin = (T*) ::operator new( 2 * capacity() * sizeof(T) );
                T* newCapacityEnd = newCapacityBegin + 2 * capacity();
                T* newDataBegin = newCapacityBegin + capacity();
                T* newDataEnd = newDataBegin;
                
                for (uint64_t k = 0; k < length(); ++k) {
                    new (newDataEnd) T{ std::move(dataBegin[k]) };
                    ++newDataEnd;
                }
                
                --newDataBegin;
                new (newDataBegin) T{ std::move(element) };
                destroy();
                
                dataBegin = newDataBegin;
                dataEnd = newDataEnd;
                capacityBegin = newCapacityBegin;
                capacityEnd = newCapacityEnd;
                
                versionNumber[MEMORY_ALLOCATION] += 1;
            }
            else {
                --dataBegin;
                new (dataBegin) T{ std::move(element) };
                
                versionNumber[NORMAL_PUSH_OR_POP] += 1;
            }
            
        }
        
        /* (A) if the array is empty, throw a std::out_of_range exception (with any reasonable string error message you want). Otherwise, destroy the object at the end (or beginning) of the array and update the length (and any pointers you need). Do not reallocate storage, even if the vector becomes empty. Obviously, the available capacity increases by one. It is possible that a vector can have available capacity at both the front and back simultaneously. */
        void pop_back(void) {
            if (!length()) {
                throw std::out_of_range{"fatal error: popping from an empty vector"};
            }
            --dataEnd;
            dataEnd -> ~T();
            versionNumber[NORMAL_PUSH_OR_POP] += 1;
        }
        
        void pop_front(void) {
            if (!length()) {
                throw std::out_of_range{"fatal error: popping from an empty vector"};
            }
            dataBegin -> ~T();
            ++dataBegin;
            versionNumber[NORMAL_PUSH_OR_POP] += 1;
        }
        /* Phase C */
    public:
        enum VersionNumber {
            NORMAL_PUSH_OR_POP,             // push or pop with memory allocation, MILD
            VECTOR_ASSIGNMENT,              // vector assignment occurs, MODERATE
            MEMORY_ALLOCATION,              // memory allocation occurs, MODERATE
            VERSION_NUMBER_COUNT,           // number of events
            OUT_OF_RANGE,                   // SEVERE
            DIFFERENT_VECTOR,               // different vectors, SEVERE
            NULLPTR_VECTOR,                 // SEVERE
            COMBINED_LEADING_TO_SEVERE,     // combined events leading to SEVERE
            COMBINED_LEADING_TO_MODERATE,   // combined events leading to MODERATE
            VALID                           // valid version
        };
        
        uint64_t versionNumber[VERSION_NUMBER_COUNT];   // vector's version numbers
        struct iterator;
        struct const_iterator;
        
        /* constructor that will initialize a vector from a std::initializer_list<T> */
        vector(std::initializer_list<T> list){
            uint64_t size =list.size();
            dataBegin = dataEnd = capacityBegin = (T*) ::operator new( size * sizeof(T) );
            capacityEnd = capacityBegin + size;
            for( auto it = list.begin(); it != list.end(); ++it ) {  *dataEnd= *it; ++dataEnd; }
            initVersionNumber();
        }
        
        /* template constructor */
        template <typename IT>
        vector(IT itb, IT ite){
            typename std::iterator_traits<IT>::iterator_category itc;
            initVector(itb, ite, itc);
        }
        
        template <typename RAI>
        void initVector(RAI itb, RAI ite, std::random_access_iterator_tag) {
            uint64_t size = ite - itb;
            dataBegin = dataEnd = capacityBegin = (T*) ::operator new( size * sizeof(T) );
            capacityEnd = capacityBegin + size;
            for(; itb != ite; ++itb) { *dataEnd = *itb; ++ dataEnd; }
            initVersionNumber();
        }
        
        template <typename II>
        void initVector(II itb, II ite, std::input_iterator_tag) {
            dataBegin = dataEnd = capacityBegin = capacityEnd = nullptr;
            initVersionNumber();
            for(; itb != ite; ++itb) { push_back(*itb); }
        }
        
        void initVersionNumber() {
            for( uint64_t i = 0; i< VERSION_NUMBER_COUNT; ++i ) versionNumber[i] = 0;
        }
        
        /* begin and end function for vector */
        iterator begin() {
            iterator temp;
            temp.vptr = this;
            temp.offset = 0;
            temp.versionNumber[NORMAL_PUSH_OR_POP] = this->versionNumber[NORMAL_PUSH_OR_POP];
            temp.versionNumber[VECTOR_ASSIGNMENT] = this->versionNumber[VECTOR_ASSIGNMENT];
            temp.versionNumber[MEMORY_ALLOCATION] = this->versionNumber[MEMORY_ALLOCATION];
            return temp;
        }
        
        iterator end() {
            iterator temp;
            temp.vptr = this;
            temp.offset = length();
            temp.versionNumber[NORMAL_PUSH_OR_POP] = this->versionNumber[NORMAL_PUSH_OR_POP];
            temp.versionNumber[VECTOR_ASSIGNMENT] = this->versionNumber[VECTOR_ASSIGNMENT];
            temp.versionNumber[MEMORY_ALLOCATION] = this->versionNumber[MEMORY_ALLOCATION];
            return temp;
        }
        
        const_iterator begin() const {
            const_iterator temp;
            temp.vptr = this;
            temp.offset = 0;
            temp.versionNumber[NORMAL_PUSH_OR_POP] = this->versionNumber[NORMAL_PUSH_OR_POP];
            temp.versionNumber[VECTOR_ASSIGNMENT] = this->versionNumber[VECTOR_ASSIGNMENT];
            temp.versionNumber[MEMORY_ALLOCATION] = this->versionNumber[MEMORY_ALLOCATION];
            return temp;
        }

        const_iterator end() const {
            const_iterator temp;
            temp.vptr = this;
            temp.offset = length();
            temp.versionNumber[NORMAL_PUSH_OR_POP] = this->versionNumber[NORMAL_PUSH_OR_POP];
            temp.versionNumber[VECTOR_ASSIGNMENT] = this->versionNumber[VECTOR_ASSIGNMENT];
            temp.versionNumber[MEMORY_ALLOCATION] = this->versionNumber[MEMORY_ALLOCATION];
            return temp;
        }
        
        template <typename TV>
        struct iterator_base {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = ptrdiff_t;
            using pointer = T*;
            using reference = T&;
            using Iterator_base = iterator_base<TV>;
            
            TV* vptr;
            uint64_t offset;
            uint64_t versionNumber[VERSION_NUMBER_COUNT];   // iterator's version numbers
            
            iterator_base() {
                vptr = nullptr;
                initVersionNumber(vptr);
            }
            
            void initVersionNumber(TV* p) {
                for ( uint64_t i = 0; i < VERSION_NUMBER_COUNT; ++i )
                    versionNumber[i] = ( p == nullptr )? 0 : p->versionNumber[i];
            }
            
            uint64_t checkRange() const {
                if ( offset >= vptr->size() ) return OUT_OF_RANGE;
                return VALID;
            }
            
            uint64_t getValidType() const {
                if (vptr == nullptr)
                    return NULLPTR_VECTOR;
                else if (vptr->versionNumber[MEMORY_ALLOCATION] != versionNumber[MEMORY_ALLOCATION])
                    return MEMORY_ALLOCATION;
                else if (vptr->versionNumber[VECTOR_ASSIGNMENT] != versionNumber[VECTOR_ASSIGNMENT])
                    return VECTOR_ASSIGNMENT;
                else if (vptr->versionNumber[NORMAL_PUSH_OR_POP] != versionNumber[NORMAL_PUSH_OR_POP])
                    return NORMAL_PUSH_OR_POP;
                else
                    return VALID;
            }
            
            uint64_t getValidType(const Iterator_base &it) const {
                if (this->vptr != it.vptr)
                    return DIFFERENT_VECTOR;
                else
                    return VALID;
            }
            
            void checkException(uint64_t exceptionType) const {
                switch ( exceptionType ) {
                    case NULLPTR_VECTOR:
                        std::cout << "NULLPTR_VECTOR" << std::endl;
                        throw invalid_iterator(invalid_iterator::SEVERE);
                        break;
                    case OUT_OF_RANGE:
                        std::cout << "OUT_OF_RANGE" << std::endl;
                        throw invalid_iterator(invalid_iterator::SEVERE);
                        break;
                    case DIFFERENT_VECTOR:
                        std::cout << "DIFFERENT_VECTOR" << std::endl;
                        throw invalid_iterator(invalid_iterator::SEVERE);
                        break;
                    case MEMORY_ALLOCATION:
                        std::cout << "MEMORY_ALLOCATION" << std::endl;
                        throw invalid_iterator(invalid_iterator::MODERATE);
                        break;
                    case VECTOR_ASSIGNMENT:
                        std::cout << "VECTOR_ASSIGNMENT" << std::endl;
                        throw invalid_iterator(invalid_iterator::MODERATE);
                        break;
                    case NORMAL_PUSH_OR_POP:
                        std::cout << "NORMAL_PUSH_OR_POP" << std::endl;
                        throw invalid_iterator(invalid_iterator::MILD);
                        break;
                    case COMBINED_LEADING_TO_SEVERE:
                        std::cout << "COMBINED_LEADING_TO_SEVERE" << std::endl;
                        throw invalid_iterator(invalid_iterator::SEVERE);
                        break;
                    case COMBINED_LEADING_TO_MODERATE:
                        std::cout << "COMBINED_LEADING_TO_MODERATE" << std::endl;
                        throw invalid_iterator(invalid_iterator::MODERATE);
                        break;
                    default:
                        break;
                }
            }
            
            void copy(const Iterator_base& rhs) {
                rhs.checkException(rhs.getValidType());
                vptr = rhs.vptr;
                offset = rhs.offset;
                initVersionNumber(rhs.vptr);
            }
            
            iterator_base(const Iterator_base& rhs) {
                if(this != &rhs)
                    copy(rhs);
            }
            
            Iterator_base& operator=(const Iterator_base& rhs) {
                if(this != &rhs)
                    copy(rhs);
                return *this;
            }
            
            bool operator==(const Iterator_base& rhs) const {
                rhs.checkException(rhs.getValidType());
                checkException(getValidType());
                return (vptr == rhs.vptr && offset == rhs.offset);
            }
            
            bool operator<(const Iterator_base& rhs) const {
                rhs.checkException(rhs.getValidType());
                checkException(getValidType());
                return ((*this-rhs)<0)? true : false;
            }
            
            bool operator!=(const Iterator_base& rhs) const {
                const Iterator_base& lhs = *this;
                return !(lhs == rhs);
            }
            
            bool operator>(const Iterator_base& rhs) const {
                const Iterator_base& lhs = *this;
                return rhs < lhs;
            }
            
            bool operator<=(const Iterator_base& rhs) const {
                const Iterator_base& lhs = *this;
                return !(rhs < lhs);
            }
            
            bool operator>=(const Iterator_base& rhs) const {
                const Iterator_base& lhs = *this;
                return !(lhs < rhs);
            }
            
            ptrdiff_t operator-(const Iterator_base& rhs) const {
                rhs.checkException(rhs.getValidType());
                checkException(getValidType());
                return this->offset - rhs.offset;
            }
            
            void iteratorInc(uint64_t i) {
                offset += i;
            }
            
            void iteratorDec(uint64_t i){
                offset -= i;
            }
        };
        
        struct iterator : public iterator_base< vector<T> > {
            using pointer = T*;
            using reference = T&;
            using Iterator_base = iterator_base< vector<T> >;
            // ???
            iterator() = default;
            
            operator const_iterator() {
                return const_iterator(*this);
            }
            // ???
            reference operator*() const {
                Iterator_base::checkException(Iterator_base::checkRange());
                Iterator_base::checkException(Iterator_base::getValidType());
                return Iterator_base::vptr->at(this->offset);
            }
            // why temp, const
            reference operator[](uint64_t i) const {
                iterator temp = *this;
                return *(temp += i);
            }
            // why pointer
            pointer operator->() const {
                Iterator_base::checkException(Iterator_base::checkRange());
                Iterator_base::checkException(Iterator_base::getValidType());
                return &(Iterator_base::vptr->at(this->offset));
            }
            // what is this here, a pointer to object iterator?
            iterator operator+(uint64_t i) const {
                iterator temp = *this;
                return temp += i;
            }
            iterator operator-(uint64_t i) const {
                iterator temp = *this;
                return temp -= i;
            }
            // why const_iterator here
            ptrdiff_t operator-(const const_iterator& it) const {
                return Iterator_base::operator-(it);
            }
            // ++p
            iterator& operator++() { return this->operator+=(1); }
            iterator& operator--() { return this->operator-=(1); }
            // p++
            iterator operator++(int) {
                Iterator_base::checkException(Iterator_base::getValidType());
                iterator temp = *this;
                ++(*this);
                return temp;
            }
            iterator operator--(int) {
                Iterator_base::checkException(Iterator_base::getValidType());
                iterator temp = *this;
                --(*this);
                return temp;
            }
            
            iterator& operator+=(uint64_t i) {
                Iterator_base::checkException(Iterator_base::getValidType());
                Iterator_base::iteratorInc(i);
                return *this;
            }
            
            iterator& operator-=(uint64_t i) {
                Iterator_base::checkException(Iterator_base::getValidType());
                Iterator_base::iteratorDec(i);
                return *this;
            }
            
            // why friend
            friend iterator operator+(uint64_t i, const iterator& it) {
                return it.operator+(i);
            }
            
        };
        
        struct const_iterator : public iterator_base< const vector<T> > {
            using pointer = const T*;
            using reference = const T&;
            using Iterator_base = iterator_base< const vector<T> >;
            // ???
            const_iterator()=default;
            
            const_iterator(const  iterator& rhs) {
                rhs.checkException(rhs.getValidType());
                this->vptr = rhs.vptr;
                this->offset = rhs.offset;
                Iterator_base::initVersionNumber(rhs.vptr);
            }
            // ???
            reference operator*() const {
                Iterator_base::checkException(Iterator_base::checkRange());
                Iterator_base::checkException(Iterator_base::getValidType());
                return Iterator_base::vptr->at(this->offset);
            }
            // why temp, const
            reference operator[](uint64_t i) const {
                const_iterator temp = *this;
                return *(temp += i);
            }
            // why pointer
            pointer operator->() const {
                Iterator_base::checkException(Iterator_base::checkRange());
                Iterator_base::checkException(Iterator_base::getValidType());
                return &(Iterator_base::vptr->at(this->offset));
            }
            // what is this here, a pointer to object iterator?
            const_iterator operator+(uint64_t i) const {
                const_iterator temp = *this;
                return temp += i;
            }
            const_iterator operator-(uint64_t i) const {
                const_iterator temp = *this;
                return temp -= i;
            }
            // why const_iterator here
            ptrdiff_t operator-(const const_iterator& it) const {
                return Iterator_base::operator-(it);
            }
            // ++p
            const_iterator& operator++() { return this->operator+=(1); }
            const_iterator& operator--() { return this->operator-=(1); }
            // p++
            const_iterator operator++(int) {
                Iterator_base::checkException(Iterator_base::getValidType());
                const_iterator temp = *this;
                ++(*this);
                return temp;
            }
            const_iterator operator--(int) {
                Iterator_base::checkException(Iterator_base::getValidType());
                const_iterator temp = *this;
                --(*this);
                return temp;
            }
            
            const_iterator& operator+=(uint64_t i) {
                Iterator_base::checkException(Iterator_base::getValidType());
                Iterator_base::iteratorInc(i);
                return *this;
            }
            
            const_iterator& operator-=(uint64_t i) {
                Iterator_base::checkException(Iterator_base::getValidType());
                Iterator_base::iteratorDec(i);
                return *this;
            }
            
            // why friend
            friend const_iterator operator+(uint64_t i, const const_iterator& it) {
                return it.operator+(i);
            }
            
        };
        
        // why two at
        T& at(uint64_t k) {
            if (k>=length()) {
                throw std::out_of_range ("index out of range");
            }
            else {
                return *(dataBegin+k);
            }
        }
        
        const T& at(uint64_t k) const {
            if (k>=length()) {
                throw std::out_of_range ("index out of range");
            }
            else {
                return *(dataBegin+k);
            }
        }
        
        /*vector( const const_iterator & itb, const const_iterator &ite )*/
    };
    
} //namespace epl

#endif