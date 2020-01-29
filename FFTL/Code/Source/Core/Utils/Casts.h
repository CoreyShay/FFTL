#pragma once


#include "../defs.h"
#include "../Math/MathCommon.h"
#include <limits>
#include <type_traits>


namespace FFTL
{

template <typename T_TO, typename T_FROM>
constexpr typename std::enable_if<(std::is_same<T_TO, T_FROM>::value), T_TO>::type safestatic_cast(T_FROM a)
{
	return a;
}

template <typename T_TO, typename T_FROM>
constexpr typename std::enable_if<(!std::is_same<T_TO, T_FROM>::value) && std::numeric_limits<T_TO>::is_integer && std::numeric_limits<T_FROM>::is_integer, T_TO>::type safestatic_cast(T_FROM a)
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(a == static_cast<T_FROM>(ret), "Casting has lost precision or overflowed / underflowed");
	return ret;
}

template <typename T_TO, typename T_FROM>
constexpr typename std::enable_if<std::numeric_limits<T_TO>::is_integer && std::is_floating_point<T_FROM>::value, T_TO>::type safestatic_cast(T_FROM a)
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(Abs(a - static_cast<T_FROM>(ret)) < (T_FROM)1, "Casting has lost precision or overflowed / underflowed");
	return ret;
}

template <typename T_TO, typename T_FROM>
constexpr typename std::enable_if<std::is_floating_point<T_TO>::value && std::numeric_limits<T_FROM>::is_integer, T_TO>::type safestatic_cast(T_FROM a)
{
	const T_TO ret = static_cast<T_TO>(a);
	FFTL_ASSERT_MSG(Abs(ret - static_cast<long double>(a)) < (long double)1, "Casting has lost precision or overflowed / underflowed");
	return ret;
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr typename std::underlying_type<T>::type underlying_cast(T e)
{
	return static_cast<typename std::underlying_type<T>::type>(e);
}


template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator|(T a, T b)
{
	return static_cast<T>(underlying_cast(a) | underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator|=(T& a, T b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) |= underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator&(T a, T b)
{
	return static_cast<T>(underlying_cast(a) & underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator&=(T& a, T b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) &= underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator^(T a, T b)
{
	return static_cast<T>(underlying_cast(a) ^ underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator^=(T& a, T b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) ^= underlying_cast(b));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator~(T a)
{
	return static_cast<T>(~underlying_cast(a));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator+(T a, typename std::underlying_type<T>::type b)
{
	return static_cast<T>(underlying_cast(a) + b);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator+=(T& a, typename std::underlying_type<T>::type b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) += b);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator-(T a, typename std::underlying_type<T>::type b)
{
	return static_cast<T>(underlying_cast(a) - b);
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T& operator-=(T& a, typename std::underlying_type<T>::type b)
{
	return reinterpret_cast<T&>(reinterpret_cast<typename std::underlying_type<T>::type&>(a) -= b);
}



}