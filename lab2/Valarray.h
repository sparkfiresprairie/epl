// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h
#include "Vector.h"
#include <vector>
//using std::vector; // during development and testing
using epl::vector; // after submission

/******************************************/
// If operand is a proxy, we should use its value instead of its reference.
/******************************************/
template <typename T> struct to_ref { using type = T; };

template <typename T> struct to_ref<vector<T>> { using type = const vector<T>&; };  // Here const is important!!!

template <typename T> using Ref = typename to_ref<T>::type;   // type alias
/******************************************/


/******************************************/
// Choose appropriate return type
/******************************************/
template <typename T>
struct rank;

template <> struct rank<int> { static constexpr int value = 1; };
template <> struct rank<float> { static constexpr int value = 2; };
template <> struct rank<double> { static constexpr int value = 3; };
template <typename T> struct rank<std::complex<T>> { static constexpr int value = rank<T>::value; };

template <int R>
struct stype;

template <> struct stype<1> { using type = int; };
template <> struct stype<2> { using type = float; };
template <> struct stype<3> { using type = double; };

template <typename T> struct is_complex : public std::false_type {};
template <typename T> struct is_complex<std::complex<T>> : public std::true_type {};

template <bool p, typename T> struct ctype;
template <typename T> struct ctype<true, T> { using type = std::complex<T>; };
template <typename T> struct ctype<false, T> { using type = T; };

template <typename T1, typename T2>
struct choose_type {
    static constexpr int t1_rank = rank<T1>::value;
    static constexpr int t2_rank = rank<T2>::value;
    static constexpr int max_rank = (t1_rank > t2_rank) ? t1_rank : t2_rank;
    
    using my_type = typename stype<max_rank>::type;
    
    static constexpr bool t1_complex = is_complex<T1>::value;
    static constexpr bool t2_complex = is_complex<T2>::value;
    static constexpr bool my_complex = t1_complex || t2_complex;
    
    using type = typename ctype<my_complex, my_type>::type;
};
/******************************************/


/******************************************/
// vector, BinaryProxy, UnaryProxy, Scalar are all VECTOR CONCEPT.
/******************************************/
template <typename Left, typename Right>
class BinaryProxy {
public:
    Ref<Left> _l;
    Ref<Right> _r;
    char _op;
public:
    using value_type = typename choose_type<typename Left::value_type, typename Right::value_type>::type;
    
    BinaryProxy() = default;
    BinaryProxy(Ref<Left> l, Ref<Right> r, char op): _l{l}, _r{r}, _op{op} {}
    BinaryProxy(const BinaryProxy& that) : _l{that._l}, _r{that._r}, _op{that._op} {}
    ~BinaryProxy() {}
    
    uint64_t size() const { return _l.size() < _r.size() ? _l.size() : _r.size(); }
    
    value_type operator[](uint64_t idx) const {
        switch (_op) {
            case '+': return (value_type)(_l[idx]) + (value_type)(_r[idx]);
            case '-': return (value_type)(_l[idx]) - (value_type)(_r[idx]);
            case '*': return (value_type)(_l[idx]) * (value_type)(_r[idx]);
            case '/': return (value_type)(_l[idx]) / (value_type)(_r[idx]);
            default: return 0;
        }
    };
    
    // const_iterator: we can't change the value it points to
    // const iterator: it can't change(such as ++, --)
    class const_iterator {
    private:
        const BinaryProxy<Left, Right> parent;
        uint64_t index;
    public:
        const_iterator() : parent{nullptr}, index{0} {}
        const_iterator(const BinaryProxy<Left, Right> p, const uint64_t& i) : parent{p}, index{i} {}
        value_type operator*() const { return parent[index]; }
        const_iterator& operator++() {
            index++;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator t{*this};
            this->operator++();
            return t;
        }
        const_iterator& operator--() {
            index--;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator t{*this};
            this->operator--();
            return t;
        }
        bool operator==(const const_iterator& that) const {
            return this->index == that.index;
        }
        bool operator!=(const const_iterator& that) const {
            return !(*this == that);
        }
    };
    // we have to pass value instead of by pointer, because proxy is a temp!!!
    // Besides since it is a proxy, copy is trivial.
    const_iterator begin() const { return const_iterator{*this, 0}; };
    const_iterator end() const { return const_iterator{*this, size()}; };
};

template <typename T, typename FUN>
class UnaryProxy {
public:
    Ref<T> _val;
    FUN _op;
public:
    using value_type = typename T::value_type;
    
    UnaryProxy() = default;
    // excess elements in struct initializer
    // http://stackoverflow.com/a/14990955
    UnaryProxy(Ref<T> val, FUN op): _val(val), _op(op) {}
    UnaryProxy(const UnaryProxy& that): _val(that._val), _op(that._op) {}
    ~UnaryProxy() {}
    
    uint64_t size() const { return _val.size(); }
    
    typename FUN::result_type operator[](uint64_t idx) const {
        return _op(_val[idx]);
    };
    
    class const_iterator {
    private:
        const UnaryProxy<T, FUN> parent;
        uint64_t index;
    public:
        const_iterator() : parent{nullptr}, index{0} {}
        const_iterator(const UnaryProxy<T, FUN> p, const uint64_t& i) : parent{p}, index{i} {}
        value_type operator*() const { return parent[index]; }
        const_iterator& operator++() {
            index++;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator t{*this};
            this->operator++();
            return t;
        }
        const_iterator& operator--() {
            index--;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator t{*this};
            this->operator--();
            return t;
        }
        bool operator==(const const_iterator& that) const {
            return this->index == that.index;
        }
        bool operator!=(const const_iterator& that) const {
            return !(*this == that);
        }
    };
    const_iterator begin() const { return const_iterator{*this, 0}; }
    const_iterator end() const { return const_iterator{*this, size()}; }
};

template <typename T>
class Scalar {
private:
    T _val;
public:
    using value_type = T;
    
    Scalar() : _val{0} {}
    Scalar(const T& val): _val{val} {}
    Scalar(const Scalar& that): _val{that[0]} {}
    ~Scalar() {}
    
    uint64_t size() const { return std::numeric_limits<uint64_t>::max(); }
    
    T operator[] (uint64_t idx) const { return _val; }  // rvalue
    
    class const_iterator {
    private:
        const Scalar<T> parent;
        uint64_t index;
    public:
        const_iterator() : parent{nullptr}, index{0} {}
        const_iterator(const Scalar<T> p, const uint64_t& i) : parent{p}, index{i} {}
        uint64_t operator*() { return parent[0]; }
        const_iterator& operator++() { return *this; }
        const_iterator operator++(int) { return *this; }
        const_iterator& operator--() { return *this; }
        const_iterator operator--(int) { return *this; }
        bool operator==(const const_iterator& that) const { return true; }
        bool operator!=(const const_iterator& that) const { return false; }
    };
    const_iterator begin() const { return const_iterator{*this, 0}; };
    const_iterator end() const { return const_iterator{*this, 0}; };
};
/******************************************/


/******************************************/
// function object
/******************************************/
template <typename T>
struct root {
    using result_type = typename ctype<is_complex<T>::value, double>::type;
    result_type operator() (T x) const { return sqrt(x); }
};

/******************************************/


/******************************************/
// vector (concept) wrapper
/******************************************/
template <typename V>
struct vec_wrap : public V {
    vec_wrap() : V() {}
    vec_wrap(const V& that) : V(that) {}
    ~vec_wrap() {}
    
    template <typename FUN>
    typename FUN::result_type accumulate(FUN f) {
        typename FUN::result_type sum{(*this)[0]};
        for (auto it = (++this->begin()); it != this->end(); ++it)
            sum = f(sum, *it);
        return sum;
    }
    
    typename std::plus<typename V::value_type>::result_type sum() {
        return accumulate(std::plus<typename V::value_type>{});
    }
    
    template <typename FUN>
    vec_wrap<UnaryProxy<V, FUN>> apply(FUN f) {
        return vec_wrap<UnaryProxy<V, FUN>>{ UnaryProxy<V, FUN>{*this, f} };
    }
    
    vec_wrap<UnaryProxy<V, std::negate<typename V::value_type>>> operator-(void) {
        return apply(std::negate<typename V::value_type>{});
    }
    
    vec_wrap<UnaryProxy<V, root<typename V::value_type>>> sqrt(void) {
        return apply(root<typename V::value_type>{});
    }
    
};

template <typename T>
struct vec_wrap<vector<T>> : public vector<T> {
    vec_wrap<vector<T>>() : vector<T>() {}
    explicit vec_wrap<vector<T>>(uint64_t sz) : vector<T>(sz) {}
    vec_wrap<vector<T>>(std::initializer_list<T> list) : vector<T>(list) {}
    vec_wrap<vector<T>>(const vec_wrap<vector<T>>& that) : vector<T>(that) {}
    template <typename V>
    vec_wrap<vector<T>>(const vec_wrap<V>& that ) {
        for (auto val : that) {
            this->push_back((T)val);
        }
    }
    ~vec_wrap<vector<T>>() {}
    
    vec_wrap<vector<T>>& operator=(const vec_wrap<vector<T>>& that) {
        if ((void*)this != (void*)&that) {
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for (uint64_t i = 0; i < min_size; ++i) {
                (*this)[i] = (T)(that[i]);
            }
        }
        return *this;
    }
    
    template <typename V>
    vec_wrap<vector<T>>& operator=(const vec_wrap<V>& that) {
        if ((void*)this != (void*)&that) {
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for (uint64_t i = 0; i < min_size; ++i) {
                (*this)[i] = (T)(that[i]);
            }
        }
        return *this;
    }
    
    void operator=(const T& that) {
        this->operator=( vec_wrap<Scalar<T>>{that} );
    }
    
    template <typename FUN>
    typename FUN::result_type accumulate(FUN f) {
        typename FUN::result_type sum{(*this)[0]};
        for (auto it = (++this->begin()); it != this->end(); ++it)
            sum = f(sum, *it);
        return sum;
    }
    
    typename std::plus<T>::result_type sum() {
        return accumulate(std::plus<T>{});
    }
    
    template <typename FUN>
    vec_wrap<UnaryProxy<vector<T>, FUN>> apply(FUN f) {
        return vec_wrap<UnaryProxy<vector<T>, FUN>>{ UnaryProxy<vector<T>, FUN>{*this, f} };
    }
    
    vec_wrap<UnaryProxy<vector<T>, std::negate<typename vector<T>::value_type>>> operator-(void) {
        return apply(std::negate<typename vector<T>::value_type>{});
    }
    
    vec_wrap<UnaryProxy<vector<T>, root<typename vector<T>::value_type>>> sqrt(void) {
        return apply(root<typename vector<T>::value_type>{});
    }

};
/******************************************/


/******************************************/
// type alias
/******************************************/
template <typename T>
using valarray = vec_wrap<vector<T>>;
/******************************************/

/******************************************/
// binary operations
/******************************************/
// Add
template <typename V1, typename V2>
vec_wrap<BinaryProxy<V1, V2>> operator+(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
    return vec_wrap<BinaryProxy<V1,V2>>{ BinaryProxy<V1, V2>{lhs, rhs, '+'} };
}
template <typename T, typename V>
vec_wrap<BinaryProxy<Scalar<T>, V>> operator+(const T& lhs, const vec_wrap<V>& rhs) {
    return vec_wrap<BinaryProxy<Scalar<T>, V>>{ BinaryProxy<Scalar<T>, V>{vec_wrap<Scalar<T>>{lhs}, rhs, '+'} };
}
template <typename V, typename T>
vec_wrap<BinaryProxy<V, Scalar<T>>> operator+(const vec_wrap<V>& lhs, const T& rhs) {
    return vec_wrap<BinaryProxy<V, Scalar<T>>>{ BinaryProxy<V, Scalar<T>>{lhs, vec_wrap<Scalar<T>>{rhs}, '+'} };
}

// Sub
template <typename V1, typename V2>
vec_wrap<BinaryProxy<V1, V2>> operator-(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
    return vec_wrap<BinaryProxy<V1,V2>>{ BinaryProxy<V1, V2>{lhs, rhs, '-'} };
}
template <typename T, typename V>
vec_wrap<BinaryProxy<Scalar<T>, V>> operator-(const T& lhs, const vec_wrap<V>& rhs) {
    return vec_wrap<BinaryProxy<Scalar<T>, V>>{ BinaryProxy<Scalar<T>, V>{vec_wrap<Scalar<T>>{lhs}, rhs, '-'} };
}
template <typename V, typename T>
vec_wrap<BinaryProxy<V, Scalar<T>>> operator-(const vec_wrap<V>& lhs, const T& rhs) {
    return vec_wrap<BinaryProxy<V, Scalar<T>>>{ BinaryProxy<V, Scalar<T>>{lhs, vec_wrap<Scalar<T>>{rhs}, '-'} };
}

// Mul
template <typename V1, typename V2>
vec_wrap<BinaryProxy<V1, V2>> operator*(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
    return vec_wrap<BinaryProxy<V1,V2>>{ BinaryProxy<V1, V2>{lhs, rhs, '*'} };
}
template <typename T, typename V>
vec_wrap<BinaryProxy<Scalar<T>, V>> operator*(const T& lhs, const vec_wrap<V>& rhs) {
    return vec_wrap<BinaryProxy<Scalar<T>, V>>{ BinaryProxy<Scalar<T>, V>{vec_wrap<Scalar<T>>{lhs}, rhs, '*'} };
}
template <typename V, typename T>
vec_wrap<BinaryProxy<V, Scalar<T>>> operator*(const vec_wrap<V>& lhs, const T& rhs) {
    return vec_wrap<BinaryProxy<V, Scalar<T>>>{ BinaryProxy<V, Scalar<T>>{lhs, vec_wrap<Scalar<T>>{rhs}, '*'} };
}

// Div
template <typename V1, typename V2>
vec_wrap<BinaryProxy<V1, V2>> operator/(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
    return vec_wrap<BinaryProxy<V1,V2>>{ BinaryProxy<V1, V2>{lhs, rhs, '/'} };
}
template <typename T, typename V>
vec_wrap<BinaryProxy<Scalar<T>, V>> operator/(const T& lhs, const vec_wrap<V>& rhs) {
    return vec_wrap<BinaryProxy<Scalar<T>, V>>{ BinaryProxy<Scalar<T>, V>{vec_wrap<Scalar<T>>{lhs}, rhs, '/'} };
}
template <typename V, typename T>
vec_wrap<BinaryProxy<V, Scalar<T>>> operator/(const vec_wrap<V>& lhs, const T& rhs) {
    return vec_wrap<BinaryProxy<V, Scalar<T>>>{ BinaryProxy<V, Scalar<T>>{lhs, vec_wrap<Scalar<T>>{rhs}, '/'} };
}

// Output
template <typename V>
std::ostream& operator<<(std::ostream& out, const vec_wrap<V>& v) {
    for (uint64_t i = 0; i < v.size(); ++i) {
        out << v[i] << " ";
    }
    out << "\n";
    return out;
}
#endif /* _Valarray_h */