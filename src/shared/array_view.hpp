/// \file Span for pre-C++20

#pragma once

#include <type_traits>
#include <vector>
#include <string_view>

template <typename T>
struct array_view
{
	size_t count;
	T* data;

	using unqual_element_type = std::remove_cv_t<T>;

	constexpr array_view()
		: count(0), data(nullptr)
	{}

	template <size_t Sz>
	constexpr array_view(T (&arr)[Sz])
		: count(Sz), data(arr)
	{}

	constexpr array_view(T* ptr, size_t sz)
		: count(sz), data(ptr)
	{}

	array_view(std::conditional_t<std::is_const_v<T>, std::vector<unqual_element_type> const, std::vector<unqual_element_type>>& vec)
		: count(vec.size()), data(vec.data())
	{}

	template <typename U = T, typename = std::enable_if_t<std::is_trivial_v<U>>>
	array_view(std::basic_string_view<U> str)
		: count(str.size()), data(str.data())
	{}

	template <typename U = T, typename = std::enable_if_t<!std::is_const_v<U>>>
	operator array_view<U const>() const
	{
		return {data, count};
	}

	size_t byte_count() const
	{
		return count * sizeof(T);
	}

	template <typename U = T, typename = std::enable_if_t<!std::is_const_v<U>>>
	array_view<std::byte> byte_view()
	{
		return {reinterpret_cast<std::byte*>(data), byte_count()};
	}

	array_view<std::byte const> byte_view() const
	{
		return {reinterpret_cast<std::byte const*>(data), byte_count()};
	}

	T* begin() { return data; }
	T* end() { return data + count; }
	T const* begin() const { return data; }
	T const* end() const { return data + count; }
};
