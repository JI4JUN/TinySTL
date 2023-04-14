#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "Allocator.h"
#include "ReserverseIterator.h"
#include "UninitializedFunctions.h"

namespace TinySTL
{
	// 设计vector_base的原因是：
	// 如果在vector构造函数中抛出异常，则不会调用vector中的析构函数，导致内存泄漏
	// 如果有vector_base类，则会自动调用vector_base类中的析构函数，释放分配的内存
	template <class T, class Alloc = TinySTL::allocator<T>>
	class vector_base
	{
	public:
		using allocator_type = TinySTL::allocator<T>;

		allocator_type get_allocator() const { return allocator_type(); }

		vector_base(const allocator_type& Alloc)
			: start_(), finish_(), end_of_storage() {}
		vector_base(size_t numElements, const allocator_type& Alloc)
			: start_(allocate(numElements))
			, finish_(start_)
			, end_of_storage(start_ + numElements) {}

		~vector_base()
		{
			size_t Diff = static_cast<size_t>(end_of_storage - start_);
			deallocate(start_, Diff);
		}

	protected:
		T* start_;
		T* finish_;
		T* end_of_storage;

	protected:
		using data_allocator = allocator_type;
		T* allocate(size_t numElements) 
		{ 
			return data_allocator::allocate(numElements); 
		}
		void deallocate(T* ptr, size_t numElements)
		{
			data_allocator::deallocate(ptr, numElements);
		}
	};

	template<class T, class Alloc = TinySTL::allocator<T>>
	class vector : protected vector_base<T, Alloc>
	{
	private:
		using base_           = vector_base<T, Alloc>;

	public:
		using value_type      = T;
		using pointer         = value_type*;
		using const_pointer   = const value_type*;
		using iterator        = value_type*;
		using const_iterator  = const value_type*;
		using reference       = value_type&;
		using const_reference = const value_type&;
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;

		using allocator_type  = typename base_::allocator_type;
		allocator_type get_allocator() const { return base_::allocator_type(); }

		// 先定义const_reverse_iterator,再定义reverse_iterator,否则先定义的reverse_iterator
		// 会将全局空间内的reverse_iterator覆盖掉,从而在定义const_reverse_iterator时,出现错误
		using const_reverse_iterator = TinySTL::reverse_iterator<const_iterator>;
		using reverse_iterator       = TinySTL::reverse_iterator<iterator>;

	protected:
		using base_::allocate;
		using base_::deallocate;
		using base_::start_;
		using base_::finish_;
		using base_::end_of_storage;

	public:
		explicit vector(const allocator_type& Alloc = allocator_type()) : base_(Alloc) {}
		vector(size_type numElements, const_iterator Val, const allocator_type Alloc = allocator_type()) : base_(numElements, Alloc)
		{	// 分配内存在基类的构造函数中完成,此处仅进行初始化
			finish_ = TinySTL::uninitialized_fill_n(start_, numElements, Val);
		}
		explicit vector(size_type numElements) : base_(numElements, allocator_type())
		{
			finish_ = TinySTL::uninitialized_fill_n(start_, numElements, value_type());
		}
		vector(const vector<T, Alloc>& Other) : base_(Other.size(), Other.get_allocator())
		{
			finish_ = TinySTL::uninitialized_copy(Other.begin(), Other.end(), start_);
		}
		// Check whether it's an integral type.  If so, it's not an iterator
		template <class InIt>
		vector(InIt First, InIt Last, const allocator_type& Alloc = allocator_type()) : base_(Alloc)
		{
			initialize_aux(First, Last, TinySTL::is_integral<InIt>());
		}
		~vector()
		{	// 析构所有对象，回收内存在基类析构函数中完成
			TinySTL::destroy(start_, finish_);
		}

	public:
		vector<T, Alloc>& operator=(const vector<T, Alloc>& Right);

	protected:
		template<class Integer>
		void initialize_aux(Integer numElements, Integer Val, TinySTL::true_type);
		template<class InIt>
		void initialize_aux(InIt First, InIt Last, TinySTL::false_type);

		template<class InIt>
		void range_initialize(InIt First, InIt Last);

		template<class FwdIt>
		iterator allocate_and_copy(size_type numElements, FwdIt First, FwdIt Last);

		void insert_aux(iterator Pos, const value_type& Val);
		void insert_aux(iterator Pos);
		void fill_insert(iterator Pos, size_type numElements, const value_type& Val);
		template<class Integer>
		void insert_dispatch(iterator Pos, Integer Count, Integer Val, true_type);
		template<class InIt>
		void insert_dispatch(iterator Pos, InIt First, InIt Last, false_type);
		template<class InIt>
		void range_insert(iterator Pos, InIt First, InIt Last, TinySTL::input_iterator_tag);
		template<class FwdIt>
		void range_insert(iterator Pos, FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag);

		template<class Integer>
		void assign_dispatch(Integer Count, Integer Val, true_type);
		template<class InIt>
		void assign_dispatch(InIt First, InIt Last, false_type);	
		void fill_assign(size_type Count, const value_type& Val);
		template<class InIt>
		void assign_aux(InIt First, InIt Last, TinySTL::input_iterator_tag);
		template<class FwdIt>
		void assign_aux(FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag);
	
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

		// Element access:
		reference operator[](size_type Pos);
		const_reference operator[](size_type Pos) const;
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;

		// Capacity:
		size_type size() const;
		size_type max_size() const;
		void resize(size_type newSize, const value_type& Val);
		void resize(size_type newSize);
		size_type capacity() const;
		bool empty() const;
		void reserve(size_type Newcapacity);

		// Modifiers:
		template<class InIt>
		void assign(InIt First, InIt Last);
		void assign(size_type numElements, const value_type& Val);
		void push_back(const value_type& Val);
		void pop_back();
		iterator insert(iterator Pos, const value_type& Val);
		void insert(iterator Pos, size_type numElements, const value_type& Val);
		template<class InIt>
		void insert(iterator Pos, InIt First, InIt Last);
		iterator erase(iterator Pos);
		iterator erase(iterator First, iterator Last);
		void swap(vector& Other);
		void clear();
	};

//////////////////////////////////////////// 实现 //////////////////////////////////////////////////////////

	template<class T, class Alloc>
	template<class Integer>
	inline void vector<T, Alloc>::initialize_aux(Integer numElements, Integer Val, TinySTL::true_type)
	{
		start_ = allocate(numElements);
		end_of_storage = start_ + numElements;
		finish_ = TinySTL::uninitialized_fill_n(start_, numElements, Val);
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void vector<T, Alloc>::initialize_aux(InIt First, InIt Last, TinySTL::false_type)
	{
		range_initialize(First, Last);
	}

	// This function is only called by the constructor.
	template<class T, class Alloc>
	template<class InIt>
	inline void vector<T, Alloc>::range_initialize(InIt First, InIt Last)
	{
		size_type Diff = static_cast<size_type>(TinySTL::distance(First, Last));
		//再根据元素个数分配合适的内存,防止了多次内存分配以及由其导致的大量对象的构造和析构
		start_ = allocate(Diff);
		end_of_storage = start_ + Diff;
		finish_ = TinySTL::uninitialized_copy(First, Last, start_);
	}

	template<class T, class Alloc>
	template<class FwdIt>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::allocate_and_copy(size_type numElements, FwdIt First, FwdIt Last)
	{
		iterator result = allocate(numElements);
		try
		{
			TinySTL::uninitialized_copy(First, Last, result);
			return result;
		}
		catch (...)
		{
			deallocate(result, numElements);
		}
	}

	template<class T, class Alloc>
	template<class Integer>
	inline void vector<T, Alloc>::insert_dispatch(iterator Pos, Integer Count, Integer Val, true_type)
	{
		fill_insert(Pos, static_cast<size_type>(Count), static_cast<value_type>(Val));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void vector<T, Alloc>::insert_dispatch(iterator Pos, InIt First, InIt Last, false_type)
	{
		range_insert(Pos, First, Last, TinySTL::iterator_category(First));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void vector<T, Alloc>::range_insert(iterator Pos, InIt First, InIt Last, TinySTL::input_iterator_tag)
	{
		for (; First != Last; ++First)
		{
			Pos = insert(Pos, *First);
			++Pos;
		}
	}

	template<class T, class Alloc>
	template<class FwdIt>
	inline void vector<T, Alloc>::range_insert(iterator Pos, FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag)
	{
		if (First != Last)
		{
			size_type Diff = static_cast<size_type>(TinySTL::distance(First, Last));
			if (static_cast<size_type>(end_of_storage - finish_) >= Diff)
			{
				const size_type Elems_after = finish_ - Pos;
				iterator Oldfinish = finish_;
				if (Elems_after > Diff)
				{
					TinySTL::uninitialized_copy(finish_ - Diff, finish_, finish_);
					finish_ += Diff;
					TinySTL::copy_backward(Pos, Oldfinish - Diff, Oldfinish);
					TinySTL::copy(First, Last, Pos);
				}
				else
				{
					FwdIt Mid = First;
					TinySTL::advance(Mid, Elems_after);
					TinySTL::uninitialized_copy(Mid, Last, finish_);
					finish_ += Diff + Elems_after;
					TinySTL::uninitialized_copy(Pos, Oldfinish, finish_);
					finish_ += Elems_after;
					TinySTL::copy(First, Mid, Pos);
				}
			}
			else
			{
				const size_type Oldsize = size();
				const size_type Len = Oldsize + TinySTL::max(Oldsize, Diff);
				iterator Newstart = allocate(Len);
				iterator Newfinish = Newstart;
				try
				{
					Newfinish = TinySTL::uninitialized_copy(start_, Pos, Newstart);
					Newfinish = TinySTL::uninitialized_copy(First, Last, Newfinish);
					Newfinish = TinySTL::uninitialized_copy(Pos, finish_, Newfinish);
				}
				catch (...)
				{
					TinySTL::destroy(Newstart, Newfinish);
					deallocate(Newstart, Diff);
				}
				destroy(start_, finish_);
				deallocate(start_, end_of_storage - start_);
				start_ = Newstart;
				finish_ = Newfinish;
				end_of_storage = Newstart + Len;
			}
		}
	}

	template<class T, class Alloc>
	template<class Integer>
	inline void vector<T, Alloc>::assign_dispatch(Integer Count, Integer Val, true_type)
	{
		fill_assign(static_cast<size_type>(Count), static_cast<value_type>(Val));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void vector<T, Alloc>::assign_dispatch(InIt First, InIt Last, false_type)
	{
		assign_aux(First, Last, TinySTL::iterator_category(First));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void vector<T, Alloc>::assign_aux(InIt First, InIt Last, TinySTL::input_iterator_tag)
	{
		iterator Cur = begin();
		for (; First != Last && Cur != end(); ++Cur, (void)++First)
		{
			*Cur = *First;
		}
		if (First == Last) // 赋值区间比原有区间小,删除多余元素
		{
			erase(Cur, end());
		}
		else // 赋值区间比原有区间大,插入剩余元素,此处有可能遭遇多次内存分配
		{
			insert(end(), First, Last);
		}
	}

	template<class T, class Alloc>
	template<class FwdIt>
	inline void vector<T, Alloc>::assign_aux(FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag)
	{
		size_type Diff = static_cast<size_type>(TinySTL::distance(First, Last));
		if (Diff > capacity())
		{
			iterator Tmp = allocate_and_copy(Diff, First, Last);
			TinySTL::destroy(start_, finish_);
			deallocate(start_, end_of_storage - start_);
			start_ = Tmp;
			finish_ = start_ + Diff;
			end_of_storage = finish_;
		}
		else if (Diff <= size())
		{
			iterator Newfinish = TinySTL::copy(First, Last, start_);
			TinySTL::destroy(Newfinish, finish_);
			finish_ = Newfinish;
		}
		else
		{
			FwdIt Mid = First;
			TinySTL::advance(Mid, size());
			TinySTL::copy(First, Mid, start_);
			finish_ = TinySTL::uninitialized_copy(Mid, Last, finish_);
		}
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void vector<T, Alloc>::assign(InIt First, InIt Last)
	{
		using Integral = typename TinySTL::is_integral<InIt>::type;
		assign_dispatch(First, Last, Integral());
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void vector<T, Alloc>::insert(iterator Pos, InIt First, InIt Last)
	{	// 根据类别做不同的处理
		using Integral = typename TinySTL::is_integral<InIt>::type;
		insert_dispatch(Pos, First, Last, Integral());
	}

	template<class T, class Alloc>
	inline vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>&Right)
	{	//如果是同一个对象,则不需要执行下面的操作
		if (this != &Right)
		{
			const size_type Right_len = Right.size();
			if (Right_len > capacity())
			{	//比本vector的容量还大,需要分配更大内存
				iterator tmp = allocate_and_copy(Right_len, Right.begin(), Right.end_of_storage());
				TinySTL::destroy(start_, finish_);           // 析构原有内存空间的对象
				deallocate(start_, end_of_storage - start_); // 归还原有内存
				start_ = tmp;
				end_of_storage = start_ + tmp;
			}
			else if (Right_len <= size())
			{	//比本vector的元素个数还少
				iterator idx = TinySTL::copy(Right.begin(), Right.end(), begin()); //直接复制,然后析构多余对象
				TinySTL::destroy(idx, finish_);
			}
			else
			{	//比本vector的size()大，但是比本vector的容量小
				//此段内存已创建有对象,可直接通过copy函数调用其赋值运算符将提供的对象赋值给原有对象
				TinySTL::copy(Right.begin(), Right.begin() + size(), start_);
				//此段内存空白,需通过 uninitialized_copy调用其复制构造函数来创建新的对象
				TinySTL::uninitialized_copy(Right.begin() + size(), Right.end(), finish_);
			}
			finish_ = start_ + Right_len;
		}
		return *this;
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::insert_aux(iterator Pos, const value_type& Val)
	{
		if (finish_ != end_of_storage) // vector容量足够但插入位置不是末尾
		{	// 从最后一个对象开始到Pos依次后移，为新插入的对象腾出空间
			TinySTL::construct(finish_, *(finish_ - 1));
			++finish_;
			value_type Valcopy = Val;
			TinySTL::copy_backward(Pos, finish_ - 2, finish_ - 1);
			// 将需要插入的值赋值给适合位置
			*Pos = Valcopy;
		}
		else // vector容量不够
		{
			const size_type Oldsize = size();
			const size_type Len = Oldsize != 0 ? 2 * Oldsize : 1; // 防止多次内存分配
			iterator Newstart = allocate(Len);
			iterator Newfinish = Newstart;
			try
			{
				Newfinish = TinySTL::uninitialized_copy(start_, Pos, Newstart);
				TinySTL::construct(Newfinish, Val);
				++Newfinish;
				Newfinish = TinySTL::uninitialized_copy(Pos, finish_, Newfinish);
			}
			catch (...)
			{	// 一旦失败，析构创建的对象，归还分配的内存
				TinySTL::destroy(Newstart, Newfinish);
				deallocate(Newstart, Len);
			}
			TinySTL::destroy(begin(), end());
			deallocate(start_, static_cast<size_t>(end_of_storage - start_));
			start_ = Newstart;
			finish_ = Newfinish;
			end_of_storage = Newstart + Len;
		}
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::insert_aux(iterator Pos)
	{
		if (finish_ != end_of_storage)
		{
			TinySTL::construct(finish_, *(finish_ - 1));
			++finish_;
			TinySTL::copy_backward(Pos, finish_ - 2, finish_ - 1);
			*Pos = T();
		}
		else
		{
			const size_type Oldsize = size();
			const size_type Len = Oldsize != 0 ? 2 * Oldsize : 1;
			iterator Newstart = allocate(Len);
			iterator Newfinish = Newstart;
			try
			{
				Newfinish = TinySTL::uninitialized_copy(start_, Pos, Newstart);
				TinySTL::construct(Newfinish, T());
				++Newfinish;
				Newfinish = TinySTL::uninitialized_copy(Pos, finish_, Newfinish);
			}
			catch (...)
			{
				TinySTL::destroy(start_, end_of_storage - start_);
				deallocate(Newstart, Len);
			}
			TinySTL::destroy(begin(), end());
			deallocate(start_, static_cast<size_t>(end_of_storage - start_));
			start_ = Newstart;
			finish_ = Newfinish;
			end_of_storage = Newstart + Len;
		}
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::fill_insert(iterator Pos, size_type numElements, const value_type& Val)
	{
		if (numElements != 0)
		{
			value_type Valcopy = Val;
			if (static_cast<size_type>(end_of_storage - finish_) >= numElements) // vector剩余容量足以容纳n个对象
			{
				const size_type Elems_after = finish_ - Pos;
				iterator Oldfinish = finish_;
				if (Elems_after > numElements) // Pos后续元素数量多于numElements
				{
					TinySTL::uninitialized_copy(finish_ - numElements, finish_, finish_); // 将后numElements个元素移动至finish_后
					finish_ += numElements;
					TinySTL::copy_backward(Pos, Oldfinish - numElements, Oldfinish);	  // 再将Pos后尚未移完的元素后移
					TinySTL::fill(Pos, Pos + numElements, Valcopy);
				}
				else// Pos后续元素数量少于numElements
				{	// 由于充入对象区间将超出原有区间范围,因此需分两次充入,一次是通过直接赋值充入
					// 一次是创建新对象充入
					// 在空白空间充入numElements - Elems_after个Valcopy
					TinySTL::uninitialized_fill_n(finish_, numElements - Elems_after, Valcopy);
					finish_ += numElements - Elems_after;
					// 将Pos后的原有对象移动到已充入对象之后
					TinySTL::uninitialized_copy(Pos, Oldfinish, finish_);
					finish_ += Elems_after;
					// 在腾出空间充入Valcopy
					TinySTL::fill(Pos, Oldfinish, Valcopy);
				}
			}
			else //vector剩余容量不足以容纳n个对象,需重新分配内存
			{
				const size_type Oldsize = size();
				const size_type Len = Oldsize + TinySTL::max(Oldsize, numElements);
				iterator Newstart = allocate(Len);
				iterator Newfinish = Newstart;
				try
				{
					Newfinish = TinySTL::uninitialized_copy(start_, Pos, Newstart);
					Newfinish = TinySTL::uninitialized_fill_n(Newfinish, numElements, Valcopy);
					Newfinish = TinySTL::uninitialized_copy(Pos, finish_, Newfinish);
				}
				catch (...)
				{
					TinySTL::destroy(Newstart, Newfinish);
					deallocate(Newstart, Len);
				}
				TinySTL::destroy(start_, finish_);
				deallocate(start_, static_cast<size_type>(end_of_storage - start_));
				start_ = Newstart;
				finish_ = Newfinish;
				end_of_storage = Newstart + Len;
			}
		}
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::fill_assign(size_type Count, const value_type& Val)
	{
		if (Count > capacity())
		{	//通过局部对象的创建和对象的交换完成了内存的重新分配和旧内存对象的析构
			vector<T, Alloc> Tmpvec(Count, Val, get_allocator());
			Tmpvec.swap(*this);
		}
		else if (Count > size())
		{
			TinySTL::fill(begin(), end(), Val);
			finish_ = TinySTL::uninitialized_fill_n(finish_, Count - size(), Val);
		}
		else
		{
			erase(TinySTL::fill_n(begin(), Count, Val), end());
		}
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::begin()
	{
		return start_;
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::begin() const
	{
		return start_;
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::end()
	{
		return finish_;
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::end() const
	{
		return finish_;
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::reverse_iterator vector<T, Alloc>::rbegin()
	{
		return reverse_iterator(end());
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::rbegin() const
	{
		return const_reverse_iterator(end());
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::reverse_iterator vector<T, Alloc>::rend()
	{
		return reverse_iterator(begin());
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::rend() const
	{
		return const_reverse_iterator(begin());
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::cbegin() const
	{
		return begin();
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::crbegin() const
	{
		return rbegin();
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::cend() const
	{
		return end();
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::crend() const
	{
		return rend();
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::reference vector<T, Alloc>::operator[](size_type Pos)
	{ 
		return *(begin() + static_cast<difference_type>(Pos));
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_reference vector<T, Alloc>::operator[](size_type Pos) const
	{ 
		return *(begin() + static_cast<difference_type>(Pos));
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::reference vector<T, Alloc>::front()
	{
		return *begin();
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_reference vector<T, Alloc>::front() const
	{
		return *begin();
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::reference vector<T, Alloc>::back()
	{
		return *end();
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::const_reference vector<T, Alloc>::back() const
	{
		return *end();
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::size_type vector<T, Alloc>::size() const
	{
		return finish_ - start_;
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::size_type vector<T, Alloc>::max_size() const
	{
		return size_type(-1) / sizeof(T);
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::resize(size_type newSize)
	{
		resize(newSize, T());
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::resize(size_type newSize, const value_type& Val)
	{
		const size_type oldSize = size();
		if (newSize < oldSize)
		{
			erase(begin() + newSize, end());
		}
		else
		{
			insert(end(), newSize - size(), Val);
		}
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::size_type vector<T, Alloc>::capacity() const
	{
		return static_cast<size_type>(end_of_storage - begin());
	}

	template<class T, class Alloc>
	inline bool vector<T, Alloc>::empty() const
	{
		return begin() == end();
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::reserve(size_type Newcapacity)
	{
		if (Newcapacity > capacity())
		{
			const size_type Oldsize = size();
			// 重新分配内存,并将原内存数据copy到新分配的内存
			iterator Tmp = allocate_and_copy(Newcapacity, start_, finish_);
			TinySTL::destroy(start_, finish_); // 一一析构原内存中的对象
			start_ = Tmp;
			finish_ = Tmp + Oldsize;
			end_of_storage = start_ + Newcapacity;
		}
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::assign(size_type numElements, const value_type& Val)
	{
		fill_assign(numElements, Val);
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::push_back(const value_type& Val)
	{
		if (finish_ != end_of_storage) // 尚有空间
		{
			TinySTL::construct(finish_, Val);
			++finish_;
		}
		else
		{
			insert_aux(end(), Val);
		}
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::pop_back()
	{
		--finish_;
		TinySTL::destroy(finish_);
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator Pos, const value_type& Val)
	{
		size_type Numelements = Pos - begin();
		if (end() != end_of_storage && Pos == end())
		{	//vector尚有空间,而且插入位置为末尾
			TinySTL::construct(finish_, Val);
			++finish_;
		}
		else // 没有空间插入或有插入位置不是末尾
		{
			insert_aux(Pos, Val);
		}
		return begin() + Numelements;
	}

	template<class T, class Alloc>
	inline void vector<T, Alloc>::insert(iterator Pos, size_type numElements, const value_type& Val)
	{
		fill_insert(Pos, numElements, Val);
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator Pos)
	{	//将Pos以后的每个对象依次向前移动一位,再将最后一个元素析构,效率不高(删除尾部元素除外)
		if (Pos + 1 != end())
		{
			TinySTL::copy(Pos + 1, finish_, Pos);
		}
		--finish_;
		TinySTL::destroy(finish_);
		return Pos;
	}

	template<class T, class Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator First, iterator Last)
	{
		iterator idx = TinySTL::copy(Last, finish_, First);
		TinySTL::destroy(idx, finish_);
		finish_ = finish_ - (Last - First);
		return First;
	}
	template<class T, class Alloc>
	inline void vector<T, Alloc>::swap(vector& Other)
	{	//vector的swap方法,只是将其三个标志指针交换,因此会有很高的效率
		TinySTL::swap(start_, Other.start_);
		TinySTL::swap(finish_, Other.finish_);
		TinySTL::swap(end_of_storage, Other.end_of_storage);
	}
	template<class T, class Alloc>
	inline void vector<T, Alloc>::clear()
	{
		erase(begin(), end());
	}

	template<class T, class Alloc>
	bool operator==(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{
		return Left.size() == Right.size() && TinySTL::equal(Left.begin(), Left.end(), Right.begin());
	}

	template<class T, class Alloc>
	bool operator!=(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{
		return !(Left == Right);
	}

	template<class T, class Alloc>
	bool operator<(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{
		return TinySTL::compare(Left.begin(), Left.end(), Right.begin(), Right.end());
	}

	template<class T, class Alloc>
	bool operator<=(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{
		return !(Right < Left);
	}

	template<class T, class Alloc>
	bool operator>(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{
		return Right < Left;
	}

	template<class T, class Alloc>
	bool operator>=(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{
		return !(Left < Right);
	}
}

#endif // !_VECTOR_H_

