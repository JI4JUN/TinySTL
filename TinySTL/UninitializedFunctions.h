#ifndef _UNINITIALIZED_FUNCTIONS_H_
#define _UNINITIALIZED_FUNCTIONS_H_

#include "Construct.h"
#include "Algorithm.h"

namespace TinySTL
{
	/*
	* ***********************************
	* STL有五个全局函数（until C++11），用来处理未初始化空间
	* 除了construct()和destroy()外，还有，
	* uninitialized_copy()
	* uninitialized_copy_n() C++11
	* uninitialized_fill()
	* uninitialized_fill_n()
	* ***********************************
	*/
	/*
	* ***********************************
	* uninitialized_copy
	* copy [_First, _Last) to raw [_Dest, ...)
	* ***********************************
	*/
	//template <class InIt, class FwdIt>
	//FwdIt _uninitialized_copy(InIt First, InIt Last, FwdIt Dest, TinySTL::__true_type)
	//{
	//	auto FirstPtr              = To_address(First);
	//	auto LastPtr               = To_address(Last);
	//	auto DestPtr               = To_address(Dest);
	//	// const volatile char*: 声明一个指针，指向char类型数据。并且这个数据是只读的。并告知编译器，每次使用此变量都要去内存里重新读取，不可使用通用寄存器里缓存的值。
	//	const char* const First_ch = const_cast<const char*>(reinterpret_cast<const volatile char*>(FirstPtr));
	//	const char* const Last_ch  = const_cast<const char*>(reinterpret_cast<const volatile char*>(LastPtr));
	//	char* const Dest_ch        = const_cast<char*>(reinterpret_cast<const volatile char*>(DestPtr));
	//	const auto Count           = static_cast<size_t>(Last_ch - First_ch);
	//	memmove(Dest_ch, First_ch, Count);
	//	if constexpr (TinySTL::is_pointer_v<FwdIt>)
	//		return reinterpret_cast<FwdIt>(Dest_ch + Count);
	//	else
	//		return Dest + (LastPtr - FirstPtr);
	//}

	//template <class InIt, class FwdIt>
	//FwdIt _uninitialized_copy(InIt First, InIt Last, FwdIt Dest, TinySTL::__false_type)
	//{
	//	for (; First != Last; (void)++Dest, ++First) // (void)防止调用用户重载的操作符
	//	{
	//		TinySTL::construct(Dest, *First);
	//	}
	//	return Dest;
	//}

	template <class InIt, class FwdIt>
	inline FwdIt _uninitialized_copy_aux(InIt First, InIt Last, FwdIt Dest, TinySTL::__true_type)
	{
		return TinySTL::copy(First, Last, Dest);
	}

	template <class InIt, class FwdIt>
	inline FwdIt _uninitialized_copy_aux(InIt First, InIt Last, FwdIt Dest, TinySTL::__false_type)
	{
		for (; First != Last; (void)++Dest, ++First) // (void)防止调用用户重载的操作符
		{
			TinySTL::construct(Dest, *First);
		}
		return Dest;
	}

	template <class InIt, class FwdIt, class T>
	inline FwdIt _uninitialized_copy(InIt First, InIt Last, FwdIt Dest, T*)
	{
		using is_POD = typename TinySTL::__type_traits<T>::is_POD_type;
		return _uninitialized_copy_aux(First, Last, Dest, is_POD());
	}

	template <class InIt, class FwdIt>
	inline FwdIt uninitialized_copy(InIt First, InIt Last, FwdIt Dest) // 将迭代器返回到目标范围中的最后一个元素
	{
		return _uninitialized_copy(First, Last, Dest, TinySTL::value_type(Dest));
	}

	inline char* uninitialized_copy(const char* First, const char* Last, char* Dest)
	{
		size_t Diff = static_cast<size_t>(Last - First);
		memmove(Dest, First, Diff);
		return Dest + Diff;
	}

	inline wchar_t* uninitialized_copy(const wchar_t* First, const wchar_t* Last, wchar_t* Dest)
	{
		size_t Diff = static_cast<size_t>(Last - First);
		memmove(Dest, First, sizeof(wchar_t) * Diff);
		return Dest + Diff;
	}

	/*
	* ***********************************
	* uninitialized_copy_n()
	* copy [_First, _First + _Count) to [_Dest, ...)
	* ***********************************
	*/
	template <class InIt, class Diff, class FwdIt>
	inline FwdIt _uninitialized_copy_n_aux(InIt First, Diff Count, FwdIt Dest, TinySTL::__true_type)
	{
		return _uninitialized_copy_aux(First, First + Count, Dest, TinySTL::__true_type);
	}

	template <class InIt, class Diff, class FwdIt>
	inline FwdIt _uninitialized_copy_n_aux(InIt First, Diff Count, FwdIt Dest, TinySTL::__false_type)
	{
		for (; Count > 0; ++First, (void)++Dest, --Count)
		{
			TinySTL::construct(Dest, *First);
		}
		return Dest;
	}

	template <class InIt, class Diff, class FwdIt, class T>
	inline FwdIt _uninitialized_copy_n(InIt First, Diff Count, FwdIt Dest, T*)
	{
		using is_POD = typename TinySTL::__type_traits<T>::is_POD_type;
		return _uninitialized_copy_n_aux(First, Count, Dest, is_POD());
	}

	template <class InIt, class Diff, class FwdIt>
	inline FwdIt uninitialized_copy_n(InIt First, Diff Count, FwdIt Dest)
	{
		if (Count <= 0)
			return Dest;
		//using isPODType = typename TinySTL::__type_traits<iterator_traits<InIt>::value_type>::is_POD_type;
		return _uninitialized_copy_n(First, Count, Dest, TinySTL::value_type(Dest));
	}

	/*
	* ***********************************
	* uninitialized_fill()
	* copy _Val throughout raw [_First, _Last)
	* ***********************************
	*/
	template <class FwdIt, class Tval>
	inline void _uninitialized_fill_aux(FwdIt First, FwdIt Last, const Tval& Val, TinySTL::__true_type)
	{
		TinySTL::fill(First, Last, Val);
	}

	template <class FwdIt, class Tval>
	inline void _uninitialized_fill_aux(FwdIt First, FwdIt Last, const Tval& Val, TinySTL::__false_type)
	{
		for (; First != Last; ++First)
			TinySTL::construct(First, Val);
	}

	template <class FwdIt, class Tval, class T>
	inline void _uninitialized_fill(FwdIt First, FwdIt Last, const Tval& Val, T*)
	{
		using is_POD = typename TinySTL::__type_traits<T>::is_POD_type;
		_uninitialized_fill_aux(First, Last, Val, is_POD());
	}

	template <class FwdIt, class Tval>
	inline void uninitialized_fill(FwdIt First, FwdIt Last, const Tval& Val)
	{
		_uninitialized_fill(First, Last, Val, TinySTL::value_type(First));
	}

	/*
	* ***********************************
	* uninitialized_fill_n()
	* copy _Count copies of _Val to raw _First
	* ***********************************
	*/
	template <class FwdIt, class Diff, class Tval>
	inline FwdIt _uninitialized_fill_n_aux(FwdIt First, Diff Count, const Tval& Val, TinySTL::__true_type)
	{
		typename TinySTL::iterator_traits<FwdIt>::value_type DestVal = Val;
		return TinySTL::fill_n(First, Count, DestVal);
	}

	template <class FwdIt, class Diff, class Tval>
	inline FwdIt _uninitialized_fill_n_aux(FwdIt First, Diff Count, const Tval& Val, TinySTL::false_type)
	{
		typename TinySTL::iterator_traits<FwdIt>::value_type DestVal = Val;
		for (; Count > 0; ++First, (void)--Count)
		{
			TinySTL::construct(First, Val);
		}
		return First;
	}

	template <class FwdIt, class Diff, class Tval, class T>
	inline FwdIt _uninitialized_fill_n(FwdIt First, Diff Count, const Tval& Val, T*)
	{
		using is_POD = typename TinySTL::__type_traits<T>::is_POD_type;
		return _uninitialized_fill_n_aux(First, Count, Val, is_POD());
	}

	template <class FwdIt, class Diff, class Tval>
	inline FwdIt uninitialized_fill_n(FwdIt First, Diff Count, const Tval& Val)
	{
		if (Count <= 0)
			return First;
		return _uninitialized_fill_n(First, Count, Val, TinySTL::value_type(First));
	}

	/*
	* ***********************************
	* uninitialized_copy_copy()
	* 拷贝[first1, last1)到[result, result + (last1 - first1))
	* 同时拷贝[first2, last2)到[result + (last1 - first1), result + (last1 - first1) + (last2 - first2)]
	* ***********************************
	*/
	template<class InIt1, class InIt2, class FwdIt>
	inline FwdIt uninitialized_copy_copy(InIt1 First1, InIt2 Last1, InIt2 First2, InIt2 Last2, FwdIt Result)
	{
		FwdIt Mid = uninitialized_copy(First1, Last1, Result);
		try
		{
			return uninitialized_copy(First2, Last2, Mid);
		}
		catch (...)
		{
			TinySTL::destroy(Result, Mid);
		}
	}

	/*
	* ***********************************
	* uninitialized_fill_copy()
	* 用Val填充[result, mid),同时拷贝[first, last)到[mid, mid + (last - first))
	* ***********************************
	*/
	template<class FwdIt, class T, class InIt>
	inline FwdIt uninitialized_fill_copy(FwdIt Result, FwdIt Mid, const T& Val, InIt First, InIt Last)
	{
		uninitialized_fill(Result, Mid, Val);
		try
		{
			return uninitialized_copy(First, Last, Mid);
		}
		catch (...)
		{
			TinySTL::destroy(Result, Mid);
		}
	}

	/*
	* ***********************************
	* uninitialized_copy_fill()
	* 拷贝[first1, last1)到[first2, first2 + (last1 - first1))
	* 并且用x填充[first2 + (last1 - first1), last2]
	* ***********************************
	*/
	template<class InIt, class FwdIt, class T>
	inline void uninitialized_copy_fill(InIt First1, InIt Last1, FwdIt First2, FwdIt Last2, const T& Val)
	{
		FwdIt Mid = uninitialized_copy(First1, Last1, First2);
		try
		{
			uninitialized_fill(Mid, Last2, Val);
		}
		catch (...)
		{
			TinySTL::destroy(First2, Mid);
		}
	}
}

#endif // !_UNINITIALIZED_FUNCTIONS_H_

