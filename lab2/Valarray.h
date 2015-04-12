#ifndef _Valarray_h
#define _Valarray_h
#include "Vector.h"
#include <vector>
//using std::vector; // during development and testing
using epl::vector; // after submission

// If operand is a proxy, we should use its value instead of its reference.
template <typename T> struct to_ref { using type = T; };

template <typename T> struct to_ref<vector<T>> { using type = const vector<T>&; };  // Here const is important!!!

template <typename T> using Ref = typename to_ref<T>::type;   // type alias

// Choose appropriate return type
template <typename T> struct rank { static constexpr int value = 0; };
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

template <typename T1, typename T2>
using get_type = typename choose_type<typename T1::value_type, typename T2::value_type>::type;

template <bool p, typename T>
using EnableIf = typename std::enable_if<p, T>::type;

template <typename V> struct vec_wrap;

template <typename T>
using valarray = vec_wrap<vector<T>>;

// vector concepts'(proxy, scalar, etc) const_iterator
template <typename V>
class const_iterator {
private:
    const V parent;
    uint64_t index;
public:
    const_iterator(const V p, const uint64_t& i) : parent(p), index(i) {}
    typename V::value_type operator*() const { return parent[index]; }
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

// user-defined function object
template <typename T>
struct root {
    using result_type = typename ctype<is_complex<T>::value, double>::type;
    result_type operator() (T x) const { return sqrt(x); }
};

// vector, BinaryProxy, UnaryProxy, Scalar are all VECTOR CONCEPT.
template <typename Left, typename Right, typename FUN>
class BinaryProxy {
public:
    Ref<Left> _l;
    Ref<Right> _r;
    FUN _op;
public:
    using value_type = get_type<Left, Right>;
    BinaryProxy() = default;
    BinaryProxy(Ref<Left> l, Ref<Right> r, FUN op): _l(l), _r(r), _op(op) {}
    BinaryProxy(const BinaryProxy& that) : _l(that._l), _r(that._r), _op(that._op) {}
    ~BinaryProxy() = default;
    uint64_t size() const { return _l.size() < _r.size() ? _l.size() : _r.size(); }
    typename FUN::result_type operator[](uint64_t idx) const { return _op(_l[idx], _r[idx]); };
    const_iterator<BinaryProxy> begin() const { return const_iterator<BinaryProxy>(*this, 0); };
    const_iterator<BinaryProxy> end() const { return const_iterator<BinaryProxy>(*this, size()); };
};


template <typename T, typename FUN>
class UnaryProxy {
public:
    Ref<T> _val;
    FUN _op;
public:
    using value_type = typename T::value_type;
    UnaryProxy() = default;
    UnaryProxy(Ref<T> val, FUN op): _val(val), _op(op) {}
    UnaryProxy(const UnaryProxy& that): _val(that._val), _op(that._op) {}
    ~UnaryProxy() =default;
    uint64_t size() const { return _val.size(); }
    typename FUN::result_type operator[](uint64_t idx) const { return _op(_val[idx]); };
    const_iterator<UnaryProxy> begin() const { return const_iterator<UnaryProxy>(*this, 0); };
    const_iterator<UnaryProxy> end() const { return const_iterator<UnaryProxy>(*this, size()); };
};

template <typename T>
class Scalar {
private:
    T _val;
public:
    using value_type = T;
    Scalar() : _val{0} {}
    Scalar(const T& val): _val(val) {}
    Scalar(const Scalar& that): _val(that[0]) {}
    ~Scalar() = default;
    uint64_t size() const { return std::numeric_limits<uint64_t>::max(); }
    T operator[] (uint64_t idx) const { return _val; }  // rvalue
    const_iterator<Scalar> begin() const { return const_iterator<Scalar>(*this, 0); };
    const_iterator<Scalar> end() const { return const_iterator<Scalar>(*this, size()); };
};

// vector (concept) wrapper
template <typename V>
struct vec_wrap : public V {
    vec_wrap() : V() {}

    vec_wrap(const V& that) : V(that) {}
    
    explicit vec_wrap(uint64_t sz) : V(sz) {}
    
    template <typename T>
    vec_wrap(std::initializer_list<T> list) : V(list) {}
    
    template <typename RHS>
    vec_wrap(const vec_wrap<RHS>& that ) {
        for (auto val : that) {
            this->push_back( static_cast<typename V::value_type>(val) );
        }
    }

    ~vec_wrap() = default;
    
    vec_wrap& operator=(const vec_wrap<V>& that) {
        if ((void*)this != (void*)&that) {
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for (uint64_t i = 0; i < min_size; ++i) {
                (*this)[i] = static_cast<typename V::value_type>(that[i]);
            }
        }
        return *this;
    }
    
    template <typename RHS>
    vec_wrap& operator=(const vec_wrap<RHS>& that) {
        if ((void*)this != (void*)&that) {
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for (uint64_t i = 0; i < min_size; ++i) {
                (*this)[i] = static_cast<typename V::value_type>(that[i]);
            }
        }
        return *this;
    }
    
    template <typename FUN>
    typename FUN::result_type accumulate(FUN f) {
        typename FUN::result_type sum((*this)[0]);
        for (auto it = (++this->begin()); it != this->end(); ++it)
            sum = f(sum, *it);
        return sum;
    }
    
    typename std::plus<typename V::value_type>::result_type sum() {
        return accumulate(std::plus<typename V::value_type>{});
    }
    
    template <typename T>
    EnableIf<rank<T>::value, void> operator=(const T& that) {
        this->operator=(vec_wrap<Scalar<T>>(that));
    }
    
    template <typename FUN>
    vec_wrap<UnaryProxy<V, FUN>> apply(FUN f) {
        return vec_wrap<UnaryProxy<V, FUN>>( UnaryProxy<V, FUN>(*this, f) );
    }
    
    vec_wrap<UnaryProxy<V, std::negate<typename V::value_type>>> operator-(void) {
        return apply(std::negate<typename V::value_type>{});
    }
    
    vec_wrap<UnaryProxy<V, root<typename V::value_type>>> sqrt(void) {
        return apply(root<typename V::value_type>{});
    }
};

/******************************************/
// binary operations
/******************************************/
// Add
template <typename V1, typename V2, typename FUN = std::plus<get_type<V1, V2>>>
vec_wrap<BinaryProxy<V1, V2, FUN>> operator+(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
    return vec_wrap<BinaryProxy<V1, V2, FUN>>(BinaryProxy<V1, V2, FUN>(lhs, rhs, FUN{}));
}
template <typename T, typename V, typename FUN = std::plus<get_type<Scalar<T>, V>>>
EnableIf<rank<T>::value, vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>>
operator+(const T& lhs, const vec_wrap<V>& rhs) {
    return vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>(BinaryProxy<Scalar<T>, V, FUN>(vec_wrap<Scalar<T>>(lhs), rhs, FUN{}));
}
template <typename V, typename T, typename FUN = std::plus<get_type<V, Scalar<T>>>>
EnableIf<rank<T>::value, vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>>
operator+(const vec_wrap<V>& lhs, const T& rhs) {
    return vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>(BinaryProxy<V, Scalar<T>, FUN>(lhs, vec_wrap<Scalar<T>>(rhs), FUN{}));
}

// Sub
template <typename V1, typename V2, typename FUN = std::minus<get_type<V1, V2>>>
vec_wrap<BinaryProxy<V1, V2, FUN>> operator-(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
    return vec_wrap<BinaryProxy<V1, V2, FUN>>(BinaryProxy<V1, V2, FUN>(lhs, rhs, FUN{}));
}
template <typename T, typename V, typename FUN = std::minus<get_type<Scalar<T>, V>>>
EnableIf<rank<T>::value, vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>>
operator-(const T& lhs, const vec_wrap<V>& rhs) {
    return vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>(BinaryProxy<Scalar<T>, V, FUN>(vec_wrap<Scalar<T>>(lhs), rhs, FUN{}));
}
template <typename V, typename T, typename FUN = std::minus<get_type<V, Scalar<T>>>>
EnableIf<rank<T>::value, vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>>
operator-(const vec_wrap<V>& lhs, const T& rhs) {
    return vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>(BinaryProxy<V, Scalar<T>, FUN>(lhs, vec_wrap<Scalar<T>>(rhs), FUN{}));
}

// Mul
template <typename V1, typename V2, typename FUN = std::multiplies<get_type<V1, V2>>>
vec_wrap<BinaryProxy<V1, V2, FUN>> operator*(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
    return vec_wrap<BinaryProxy<V1, V2, FUN>>(BinaryProxy<V1, V2, FUN>(lhs, rhs, FUN{}));
}
template <typename T, typename V, typename FUN = std::multiplies<get_type<Scalar<T>, V>>>
EnableIf<rank<T>::value, vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>>
operator*(const T& lhs, const vec_wrap<V>& rhs) {
    return vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>(BinaryProxy<Scalar<T>, V, FUN>(vec_wrap<Scalar<T>>(lhs), rhs, FUN{}));
}
template <typename V, typename T, typename FUN = std::multiplies<get_type<V, Scalar<T>>>>
EnableIf<rank<T>::value, vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>>
operator*(const vec_wrap<V>& lhs, const T& rhs) {
    return vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>(BinaryProxy<V, Scalar<T>, FUN>(lhs, vec_wrap<Scalar<T>>(rhs), FUN{}));
}

// Div
template <typename V1, typename V2, typename FUN = std::divides<get_type<V1, V2>>>
vec_wrap<BinaryProxy<V1, V2, FUN>> operator/(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
    return vec_wrap<BinaryProxy<V1, V2, FUN>>(BinaryProxy<V1, V2, FUN>(lhs, rhs, FUN{}));
}
template <typename T, typename V, typename FUN = std::divides<get_type<Scalar<T>, V>>>
EnableIf<rank<T>::value, vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>>
operator/(const T& lhs, const vec_wrap<V>& rhs) {
    return vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>(BinaryProxy<Scalar<T>, V, FUN>(vec_wrap<Scalar<T>>(lhs), rhs, FUN{}));
}
template <typename V, typename T, typename FUN = std::divides<get_type<V, Scalar<T>>>>
EnableIf<rank<T>::value, vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>>
operator/(const vec_wrap<V>& lhs, const T& rhs) {
    return vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>(BinaryProxy<V, Scalar<T>, FUN>(lhs, vec_wrap<Scalar<T>>(rhs), FUN{}));
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