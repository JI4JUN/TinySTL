#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "Deque.h"
#include "Vector.h"
#include "Utility.h"
#include "Algorithm.h"

namespace TinySTL
{
	template<class T, class Container = TinySTL::deque<T>>
	class queue
	{
	public:
		using Container_type  = Container;
		using value_type      = typename Container::value_type;
		using size_type       = typename Container::size_type;
		using reference       = typename Container::reference;
		using const_reference = typename Container::const_reference;

	protected:
		Container c; // 底层容器

	public:
		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference& front() { return c.front(); }
		const_reference& front() const { return c.front(); }
		reference& back() { return c.back(); }
		const_reference& back() const { return c.back(); }
		void push(const value_type& val) { c.push_back(val); }
		void pop() { c.pop_back(); }
		void swap(queue& Other) { c.swap(Other.c); }
	};

	template<class T, class Container>
	bool operator==(const queue<T, Container>& x, const queue<T, Container>& y)
	{
		return x.c == y.c;
	}

	template<class T, class Container>
	bool operator!=(const queue<T, Container>& x, const queue<T, Container>& y)
	{
		return !(x.c == y.c);
	}

	////////////////////////////priority_queue/////////////////////////////
	template<class T, class Container = TinySTL::vector<T>, class Compare = TinySTL::less<typename Container::value_type>>
	class priority_queue
	{
	public:
		using Container_type  = Container;
		using value_type      = typename Container::value_type;
		using size_type       = typename Container::size_type;
		using reference       = typename Container::reference;
		using const_reference = typename Container::const_reference;

	protected:
		Container c;  // 底层容器
		Compare comp; // 元素大小比较标准

	public:
		priority_queue() : c() {}
		explicit priority_queue(const Compare& cmp) : c(), comp(cmp) {}
		// 以下用到的make_heap(), push_heap(), pop_heap()都是泛型算法
		// 任一构造函数都立刻于底层容器内产生一个implict representation heap
		template<class InIt>
		priority_queue(InIt First, InIt Last, const Compare& cmp) : c(First, Last), comp(cmp)
		{
			make_heap(c.begin(), c.end(), comp);
		}
		template<class InIt>
		priority_queue(InIt First, InIt Last) : c(First, Last)
		{
			make_heap(c.begin(), c.end(), comp);
		}

	public:
		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		const_reference top() const { return c.front(); }
		void push(const value_type& Val)
		{
			try
			{	// push_heap是泛型算法，先利用底层容器的push_back()将新元素推入末端，再重排heap
				c.push_back(Val);
				push_heap(c.begin(), c.end(), comp);
			}
			catch (...)
			{
				c.clear();
			}
		}
		void pop()
		{
			try
			{	// pop_heap是泛型算法，从heap内取出一个元素，他并不是真正将元素弹出，而是重排heap
				pop_heap(c.begin(), c.end(), comp);
				c.pop_back();
			}
			catch (...)
			{
				c.clear();
			}
		}
	};
}

#endif // !_QUEUE_H_
