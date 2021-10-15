//#pragma once
//
//#include <algorithm>
//#include <iterator>
//#include <set>
//
//namespace  SGA
//{
//	template<class TInputIterator, class T> inline
//		bool contains(TInputIterator first, TInputIterator last, const T& value)
//	{
//		return std::find(first, last, value) != last;
//	}
//
//	template<class TContainer, class T> inline
//		bool contains(const TContainer& container, const T& value)
//	{
//		//  2. Including the utility functions below.
//		return contains(std::begin(container), std::end(container), value);
//		
//	}
//
//	template<class T> inline
//		bool contains(const std::set<T>& container, const T& value)
//	{
//		return container.find(value) != container.end();
//	}
//
//	template<class T, class T2> inline
//	bool contains(const std::unordered_map<T, T2>& container, const T& value)
//	{
//		return container.find(value) != container.end();
//	}
//}
