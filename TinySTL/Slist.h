#ifndef _SLIST_H_
#define _SLIST_H_

#include "Iterator.h"
#include "Alloc.h"
#include "Allocator.h"
#include "TypeTraits.h"
#include "Algorithm.h"

namespace TinySTL
{
	// 单项链表的节点基本结构
	struct _slist_node_base
	{
		_slist_node_base* _M_next;
	};

	// 将__new_node链在__prev_node后面
	inline _slist_node_base* __slit_make_link(_slist_node_base* __prev_node, _slist_node_base* __new_node)
	{
		__new_node->_M_next = __prev_node->_M_next;
		__prev_node->_M_next = __new_node;
		return __new_node;
	}

	// 查找__node的前一个结点
	inline _slist_node_base* __slist_previous(_slist_node_base* __head, const _slist_node_base* __node)
	{
		while (__head && __head->_M_next != __node)
			__head = __head->_M_next;
		return __head;
	}

	inline const _slist_node_base* __slist_previous(const _slist_node_base* __head, const _slist_node_base* __node)
	{
		while (__head && __head->_M_next != __node)
			__head = __head->_M_next;
		return __head;
	}

	// 将(__before_first, __before_last]从原位置摘下来，插入到__pos之后
	inline void __slist_splice_after(_slist_node_base* __pos, _slist_node_base* __before_first, _slist_node_base* __before_last)
	{
		if (__pos != __before_first && __pos != __before_last)
		{
			_slist_node_base* __first = __before_first->_M_next;
			_slist_node_base* __after = __pos->_M_next;
			__before_first->_M_next = __before_last->_M_next;
			__pos->_M_next = __first;
			__before_last->_M_next = __after;
		}
	}

	// 将(__head, nullptr)从原位置摘下来，插入__pos之后
	inline void __slist_splice_after(_slist_node_base* __pos, _slist_node_base* __head)
	{
		_slist_node_base* __before_last = __slist_previous(__head, nullptr);
		if (__before_last != __head)
		{
			_slist_node_base* __after = __pos->_M_next;
			__pos->_M_next = __head->_M_next;
			__head->_M_next = nullptr;
			__before_last->_M_next = __after;
		}
	}

	// 从node开始，将整个链表翻转
	inline _slist_node_base* __slist_reverse(_slist_node_base* __node)
	{
		_slist_node_base* __result = __node;
		__node = __node->_M_next;
		__result->_M_next = nullptr;
		while (__node)
		{
			_slist_node_base* __next = __node->_M_next;
			__node->_M_next = __result; // 将_M_next指向前一个结点
			__result = __node;
			__node = __next;
		}
		return __result;
	}

	// 计算[__node, 0)的节点数
	inline size_t __slist_size(_slist_node_base* __node)
	{
		size_t __result = 0;
		for (; __node != nullptr; __node = __node->_M_next)
			++__result;
		return __result;
	}

	template<class T>
	struct _slist_node : public _slist_node_base
	{
		T _M_data ;
	};

	struct _slist_iterator_base
	{
		using size_type         = size_t;
		using different_type    = ptrdiff_t;
		using iterator_category = forward_iterator_tag; // 单向

		_slist_node_base* _M_node;  // 指向节点基本结构

		_slist_iterator_base(_slist_node_base* x) : _M_node(x) {}

		void incr() { _M_node = _M_node->_M_next; } // 前进一个节点

		bool operator==(const _slist_iterator_base& x) const { return _M_node == x._M_node; }
		bool operator!=(const _slist_iterator_base& x) const { return !(_M_node == x._M_node); }
	};

	// 单向链表的迭代器结构
	template<class T, class Ref, class Ptr>
	struct _slist_iterator : public _slist_iterator_base
	{
		using iterator       = _slist_iterator<T, T&, T*>;
		using const_iterator = _slist_iterator<T, const T&, const T*>;
		using self           = _slist_iterator<T, Ref, Ptr>;

		using value_type     = T;
		using pointer        = Ptr;
		using reference      = Ref;
		using list_node      = _slist_node<T>;

		//using list_node_p      = _slist_node<T>*;

		_slist_iterator(list_node* x) : _slist_iterator_base(x) {}
		// 调用slist<T>::end()时会造成_slist_iterator(0)，于是调用上述函数
		_slist_iterator() : _slist_iterator_base(0) {}
		_slist_iterator(const iterator& x) : _slist_iterator_base(x._M_node) {}

		reference operator*() const { return ((list_node*)_M_node)->_M_data; }
		//reference operator*() const { return list_node_p(_M_node)->_M_data; }
		pointer operator->() const { return &(operator*()); }

		self& operator++() // 前置
		{
			incr();
			return *this;
		}
		self& operator++(int) // 后置
		{
			self tmp = *this;
			incr();
			return *this;
		}
	};

	template<class T, class Alloc>
	struct _slist_base
	{
		using allocator_type = Alloc;
		allocator_type get_allocator() const { return allocator_type(); }

		_slist_base(const allocator_type&) { _M_head._M_next = nullptr; }
		~_slist_base() {} // 清空链表

	protected:
		using Alloc_type = simple_alloc<_slist_node<T>, Alloc>;

		_slist_node<T>* _M_get_node() { return Alloc_type::allocate(1); }
		void _M_put_node(_slist_node<T>* __p) { Alloc_type::deallocate(__p, 1); }

		// 删除__pos->_M_next
		_slist_node_base* _M_erase_after(_slist_node_base* __pos)
		{
			_slist_node<T>* __next = (_slist_node<T>*)(__pos->_M_next);
			_slist_node_base* __next_next = __next->_M_next;
			__pos->_M_next = __next_next;
			destroy(&__next->_M_data );
			_M_put_node(__next);
			return __next_next;
		}
		// 删除(__before_first,__last_node)
		_slist_node_base* _M_erase_after(_slist_node_base* __before_first, _slist_node_base* __last_node);

	protected:
		_slist_node_base _M_head;
	};

	template<class T, class Alloc>
	inline _slist_node_base* _slist_base<T, Alloc>::_M_erase_after(_slist_node_base* __before_first, _slist_node_base* __last_node)
	{
		_slist_node<T>* __cur =  (_slist_node<T>*)(__before_first->_M_next);
		while (__cur != __last_node)
		{
			_slist_node<T>* __tmp = __cur;
			__cur = (_slist_node<T>*)(__cur->_M_next);
			destroy(&__tmp->_M_data );
			_M_put_node(__tmp);
		}
		__before_first->_M_next = __last_node;
		return __last_node;
	}

	template<class T, class Alloc = TinySTL::alloc>
	class slist : private _slist_base<T, alloc>
	{
	private:
		using _Base = _slist_base<T, alloc>;

	public:
		using value_type      = T;
		using pointer         = value_type*;
		using const_pointer   = const pointer;
		using reference       = value_type&;
		using const_reference = const reference;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;

		using iterator        = _slist_iterator<T, T&, T*>;
		using const_iterator  = _slist_iterator<T, const T&, const T*>;

		using allocator_type  = typename _Base::allocator_type;
		allocator_type get_allocator() const { return _Base::get_allocator(); }

	private:
		using _Node          = _slist_node<T>;
		using _Node_base     = _slist_node_base;
		using _Iterator_base = _slist_iterator_base;

		// 构造一个数据元素为x的结点
		_Node* _M_create_node(const value_type& x)
		{
			_Node* __node = this->_M_get_node();
			try
			{
				construct(&__node->_M_data , x);
				__node->_M_next = nullptr;
			}
			catch (...)
			{
				this->_M_put_node(__node);
			}

			return __node;
		}
		_Node* _M_create_node()
		{
			_Node* __node = this->_M_get_node();
			try
			{
				construct(&__node->_M_data );
				__node->_M_next = nullptr;
			}
			catch (...)
			{
				this->_M_put_node(__node);
			}

			return __node;
		}

	public:
		explicit slist(const allocator_type& __a = allocator_type()) : _Base(__a) {}
		slist(size_type __n, const value_type& __x, const allocator_type& __a = allocator_type()) : _Base(__a)
		{
			_M_insert_after_fill(&this->_M_head, __n, __x);
		}
		slist(size_type __n) : _Base(allocator_type())
		{
			_M_insert_after_fill(&this->_M_head, __n, value_type());
		}


		// We don't need any dispatching tricks here, because _M_insert_after_range
		// already does them
		template<class InIt>
		slist(InIt __first, InIt __last, const allocator_type& __a = allocator_type()) : _Base(__a)
		{
			_M_insert_after_range(&this->_M_head, __first, __last);
		}
		template<class InIt>
		slist(const slist& __x) : _Base(__x.get_allocator())
		{
			_M_insert_after_range(&this->_M_head, __x.begin(), __x.end());
		}

		slist& operator=(const slist& __x);

		~slist() {} // 善后留给基类析构函数

	public:
		void assign(size_type __n, const T& __val) { _M_fill_assign(__n, __val); }
		void _M_fill_assign(size_type __n, const T& __val);

		template<class InIt>
		void assign(InIt __first, InIt __last)
		{
			using _Integral = typename TinySTL::is_integral<InIt>::type;
			_M_assign_dispatch(__first, __last, _Integral());
		}
		template<class Integer>
		void _M_assign_dispatch(Integer __n, Integer __val, true_type)
		{
			_M_fill_assign((size_type)__n, (T)__val);
		}
		template<class InIt>
		void _M_assign_dispatch(InIt __first, InIt __last, false_type);

	public:
		iterator begin() { return iterator((_Node*)this->_M_head._M_next); }
		const_iterator begin() const { return const_iterator((_Node*)this->_M_head._M_next); }
		iterator end() { return iterator(nullptr); }
		const_iterator end() const { return const_iterator(nullptr); }

		iterator before_begin() { return iterator((_Node*)&this->_M_head); }
		const_iterator before_begin() const { return const_iterator((_Node*)&this->_M_head); }

		size_type size() const { return __slist_size(this->_M_head._M_next); }

		size_type max_size() const { return size_type(-1); }

		bool empty() const { return this->_M_head._M_next == nullptr; }

		void swap(slist& __x) { TinySTL::swap(this->_M_head._M_next, __x._M_head._M_next); }

	public:
		reference front() { return ((_Node*)this->_M_head._M_next)->_M_data; }
		const_reference front() const { return ((_Node*)this->_M_head._M_next)->_M_data; }

		void push_front(const value_type& __x)
		{
			__slit_make_link(&this->_M_head, _M_create_node(__x));
		}
		void push_front()
		{
			__slit_make_link(&this->_M_head, _M_create_node());
		}

		void pop_front()
		{
			_Node* __node = (_Node*)this->_M_head._M_next;
			this->_M_head._M_next = __node->_M_next;
			destroy(&__node->_M_data);
			this->_M_put_node(__node);
		}

		iterator previous(const_iterator __pos)
		{
			return iterator((_Node*)__slist_previous(&this->_M_head, __pos._M_node));
		}
		const_iterator previous(const_iterator __pos) const
		{
			return const_iterator((_Node*)__slist_previous(&this->_M_head, __pos._M_node));
		}

	private:
		_Node* _M_insert_after(_Node_base* __pos, const value_type& __x)
		{
			return (_Node*)(__slit_make_link(__pos, _M_create_node(__x)));
		}
		_Node* _M_insert_after(_Node_base* __pos)
		{
			return (_Node*)(__slit_make_link(__pos, _M_create_node()));
		}

		// 在__pos之后插入__n个数据值为__x的结点
		void _M_insert_after_fill(_Node_base* __pos, size_type __n, const value_type& __x)
		{
			for (size_t i = 0; i < __n; i++)
			{
				__pos = __slit_make_link(__pos, _M_create_node(__x));
			}
		}

		template<class InIt>
		void _M_insert_after_range(_Node_base* __pos, InIt __first, InIt __last)
		{
			using _Integral = typename TinySTL::is_integral<InIt>::type;
			_M_insert_after_range(__pos, __first, __last, _Integral());
		}
		template<class _Integer>
		void _M_insert_after_range(_Node_base* __pos, _Integer __n, _Integer __x, true_type)
		{
			_M_insert_after_fill(__pos, __n, __x);
		}
		template<class InIt>
		void _M_insert_after_range(_Node_base* __pos, InIt __first, InIt __last, false_type)
		{
			while (__first != __last)
			{
				__pos = __slit_make_link(__pos, _M_create_node(*__first));
				++__first;
			}
		}

	public:
		iterator insert_after(iterator __pos, const value_type& __x)
		{
			return iterator(_M_insert_after(__pos._M_node, __x));
		}
		iterator insert_after(iterator __pos)
		{
			return insert_after(__pos, value_type());
		}

		void insert_after(iterator __pos, size_type __n, const value_type& __x)
		{
			_M_insert_after_fill(__pos._M_node, __n, __x);
		}

		template<class InIt>
		void insert_after(iterator __pos, InIt __first, InIt __last)
		{
			_M_insert_after_range(__pos._M_node, __first, __last);
		}

		// 由于slist是单向链表,因此多采用insert_after来实现插入
		// 提供的insert函数也实现找到插入位置的前驱结点,然后调用insert_after来实现的
		iterator insert(iterator __pos, const value_type& __x)
		{
			return iterator(_M_insert_after(__slist_previous(&this->_M_head, __pos._M_node), __x));
		}
		iterator insert(iterator __pos)
		{
			return iterator(_M_insert_after(__slist_previous(&this->_M_head, __pos._M_node), value_type()));
		}

		void insert(iterator __pos, size_type __n, const value_type& __x)
		{
			_M_insert_after_fill(__slist_previous(&this->_M_head, __pos._M_node), __n, __x);
		}

		template<class InIt>
		void insert(iterator __pos, InIt __first, InIt __last)
		{
			_M_insert_after_range(__slist_previous(&this->_M_head, __pos._M_node), __first, __last);
		}

	public:
		iterator erase_after(iterator __pos)
		{
			return iterator((_Node*)this->_M_erase_after(__pos._M_node));
		}
		iterator erase_after(iterator __before_first, iterator __last)
		{
			return iterator((_Node*)this->_M_erase_after(__before_first._M_node, __last._M_node));
		}
	
		//由于slist是单向链表,因此多采用erase_after来实现删除
		//提供的erase函数也实现找到删除位置的前驱结点,然后调用erase_after来实现的
		iterator erase(iterator __pos)
		{
			return (_Node*)this->_M_erase_after(__slist_previous(&this->_M_head, __pos._M_node));
		}
		iterator erase(iterator __first, iterator __last)
		{
			return (_Node*)this->_M_erase_after(__slist_previous(&this->_M_head, __first._M_node), __last._M_node);
		}

		void resize(size_type new_size, const T& __x);
		void resize(size_type new_size) { resize(new_size, T()); }

		void clear() { this->_M_erase_after(&this->_M_head, nullptr); }

	public:
        // Moves the range (__before_first, __before_last ] to *this,
        // inserting it immediately after __pos.  This is constant time.
		void splice_after(iterator __pos, iterator __before_first, iterator __before_last)
		{
			if (__before_first != __before_last)
				__slist_splice_after(__pos._M_node, __before_first._M_node, __before_last._M_node);
		}
        // Moves the element that follows __prev to *this, inserting it immediately
        //  after __pos.  This is constant time.
		void splice_after(iterator __pos, iterator __prev)
		{
			__slist_splice_after(__pos._M_node, __prev._M_node, __prev._M_node->_M_next);
		}
	    // Removes all of the elements from the list __x to *this, inserting
	    // them immediately after __pos.  __x must not be *this.  Complexity:
	    // linear in __x.size().
		void splice_after(iterator __pos, slist& __x)
		{
			__slist_splice_after(__pos._M_node, &__x._M_head);
		}

		// Linear in distance(begin(), __pos), and linear in __x.size().
		void splice(iterator __pos, slist& __x)
		{
			if (__x._M_head._M_next)
			{
				__slist_splice_after(__slist_previous(&this->_M_head, __pos._M_node),
									 &__x._M_head,
									 __slist_previous(&__x._M_head, nullptr));
			}
		}
		// Linear in distance(begin(), __pos), and linear in distance(__x.begin(), __i).
		void splice(iterator __pos, slist& __x, iterator __i)
		{
			__slist_splice_after(__slist_previous(&this->_M_head, __pos._M_node),
								 __slist_previous(&__x->_M_head, __i._M_node),
								 __i._M_node);
		}
        // Linear in distance(begin(), __pos), in distance(__x.begin(), __first),
	    // and in distance(__first, __last).
		void splice(iterator __pos, slist& __x, iterator __first, iterator __last)
		{
			if (__first != __last)
			{
				__slist_splice_after(__slist_previous(&this->_M_head, __pos._M_node),
									 __slist_previous(&__x->_M_head, __first._M_node),
									 __slist_previous(__first._M_node, __last._M_node));
			}
		}

	public:
		void reverse()
		{
			if (this->_M_head._M_next)
				this->_M_head._M_next = __slist_reverse(this->_M_head._M_next);
		}
		void remove(const T& __val);
		void unique();
		void merge(slist& __x); // 合并两个已经排好序的slist
		void sort();

		template<class _Predicate>
		void remove_if(_Predicate __pred);

		template<class _BinaryPredicate>
		void unique(_BinaryPredicate __pred);

		template<class _StrictWeakOrdering>
		void merge(slist& __x, _StrictWeakOrdering __comp);

		template<class _StrictWeakOrdering>
		void sort(_StrictWeakOrdering __comp);
	};

	template<class T, class Alloc>
	inline slist<T, Alloc>& slist<T, Alloc>::operator=(const slist<T, Alloc>& __x)
	{
		if (this != &__x)
		{
			_Node_base* __p1 = &this->_M_head;
			_Node* __n1 = (_Node*)this->_M_head._M_next;
			const _Node* __n2 = (const _Node*)__x._M_head._M_next;
			while (__n1 && __n2)
			{
				__n1->_M_data = __n2->_M_data;
				__p1 = __n1; // 赋值过程中记录前一个节点指针，方便后面的处理
				__n1 = (_Node*)__n1->_M_next;
				__n2 = (const _Node*)__n2->_M_next;
			}

			if (__n2 == nullptr)
				this->_M_erase_after(__p1, nullptr);
			else
				_M_insert_after_range(__p1, const_iterator((_Node*)__n2), const_iterator(nullptr));
		}
	}
	template<class T, class Alloc>
	inline void slist<T, Alloc>::_M_fill_assign(size_type __n, const T& __val)
	{
		_Node_base* __prev = &this->_M_head;
		_Node* __node = (_Node*)this->_M_head._M_next;

		for (; __node != nullptr && __n > 0; --__n)
		{
			__node->_M_data = __val;
			__prev = __node;
			__node = (_Node*)__node->_M_next;
		}

		if (__n > 0)
			_M_insert_after_fill(__prev, __n, __val);
		else
			this->_M_erase_after(__prev, nullptr);
	}

	template<class T, class Alloc>
	inline void slist<T, Alloc>::resize(size_type new_size, const T& __x)
	{
		int __len = new_size;
		_Node_base* __cur = &this->_M_head;
		while (__cur->_M_next != nullptr && __len > 0)
		{
			--__len;
			__cur = __cur->_M_next;
		}
		if (__cur->_M_next)
			this->_M_erase_after(__cur, nullptr);
		else
			_M_insert_after_fill(__cur, __len, __x);
	}

	template<class T, class Alloc>
	inline void slist<T, Alloc>::remove(const T& __val)
	{
		_Node_base* __cur = &this->_M_head;
		while (__cur && __cur->_M_next)
		{
			if (((_Node*)__cur->_M_next)->_M_data == __val) // 比较下一个结点的值和val是否相等
				this->_M_erase_after(__cur);
			else
				__cur = __cur->_M_next;
		}
	}

	template<class T, class Alloc>
	inline void slist<T, Alloc>::unique()
	{
		_Node_base* __cur = this->_M_head._M_next;
		if (__cur)
		{
			while (__cur->_M_next)
			{
				if (((_Node*)__cur)->_M_data == ((_Node*)(__cur->_M_next))->_M_data)
					this->_M_erase_after(__cur);
				else
					__cur = __cur->_M_next;
			}
		}
	}

	template<class T, class Alloc>
	inline void slist<T, Alloc>::merge(slist& __x)
	{
		_Node_base* __n1 = &this->_M_head;
		while (__n1->_M_next && __x._M_head._M_next)
		{
			if (((_Node*)__x._M_head._M_next)->_M_data < ((_Node*)__n1->_M_next)->_M_data)
			{
				__slist_splice_after(__n1, &__x._M_head, __x._M_head._M_next);
			}
			__n1 = __n1->_M_next;
		}

		if (__x._M_head._M_next)
		{
			__n1->_M_next = __x._M_head._M_next;
			__x._M_head._M_next = nullptr;
		}
	}

	// 由于STL sort算法要求必须为随机迭代器,因此slist实现了自己的专用sort算法
	// 该算法采用的是归并排序的思想
	template<class T, class Alloc>
	inline void slist<T, Alloc>::sort()
	{	// Do nothing if the slist has length 0 or 1
		if (this->_M_head._M_next && this->_M_head._M_next->_M_next)
		{
			// 一些新的slist，作为中介数据存放区
			slist __carry;
			slist __counter[64];
			int __fill = 0;
			while (!empty())
			{
				__slist_splice_after(&__carry._M_head, &this->_M_head, this->_M_head._M_next);
				int __i = 0;
				while (__i < __fill && !__counter[__i].empty())
				{
					__counter[__i].merge(__carry);
					__carry.swap(__counter[__i]);
					++__i;
				}
				__carry.swap(__counter[__i]);
				if (__i == __fill)
					++__fill;
			}
			for (int __i = 1; __i < __fill; ++__i)
				__counter[__i].merge(__counter[__i - 1]);
			this->swap(__counter[__fill - 1]);
		}
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void slist<T, Alloc>::_M_assign_dispatch(InIt __first, InIt __last, false_type)
	{
		_Node_base* __prev = &this->_M_head;
		_Node* __node = (_Node*)this->_M_head._M_next;

		while (__node != nullptr && __first != __last)
		{
			__node->_M_data = *__first;
			__prev = __node;
			__node = (_Node*)__node->_M_next;
			++__first;
		}

		if (__first != __last)
			_M_insert_after_range(__prev, __first, __last);
		else
			this->_M_erase_after(__prev, nullptr);
	}
	template<class T, class Alloc>
	template<class _Predicate>
	inline void slist<T, Alloc>::remove_if(_Predicate __pred)
	{
		_Node_base* __cur = &this->_M_head;
		while (__cur->_M_next)
		{
			if (__pred(((_Node*)__cur->_M_next)->_M_data))
				this->_M_erase_after(__cur);
			else
				__cur = __cur->_M_next;
		}
	}

	template<class T, class Alloc>
	template<class _BinaryPredicate>
	inline void slist<T, Alloc>::unique(_BinaryPredicate __pred)
	{
		_Node_base* __cur = this->_M_head._M_next;
		if (__cur)
		{
			while (__cur->_M_next)
			{
				if (((_Node*)__cur)->_M_data == ((_Node*)(__cur->_M_next))->_M_data)
					this->_M_erase_after(__cur);
				else
					__cur = __cur->_M_next;
			}
		}
	}
	template<class T, class Alloc>
	template<class _StrictWeakOrdering>
	inline void slist<T, Alloc>::merge(slist& __x, _StrictWeakOrdering __comp)
	{
		_Node_base* __n1 = &this->_M_head;
		while (__n1->_M_next && __x._M_head._M_next)
		{
			if (__comp(((_Node*)__x._M_head._M_next)->_M_data, ((_Node*)__n1->_M_next)->_M_data))
			{
				__slist_splice_after(__n1, &__x._M_head, &__x._M_head._M_next);
			}
			__n1 = __n1->_M_next;
		}
		if (__x._M_head._M_next)
		{
			__n1->_M_next = __x._M_head._M_next;
			__x._M_head._M_next = nullptr;
		}
	}
	template<class T, class Alloc>
	template<class _StrictWeakOrdering>
	inline void slist<T, Alloc>::sort(_StrictWeakOrdering __comp)
	{
		if (this->_M_head._M_next && this->_M_head._M_next->_M_next)
		{
			slist __carry;
			slist __counter[64];
			int __fill = 0;
			while (!empty())
			{
				__slist_splice_after(&__carry._M_head, &this->_M_head, &this->_M_head._M_next);
				int __i = 0;
				while (__i < __fill && !__counter[__i].empty())
				{
					__counter[__i].merge(__carry, __comp);
					__carry.swap(__counter[__i]);
					++__i;
				}
				__carry.swap(__counter[__i]);
				if (__i == __fill)
					++__fill;
			}
			for (int __i = 1; __i < __fill; ++__i)
				__counter[__i].merge(__counter[__i - 1], __comp);
			this->swap(__counter[__fill - 1]);
		}
	}
}

#endif // !_SLIST_H_

