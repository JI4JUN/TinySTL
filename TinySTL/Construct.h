#ifndef _CONSTRUCT_H_
#define _CONSTRUCT_H_

#include "TypeTraits.h"

namespace TinySTL
{
	template<typename _T1, typename _T2>
	inline void _Construct(_T1* __p, const _T2& __value)
	{
		new((void*)__p) _T1(__value); // 当调用形式为construct(ptr, _TP())时，_T1(_TP())为拷贝构造
	}

	template<typename _T1>
	inline void _Construct(_T1* __p)
	{
		new((void*)__p) _T1(); // 当调用形式为construct(ptr, _TP())时，_T1(_TP())为拷贝构造
	}

	template<typename _TP>
	inline void _Destroy(_TP* __pointer)
	{
		__pointer->~_TP();
	}

	template<class FwdIt>
	inline void __destroy_aux(FwdIt __first, FwdIt __last, __true_type) {};

	template<class FwdIt>
	inline void __destroy_aux(FwdIt __first, FwdIt __last, __false_type)
	{
		for (; __first != __last; ++__first)
			destroy(&*__first); // 传地址
	}

	template<class FwdIt, class _TP>
	inline void __destroy(FwdIt __first, FwdIt __last, _TP*)
	{
		// typedef typename TinySTL::___type_traits<ForwodrdIterator>::has_trivial_destructor trivial_destructor;
		using _Trivial_destructor = typename TinySTL::__type_traits<_TP>::has_trivial_destructor;
		__destroy_aux(__first, __last, _Trivial_destructor());
	}

	// 推断迭代器所指类型
	template<class FwdIt>
	inline void _Destroy(FwdIt ____first, FwdIt ____last)
	{
		__destroy(____first, ____last, value_type(____first));
	}

	inline void _Destroy(char*, char*) {}
	inline void _Destroy(wchar_t*, wchar_t*) {}
	inline void _Destroy(int*, int*) {}
	inline void _Destroy(long*, long*) {}
	inline void _Destroy(float*, float*) {}
	inline void _Destroy(double*, double*) {}

	// 接受一个指针
	template<class _T1>
	inline void construct(_T1* __p)
	{
		_Construct(__p);
	}

	// 接受两个迭代器
	template<class _T1, class _T2>
	inline void construct(_T1* __p, const _T2& __value)
	{
		_Construct(__p, __value);
	}

	// 接受一个指针
	template<class _TP>
	inline void destroy(_TP* __pointer)
	{
		_Destroy(__pointer);
	}

	// 接受两个迭代器
	template<class FwdIt>
	inline void destroy(FwdIt __first, FwdIt __last)
	{
		_Destroy(__first, __last);
	}
}

#endif // !__CONSTRUCT_H__
