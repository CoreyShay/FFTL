#pragma once


#include "../defs.h"
#include <cmath>
#include <limits>
#include <type_traits>


namespace FFTL
{


//	Same as static_cast except it will assert in some builds if information is lost.
template <typename T_TO, typename T_FROM>
FFTL_NODISCARD constexpr typename std::enable_if<(std::is_same<T_TO, T_FROM>::value), T_TO>::type safestatic_cast(T_FROM a)
{
	return a;
}

template <typename T_TO, typename T_FROM>
FFTL_NODISCARD constexpr typename std::enable_if<(!std::is_same<T_TO, T_FROM>::value) && std::numeric_limits<T_TO>::is_integer && std::numeric_limits<T_FROM>::is_integer, T_TO>::type safestatic_cast(T_FROM a)
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(a == static_cast<T_FROM>(ret), "Casting has lost precision or overflowed / underflowed");
	return ret;
}

template <typename T_TO, typename T_FROM>
FFTL_NODISCARD constexpr typename std::enable_if<std::numeric_limits<T_TO>::is_integer && std::is_floating_point<T_FROM>::value, T_TO>::type safestatic_cast(T_FROM a)
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(std::abs(a - static_cast<T_FROM>(ret)) < static_cast<T_FROM>(1), "Casting has lost precision or overflowed / underflowed");
	return ret;
}

template <typename T_TO, typename T_FROM>
FFTL_NODISCARD constexpr typename std::enable_if<std::is_floating_point<T_TO>::value && std::numeric_limits<T_FROM>::is_integer, T_TO>::type safestatic_cast(T_FROM a)
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(std::abs(ret - static_cast<long double>(a)) < static_cast<long double>(1), "Casting has lost precision or overflowed / underflowed");
	return ret;
}

//	Gets the integral type. Useful for enum classes.
template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr typename std::underlying_type<T>::type underlying_cast(T e)
{
	return static_cast<typename std::underlying_type<T>::type>(e);
}

#if FFTL_HAS_CONSTEXPR_BIT_CAST
template <typename T_TO, typename T_FROM>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T_TO bit_cast_constexpr(const T_FROM& v)
{
#if defined(__clang__)
	return __builtin_bit_cast(T_TO, v);
#else
	return std::bit_cast<T_TO, T_FROM>(v);
#endif
}
#else
template <typename T_TO, typename T_FROM>
FFTL_NODISCARD FFTL_FORCEINLINE T_TO bit_cast_constexpr(const T_FROM& v)
{
	return *reinterpret_cast<const T_TO*>(&v);
}
#endif

template <typename T_TO, typename T_FROM>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR T_TO bit_cast(const T_FROM& v)
{
	return bit_cast_constexpr<T_TO, T_FROM>(v);
}

// -------------------------------------------------------------------
// --- Reversed iterable

template <typename T>
struct reversion_wrapper { T& iterable; };

template <typename T>
auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T>
auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T>
reversion_wrapper<T> reverse(T&& iterable) { return { iterable }; }


#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4308) // negative integral constant converted to unsigned type
#endif
template <auto T_START, auto T_END, auto T_INCREMENT, auto I, class T_FUNCTOR>
constexpr void constexpr_for_recurse(const T_FUNCTOR& func)
{
	if constexpr ((T_START < T_END && I < T_END) || (T_START > T_END && I > T_END))
	{
		func(std::integral_constant<decltype(I), I>());
		constexpr_for_recurse<T_START, T_END, T_INCREMENT, I + T_INCREMENT, T_FUNCTOR>(func);
	}
}
#ifdef _MSC_VER
#	pragma warning(pop)
#endif


//	Integral type for loop. Unrolled at compile time. 
/* Example usage:
template <int N>
constexpr std::array<float, N> make_data();

template <int T_START, int T_END>
constexpr float data_sum()
{
	float sum = 0;
	constexpr_for<T_START, T_END, 1>([&sum](auto i)
	{
		auto data = make_data<i>();
		for (auto v : data)
			sum += v;
	});
	return sum;
}
*/
template <auto T_START, auto T_END, auto T_INCREMENT, class T_FUNCTOR>
constexpr void constexpr_for(const T_FUNCTOR& func)
{
//	if constexpr ((T_START < T_END && I < T_END) || T_START > T_END && I > T_END)
//	{
//		func(std::integral_constant<decltype(T_START), T_START>());
//		constexpr_for<T_START, T_END, T_INCREMENT, I + T_INCREMENT, T_FUNCTOR>(func);
//	}

	constexpr_for_recurse<T_START, T_END, T_INCREMENT, T_START, T_FUNCTOR>(func);
}

//	Iterates over each argument
/* Example usage:
template <class... T_ARGS>
void print_all(T_ARGS const&... args)
{
	constexpr_for([](auto const& v)
	{
		std::cout << v << std::endl;
	}, args...);
}
*/
template <class T_FUNCTOR, class... T_ARGS>
constexpr void constexpr_for(T_FUNCTOR&& func, T_ARGS&&... args)
{
	(func(std::forward<T_ARGS>(args)), ...);
}

//	Iterates over each element of a tuple.
/* Example usage:
constexpr_for_tuple([](auto const& v)
{
	std::cout << v << std::endl;
}, std::make_tuple(1, 'c', true));
*/
template <class T_FUNCTOR, class T_TUPLE>
constexpr void constexpr_for_tuple(T_FUNCTOR&& func, T_TUPLE&& tuple)
{
	constexpr size_t cnt = std::tuple_size_v<std::decay_t<T_TUPLE>>;

	constexpr_for<size_t(0), cnt, size_t(1)>([&](auto i)
	{
		func(std::get<i.value>(tuple));
	});
}



}