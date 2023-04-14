#ifndef _STACK_H_
#define _STACK_H_

#include "Deque.h"

namespace TinySTL
{
	template<class T, class Container = TinySTL::deque<T>>
	class stack
	{
	public:
		using Container_type  = Container;
		using value_type      = typename Container::value_type;
		using size_type       = typename Container::size_type;
		using reference       = typename Container::reference;
		using const_reference = typename Container::const_reference;

	protected:
		Container c; // 底层容器

	//public:
	//	stack() : stack(Container()) {} // 默认构造函数
	//	explicit stack(const Container& cont = Container()) : c(cont) {} // 以cont的内容复制构造底层容器c 。此亦为默认构造函数
	//	explicit stack(const Container& cont) : c(cont) {}
	//	stack(const stack& Other) : c(Other.c) {}
	//	template<class InIt>
	//	stack(InIt First, InIt Last)

	public:
		// 以下完全利用底层容器完成stack的操作
		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference top() { return c.back(); }
		const_reference top() const { return c.back(); }
		void push(const value_type& val) { c.push_back(val); }
		void pop() { c.pop_back(); }
	};

	template<class T, class Container>
	bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return lhs.c == rhs.c;
	}

	template<class T, class Container>
	bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return !(lhs.c == rhs.c);
	}

	template<class T, class Container>
	bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return lhs.c < rhs.c;
	}

	template<class T, class Container>
	bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return !(rhs.c < lhs.c);
	}

	template<class T, class Container>
	bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return rhs.c < lhs.c;
	}

	template<class T, class Container>
	bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
	{
		return !(lhs.c < rhs.c);
	}
}


#endif // !_STACK_H_

