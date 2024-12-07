#pragma once


#include "../defs.h"
#include <cmath>
#include <limits>
#include <type_traits>
#include <utility>

#if FFTL_CPP_VERSION >= 2020 && __has_include(<bit>)
#	include <bit>
#	define FFTL_HAS_BIT_CAST_HEADER 1
#endif


namespace FFTL
{


//	Same as static_cast except it will assert in some builds if information is lost.
template <typename T_TO, typename T_FROM>
FFTL_NODISCARD constexpr typename std::enable_if<(std::is_same<T_TO, T_FROM>::value), T_TO>::type safestatic_cast(T_FROM a) noexcept
{
	return a;
}

template <typename T_TO, typename T_FROM>
FFTL_NODISCARD constexpr typename std::enable_if<(!std::is_same<T_TO, T_FROM>::value) && std::numeric_limits<T_TO>::is_integer && std::numeric_limits<T_FROM>::is_integer, T_TO>::type safestatic_cast(T_FROM a) noexcept
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(a == static_cast<T_FROM>(ret), "Casting has lost precision or overflowed / underflowed");
	return ret;
}

template <typename T_TO, typename T_FROM>
FFTL_NODISCARD constexpr typename std::enable_if<std::numeric_limits<T_TO>::is_integer && std::is_floating_point<T_FROM>::value, T_TO>::type safestatic_cast(T_FROM a) noexcept
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(std::abs(a - static_cast<T_FROM>(ret)) < static_cast<T_FROM>(1), "Casting has lost precision or overflowed / underflowed");
	return ret;
}

template <typename T_TO, typename T_FROM>
FFTL_NODISCARD constexpr typename std::enable_if<std::is_floating_point<T_TO>::value && std::numeric_limits<T_FROM>::is_integer, T_TO>::type safestatic_cast(T_FROM a) noexcept
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(std::abs(ret - static_cast<long double>(a)) < static_cast<long double>(1), "Casting has lost precision or overflowed / underflowed");
	return ret;
}

//	Gets the integral type. Useful for enum classes.
template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
FFTL_NODISCARD constexpr typename std::underlying_type<T>::type underlying_cast(T e) noexcept
{
	return static_cast<typename std::underlying_type<T>::type>(e);
}

#if FFTL_HAS_CONSTEXPR_BIT_CAST
template <typename T_TO, typename T_FROM>
FFTL_NODISCARD FFTL_FORCEINLINE constexpr T_TO bit_cast_constexpr(const T_FROM& v) noexcept
{
#if FFTL_HAS_BUILTIN(__builtin_bit_cast)
	return __builtin_bit_cast(T_TO, v);
#elif defined(FFTL_HAS_BIT_CAST_HEADER)
	return std::bit_cast<T_TO, T_FROM>(v);
#else
#	error "Missing bit_cast implementation"
#endif
}
#else
template <typename T_TO, typename T_FROM>
FFTL_NODISCARD FFTL_FORCEINLINE T_TO bit_cast_constexpr(const T_FROM& v) noexcept
{
	return *reinterpret_cast<const T_TO*>(&v);
}
#endif

template <typename T_TO, typename T_FROM>
FFTL_NODISCARD FFTL_FORCEINLINE FFTL_BIT_CAST_CONSTEXPR T_TO bit_cast(const T_FROM& v) noexcept
{
	return bit_cast_constexpr<T_TO, T_FROM>(v);
}

// -------------------------------------------------------------------
// --- Reversed iterable

template <typename T>
struct reversion_wrapper { T& iterable; };

template <typename T>
auto begin(reversion_wrapper<T> w) noexcept { return w.iterable.rbegin(); }

template <typename T>
auto end(reversion_wrapper<T> w) noexcept { return w.iterable.rend(); }

template <typename T>
reversion_wrapper<T> reverse(T&& iterable) noexcept { return { iterable }; }


#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4308) // negative integral constant converted to unsigned type
#endif
template <auto T_START, auto T_END, auto T_INCREMENT, auto I, class T_FUNCTOR>
constexpr void constexpr_for_recurse(const T_FUNCTOR& func) noexcept
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
constexpr void constexpr_for(const T_FUNCTOR& func) noexcept
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
constexpr void constexpr_for(T_FUNCTOR&& func, T_ARGS&&... args) noexcept
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
constexpr void constexpr_for_tuple(T_FUNCTOR&& func, T_TUPLE&& tuple) noexcept
{
	constexpr size_t cnt = std::tuple_size_v<std::decay_t<T_TUPLE>>;

	constexpr_for<size_t(0), cnt, size_t(1)>([&](auto i)
	{
		func(std::get<i.value>(tuple));
	});
}


//	Static log2 for compile time only
template <size_t T_VALUE> struct log2st { static constexpr size_t value = 1 + log2st<(T_VALUE >> 1)>::value; };
template <> struct log2st<0> { static constexpr size_t value = 0; };
template <> struct log2st<1> { static constexpr size_t value = 0; };

//	Number of bits needed to represent a value
template <size_t T_VALUE> struct bits_for_value { static constexpr size_t value = 1 + log2st<T_VALUE>::value; };

//	Number of bits needed to represent a value less than range
template <size_t T_RANGE> struct bits_for_range { static constexpr size_t value = bits_for_value<T_RANGE - 1>::value; };
template <> struct bits_for_range<0> { static constexpr size_t value = 0; };
template <> struct bits_for_range<1> { static constexpr size_t value = 0; };

//	Smallest integral type needed to represent a value
template <size_t T_VALUE>
class integral_type_for_value
{
	static_assert(T_VALUE > 0, "T_VALUE must be greater than 0");
	static constexpr auto num_bits = bits_for_value<T_VALUE>::value;

public:
	using type =
		std::conditional_t<num_bits <= 8, uint8_t,
		std::conditional_t<num_bits <= 16, uint16_t,
		std::conditional_t<num_bits <= 32, uint32_t,
		std::conditional_t<num_bits <= 64, uint64_t,
		void>>>>;
};

//	Smallest integral type needed to represent a value less than range
template <size_t T_RANGE> using integral_type_for_range = integral_type_for_value<T_RANGE - 1>;


}