#pragma once
#include <iterator>
#include <vector>
#include <memory>

namespace SGA
{
	/// <summary>
	/// Generates a vector with pointers to all elements in a given iterator.
	/// Helpful to work with various kinds of containers that may contain shared_ptr, unique_ptr, etc.
	/// </summary>
	/// <typeparam name="Iterator">The type of the iterator.</typeparam>
	/// <typeparam name="BaseType">The type of the elements that should be contained in the returned vector.</typeparam>
	/// <typeparam name="ValueType">Deducted automatically. The type currently contained in the container.</typeparam>
	template<typename Iterator, typename BaseType, typename ValueType = typename std::iterator_traits<Iterator>::value_type>
	struct IteratorToPtrVector
	{
		static std::vector<BaseType*> convert(Iterator begin, Iterator end) = delete;
	};

	template<typename Iterator, typename BaseType>
	struct IteratorToPtrVector<Iterator, BaseType, BaseType>
	{
		static std::vector<BaseType*> convert(Iterator begin, Iterator end)
		{
			std::vector<BaseType*> vec;
			vec.reserve(std::distance(begin, end));
			for(auto it = begin; it != end; ++it)
			{
				vec.emplace_back(&(*it));
			}
			return vec;
		}
	};

	template<typename Iterator, typename BaseType>
	struct IteratorToPtrVector<Iterator, BaseType, BaseType*>
	{
		static std::vector<BaseType*> convert(Iterator begin, Iterator end)
		{
			return std::vector<BaseType*>(begin, end);
		}
	};

	template<typename Iterator, typename BaseType>
	struct IteratorToPtrVector<Iterator, BaseType, std::unique_ptr<BaseType>>
	{
		static std::vector<BaseType*> convert(Iterator begin, Iterator end)
		{
			std::vector<BaseType*> vec;
			vec.reserve(std::distance(begin, end));
			for (auto it = begin; it != end; ++it)
			{
				vec.emplace_back(*it ? it->get() : nullptr);
			}
			return vec;
		}
	};

	template<typename Iterator, typename BaseType>
	struct IteratorToPtrVector<Iterator, BaseType, std::shared_ptr<BaseType>>
	{
		static std::vector<BaseType*> convert(Iterator begin, Iterator end)
		{
			std::vector<BaseType*> vec;
			vec.reserve(static_cast<size_t>(std::distance(begin, end)));
			for (auto it = begin; it != end; ++it)
			{
				vec.emplace_back(*it ? it->get() : nullptr);
			}
			return vec;
		}
	};
}