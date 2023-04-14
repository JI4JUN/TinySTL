#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include <cstring>

#include "Utility.h"
#include "Functional.h"
#include "Iterator.h"

namespace TinySTL
{
	/*
	* ***********************************
	* distance advance
	* ***********************************
	*/
	template <class InIt>
	inline typename TinySTL::iterator_traits<InIt>::difference_type distance(InIt First, InIt Last)
	{
		if constexpr (TinySTL::is_random_iter_v<InIt>)
		{
			return Last - First;
		}
		else
		{
			typename TinySTL::iterator_traits<InIt>::difference_type Off = 0;
			for (; First != Last; ++First)
				++Off;
			return Off;
		}
	}

	template <class InIt, class Diff>
	inline typename void advance(InIt Where, Diff Off)
	{
		if constexpr (TinySTL::is_random_iter_v<InIt>)
		{
			Where += Off;
		}
		else
		{
			if constexpr (Off < 0 && TinySTL::is_bidi_iter_v<InIt>)
			{
				for (; Off < 0; ++Off)
					--Where;
			}
			for (; Off > 0; --Off)
				++Where;
		}
	}

	/*
	* ************************************
	* fill
	* copy Val through [_First, _Last)
	* Algorithm Complexity: O(N)
	* ************************************
	*/
	template <class ForwardIterator, class T>
	inline void fill(ForwardIterator First, ForwardIterator Last, const T& Val)
	{
		for (; First != Last; ++First)
			*First = Val;
	}
	// Specialization: for one-byte types we can use memset.
	inline void fill(unsigned char* First, unsigned char* Last, const unsigned char& Val)
	{
		unsigned char Tmp = Val;
		memset(First, Tmp, static_cast<size_t>(Last - First));
	}
	inline void fill(signed char* First, signed char* Last, const signed char& Val)
	{
		signed char Tmp = Val;
		memset(First, static_cast<unsigned char>(Tmp), static_cast<size_t>(Last - First));
	}
	inline void fill(char* First, char* Last, const char& Val)
	{
		char Tmp = Val;
		memset(First, static_cast<unsigned char>(Tmp), static_cast<size_t>(Last - First));
	}

	/*
	* ***********************************
	* fill_n
	* copy _Val _Count times through [_Dest, ...)
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class OutIt, class Diff, class T>
	inline OutIt fill_n(OutIt First, Diff Count, const T& Val)
	{
		for (; 0 < Count; --Count, ++First)
			*First = Val;
		return First;
	}
	// Specialization: for one-byte types we can use memset.
	template <class Diff>
	inline char* fill_n(char* First, Diff Count_raw, const unsigned char& Val)
	{
		unsigned char Tmp = Val;
		memset(First, Tmp, static_cast<size_t>(Count_raw));
		return First;
	}
	template <class Diff>
	inline char* fill_n(char* First, Diff Count_raw, const signed char& Val)
	{
		signed char Tmp = Val;
		size_t Count = static_cast<size_t>(Count_raw);
		memset(First, static_cast<unsigned char>(Tmp), Count);
		return First + Count;
	}
	template <class Diff>
	inline char* fill_n(char* First, Diff Count_raw, const char& Val)
	{
		char Tmp = Val;
		size_t Count = static_cast<size_t>(Count_raw);
		memset(First, static_cast<unsigned char>(Tmp), Count);
		return First + Count;
	}

	/*
	* ***********************************
	* min
	* Algorithm Complexity: O(1)
	* ***********************************
	*/
	template <class T>
	inline constexpr const T& min(const T& Left, const T& Right)
	{
		return Left < Right ? Left : Right;
	}
	// 可选比较方法
	// Comp:
	// Binary function that accepts two values of type T as arguments, and returns a value convertible to bool.
	// The value returned indicates whether the element passed as first argument is considered less than the second.
	// The function shall not modify any of its arguments.
	// This can either be a function pointer or a function object.
	template <class T, class Compare>
	inline constexpr const T& min(const T& Left, const T& Right, Compare Comp)
	{
		return Comp(Left, Right) ? Left : Right; // Left >= Right返回Right
	}

	/*
	* ***********************************
	* max
	* Algorithm Complexity: O(1)
	* ***********************************
	*/
	template <class T>
	inline constexpr const T& max(const T& Left, const T& Right)
	{
		return Left < Right ? Right : Left;
	}
	// 可选比较方法
	template <class T, class Compare>
	inline constexpr const T& max(const T& Left, const T& Right, Compare Comp)
	{
		return Comp(Left, Right) ? Right : Left; // Left >= Right返回Left
	}

	/*
	* ***********************************
	* push_heap
	* push *(_Last - 1) onto heap at [_First, _Last - 1)
	* Algorithm Complexity: O(logN)
	* ***********************************
	*/
	//template <class RanIt, class Compare>
	//inline static void percUp(RanIt First, RanIt Last, RanIt Head, Compare Comp)
	//{
	//	if (First != Last)
	//	{
	//		using Diff = typename TinySTL::iterator_traits<RanIt>::difference_type;
	//		Diff index = Last - Head - 1;
	//		Diff parentIndex = (index - 1) >> 1;
	//		for (RanIt curr = First + index; parentIndex >= 0 && curr != Head; parentIndex = (index - 1) >> 1)
	//		{
	//			RanIt parent = Head + parentIndex;
	//			if (Comp(*parent, *curr))
	//				TinySTL::swap(*parent, *curr);
	//			curr = parent;
	//			index = curr - Head;
	//		}
	//	}
	//}

	//template <class RanIt, class Compare>
	//inline void push_heap(RanIt First, RanIt Last, Compare Comp)
	//{
	//	TinySTL::percUp(First, Last, First, Comp);
	//}

	//template<class RanIt, class Distance, class T>
	//inline void __push_heap(RanIt First, Distance holeIndex, Distance topIndex, T Val)
	//{
	//	Distance parentIndex = (holeIndex - 1) >> 1; // 找出父节点 i/2
	//	while (holeIndex > topIndex && *(First + parentIndex) < Val)
	//	{	// 尚未达到顶端，且父节点小于新值（不符合heap的次序特性）（大顶堆）
	//		*(First + holeIndex) = *(First + parentIndex); // 令洞值为父值
	//		holeIndex = parentIndex; // 更新洞值坐标为父节点
	//		parentIndex = (holeIndex - 1) >> 1; // 更新下一个父节点为洞节点的父节点
	//	}
	//	*(First + holeIndex) = Val; // 令洞值为新值，完成插入操作
	//}

	//template<class RanIt, class Distance, class T>
	//inline void __push_heap_aux(RanIt First, RanIt Last, Distance*, T*)
	//{	// (Last - First) - 1为容器最尾端的坐标
	//	__push_heap(First, Distance((Last - First) - 1), Distance(0), T(*(Last - 1)));
	//}

	//template<class RanIt>
	//inline void push_heap(RanIt First, RanIt Last)
	//{	// 此函数被调用时，新元素应已置于底部容器的最尾端
	//	__push_heap_aux(First, Last, distance_type(First), value_type(First));
	//}

	template<class RanIt, class Distance, class T, class Compare>
	inline void __push_heap(RanIt First, Distance holeIndex, Distance topIndex, T Val, Compare Comp)
	{
		Distance parentIndex = (holeIndex - 1) >> 1; // 找出父节点 i/2
		while (holeIndex > topIndex && Comp(*(First + parentIndex), Val))
		{	// 尚未达到顶端，且父节点小于新值（不符合heap的次序特性）（大顶堆）
			*(First + holeIndex) = *(First + parentIndex); // 令洞值为父值
			holeIndex = parentIndex; // 更新洞值坐标为父节点
			parentIndex = (holeIndex - 1) >> 1; // 更新下一个父节点为洞节点的父节点
		}
		*(First + holeIndex) = Val; // 令洞值为新值，完成插入操作
	}

	template<class RanIt, class Compare, class Distance, class T>
	inline void __push_heap_aux(RanIt First, RanIt Last, Compare Comp, Distance*, T*)
	{	// (Last - First) - 1为容器最尾端的坐标
		__push_heap(First, Distance((Last - First) - 1), Distance(0), T(*(Last - 1)), Comp);
	}

	template<class RanIt, class Compare>
	inline void push_heap(RanIt First, RanIt Last, Compare Comp)
	{	// 此函数被调用时，新元素应已置于底部容器的最尾端
		__push_heap_aux(First, Last, distance_type(First), Comp, value_type(First));
	}

	template<class RanIt>
	inline void push_heap(RanIt First, RanIt Last)
	{	// 此函数被调用时，新元素应已置于底部容器的最尾端
		__push_heap_aux(First, Last, TinySTL::less<typename TinySTL::iterator_traits<RanIt>::value_type>(), distance_type(First), value_type(First));
	}

	/*
	* ***********************************
	* pop_heap
	* pop *_First to *(_Last - 1) and reheap
	* Algorithm Complexity: O(logN)
	* ***********************************
	*/
	//template <class RanIt, class Compare>
	//inline void pop_heap(RanIt First, RanIt Last, Compare Comp)
	//{
	//	TinySTL::swap(*First, *(Last - 1));
	//	if (2 <= Last - First)
	//	{
	//		--Last;
	//		TinySTL::percDown(First, Last, First, Comp);
	//	}
	//}

	//template <class RanIt>
	//inline void pop_heap(RanIt First, RanIt Last)
	//{
	//	TinySTL::pop_heap(First, Last, TinySTL::less<typename TinySTL::iterator_traits<RanIt>::value_type>());
	//}

	template<class RanIt, class Compare, class Distance, class T>
	inline void __adjust_heap(RanIt First, Distance holeIndex, Distance Len, T Val, Compare Comp)
	{
		Distance topIndex = holeIndex;
		Distance secondChild = 2 * holeIndex + 2; // 洞节点之右节点
		while (secondChild < Len)
		{	// 比较洞节点之左右两个子值，然后以secondChild代表最大子节点
			/*if (*(First + secondChild) < *(First + (secondChild - 1)))
				secondChild--;*/
			if (Comp(*(First + secondChild), *(First + (secondChild - 1))))
				secondChild--;
			// Percolate down: 令较大子值为洞值，再令洞号下移值较大子结点处
			*(First + holeIndex) = *(First + secondChild);
			holeIndex = secondChild;
			// 找出新洞节点的右子节点
			secondChild = 2 * secondChild + 2;
		}
		if (secondChild == Len)
		{	// 没有右子节点，只有左子节点
			// Percolate down：令左子值为洞值，再令洞号下移至左子节点处
			*(First + holeIndex) = *(First + (secondChild - 1));
			holeIndex = secondChild - 1;
		}
		// 将欲调整值填入目前的洞号内，此时肯定满足次序特征
		__push_heap(First, holeIndex, topIndex, Val, Comp);
	}

	template<class RanIt, class T, class Compare, class Distance>
	inline void __pop_heap(RanIt First, RanIt Last, RanIt Result, T Val, Distance*, Compare Comp)
	{
		*Result = *First; // 设定尾值为首值，于是尾值即为欲求结果，可由稍后再以底层容器之pop_back()取出尾值
		__adjust_heap(First, Distance(0), Distance(Last - First), Val, Comp);
		// 以上欲重新整理heap，洞号为0（即为树根处），欲调整值为Val（原尾值）
	}

	template<class RanIt, class Compare, class T>
	inline void __pop_heap_aux(RanIt First, RanIt Last, T*, Compare Comp)
	{
		__pop_heap(First, Last - 1, Last - 1, T(*(Last - 1)), distance_type(First), Comp);
	}

	template<class RanIt, class Compare>
	inline void pop_heap(RanIt First, RanIt Last, Compare Comp)
	{
		__pop_heap_aux(First, Last, value_type(First), Comp);
	}

	template<class RanIt>
	inline void pop_heap(RanIt First, RanIt Last)
	{
		__pop_heap_aux(First, Last, value_type(First), TinySTL::less<typename TinySTL::iterator_traits<RanIt>::value_type>());
	}

	/*
	* ***********************************
	* sort_heap
	* order heap by repeatedly popping
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class RanIt, class Compare>
	void sort_heap(RanIt First, RanIt Last, Compare Comp)
	{	// 没执行一次pop_heap()，极值即被放在尾端
		// 扣除尾端再执行一次pop_heap()，次极值又被放在新尾端，一直到完成排序
		while (Last - First > 1)
			pop_heap(First, Last--); // 每执行pop_heap()一次，操作范围即退缩一格
	}

	template <class RanIt>
	void sort_heap(RanIt First, RanIt Last)
	{
		sort_heap(First, Last, TinySTL::less<typename TinySTL::iterator_traits<RanIt>::value_type>());
	}

	/*
	* ***********************************
	* make_heap
	* make [_First, _Last) into a heap
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	//template <class RanIt, class Compare>
	//inline static void percDown(RanIt First, RanIt Last, RanIt Head, Compare Comp)
	//{
	//	using Diff = typename TinySTL::iterator_traits<RanIt>::difference_type;
	//	Diff index = First - Head;
	//	Diff leftChildIndex = 2 * index + 1; // 第一个非叶子节点的左子节点
	//	for (RanIt curr = First; leftChildIndex < Last - Head && curr < Last; leftChildIndex = 2 * index + 1)
	//	{
	//		RanIt leftChild = Head + leftChildIndex;
	//		if (leftChild + 1 < Last && *(leftChild + 1) > *leftChild)
	//			leftChild = leftChild + 1;
	//		if (Comp(*curr, *leftChild))
	//			TinySTL::swap(*curr, *leftChild);
	//		curr = leftChild;
	//		index = curr - Head;
	//	}
	//}
	//// 有比较函数的建堆
	//template <class RanIt, class Compare>
	//inline void make_heap(RanIt First, RanIt Last, Compare Comp)
	//{
	//	using Diff = typename TinySTL::iterator_traits<RanIt>::difference_type;
	//	const Diff range = Last - First;
	//	for (RanIt curr = First + (range >> 1) - 1; curr >= First; --curr) // 第一次循环curr为当前第一个非叶子节点
	//	{
	//		TinySTL::percDown(curr, Last, First, Comp);
	//		if (curr == First)
	//			return;
	//	}
	//}
	//// 没有比较函数的建堆
	//template <class RanIt>
	//inline void make_heap(RanIt First, RanIt Last)
	//{
	//	TinySTL::make_heap(First, Last, typename TinySTL::less<TinySTL::iterator_traits<RanIt>::value_type>());
	//}

	template<class RanIt, class Compare, class T, class Distance>
	inline void __make_heap(RanIt First, RanIt Last, T*, Distance*, Compare Comp)
	{
		if (Last - First < 2) return; // 如果长度为0或1，不需要排序
		Distance Len = Last - First;
		// 找出第一个需要重排的子树头部，以holeIndex标示出。由于任何叶节点都不要执行perlocate down，
		// 所以有以下计算
		Distance holeIndex = (Len - 2) >> 1;

		while (true)
		{	// 重排以parent为首的子树，len是为了让__adjust_heap()判断操作范围
			__adjust_heap(First, holeIndex, Len, T(*(First + holeIndex)), Comp);
			if (0 == holeIndex) return; // 走完根节点结束
			holeIndex--; // 即将重排之子树的头部向前一个节点
		}
	}

	template<class RanIt, class Compare>
	inline void make_heap(RanIt First, RanIt Last, Compare Comp)
	{
		__make_heap(First, Last, value_type(First), distance_type(First), Comp);
	}

	template<class RanIt>
	inline void make_heap(RanIt First, RanIt Last)
	{
		__make_heap(First, Last, value_type(First), distance_type(First), TinySTL::less<typename TinySTL::iterator_traits<RanIt>::value_type>());
	}

	/*
	* ***********************************
	* is_heap
	* test if range is a heap
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class RanIt, class Compare>
	inline bool is_heap(RanIt First, RanIt Last, Compare Comp)
	{
		using Diff = typename TinySTL::iterator_traits<RanIt>::difference_type;
		const Diff range = Last - First;
		Diff index = (range >> 1) - 1;
		for (RanIt curr = First + index; curr >= First; --curr, --index)
		{
			if (*(First + (index * 2 + 1)) > *curr || ((First + (index * 2 + 2)) < Last && *(First + (index * 2 + 2)) > *curr))
				return false;
			if (curr == First)
				break;
		}
		return true;
	}

	template <class RanIt>
	inline bool is_heap(RanIt First, RanIt Last)
	{
		return TinySTL::is_heap(First, Last, TinySTL::less<typename TinySTL::iterator_traits<RanIt>::value_type>());
	}

	/*
	* ***********************************
	* all_of
	* test if all elements satisfy _Pred
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Pr>
	inline bool all_of(const InIt First, const InIt Last, Pr Pred)
	{
		while (First != Last)
		{
			if (!Pred(*First))
				return false;
			++First;
		}
		return true;
	}

	/*
	* ***********************************
	* any_of
	* test if any element satisfies _Pred
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Pr>
	inline bool any_of(const InIt First, const InIt Last, Pr Pred)
	{
		while (First != Last)
		{
			if (Pred(*First))
				return true;
			++First;
		}
		return false;
	}

	/*
	* ***********************************
	* none_of
	* test if no elements satisfy _Pred
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Pr>
	inline bool none_of(const InIt First, const InIt Last, Pr Pred)
	{
		while (First != Last)
		{
			if (Pred(*First))
				return false;
			++First;
		}
		return true;
	}

	/*
	* ***********************************
	* for_each
	* test if no elements satisfy _Pred
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Fn>
	inline Fn for_each(InIt First, InIt Last, Fn Func)
	{
		while (First != Last)
		{
			Func(*First);
			++First;
		}
		return Func;
	}

	/*
	* ***********************************
	* find
	* find first matching _Val
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class T>
	inline InIt find(InIt First, const InIt Last, const T Val)
	{
		while (First != Last)
		{
			if (*First == Val)
				return First;
			++First;
		}
		return Last;
	}

	/*
	* ***********************************
	* find_if
	* find first satisfying _Pred
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Pr>
	inline InIt find_if(InIt First, const InIt Last, Pr Pred)
	{
		while (First != Last)
		{
			if (Pred(*First))
				return First;
			++First;
		}
		return Last;
	}

	/*
	* ***********************************
	* find_if_not
	* find first element that satisfies !_Pred
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Pr>
	inline InIt find_if_not(InIt First, const InIt Last, Pr Pred)
	{
		while (First != Last)
		{
			if (!Pred(*First))
				return First;
			++First;
		}
		return Last;
	}

	/*
	* ***********************************
	* find_end
	* find last [_First2, _Last2) satisfying _Pred
	* Algorithm Complexity: O(N*N)
	* ***********************************
	*/
	template <class FwdIt1, class FwdIt2>
	inline FwdIt1 find_end(FwdIt1 First1, FwdIt1 Last1, FwdIt2 First2, FwdIt2 Last2)
	{
		if (First2 == Last2)
			return Last1;
		FwdIt1 ret = Last1;
		while (First1 != Last1)
		{
			FwdIt1 it1 = First1;
			FwdIt1 it2 = First2;
			while (*it1 == *it2)
			{
				++it1;
				++it2;
				if (it2 == Last2)
				{
					ret = First1;
					break;
				}
				if (it1 == Last1)
					return ret;
			}
		}
		return ret;
	}

	template <class FwdIt1, class FwdIt2, class Pr>
	inline FwdIt1 find_end(FwdIt1 First1, FwdIt1 Last1, FwdIt2 First2, FwdIt2 Last2, Pr Pred)
	{
		if (First2 == Last2)
			return Last1;
		FwdIt1 ret = Last1;
		while (First1 != Last1)
		{
			FwdIt1 it1 = First1;
			FwdIt1 it2 = First2;
			while (Pred(*it1, *it2))
			{
				++it1;
				++it2;
				if (it2 == Last2)
				{
					ret = First1;
					break;
				}
				if (it1 == Last1)
					return ret;
			}
		}
		return ret;
	}

	/*
	* ***********************************
	* find_first_of
	* look for one of [_First2, _Last2) satisfying _Pred with element
	* Algorithm Complexity: O(N*N)
	* ***********************************
	*/
	template <class FwdIt1, class FwdIt2, class Pr>
	inline FwdIt1 find_first_of(FwdIt1 First1, FwdIt1 Last1, FwdIt2 First2, FwdIt2 Last2, Pr Pred)
	{
		while (First1 != Last1)
		{
			for (FwdIt2 it = First2; it != Last2; ++it)
			{
				if (Pred(*it, *First1))
					return First1;
			}
			++First1;
		}
		return Last1;
	}

	template <class FwdIt1, class FwdIt2>
	inline FwdIt1 find_first_of(FwdIt1 First1, FwdIt1 Last1, FwdIt2 First2, FwdIt2 Last2)
	{
		while (First1 != Last1)
		{
			for (FwdIt2 it = First2; it != Last2; ++it)
			{
				if (*it == *First1)
					return First1;
			}
			++First1;
		}
		return Last1;
	}

	/*
	* ***********************************
	* adjacent_find
	* find first satisfying _Pred with successor
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class FwdIt>
	inline FwdIt adjacent_find(const FwdIt First, FwdIt Last)
	{
		return TinySTL::adjacent_find(First, Last, TinySTL::equal_to<typename  iterator_traits<FwdIt>::value_type>());
	}

	template <class FwdIt, class Pr>
	inline FwdIt adjacent_find(const FwdIt First, FwdIt Last, Pr Pred)
	{
		if (First != Last)
		{
			FwdIt next = First;
			++next;
			while (next != Last)
			{
				if (Pred(*First, *next))
					return First;
				++First;
				++next;
			}
		}
		return Last;
	}

	/*
	* ***********************************
	* count
	* count elements that match _Val
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class T>
	inline typename TinySTL::iterator_traits<InIt>::difference_type count(InIt First, InIt Last, const T& Val)
	{
		typename TinySTL::iterator_traits<InIt>::difference_type ret = 0;
		while (First != Last)
		{
			if (*First == Val)
				++ret;
			++First;
		}
		return ret;
	}

	/*
	* ***********************************
	* count_if
	* count elements that match _Val
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Pr>
	inline typename TinySTL::iterator_traits<InIt>::difference_type count_if(InIt First, InIt Last, Pr Pred)
	{
		typename TinySTL::iterator_traits<InIt>::difference_type ret = 0;
		while (First != Last)
		{
			if (Pred(*First))
				++ret;
			++First;
		}
		return ret;
	}

	/*
	* ***********************************
	* mismatch
	* return first position where two ranges differ
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt1, class InIt2>
	TinySTL::pair<InIt1, InIt2> mismatch(InIt1 First1, InIt1 Last1, InIt2 First2)
	{
		while (First1 != Last1 && *First1 == *First2)
		{
			++First1;
			++First2;
		}
		return TinySTL::make_pair(First1, First2);
	}

	template <class InIt1, class InIt2, class Pr>
	TinySTL::pair<InIt1, InIt2> mismatch(InIt1 First1, InIt1 Last1, InIt2 First2, Pr Pred)
	{
		while (First1 != Last1 && Pred(*First1, *First2))
		{
			++First1;
			++First2;
		}
		return TinySTL::make_pair(First1, First2);
	}

	/*
	* ***********************************
	* equal
	* Test whether the elements in two ranges are equal
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt1, class InIt2>
	inline bool equal(InIt1 First1, InIt1 Last1, InIt2 First2)
	{
		while (First1 != Last1)
		{
			if (!(*First1 == *First2))
				return false;
			++First1;
			++First2;
		}
		return true;
	}

	template <class InIt1, class InIt2, class Pr>
	inline bool equal(InIt1 First1, InIt1 Last1, InIt2 First2, Pr Pred)
	{
		while (First1 != Last1)
		{
			if (!Pred(*First1, *First2))
				return false;
			++First1;
			++First2;
		}
		return true;
	}

	/*
	* ***********************************
	* is_permutation
	* Test whether range is permutation of another
	* Algorithm Complexity: O(N*N)
	* ***********************************
	*/
	template <class FwdIt1, class FwdIt2>
	inline bool is_permutation(FwdIt1 First1, FwdIt1 Last1, FwdIt2 First2)
	{
		return is_permutation(First1, Last1, First2, TinySTL::equal_to<typename TinySTL::iterator_traits<FwdIt1>::value_type>());
	}

	template <class FwdIt1, class FwdIt2, class Pr>
	inline bool is_permutation(FwdIt1 First1, FwdIt1 Last1, FwdIt2 First2, Pr Pred)
	{
		TinySTL::pair<FwdIt1, FwdIt2> Tmp = TinySTL::mismatch(First1, Last1, First2, Pred);
		First1 = Tmp.first_;
		First2 = Tmp.second_;
		if (First1 == Last1)
			return true;
		FwdIt2 Last2 = First2;
		TinySTL::advance(Last2, TinySTL::distance(First1, Last1));
		for (FwdIt1 it1 = First1; it1 != Last1; ++it1)
		{
			// 先判断当前的元素是否已经比较过，若已经比较过则进入下一个循环
			if (TinySTL::find_if(First1, it1, [&](decltype(*First1) Val) { return Pred(Val, *it1); }) == it1) // 这里的Pred是接受一个参数的bool函数
			{
				auto n = TinySTL::count(First2, Last2, *it1);
				if (0 == n || TinySTL::count(it1, Last1, *it1) != n)
					return false;
			}
		}
		return true;
	}

	/*
	* ***********************************
	* search
	* Search range for subsequence
	* Algorithm Complexity: O(N*N)
	* ***********************************
	*/
	template <class InIt1, class InIt2, class Pr>
	inline bool equal_rev_pred(InIt1 First1, InIt2 First2, const InIt2 Last2, Pr Pred) // compare [_First1, ...) to [_First2, _Last2)
	{
		for (; First2 != Last2; ++First1, ++First2)
		{
			if (!Pred(*First1, *First2))
				return false;
		}
		return true;
	}

	template <class FwdIt1, class FwdIt2, class Pr>
	inline FwdIt1 search(FwdIt1 First1, FwdIt1 Last1, FwdIt2 First2, FwdIt2 Last2, Pr Pred)
	{
		if constexpr (TinySTL::is_random_iter_v<FwdIt1> && TinySTL::is_random_iter_v<FwdIt2>)
		{
			typename TinySTL::iterator_traits<FwdIt2>::difference_type Count = 0;
			Count = Last2 - First2;
			if (Last1 - First1 >= Count)
			{
				const auto Last_possible = Last1 - static_cast<typename TinySTL::iterator_traits<FwdIt1>::difference_type>(Count);
				for (;; ++First1)
				{
					if (TinySTL::equal_rev_pred(First1, First2, Last2, Pred))
					{
						Last1 = First1;
						break;
					}
					if (First1 == Last_possible)
						break;
				}
			}
		}
		else
		{
			while (First1 != Last1)
			{
				FwdIt1 it1 = First1;
				FwdIt2 it2 = First2;
				while (Pred(*it1, *it2))
				{
					++it1;
					++it2;
					if (it2 == Last2)
						return First1;
					if (it1 == Last1)
						return Last1;
				}
				++First1;
			}
		}
		return Last1;
	}

	/*
	* ***********************************
	* sort
	* Sorts the elements in the range [first,last) into ascending order.
	* Algorithm Complexity: O(NlogN)
	* ***********************************
	*/
	inline constexpr int _ISORT_MAX = 32;

	template <class BidIt, class Pr>
	inline void insertionSort(const BidIt First, const BidIt Last, Pr Pred)
	{
		BidIt Prev;
		if (First != Last)
		{
			for (BidIt Mid = First + 1; Mid != Last; ++Mid)
			{
				typename TinySTL::iterator_traits<BidIt>::value_type Val = *Mid;
				for (Prev = Mid; Prev != First && Pred(Val, *(Prev - 1)); --Prev)
					*Prev = *(Prev - 1);
				*Prev = Val;
			}
		}
	}

	template <class RanIt, class Pr>
	inline typename TinySTL::iterator_traits<RanIt>::value_type median3(RanIt First, RanIt Last, Pr Pred)
	{
		RanIt Mid = First + ((Last - First) >> 1);
		if (Pred(*Mid, *First))
			TinySTL::swap(*Mid, *First);
		if (Pred(*Last, *First))
			TinySTL::swap(*Last, *First);
		if (Pred(*Last, *Mid))
			TinySTL::swap(*Last, *Mid);

		TinySTL::swap(*Mid, *(Last - 1));
		return *(Last - 1);
	}

	template <class RanIt, class Pr>
	inline void _sort(RanIt First, RanIt Last, typename TinySTL::iterator_traits<RanIt>::difference_type Ideal, Pr Pred)
	{
		using Diff = typename TinySTL::iterator_traits<RanIt>::difference_type;

		if (Last - First > _ISORT_MAX) // 快速排序
		{
			if (Ideal <= 0) // 当分割次数大于1.5*log2(N)次时，直接调用堆排序
			{
				TinySTL::make_heap(First, Last, Pred);
				TinySTL::sort_heap(First, Last, Pred);
			}
			Ideal = (Ideal >> 1) + (Ideal >> 2); // 每次乘0.75，即分割1.5*log2(N)次

			typename TinySTL::iterator_traits<RanIt>::value_type Pivot = median3(First, Last - 1, Pred); // 分割算法
			RanIt Pfirst = First;
			RanIt Plast = Last - 2;
			for (;;)
			{
				while (Pred(*++Pfirst, Pivot)) {};
				while (Pred(Pivot, *--Plast)) {};
				Diff Count = Plast - Pfirst;
				if (Count > 0)
					TinySTL::swap(*Pfirst, *Plast);
				else
					break;
			}
			TinySTL::swap(*Pfirst, *(Last - 2)); // 交换Pfirst是因为*Pfirst一定大于Mid，*Pfirst一定排在Mid右边
			
			_sort(First, Pfirst, Pfirst - First, Pred);
			_sort(Plast, Last, Last - Plast, Pred);
		}
		else // 当元素个数小于32时，使用插入排序
			insertionSort(First, Last, Pred);
	}

	template <class RanIt, class Pr>
	inline void sort(RanIt First, RanIt Last, Pr Pred)
	{
		if (First >= Last || First + 1 == Last)
			return;
		_sort(First, Last, Last - First, Pred);
	}

	template <class RanIt>
	inline void sort(RanIt First, RanIt Last)
	{
		_sort(First, Last, Last - First, TinySTL::less<typename iterator_traits<RanIt>::value_type>());
	}

	/*
	* ***********************************
	* generate
	* replace [_First, _Last) with _Func()
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class FwdIt, class Fn>
	inline void generate(FwdIt First, FwdIt Last, Fn Func)
	{
		for (; First != Last; ++First)
			*First = Func();
	}

	/*
	* ***********************************
	* generate_n
	* replace [_Dest, _Dest + _Count) with _Func()
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class OutIt, class Diff, class Fn>
	inline void generate_n(OutIt First, Diff Count, Fn Func)
	{
		for (; Count > 0; --Count, ++First)
			*First = Func();
	}

	/*
	* ***********************************
	* copy
	* copy [_First, _Last) to [_Dest, ...)
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class OutIt>
	inline OutIt copy(InIt First, InIt Last, OutIt Dest)
	{
		//while (First != Last)
		//{
		//	*Dest = *First;
		//	++Dest;
		//	++First;
		//}
		//return Dest;
		return _copy_dispatch<InIt, OutIt>()(First, Last, Dest);
	}

	inline char* copy(const char* First, const char* Last, char* Dest)
	{
		size_t Diff = static_cast<size_t>(Last - First);
		memmove(Dest, First, Diff);
		return Dest + Diff;
	}

	inline wchar_t* copy(const wchar_t* First, const wchar_t* Last, wchar_t* Dest)
	{
		size_t Diff = static_cast<size_t>(Last - First);
		memmove(Dest, First, sizeof(wchar_t) * Diff);
		return Dest + Diff;
	}

	/*
	* ***********************************
	* copy_backward
	* copy [_First, _Last) backwards to [..., _Dest)
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class BidIt1, class BidIt2>
	inline BidIt2 copy_backward(BidIt1 First, BidIt1 Last, BidIt2 Dest)
	{
		while (Last != First)
			*(--Dest) = *(--Last);
		return Dest;
	}

	/*
	* ***********************************
	* copy_if
	* copy each satisfying _Pred
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class OutIt, class Pr>
	inline OutIt copy_if(InIt First, InIt Last, OutIt Dest, Pr Pred)
	{
		while (First != Last)
		{
			if (Pred(*First))
			{
				*Dest = *First;
				++Dest;
			}
			++First;
		}
		return Dest;
	}

	/*
	* ***********************************
	* copy_n
	* copy [_First, _First + _Count) to [_Dest, ...)
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Diff, class OutIt>
	inline OutIt copy_n(InIt First, Diff Count, OutIt Dest)
	{
		if (Count > 0)
		{
			for (;;)
			{
				*Dest = *First;
				++Dest;
				--Count;
				if (0 == Count) // note that we avoid an extra ++_First here to allow istream_iterator to work
					break;
				++First;
			}
		}
		return Dest;
	}

	/*
	* ***********************************
	* partition
	* Partition range in two
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt>
	inline InIt next(InIt First, typename TinySTL::iterator_traits<InIt>::difference_type Off = 1)
	{
		TinySTL::advance(First, Off);
		return First;
	}

	template <class FwdIt, class Pr>
	inline FwdIt partition(FwdIt First, FwdIt Last, Pr Pred)
	{
		if constexpr (TinySTL::is_bidi_iter_v<FwdIt>)
		{
			for (;;)
			{
				First = TinySTL::find_if_not(First, Last, Pred);
				if (First == Last)
					return First;
				if (!Pred(*First)) // 从左往右找到第一个不符合Pred的元素
					break;

				do
				{
					--Last;
					if (First == Last)
						return First;
				} while (!Pred(*Last)); // 从右往左找到一个符合Pred的元素

				TinySTL::swap(*First, *Last);
				++First;
			}
		}
		else
		{
			First = TinySTL::find_if_not(First, Last, Pred); // 从左往右找到第一个不符合Pred的元素
			if (First == Last)
				return First;

			for (FwdIt Next = TinySTL::next(First); Next != Last; ++Next)
			{
				if (Pred(*Next))
				{
					TinySTL::swap(*First, *Next);
					++First;
				}
			}
		}
		return First;
	}

	/*
	* ***********************************
	* is_partitioned
	* Test whether range is partitioned
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Pr>
	inline bool is_partitioned(InIt First, InIt Last, Pr Pred)
	{
		for (; First != Last; ++First)
			if (!Pred(*First))
				break;
		for (; First != Last; ++First)
			if (Pred(*First))
				return false;
		return true;
	}

	/*
	* ***********************************
	* compare
	* order [_First1, _Last1) vs. [_First2, _Last2)
	* Algorithm Complexity: O(N)
	* ***********************************
	*/
	template <class InIt, class Pr>
	inline bool compare(InIt First1, InIt Last1, InIt First2, InIt Last2, Pr Pred)
	{
		for (; First1 != Last1 && First2 && Last2; ++First1, (void)++First2)
		{
			if (Pred(*First1, *First2))
				return true;
			if (Pred(*First2, *First1))
				return false;
		}
		return First1 == Last1 && First2 != Last2;
	}

	template <class InIt>
	inline bool compare(InIt First1, InIt Last1, InIt First2, InIt Last2)
	{
		return compare(First1, Last1, First2, Last2, less<typename iterator_traits<InIt>::value_type>());
	}
}

#endif // !_ALGORITHM_H_
