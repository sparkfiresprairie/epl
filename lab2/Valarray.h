#ifndef _Valarray_h
#define _Valarray_h
#include "Vector.h"
#include <vector>
//using std::vector; // during development and testing
using epl::vector; // after submission

template <typename V> struct vec_wrap;

// If operand is a proxy, we should use its value instead of its reference.
template <typename T> struct to_ref { using type = T; };
template <typename T> struct to_ref<vector<T>> { using type = const vector<T>&; };  // Here const is important!!!
template <typename T> using Ref = typename to_ref<T>::type;   // type alias

// Choose appropriate return type
template <typename T> struct rank { static constexpr int value = 0; };  // used in EnableIf
template <> struct rank<int> { static constexpr int value = 1; };
template <> struct rank<float> { static constexpr int value = 2; };
template <> struct rank<double> { static constexpr int value = 3; };
template <typename T> struct rank<std::complex<T>> { static constexpr int value = rank<T>::value; };

template <int R> struct stype;
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


// get appropriate value type of T1, T2. T1, T2 can be vec_wrap, primitive types, etc.
template <typename T1, typename T2> struct get_type;
template <typename V1, typename V2>
struct get_type<vec_wrap<V1>, vec_wrap<V2>> { using type = typename choose_type<typename V1::value_type, typename V2::value_type>::type; };
template <typename V, typename T>
struct get_type<vec_wrap<V>, T> { using type = typename choose_type<typename V::value_type, T>::type; };
template <typename T, typename V>
struct get_type<T, vec_wrap<V>> { using type = typename choose_type<T, typename V::value_type>::type; };

template <bool p, typename T>
using EnableIf = typename std::enable_if<p, T>::type;

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
    using value_type = typename choose_type<typename Left::value_type, typename Right::value_type>::type;;
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
    ~UnaryProxy() = default;
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
    
    vec_wrap& operator=(const vec_wrap<V>& that) {  // This is for case x = y, both x and y are valarray<int>
        if ((void*)this != (void*)&that) {
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for (uint64_t i = 0; i < min_size; ++i) {
                (*this)[i] = static_cast<typename V::value_type>(that[i]);
            }
        }
        return *this;
    }
    
    template <typename RHS>
    vec_wrap& operator=(const vec_wrap<RHS>& that) {    // This is for case x = y, y is any type of vec_wrap but x's
        if ((void*)this != (void*)&that) {
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for (uint64_t i = 0; i < min_size; ++i) {
                (*this)[i] = static_cast<typename V::value_type>(that[i]);
            }
        }
        return *this;
    }
    
    template <typename T>
    EnableIf<rank<T>::value, void> operator=(const T& that) {   // Use EnableIf in case of meaningless x = foo
        this->operator=(vec_wrap<Scalar<T>>(that));
    }
    
    template <typename FUN>
    typename FUN::result_type accumulate(FUN f) {
        typename FUN::result_type sum((*this)[0]);
        for (auto it = ++(this->begin()); it != this->end(); ++it)
            sum = f(sum, *it);
        return sum;
    }
    
    typename std::plus<typename V::value_type>::result_type sum() {
        return accumulate(std::plus<typename V::value_type>{});
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

// choose appropiate doit function base on operator's operands: lhs, rhs.
template <typename FUN>
struct RetType {
    template <typename V1, typename V2>
    static vec_wrap<BinaryProxy<V1, V2, FUN>> doit(const vec_wrap<V1>& lhs, const vec_wrap<V2>& rhs) {
        return vec_wrap<BinaryProxy<V1, V2, FUN>>(BinaryProxy<V1, V2, FUN>(lhs, rhs, FUN{}));
    }
    
    template <typename T, typename V>
    static EnableIf<rank<T>::value!=0, vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>> doit(const T& lhs, const vec_wrap<V>& rhs) {
        return vec_wrap<BinaryProxy<Scalar<T>, V, FUN>>(BinaryProxy<Scalar<T>, V, FUN>(vec_wrap<Scalar<T>>(lhs), rhs, FUN{}));
    }

    template <typename V, typename T>
    static EnableIf<rank<T>::value!=0, vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>> doit(const vec_wrap<V>& lhs, const T& rhs) {
        return vec_wrap<BinaryProxy<V, Scalar<T>, FUN>>(BinaryProxy<V, Scalar<T>, FUN>(lhs, vec_wrap<Scalar<T>>(rhs), FUN{}));
    }
};

template <typename T1, typename T2, typename FUN = std::plus<typename get_type<T1, T2>::type>>
auto operator+(const T1& lhs, const T2& rhs) -> decltype(RetType<FUN>::doit(lhs, rhs)) {
    return RetType<FUN>::doit(lhs, rhs);
}

template <typename T1, typename T2, typename FUN = std::minus<typename get_type<T1, T2>::type>>
auto operator-(const T1& lhs, const T2& rhs) -> decltype(RetType<FUN>::doit(lhs, rhs)) {
    return RetType<FUN>::doit(lhs, rhs);
}

template <typename T1, typename T2, typename FUN = std::multiplies<typename get_type<T1, T2>::type>>
auto operator*(const T1& lhs, const T2& rhs) -> decltype(RetType<FUN>::doit(lhs, rhs)) {
    return RetType<FUN>::doit(lhs, rhs);
}

template <typename T1, typename T2, typename FUN = std::divides<typename get_type<T1, T2>::type>>
auto operator/(const T1& lhs, const T2& rhs) -> decltype(RetType<FUN>::doit(lhs, rhs)) {
    return RetType<FUN>::doit(lhs, rhs);
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