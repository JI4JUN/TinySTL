#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include "Alloc.h"
#include "Construct.h"

namespace TinySTL
{
	/*
	* 对alloc出来的地址空间进行placement new 构造
	* 以及对自身构造出来的内容进行对应的析构
	*
	***************************************************
	* allocate()   ：内存空间分配（按对象的大小为单位从alloc中获取）
	* deallocate() : 内存空间的回收（释放给alloc进行管理）
	* construct()  : placement new 指定地址构造对象
	* destroy()    : operator delete 指定地址析构对象
	*/
	template <class T>
	class allocator
	{
	public:
		using value_type      = T;
		using pointer         = value_type*;
		using const_pointer   = const value_type*;
		using reference       = value_type&;
		using const_reference = const value_type&;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;

	public:
		// 分配未构造的内存空间，使用自带的alloc
		static T* allocate();
		static T* allocate(size_t n);
		static void deallocate(T* ptr);
		static void deallocate(T* ptr, size_t n);

		// 以下的构造和析构都是针对带有构造函数和析构函数的对象
		// 对于基本对象直接返回内存空间
		static void construct(T* ptr);
		static void construct(T* ptr, const T& value);
		static void destroy(T* ptr);
		static void destroy(T* first, T* last);

	public:
		// 构造与析构
		allocator() {};
		~allocator() {};
	};
}

template<class T>
inline T* TinySTL::allocator<T>::allocate()
{
	return static_cast<T*>(alloc::allocate(sizeof(T)));
}

template<class T>
inline T* TinySTL::allocator<T>::allocate(size_t n)
{
	return (0 == n) ? nullptr : static_cast<T*>(alloc::allocate(sizeof(T) * n));
}

template<class T>
inline void TinySTL::allocator<T>::deallocate(T* ptr)
{
	alloc::deallocate(static_cast<void*>(ptr), sizeof(T));
}

template<class T>
inline void TinySTL::allocator<T>::deallocate(T* ptr, size_t n)
{
	if (0 == n) return;
	alloc::deallocate(static_cast<void*>(ptr), sizeof(T) * n);
}

template<class T>
inline void TinySTL::allocator<T>::construct(T* ptr) // 调用default placement new
{
	TinySTL::construct(ptr, T());
}

template<class T>
inline void TinySTL::allocator<T>::construct(T* ptr, const T& value) // 调用带参placement new
{
	TinySTL::construct(ptr, value);
}

template<class T>
inline void TinySTL::allocator<T>::destroy(T* ptr)
{
	TinySTL::destroy(ptr);
}

template<class T>
inline void TinySTL::allocator<T>::destroy(T* first, T* last)
{
	TinySTL::destroy(first, last);
}

#endif // !__ALLOCATOR_H__
