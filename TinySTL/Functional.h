#ifndef _FUNCTIONAL_H_
#define _FUNCTIONAL_H_

#include "TypeTraits.h"

namespace TinySTL
{
	/*
	* 仿函数------重载了operator( )
	*/

	/*****************************************************************************/

	/*
	* unary_funciton用来呈现一元函数的参数型别和返回值类别
	* 每一个Adaptable Unary Function都应该继承此类别
	*/
	template<class Arg, class Result>
	struct unary_funciton
	{
		using argument_type = Arg;
		using result_type   = Result;
	};

	/*
	* binary_funciton用来呈现二元函数的第一个参数型别、第二个参数型别，以及返回值类型
	* 每一个Adaptable Binary Function都应该继承此类别
	*/
	template <class Arg1, class Arg2, class Result>
	struct binary_function
	{
		using first_argument_type  = Arg1;
		using second_argument_type = Arg2;
		using result_type          = Result;
	};
	
	/*****************************************************************************/

	/*
	* 算术类仿函数
	* 加法：plus<T>
	* 减法：minus<T>
	* 乘法：multiplies<T>
	* 除法：divides<T>
	* 取模：modulus<T>
	* 否定：negate<T>
	*/
	template<class T>
	struct plus : public binary_function<T, T, T>
	{
		T operator()(const T& x, const T& y) const { return x + y; }
	};

	template<class T>
	struct minus : public binary_function<T, T, T>
	{
		T operator()(const T& x, const T& y) const { return x - y; }
	};

	template<class T>
	struct multiplies : public binary_function<T, T, T>
	{
		T operator()(const T& x, const T& y) const { return x * y; }
	};

	template<class T>
	struct divides : public binary_function<T, T, T>
	{
		T operator()(const T& x, const T& y) const { return x / y; }
	};

	template<class T>
	struct modulus : public binary_function<T, T, T>
	{
		T operator()(const T& x, const T& y) const { return x % y; }
	};

	template<class T>
	struct negate : public unary_funciton<T, T>
	{
		T operator()(const T& x) const { return -x; }
	};

	// 证同元素
	// 数值A若与该元素做op运算，会得到A自己。加法证同元素为0， 乘法证同元素为1
	template<class T>
	inline T identity_element(plus<int>) { return T(0); }

	template<class T>
	inline T identity_element(multiplies<T>) { return T(1); }

	/*
	* 关系运算类仿函数
	* 等于：qual_to<T>
	* 不等于：not_equal_to<T>
	* 大于：greater<T>
	* 大于或等于：greater_equal<T>
	* 小于：less<T>
	* 小于或等于：less_equal<T>
	*/
	template<class T>
	struct equal_to : public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x == y; }
	};

	template<class T>
	struct not_equal_to : public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x != y; }
	};

	template<class T>
	struct greater : public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x > y; }
	};

	template<class T>
	struct greater_equal : public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x >= y; }
	};

	template<class T>
	struct less : public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x < y; }
	};

	template<class T>
	struct less_equal : public binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x <= y; }
	};

	/*
	* 逻辑运算类仿函数
	* And：logical_and<T>
	* Or：logical_or<T>
	* Not：logical_not<T>
	*/
	template<class T>
	struct logical_and : binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x && y; }
	};

	template<class T>
	struct logical_or : binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const { return x || y; }
	};

	template<class T>
	struct logical_nor : unary_funciton<T, bool>
	{
		bool operator()(const T& x) const { return !x; }
	};

	/*
	* identity function，任何数值通过此函数后，不会有任何改变
	* 用来指定RB-tree所需的KeyOfValue op
	* 由于set元素的键值即为实值，所以采用identity
	*/
	template<class T>
	struct identity : public unary_funciton<T, T>
	{
		const T& operator()(const T& x) const { return x; }
	};

	/*
	* selection function，接受一个pair，传回其第一元素
	* 用来指定RB-tree所需的KeyOfValue op
	* 由于map以pair元素的第一个元素为其键值，所以采用select1st
	*/
	template<class Pair>
	struct select1st : public unary_funciton<Pair, typename Pair::first_type>
	{
		const typename Pair::first_type& operator()(const Pair& x) const { return x.first; }
	};

	/*
	* 接受一个pair，传回其第二元素
	*/
	template<class Pair>
	struct select2nd : public unary_funciton<Pair, typename Pair::first_type>
	{
		const typename Pair::first_type& operator()(const Pair& x) const { return x.second; }
	};
	
	/*
	* 投射函数：传回第一参数，忽略第二参数
	*/
	template<class Arg1, class Arg2>
	struct project1st : public binary_function<Arg1, Arg2, Arg1>
	{
		Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
	};

	/*
	* 投射函数：传回第二参数，忽略第一参数
	*/
	template<class Arg1, class Arg2>
	struct project2nd : public binary_function<Arg1, Arg2, Arg2>
	{
		Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
	};

	//// 比较大小的类
	//template <class T>
	//struct less
	//{
	//	using first_argument_type  = T;
	//	using second_argument_type = T;
	//	using result_type          = bool;

	//	// 重载operator( )
	//	result_type operator()(const first_argument_type& x, const second_argument_type& y)
	//	{
	//		return x < y;
	//	}
	//};

	//// 判断是否相等
	//template <class T>
	//struct equal_to
	//{
	//	using first_argument_type = T;
	//	using second_argument_type = T;
	//	using result_type = bool;

	//	result_type operator()(const first_argument_type& x, const second_argument_type& y)
	//	{
	//		return x == y;
	//	}
	//};

	/*****************************************************************************/
	template <class RanIt, class OutIt, class Diff>
	inline OutIt _copy_d(RanIt First, RanIt Last, OutIt Dest, Diff*)
	{
		for (Diff Count = Last - First; Count > 0; --Count, ++Dest, (void)++First)
			*Dest = *First;
		return Dest;
	}
	
	template <class T>
	inline T* _copy_t(const T* First, const T* Last, T* Dest, TinySTL::__true_type)
	{
		memmove(Dest, First, sizeof(T) * (Last - First));
		return Dest + (Last - First);
	}

	template <class T>
	inline T* _copy_t(const T* First, const T* Last, T* Dest, TinySTL::__false_type)
	{
		return _copy_d(First, Last, Dest, (ptrdiff_t*)0);
	}

	template <class InIt, class OutIt>
	inline OutIt _copy(InIt First, InIt Last, OutIt Dest, TinySTL::input_iterator_tag)
	{
		for (; First != Last; ++Dest, (void)++First)
			*Dest = *First;
		return Dest;
	}

	template <class RanIt, class OutIt>
	inline OutIt _copy(RanIt First, RanIt Last, OutIt Dest, TinySTL::random_access_iterator_tag)
	{
		return _copy_d(First, Last, Dest, TinySTL::distance_type(First));
	}

	template <class InIt, class OutIt>
	struct _copy_dispatch
	{
		OutIt operator()(InIt First, InIt Last, OutIt Dest)
		{
			return _copy(First, Last, Dest, TinySTL::iterator_category(First));
		}
	};

	template <class T>
	struct _copy_dispatch<T*, T*>
	{
		T* operator()(T* First, T* Last, T* Dest)
		{
			using t = typename TinySTL::__type_traits<T>::has_trivial_assignment_constructor;
			return _copy_t(First, Last, Dest, t());
		}
	};

	template <class T>
	struct _copy_dispatch<const T*, T*>
	{
		T* operator()(const T* First, const T* Last, T* Dest)
		{
			using t = typename TinySTL::__type_traits<T>::has_trivial_assignment_constructor;
			return _copy_t(First, Last, Dest, t());
		}
	};
}

#endif // !_FUNCTIONAL_H_

