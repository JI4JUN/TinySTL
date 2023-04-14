#ifndef __type_traits_H_
#define __type_traits_H_

#include "Iterator.h"

namespace TinySTL
{
	//template<class T>
	//struct _Identity
	//{
	//	using type = T;
	//};

	//template<class T>
	//using _Identity_t = typename _Identity<T>::type;

	// 空类(大小为1)
	struct __true_type {};
	struct __false_type {};

	// 主模板定义
	template <class>
	struct __type_traits
	{
		using has_trivial_default_constructor    = __false_type; // 默认构造
		using has_trivial_copy_constructor       = __false_type; // 拷贝构造
		using has_trivial_assignment_constructor = __false_type; // 赋值构造
		using has_trivial_destructor             = __false_type; // 析构
		using is_POD_type                        = __false_type; // POD(plain old data)型别
																// 基本数据类型、指针、union、数组、构造函数是trivial的struct或者class
	};

	// 特化
	template <class T>
	struct __type_traits<T*>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor       = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template<class T>
	struct __type_traits<const T*>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<bool>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor       = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<char>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<unsigned char>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<signed char>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<wchar_t>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<short>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<unsigned short>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<int>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<unsigned int>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<long>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<unsigned long>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<long long>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<unsigned long long>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<float>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<double>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<long double>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<char*>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<unsigned char*>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<signed char*>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<const char*>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<const unsigned char*>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	template <>
	struct __type_traits<const signed char*>
	{
		using has_trivial_default_constructor    = __true_type; // 默认构造
		using has_trivial_copy_constructor	     = __true_type; // 拷贝构造
		using has_trivial_assignment_constructor = __true_type; // 赋值构造
		using has_trivial_destructor             = __true_type; // 析构
		using is_POD_type                        = __true_type; // POD(plain old data)型别
	};

	/*
	* ***********************************
	* integral_constant
	* integra_constant 包装指定类型的静态常量。它是C++类型特征的基类。
	* bool_constant 为T为bool的常见情况定义de 助手别名模板
	* ***********************************
	*/
	template <class T, T Val>
	struct integral_constant
	{
		static constexpr T value = Val;

		using value_type = T;
		using type	     = integral_constant;

		constexpr operator value_type() const noexcept
		{
			return value;
		}

		constexpr value_type operator()() const noexcept
		{
			return value;
		}
	};

	template<bool Val>
	using bool_constant = integral_constant<bool, Val>;

	using true_type  = bool_constant<true>;
	using false_type = bool_constant<false>;

	/*
	* ***********************************
	* C++17
	* type_traits
	* _Is_input_iter_v
	* _Is_fwd_iter_v
	* _Is_bidi_iter_v
	* _Is_random_iter_v
	* ***********************************
	*/
	template <class From, class To>
	inline constexpr bool is_convertible_v = __is_convertible_to(From, To);

	template <class Iter>
	inline constexpr bool is_input_iter_v  = is_convertible_v<typename iterator_traits<Iter>::iterator_category, input_iterator_tag>;

	template <class Iter>
	inline constexpr bool is_fwd_iter_v    = is_convertible_v<typename iterator_traits<Iter>::iterator_category, forward_iterator_tag>;
	
	template <class Iter>
	inline constexpr bool is_bidi_iter_v   = is_convertible_v<typename iterator_traits<Iter>::iterator_category, bidirectional_iterator_tag>;
	
	template <class Iter>
	inline constexpr bool is_random_iter_v = is_convertible_v<typename iterator_traits<Iter>::iterator_category, random_access_iterator_tag>;

	template <class From, class To>
	struct is_convertible : bool_constant<__is_convertible_to(From, To)> {};

	/*
	* ***********************************
	* C++17
	* type_traits
	* is_pointer_v
	* ***********************************
	*/
	template <class> // 普通模板，也就是最后的出口。当所有特化都不满足才会被匹配到
	inline constexpr bool is_pointer_v = false;

	template <class T>
	inline constexpr bool is_pointer_v<T*> = true;

	template <class T>
	inline constexpr bool is_pointer_v<T* const> = true; // T* const所修饰的变量是个指针，这个指针的内容不能被修改 
														 // const T*是指向常量的指针
	template <class T>
	inline constexpr bool is_pointer_v<T* volatile> = true;

	template <class T>
	inline constexpr bool is_pointer_v<T* const volatile> = true;

	template <class T>
	struct is_pointer : bool_constant<is_pointer_v<T>> {};

	/*
	* ***********************************
	* C++17
	* type_traits
	* is_integral_v
	* ***********************************
	*/
	template <class, class>
	inline constexpr bool is_same_v = false;

	template <class T>
	inline constexpr bool is_same_v<T, T> = true;

	template <class T1, class T2>
	struct is_same : bool_constant<is_same_v<T1, T2>> {};

	template <class T>
	struct remove_cv
	{	// remove top-level const and volatile qualifiers
		using type = T;

		template <template <class> class Fn>
		using Apply = Fn<T>; // apply cv-qualifiers from the class template argument to _Fn<_Ty>
	};

	template <class T>
	struct remove_cv<const T>
	{
		using type = T;

		template <template <class> class Fn>
		using _Apply = const Fn<T>;
	};

	template <class T>
	struct remove_cv<volatile T>
	{
		using type = T;

		template <template <class> class Fn>
		using _Apply = volatile Fn<T>;
	};

	template <class T>
	struct remove_cv<const volatile T>
	{
		using type = T;

		template <template <class> class Fn>
		using _Apply = const volatile Fn<T>;
	};

	template <class T>
	using remove_cv_t = typename remove_cv<T>::type;

	template <bool First_value, class First, class... Rest>
	struct _Disjunction
	{	// handle true trait or last trait
		using type = First;
	};

	template <class False, class Next, class... Rest>
	struct _Disjunction<false, False, Next, Rest...>
	{
		using type = typename _Disjunction<Next::value, Next, Rest...>::type;
	};

	template <class... Traits>
	struct disjunction : false_type {}; // If Traits is empty, false_type

	template <class First, class... Rest>
	struct disjunction<First, Rest...> : _Disjunction<First::value, First, Rest...>::type {}; // the first true trait in _Traits, or the last trait if none are true

	template <class... Traits>
	inline constexpr bool disjunction_v = disjunction<Traits...>::value;

	template <class T, class... Types>
	inline constexpr bool _Is_any_of_v = disjunction_v<is_same<T, Types>...>;

	//如果类型 Ty 是整型类型之一，或整型类型之一的 cv-qualified 形式，则类型谓词的实例将为 true，否则为 false。
	template <class T>
	inline constexpr bool is_integral_v = _Is_any_of_v<
		remove_cv_t<T>, // cv-qualified 形式; remove_cv_t删去const或volatile
		bool,
		char,
		signed char,
		unsigned char,
		wchar_t,
		char16_t,
		char32_t,
		short,
		unsigned short,
		int,
		unsigned int, 
		long, 
		unsigned long,
		long long, 
		unsigned long long>;

	template <class T>
	struct is_integral : bool_constant<is_integral_v<T>> {};
}

#endif