#ifndef _Valarray_h
#define _Valarray_h
#include "Vector.h"
#include <vector>
#include <complex>
#include <limits>
#include <cmath>
#include <functional>

using epl::vector; 

/* type promotion */
template <typename T> struct to_rank;
template <typename T> struct to_rank<std::complex<T>> { static constexpr int value = to_rank<T>::value; };
template <> struct to_rank<int> { static constexpr int value = 1; };
template <> struct to_rank<float> { static constexpr int value = 2; };
template <> struct to_rank<double> { static constexpr int value = 3; };

template <typename T> struct is_complex : public std::false_type {};
template <typename T> struct is_complex<std::complex<T>> : public std::true_type {};

template <int RANK> struct to_type;
template <> struct to_type<1> { using type = int; };
template <> struct to_type<2> { using type = float; };
template <> struct to_type<3> { using type = double; };

template <bool IS_COMPLEX, typename T> struct to_complex;
template <typename T> struct to_complex<false, T> { using type = T; };
template <typename T> struct to_complex<true, T> { using type = std::complex<T>; };

template <typename T1, typename T2>
struct _choose_type {
	static constexpr int t1_rank = to_rank<T1>::value;
	static constexpr int t2_rank = to_rank<T2>::value;
	static constexpr int result_rank = std::max(t1_rank, t2_rank);

	static constexpr bool t1_is_complex = is_complex<T1>::value;
	static constexpr bool t2_is_complex = is_complex<T2>::value;
	static constexpr bool result_is_complex = t1_is_complex || t2_is_complex;

	using primitive_type = typename to_type<result_rank>::type;
	using type = typename to_complex<result_is_complex, primitive_type>::type;
};

template <typename T1, typename T2>
using choose_type = typename _choose_type<T1, T2>::type;

template <typename V>
class iterator_helper {
private:
	V const* ptr;
	uint64_t index;
public:
	iterator_helper(void) : ptr{ nullptr }, index{ 0 } {}
	iterator_helper(V const* p, uint64_t k) : ptr{ p }, index{ k } {}
	iterator_helper(iterator_helper const& that) : ptr{ that.ptr }, index{ that.index } {}
	auto operator*(void) {
		return (*ptr)[index];
	}
	iterator_helper& operator++(void) {
		index += 1;
		return *this;
	}
	iterator_helper operator++(int) {
		iterator_helper t{ *this };
		operator++();
		return t;
	}
	bool operator==(iterator_helper const& that) const {
		return ptr == that.ptr && index == that.index;
	}
	bool operator!=(iterator_helper const& that) const {
		return !(*this == that);
	}
};

/*
* template meta function to decide the type (reference or object)
* of left and right operands in binary_proxy.we want to store a reference
* to a valarray i.e. vector, but we need to store a copy of binary_proxy,
* because intermediate binary_proxy would vanish after being used
*/
template <typename T> struct _ref { using type = T; };
template <typename T> struct _ref<vector<T>> { using type = vector<T> const&; };
template <typename T> using ref = typename _ref<T>::type;

/* binary_proxy */
template <typename LHS, typename RHS, typename FUN>
class binary_proxy {
private:
	ref<LHS> lhs;
	ref<RHS> rhs;
	FUN f;
public:
	binary_proxy(ref<LHS> x, ref<RHS> y, FUN op) : lhs{ x }, rhs{ y }, f{ op } {}
	binary_proxy(binary_proxy const& that) : lhs{ that.lhs }, rhs{ that.rhs }, f{ that.f } {}
	/* vector concept interface */
	using const_iterator = iterator_helper<binary_proxy>;
	using value_type = choose_type<typename LHS::value_type, typename RHS::value_type>;
	uint64_t size(void) const {
		return std::min(lhs.size(), rhs.size());
	}
	auto operator[](uint64_t k) const {
		return f(static_cast<value_type>(lhs[k]), static_cast<value_type>(rhs[k]));
	}
	const_iterator begin(void) const {
		return const_iterator(this, 0);
	}
	const_iterator end(void) const {
		return const_iterator(this, size());
	}
};

/* unary_proxy */
template <typename V, typename FUN>
class unary_proxy {
private:
	ref<V> val;
	FUN f;
public:
	unary_proxy(ref<V> x, FUN op) : val{ x }, f{op} {}
	unary_proxy(unary_proxy const& that) : val{ that.val }, f{ that.f } {}
	/* vector concept interface */
	using const_iterator = iterator_helper<unary_proxy>;
	using value_type = typename V::value_type;
	uint64_t size(void) const {
		return val.size();
	}
	auto operator[](uint64_t k) const {
		return f(val[k]);
	}
	const_iterator begin(void) const {
		return const_iterator(this, 0);
	}
	const_iterator end(void) const {
		return const_iterator(this, size());
	}
};

/* scalar */
template <typename T>
class scalar {
private:
	T val;
public:
	scalar(T const& x) : val{ x } {}
	scalar(scalar const& that) : val{ that[0] } {}
	/* vector concept interface */
	using const_iterator = iterator_helper<scalar>;
	using value_type = T;
	uint64_t size(void) const {
		return std::numeric_limits<uint64_t>::max();
	}
	auto operator[](uint64_t) const {
		return val;
	}
	const_iterator begin(void) const {
		return const_iterator(this, 0);
	}
	const_iterator end(void) const {
		return const_iterator(this, size());
	}
};


template <typename T>
struct root {
	constexpr auto operator() (T const& x) const { return sqrt(x); }
};

/*                  vector concept wrapper
* vector concept should at least provide these interfaces:
* 1. value_type
* 2. size()
* 3. operator[](uint64_t k)
* 4. begin()
* 5. end()
*/
template <typename V>
class wrap : public V {
public:
	using V::V;
	using value_type = typename V::value_type;
	wrap(void) : V{} {}
	wrap(V const& that) : V{ that } {}

	/* Three cases:
	* 1. construct a proxy using a proxy - copy ctor
	* 2. construct a valarray using a valarray - copy ctor
	* 3. construct a valarray using a proxy - special ctor
	*/
	wrap(wrap const& that) : V{ that } {}

	template <typename T>
	wrap(wrap<T> const& that) : V(that.size()) {
		for (auto i = 0; i < this->size(); i += 1) {
			(*this)[i] = static_cast<value_type>(that[i]);
		}
	}

	/* Two cases:
	* 1. assign a valarray to a valarray
	* 2. assign a proxy to a valarray
	* note: proxy is not mutable
	*/
	template <typename T>
	wrap& operator=(wrap<T> const& that) {
		auto size = std::min(this->size(), that.size());
		for (auto i = 0; i < size; i += 1) {
			(*this)[i] = static_cast<value_type>(that[i]);
		}
		return *this;
	}

	template <typename T, typename SFINAE = typename to_type<to_rank<T>::value>::type>
	void operator=(T const& that) {
		this->operator=(wrap<scalar<T>>(that));
	}

	template <typename FUN>
	auto accumulate(FUN f) const {
		if (!this->size()) return 0;
		value_type result{ (*this)[0] };
		for (auto i = 1; i < this->size(); i += 1) {
			result = f(result, (*this)[i]);
		}
		return result;
	}

	template <typename FUN>
	auto apply(FUN f) const {
		V const& self{ *this };
		unary_proxy<V, FUN> proxy{ self, f };
		return wrap<unary_proxy<V, FUN>>{ proxy };
	}

	auto sum(void) const {
		return accumulate(std::plus<void>{});
	}

	auto operator-(void) const {
		return apply(std::negate<void>{});
	}

	auto sqrt(void) const {
		return apply(root<value_type>{});
	}

	void print(std::ostream& out) const {
		for (auto i = 0; i < this->size(); i += 1) {
			out << (*this)[i] << " ";
		}
		out << "\n";
	}
};

template <typename V1, typename V2, typename FUN>
auto lazy(wrap<V1> const& lhs, wrap<V2> const& rhs, FUN f) {
	V1 const& left{ lhs };
	V2 const& right{ rhs };
	binary_proxy<V1, V2, FUN> proxy{ left, right, f };
	return wrap< binary_proxy<V1, V2, FUN> >{ proxy };
};

template <typename V1, typename T, typename FUN, typename SFINAE = typename to_type<to_rank<T>::value>::type>
auto lazy(wrap<V1> const& lhs, T const& rhs, FUN f) {
	using V2 = scalar<T>;
	V1 const& left{ lhs };
	V2 right{ rhs };
	binary_proxy<V1, V2, FUN> proxy{ left, right, f };
	return wrap<binary_proxy<V1, V2, FUN>>{ proxy };
};

template <typename T, typename V2, typename FUN, typename SFINAE = typename to_type<to_rank<T>::value>::type>
auto lazy(T const& lhs, wrap<V2> const& rhs, FUN f) {
	using V1 = scalar<T>;
	V1 left{ lhs };
	V2 const& right{ rhs };
	binary_proxy<V1, V2, FUN> proxy{ left, right, f };
	return wrap<binary_proxy<V1, V2, FUN>>{ proxy };
};


template <typename T1, typename T2>
auto operator+(T1 const& lhs, T2 const& rhs) {
	return lazy(lhs, rhs, std::plus<void>{});
};

template <typename T1, typename T2>
auto operator-(T1 const& lhs, T2 const& rhs) {
	return lazy(lhs, rhs, std::minus<void>{});
};

template <typename T1, typename T2>
auto operator*(T1 const& lhs, T2 const& rhs) {
	return lazy(lhs, rhs, std::multiplies<void>{});
};

template <typename T1, typename T2>
auto operator/(T1 const& lhs, T2 const& rhs) {
	return lazy(lhs, rhs, std::divides<void>{});
};

template <typename V>
std::ostream& operator<<(std::ostream& out, wrap<V> const& that) {
	that.print(out);
	return out;
}

template <typename T>
using valarray = wrap<vector<T>>;

#endif /* _Valarray_h */
