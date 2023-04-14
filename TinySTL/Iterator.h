#ifndef _ITERATOR_H_
#define _ITERATOR_H_

#include <cstddef>

namespace TinySTL
{
	// Input<--Forward<--Bildirectional<--Random Access
	// Output
	struct input_iterator_tag {};                                             // 只读且读后往后移动
	struct forward_iterator_tag : public input_iterator_tag {};               // 可以从前往后遍历的指针
	struct bidirectional_iterator_tag : public forward_iterator_tag {};       // 从两个方向都可以遍历的指针
	struct random_access_iterator_tag : public bidirectional_iterator_tag {}; // 提供和指针相同的功能，可以跳跃而不是逐个遍历

	struct output_iterator_tag {};                                            // 只写且写后往后移动

	template<class T, class Distance>
	struct input_iterator
	{
		using iterator_category = input_iterator_tag; // 迭代器本身
		using value_type        = T;                  // 定义类型
		using difference_type   = Distance;           // 大小
		using pointer           = T*;                 // 定义指针
		using reference         = T&;                 // 定义引用
	};

	template<class T, class Distance>
	struct forward_iterator
	{
		using iterator_category = forward_iterator_tag;
		using value_type        = T;
		using difference_type   = Distance;
		using pointer           = T*;
		using reference         = T&;
	};

	template<class T, class Distance> 
	struct bidirectional_iterator
	{
		using iterator_category = bidirectional_iterator_tag;
		using value_type        = T;
		using difference_type   = Distance;
		using pointer           = T*;
		using reference         = T&;
	};

	template<class T, class Distance>
	struct random_access_iterator
	{
		using iterator_category = random_access_iterator_tag;
		using value_type        = T;
		using difference_type   = Distance;
		using pointer           = T*;
		using reference         = T&;
	};

	struct output_iterator
	{
		using iterator_category = output_iterator_tag;
		using value_type        = void;
		using difference_type   = void;
		using pointer           = void;
		using reference         = void;
	};

	// ptrdiff_t类型变量通常用来保存两个指针减法操作的结果
	template<class Category, class T, class Distance = ptrdiff_t, class Point = T*, class Reference = T&>
	struct iterator
	{
		using iterator_category = Category;
		using value_type        = T;
		using difference_type   = Distance;
		using pointer           = T*;
		using reference         = T&;
	};

	// 主模板定义
	template<class Iterator>
	struct iterator_traits
	{
		using iterator_category = typename Iterator::iterator_category;
		using value_type        = typename Iterator::value_type;
		using difference_type   = typename Iterator::difference_type;
		using pointer           = typename Iterator::pointer;
		using reference         = typename Iterator::reference;
	};

	// 针对原生指针(native pointer)设计traits偏特化版本
	template<class T>
	struct iterator_traits<T*>
	{
		using iterator_category = random_access_iterator_tag;
		using value_type        = T;
		using difference_type   = ptrdiff_t;
		using pointer           = T*;
		using reference         = T&;
	};

	// 针对原生指针(pointer-to-const)设计的traits偏特化版本
	template<class T>
	struct iterator_traits<const T*>
	{
		using iterator_category = random_access_iterator_tag;
		using value_type        = T;
		using difference_type   = ptrdiff_t;
		using pointer           = const T*;
		using reference         = const T&;
	};

	// 通过萃取得到迭代器的类型
	template<class Iterator>
	inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator& It)
	{
		using category = typename iterator_traits<Iterator>::iterator_category;
		return category();
	}

	// 函数的目的是为了 方便的决定某个迭代器的类型 value type
	template<class Iterator>
	inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator& It)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	// 函数的目的是为了 方便的决定某个迭代器的类型 distance type
	template<class Iterator>
	inline typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator& It)
	{
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	template<class Container>
	class insert_iterator
	{
	public:
		using iterator_category = output_iterator_tag;
		using value_type        = void;
		using pointer           = void;
		using reference         = void;

		using container_type    = Container;

		using difference_type   = void;
		using Wrapped_iter      = typename Container::iterator;

	public:
		inline explicit insert_iterator(Container& Cont, Wrapped_iter Where) : container(&Cont), iter(Where) {}

	public:
		inline insert_iterator& operator=(const typename Container::value_type& Val)
		{	// insert into container and increment stored iterator
			iter = container->insert(iter, Val);
			++iter;
			return *this;
		}
		// inline insert_iterator& operator=(typename Container::value_type&& Val);
		inline insert_iterator& operator*()
		{	// pretend to return designated value
			return *this;
		}
		inline insert_iterator& operator++()
		{	// pretend to preincrement
			return *this;
		}
		inline insert_iterator& operator++(int)
		{	// pretend to postincrement
			return *this;
		}

	protected:
		Container* container;
		Wrapped_iter iter;
	};

	template<class Container>
	inline insert_iterator<Container> inserter(Container& Cont, typename Container::iterator Where)
	{
		return insert_iterator<Container>(Cont, Where);
	}
}

#endif // !__ITERATOR_H__
