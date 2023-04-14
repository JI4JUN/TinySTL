#ifndef _List_H_
#define _List_H_

#include "Iterator.h"
#include "Allocator.h"
#include "ReserverseIterator.h"
#include "Algorithm.h"

namespace TinySTL
{
	struct list_node_base
	{
		list_node_base* next_;
		list_node_base* prev_;
	};

	template<class T>
	struct list_node : public list_node_base
	{
		T data_;
	};

	struct list_iterator_base
	{
	public:
		using size_type         = size_t;
		using difference_type   = ptrdiff_t;
		using iterator_category = bidirectional_iterator_tag; // 迭代器为双向迭代器
	
	public:
		list_iterator_base(list_node_base* Node) : node_(Node) {}
		list_iterator_base() {}

	public:
		void incr() { node_ = node_->next_; }
		void decr() { node_ = node_->prev_; }

		bool operator==(const list_iterator_base& Iter) const { return node_ == Iter.node_; }
		bool operator!=(const list_iterator_base& Iter) const { return node_ != Iter.node_; }

	public:
		list_node_base* node_ = nullptr;
	};

	template <class T, class Ref, class Ptr>
	struct list_iterator : public list_iterator_base
	{
		using iterator       = list_iterator<T, T&, T*>;
		using const_iterator = list_iterator<T, const T&, const T*>;
		using _Self          = list_iterator<T, Ref, Ptr>;

		using value_type     = T;
		using pointer        = Ptr;
		using reference      = Ref;
		using _Node          = list_node<T>; // 实际指向的类型

	public:
		list_iterator(_Node* Node) : list_iterator_base(Node) {}
		list_iterator() {}
		list_iterator(const iterator& Iter) : list_iterator_base(Iter.node_) {}

	public:
		// 该函数虽然可能修改结点的值,但因迭代器对象只保存指向结点的指针,因此仍然声明为const
		reference operator*() const { return ((_Node*)node_)->data_; }
		pointer operator->() const { return &(operator*()); }

		_Self operator++()
		{
			this->incr();
			return *this;
		}

		_Self operator++(int)
		{
			_Self Tmp = *this;
			this->incr();
			return Tmp;
		}

		_Self operator--()
		{
			this->decr();
			return *this;
		}

		_Self operator--(int)
		{
			_Self Tmp = *this;
			this->decr();
			return Tmp;
		}
	};

	//inline bidirectional_iterator_tag iterator_category(const list_iterator_base&)
	//{
	//	return bidirectional_iterator_tag();
	//}

	//template<class T, class Ref, class Ptr>
	//inline T* value_type(const list_iterator<T, Ref, Ptr>&)
	//{
	//	return 0;
	//}

	//inline ptrdiff_t* distance_type(const list_iterator_base&)
	//{
	//	return 0;
	//}

	template<class T, class Alloc>
	class list_base
	{
	public:
		using allocator_type = Alloc;
		allocator_type get_allocator() const { return allocator_type(); }

	public:
		list_base(const allocator_type&)
		{	//唯一的构造函数,规定了list为空时的合法状态:头结点的前后指针均指向其自身
			node_ = get_node();
			node_->next_ = node_;
			node_->prev_ = node_;
		}
		~list_base()
		{
			clear();         // 将每个结点清除
			put_node(node_); // 将头结点归还
		}

		void clear();

	protected:
		using Alloc_type = simple_alloc<list_node<T>, Alloc>;
		list_node<T>* get_node() { return Alloc_type::allocate(1); }
		void put_node(list_node<T>* Ptr) { Alloc_type::deallocate(Ptr, 1); }

	protected:
		list_node<T>* node_; // 头结点指针，为实际指向结点的类型
	};

	template<class T, class Alloc>
	inline void list_base<T, Alloc>::clear()
	{	// 由于结点next_均为基类指针，而基类指针不能直接初始化或赋值给派生类指针
		// 因此需要强制类型转化，将node_->next_强制转化为其实质类型的指针
		list_node<T>* Cur = (list_node<T>*)(node_->next_);
		while (Cur != node_)
		{
			list_node<T>* Tmp = Cur;
			Cur = (list_node<T>*)(Cur->next_);
			TinySTL::destroy(&Tmp->data_); // 析构结点数据元素
			put_node(Tmp);				   // 归还结点内存
		}
		// 使链表恢复合法状态
		node_->next_ = node_;
		node_->prev_ = node_;
	}
	
	template<class T, class Alloc = TinySTL::alloc>
	class list : protected list_base<T, Alloc>
	{
	private:
		using base_           = list_base<T, Alloc>;

	public:
		using value_type      = T;
		using pointer         = value_type*;
		using const_pointer   = const value_type*;
		using reference       = value_type&;
		using const_reference = const value_type&;
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;
		using _Node           = list_node<T>;

		using allocator_type  = typename base_::allocator_type;
		allocator_type get_allocator() const { return base_::get_allocator(); }

		using iterator               = list_iterator<T, T&, T*>;
		using const_iterator         = list_iterator<T, const T&, const T*>;
		using const_reverse_iterator = TinySTL::reverse_iterator<const_iterator>;
		using reverse_iterator       = TinySTL::reverse_iterator<iterator>;

	protected:
		using base_::node_;
		using base_::get_node;
		using base_::put_node;

	public:
		explicit list(const allocator_type& Alloc = allocator_type()) : base_(Alloc) {}
		explicit list(size_type Count) : base_(allocator_type())
		{
			insert(begin(), Count, value_type());
		}
		explicit list(size_type Count, const value_type& Val, const allocator_type& Alloc = allocator_type()) : base_(Alloc)
		{
			insert(begin(), Count, Val);
		}
		template<class InIt>
		list(InIt First, InIt Last, const allocator_type& Alloc = allocator_type()) : base_(Alloc)
		{
			insert(begin(), First, Last);
		}
		list(const list<T, Alloc>& Other) : base_(Other.get_allocator())
		{
			insert(begin(), Other.begin(), Other.end());
		}
		~list() {} //善后留给基类中的析构函数

		list<T, Alloc>& operator=(const list<T, Alloc>& Right);

	protected:
		_Node* create_node(const T& Val) // 用特定数据结构构造结点
		{
			_Node* Ptr = get_node();
			try
			{
				TinySTL::construct(&Ptr->data_, Val);
			}
			catch (...)
			{
				put_node(Ptr);
			}
			return Ptr;
		}

		_Node* create_node()             // 构造含默认值的结点
		{
			_Node* Ptr = get_node();
			try
			{
				TinySTL::construct(&Ptr->data_, T());
			}
			catch (...)
			{
				put_node(Ptr);
			}
			return Ptr;
		}

	protected:
		template<class Integer>
		void assign_dispatch(Integer Count, Integer Val, true_type);
		template<class InIt>
		void assign_dispatch(InIt First, InIt Last, false_type);
		void fill_assign(size_type Count, const T& Val);

		void fill_insert(iterator Where, size_type Count, const value_type& Val);
		template<class Integer>
		void insert_dispatch(iterator Where, Integer Count, Integer Val, true_type);
		template<class InIt>
		void insert_dispatch(iterator Where, InIt First, InIt Last, false_type);

		// 将[First,Last)从原位置中摘下来,插入到Where之前
		// 这个函数主要通过指针的修改来完成
		void transfer(iterator Where, iterator First, iterator Last);

	public:
		// Iterator:
		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
		reverse_iterator rbegin();
		const_reverse_iterator rbegin() const;
		reverse_iterator rend();
		const_reverse_iterator rend() const;
		const_iterator cbegin() const;
		const_reverse_iterator crbegin() const;
		const_iterator cend() const;
		const_reverse_iterator crend() const;

		// Capacity:
		bool empty() const;
		size_type size() const;
		size_type max_size() const;

		// Element acces:
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;

		// Modifiers:
		template<class InIt>
		void assign(InIt First, InIt Last);
		void assign(size_type Count, const value_type& Val);
		void push_front(const value_type& Val);
		void pop_front();
		void push_back(const value_type& Val);
		void pop_back();
		iterator insert(iterator Where, const value_type& Val);
		void insert(iterator Where, size_type Count, const value_type& Val);
		template<class InIt>
		void insert(iterator Where, InIt First, InIt Last);
		iterator erase(iterator Where);
		iterator erase(iterator First, iterator Last);
		void swap(list<T, Alloc>& Other);
		void resize(size_type Newsize);
		void resize(size_type Newsize, const value_type& Val);
		void clear();

	public:
		void splice(iterator Where, list& Right);
		void splice(iterator Where, list& Right, iterator First);
		void splice(iterator Where, list& Right, iterator First, iterator Last);
		void remove(const T& Val);
		template<class Pr1>
		void remove_if(Pr1 Pred);
		void unique();
		template<class Pr2>
		void unique(Pr2 Pred);
		void merge(list& Right);
		template<class Pr2>
		void merge(list& Right, Pr2 Pred);
		void sort();
		template<class Pr2>
		void sort(Pr2 Pred);
		void reverse();
	};

	//////////////////////////////////////////// 实现 //////////////////////////////////////////////////////////

	template<class T, class Alloc>
	inline list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& Right)
	{
		if (this != &Right)
		{
			iterator First1 = begin();
			iterator Last1 = end();
			const_iterator First2 = Right.begin();
			const_iterator Last2 = Right.end();
			while (First1 != Last1 && First2 != Last2)
			{
				*First1 = *First2;
				++First1;
				++First2;
			}
			if (First2 == Last2) // Right.size() <= this.size()
			{
				erase(First1, Last1);
			}
			else
			{
				insert(Last1, First2, Last2);
			}
		}
		return *this;
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::transfer(iterator Where, iterator First, iterator Last)
	{
		if (Where != Last)
		{	// Remove [First, Last) from its old Where
			Last.node_->prev_->next_ = Where.node_;
			First.node_->prev_->next_ = Last.node_;
			Where.node_->prev_->next_ = First.node_;
			// Splice [First, Last) into its new position
			list_node_base* Tmp = Where.node_->prev_;
			Where.node_->prev_ = Last.node_->prev_;
			Last.node_->prev_ = First.node_->prev_;
			First.node_->prev_ = Tmp;
		}
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::iterator list<T, Alloc>::begin()
	{
		return (_Node*)(node_->next_);
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_iterator list<T, Alloc>::begin() const
	{
		return (_Node*)(node_->next_);
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::iterator list<T, Alloc>::end()
	{
		return node_;
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_iterator list<T, Alloc>::end() const
	{
		return node_;
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rbegin()
	{
		return reverse_iterator(begin());
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::rbegin() const
	{
		return const_reverse_iterator(begin());
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rend()
	{
		return reverse_iterator(end());
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::rend() const
	{
		return const_reverse_iterator(end());
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const
	{
		return begin();
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crbegin() const
	{
		return rbegin();
	}
	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_iterator list<T, Alloc>::cend() const
	{
		return end();
	}
	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crend() const
	{
		return rend();
	}

	template<class T, class Alloc>
	inline bool TinySTL::list<T, Alloc>::empty() const
	{
		return node_->next_ == node_;
	}

	//template<class T, class Alloc>
	//inline typename list<T, Alloc>::size_type list<T, Alloc>::size() const
	//{	// list的size函数需要遍历真个list
	//	size_type Result = static_cast<size_type>(TinySTL::distance(begin(), end());
	//	return Result;
	//}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::size_type list<T, Alloc>::max_size() const
	{
		return size_type(-1) / sizeof(T);
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::reference TinySTL::list<T, Alloc>::front()
	{
		return *begin();
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_reference TinySTL::list<T, Alloc>::front() const
	{
		return *begin();
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::reference TinySTL::list<T, Alloc>::back()
	{
		return *(--end());
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::const_reference TinySTL::list<T, Alloc>::back() const
	{
		return *(--end());
	}

	template<class T, class Alloc>
	template<class Integer>
	inline void list<T, Alloc>::assign_dispatch(Integer Count, Integer Val, true_type)
	{
		fill_assign(static_cast<size_type>(Count), static_cast<size_type>(Val));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void list<T, Alloc>::assign_dispatch(InIt First, InIt Last, false_type)
	{
		iterator Begin = begin();
		iterator End = end();
		for (; Begin != End && First != Last; ++Begin, (void)++First)
		{
			*Begin = *First;
		}
		if (First == Last)
		{
			erase(Begin, End);
		}
		else
		{
			insert(End, First, Last);
		}
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::fill_assign(size_type Count, const T& Val)
	{
		iterator Start = begin();
		for (; Start != end() && Count > 0; ++Start, --Count)
		{
			*Start = Val;
		}
		if (Count > 0)
		{
			insert(end(), Count, Val);
		}
		else
		{
			erase(Start, end());
		}
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::fill_insert(iterator Where, size_type Count, const value_type& Val)
	{
		for (; Count > 0; --Count)
		{
			insert(Where, Val);
		}
	}

	template<class T, class Alloc>
	template<class Integer>
	inline void list<T, Alloc>::insert_dispatch(iterator Where, Integer Count, Integer Val, true_type)
	{
		fill_insert(Where, static_cast<size_type>(Count), static_cast<T>(Val));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void list<T, Alloc>::insert_dispatch(iterator Where, InIt First, InIt Last, false_type)
	{
		for (; First != Last; ++First)
		{
			insert(Where, *First);
		}
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void list<T, Alloc>::assign(InIt First, InIt Last)
	{
		using Integral = typename TinySTL::is_integral<InIt>::type;
		assign_dispatch(First, Last, Integral());
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::assign(size_type Count, const value_type& Val)
	{
		fill_assign(Count, Val);
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::push_front(const value_type& Val)
	{
		insert(begin(), Val);
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::pop_front()
	{
		erase(begin());
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::push_back(const value_type& Val)
	{
		insert(end(), Val);
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::pop_back()
	{
		iterator Tmp = end();
		erase(--Tmp);
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator Where, const value_type& Val)
	{	//该函数只需生成一个新节点,然后修改相关指针将该节点“链”到合适位置即可
		_Node* Tmp = create_node(Val);
		Tmp->next_ = Where.node_;
		Tmp->prev_ = Where.node_->prev_;
		Where.node_->prev_->next_ = Tmp;
		Where.node_->prev_ = Tmp;
		return Tmp;
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::insert(iterator Where, size_type Count, const value_type& Val)
	{
		fill_insert(Where, Count, Val);
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void list<T, Alloc>::insert(iterator Where, InIt First, InIt Last)
	{
		using Integral = typename TinySTL::is_integral<InIt>::type;
		insert_dispatch(Where, First, Last, Integral());
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator Where)
	{
		list_node_base* Next_node = Where.node_->next_;
		list_node_base* Prev_node = Where.node_->prev_;
		_Node* Freenode = (_Node*)Where.node_;
		Prev_node->next_ = Next_node;
		Next_node->prev_ = Prev_node;
		TinySTL::destroy(&Freenode->data_);
		put_node(Freenode);
		return iterator((_Node*)Next_node);
	}

	template<class T, class Alloc>
	inline typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator First, iterator Last)
	{
		while (First != Last)
		{
			erase(First++);
		}
		return Last;
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::swap(list<T, Alloc>& Other)
	{
		TinySTL::swap(node_, Other.node_);
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::resize(size_type Newsize)
	{
		resize(Newsize, T());
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::resize(size_type Newsize, const value_type& Val)
	{
		iterator Start = begin();
		size_type Len = 0;
		for (; Start != end() && Len < Newsize; ++Start, ++Len);
		if (Len == Newsize)
		{
			erase(Start, end());
		}
		else
		{
			insert(end(), Newsize - Len, Val);
		}
	}
	template<class T, class Alloc>
	inline void list<T, Alloc>::clear()
	{
		base_::clear();
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::splice(iterator Where, list& Right)
	{
		if (!Right.empty())
		{
			this->transfer(Where, Right.begin(), Right.end());
		}
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::splice(iterator Where, list& Right, iterator First)
	{   // splice _Right [_First, _First + 1) at _Where
		iterator Last = First;
		++Last;
		if (Where == First || Where == Last) return;
		this->transfer(Where, First, Last);
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::splice(iterator Where, list& Right, iterator First, iterator Last)
	{
		if (First != Last)
		{
			this->transfer(Where, First, Last);
		}
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::remove(const T& Val)
	{
		iterator First = begin();
		iterator Last = end();
		while (First != Last)
		{
			iterator Next = First;
			++Next;
			if (*First == Val)
				erase(First);
			First = Next;
		}
	}

	template<class T, class Alloc>
	template<class Pr1>
	inline void list<T, Alloc>::remove_if(Pr1 Pred)
	{
		iterator First = begin();
		iterator Last = end();
		while (First != Last)
		{
			iterator Next = First;
			++Next;
			if (Pred(*First))
				erase(First);
			First = Next;
		}
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::unique()
	{
		iterator First = begin();
		iterator Last = end();
		if (First == Last) return;
		iterator Next = First;
		while (++Next != Last)
		{
			if (*First == *Next)
				erase(Next);
			else
				First = Next;
			Next = First;
		}
	}

	template<class T, class Alloc>
	template<class Pr2>
	inline void list<T, Alloc>::unique(Pr2 Pred)
	{
		iterator First = begin();
		iterator Last = end();
		if (First == Last) return;
		iterator Next = First;
		while (++Next != *Last)
		{
			if (Pred(*First, *Last))
				erase(Next);
			else
				First = Next;
			Next = First;
		}
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::merge(list& Right)
	{   // merge in elements from _Right, both ordered by _Pred
		iterator First1 = begin();
		iterator Last1 = end();
		iterator First2 = Right.begin();
		iterator Last2 = Right.end();
		while (First1 != Last1 && First2 != Last2)
		{
			if (*First2 < *First1)
			{
				iterator Next = First2;
				transfer(First1, First2, ++Next);
				First2 = Next;
			}
			else
				++First1;
		}
		if (First2 != Last2)
			transfer(Last1, First2, Last2);
	}

	template<class T, class Alloc>
	template<class Pr2>
	inline void TinySTL::list<T, Alloc>::merge(list& Right, Pr2 Pred)
	{
		iterator First1 = begin();
		iterator Last1 = end();
		iterator First2 = Right.begin();
		iterator Last2 = Right.end();
		while (First1 != Last1 && First2 != Last2)
		{
			if (Pred(*First2, *First1))
			{
				iterator Next = First2;
				transfer(First1, First2, ++Next);
				First2 = Next;
			}
			else
				++First1;
		}
		if (First2 != Last2)
			transfer(Last1, First2, Last2);
	}

	// 由于STL sort算法要求必须为随机迭代器,因此list实现了自己的专用sort算法
	// 该算法采用的是归并排序的思想
	template<class T, class Alloc>
	inline void list<T, Alloc>::sort()
	{	// Do nothing if the list has length 0 or 1
		if (node_->next_ == node_ || node_->next_->next_ == node_)
			return;
		// 一些新的list，作为中介数据存放区
		list<T, Alloc> Carry;
		list<T, Alloc> Counter[64];
		/*
		*其中对于counter[i]里面最多的存储数据为2^(i+1)个节点
		*若超出则向高位进位即counter[i+1]
		*/
		int Fill = 0;
		while (!empty())
		{
			Carry.splice(Carry.begin(), *this, begin()); // Carry得到list第一个元素,list的第一个元素消失
			int Idx = 0;
			// 此循环将Counter[Fill]之前所有非空链表合并为一个链表
			while (Idx < Fill && !Counter[Idx].empty())
			{
				Counter[Idx].merge(Carry);  // 此时Carry为空
				Carry.swap(Counter[Idx++]); // 此时Counter[Idx]为空，Idx = Idx + 1
			}
			Carry.swap(Counter[Idx]); // 至此处Idx之前的所有链表均被合并至Counter[Idx]
			if (Idx == Fill)
				++Fill;
		}
		for (int Idx = 1; Idx < Fill; ++Idx)
			Counter[Idx].merge(Counter[Idx - 1]);
		swap(Counter[Fill - 1]);
	}

	template<class T, class Alloc>
	template<class Pr2>
	inline void TinySTL::list<T, Alloc>::sort(Pr2 Pred)
	{   // Do nothing if the list has length 0 or 1
		if (node_->next_ == node_ || node_->next_->next_ == node_)
			return;
		// 一些新的list，作为中介数据存放区
		list<T, Alloc> Carry;
		list<T, Alloc> Counter[64];
		int Fill = 0;
		while (!empty())
		{
			Carry.splice(Carry.begin(), *this, begin()); // Carry得到list第一个元素,list的第一个元素消失
			int Idx = 0;
			// 此循环将Counter[Fill]之前所有非空链表合并为一个链表
			while (Idx < Fill && !Counter[Idx].empty())
			{
				Counter[Idx].merge(Carry, Pred);  // 此时Carry为空
				Carry.swap(Counter[Idx++]); // 此时Counter[Idx]为空，Idx = Idx + 1
			}
			Carry.swap(Counter[Idx]); // 至此处Idx之前的所有链表均被合并至Counter[Idx]
			if (Idx == Fill)
				++Fill;
		}
		for (int Idx = 1; Idx < Fill; ++Idx)
			Counter[Idx].merge(Counter[Idx - 1], Pred);
		swap(Counter[Fill - 1]);
	}

	template<class T, class Alloc>
	inline void list<T, Alloc>::reverse()
	{
		//list_node_base* Tmp = this->node_;
		//do
		//{
		//	TinySTL::swap(Tmp->next_, Tmp->prev_);
		//	Tmp = Tmp->prev_; // Old next node is now prev
		//}
		//while (Tmp != this->node_) // 由于是循环链表,故指针再次指向this->node_时,说明list翻转完毕
		
		if (node_->next_ == node_ || node_->next_->next_ == node_)
			return;
		iterator First = begin();
		++First;
		while (First != end())
		{
			iterator Old = First;
			++First;
			transfer(begin(), Old, First);
		}
	}
}

#endif // !_List_H_
