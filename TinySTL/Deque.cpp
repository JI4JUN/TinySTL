//#include "Deque.h"
//
//namespace TinySTL
//{
//	template<class T, class Alloc>
//	void deque<T, Alloc>::reallocateMap(size_type NeedNumNodes, bool AddAtFront)
//	{
//		size_type OldNumNodes = finish_.node_ - start_.node_ + 1; // 已经分配的节点数
//		size_type NewNumNodes = OldNumNodes + NeedNumNodes;       // 加上新需要的节点后的总结点数
//
//		mapPointer newStart = nullptr;
//		if (mapSize_ > 2 * NewNumNodes)
//		{	// 中控器现有容量比需要容量的2倍还多，只需要将中控器中的元素向一端挪动以腾出足够容量即可
//			newStart = map_ + ((mapSize_ - NewNumNodes) >> 1) + (AddAtFront ? NeedNumNodes : 0);
//			
//			if (newStart < start_.node_) // 从尾端
//				TinySTL::copy(start_.node_, finish_.node_ + 1, newStart);
//			else                         // 从前端
//				TinySTL::copy_backward(start_.node_, finish_.node_ + 1, newStart + OldNumNodes);
//		}
//		else
//		{	// 否则，需要为中控器重新分配内存并且复制原有的中控器数据
//			size_type newMapSize = mapSize_ + TinySTL::max(mapSize_, NeedNumNodes) + 2;
//			// 配置一块新空间，给新map使用
//			mapPointer newMap = allocateMap(newMapSize); 
//			newStart = newMap + ((newMapSize - NewNumNodes) >> 1 ) + (AddAtFront ? NeedNumNodes : 0);
//			// 把原map的内容拷贝过来
//			TinySTL::copy(start_.node_, finish_.node_ + 1, newStart);
//			// 释放原map
//			deallocateMap(map_, mapSize_);
//			// 设定新map的起始地址与大小
//			map_ = newMap;
//			mapSize_ = newMapSize;
//		}
//		// 重新设定迭代器start_和finish_
//		start_.setNode(newStart);
//		finish_.setNode(newStart + OldNumNodes - 1);
//	}
//
//	template<class T, class Alloc>
//	void deque<T, Alloc>::reserveMapAtBack(size_type NeedNumNodes) // NeedNumNodes默认为1
//	{
//		if (NeedNumNodes + 1 > mapSize_ - (finish_.node_ - map_)) // 判断中控器后面剩余空间是否小于需要补充的空间数+1
//			reallocateMap(NeedNumNodes, false); //重新分配一个新的map
//	}
//
//	// Called only if _M_finish._M_cur == _M_finish._M_last - 1.
//	template<class T, class Alloc>
//	void deque<T, Alloc>::push_back_aux(const value_type& Val)
//	{
//		value_type CopyVal = Val;
//		reserveMapAtBack(); // 为中控器后面补充空间，若符合某种条件则必须重换一个map
//		// 在最后一个区段后面再分配一个区段
//		*(finish_.node_ + 1) = allocateNode();
//		try
//		{
//			TinySTL::construct(finish_.cur_, CopyVal);   // 在新分配的区段上创建对象，调整deque相关状态
//			finish_.setNode(finish_.node_ + 1); // 改变finish_，令其指向新节点
//			finish_.cur_ = finish_.first_;      // 设定finish_的状态
//		}
//		catch (...)
//		{
//			deallocateNode(*(finish_.node_ + 1));
//			throw;
//		}
//	}
//
//	template<class T, class Alloc>
//	void deque<T, Alloc>::push_back(const value_type & Val)
//	{
//		if (finish_.cur_ != finish_.last - 1)
//		{	// 结束迭代器所指位置之后，该区段尚有空间
//			TinySTL::construct(finish_.cur_, Val);
//			++finish_.cur_;
//		}
//		else // 已到最后一个区段的区段末尾，需另行处理
//			push_back_aux(Val);
//	}
//
//	template<class T, class Alloc>
//	void deque<T, Alloc>::clear()
//	{	// 依次析构每个区段的对象元素,然后回收析构完的区段
//		for (mapPointer dctNode = start_.node_ + 1; dctNode < finish_.node_; ++dctNode)
//		{	// 对完整区段进行统一处理
//			TinySTL::destory(*dctNode, *dctNode + buffSize);
//			deallocateNode(*dctNode);
//		}
//
//		if (start_.node_ != finish_.node_)
//		{	// 所有元素分布在不少于两个区段上,对尚未处理的第一个和最后一个区段进行处理
//			TinySTL::destory(start_.cur_, start_.last_);    // 第一个
//			TinySTL::destory(finish_.first_, finish_.cur_); // 最后一个
//			deallocateNode(finish_.first_);
//		}
//		else
//		{
//			destory(start_.cur_, finish_.cur_);
//		}
//		finish_ = start_;
//	}
//
//	template<class T, class Alloc>
//	template<class InIt>
//	void deque<T, Alloc>::rangeInitialize(InIt First, InIt Last, TinySTL::input_iterator_tag)
//	{
//		initiailizeMap(0); // 默认创建8个节点
//		try
//		{
//			for (; First != Last; ++First)
//				push_back(*First);
//		}
//		catch (...)
//		{
//			clear();
//			throw;
//		}
//	}
//
//	template<class T, class Alloc>
//	template<class InIt>
//	void deque<T, Alloc>::rangeInitialize(InIt First, InIt Last, TinySTL::forward_iterator_tag)
//	{
//		size_type Diff = static_cast<size_type>(TinySTL::distance(First, Last));
//		initiailizeMap(Diff);
//		mapPointer curNode = nullptr;
//		try
//		{
//			for (curNode = start_.node_; curNode < finish_.node_; ++curNode)
//			{
//				forward_iterator mid = First;
//				TinySTL::advance(mid, buffSize);
//				TinySTL::uninitialized_copy(First, mid, *curNode);
//				First = mid;
//			}
//			TinySTL::uninitialized_copy(First, Last, finish_.first_);
//		}
//		catch (...)
//		{
//			TinySTL::destory(start_, iterator(*curNode, curNode));
//			throw;
//		}
//	}
//
//	template<class T, class Alloc>
//	template<class InIt>
//	inline void deque<T, Alloc>::initialize_dispatch(InIt numElements, InIt Val, true_type())
//	{
//		initiailizeMap(numElements);
//		fillInitialize(Val);
//	}
//
//	template<class T, class Alloc>
//	template<class InIt>
//	inline void deque<T, Alloc>::initialize_dispatch(InIt First, InIt Last, false_type())
//	{
//		rangeInitialize(First, Last, TinySTL::iterator_category(InIt));
//	}
//
//	template<class T, class Alloc>
//	void deque<T, Alloc>::fillInitialize(const value_type& Val)
//	{
//		mapPointer Cur = nullptr;
//		try
//		{   // 对完整的缓冲器统一处理
//			for (Cur = start_.node_; Cur < finish_.node_; ++Cur)
//			{
//				TinySTL::uninitialized_fill(*Cur, *Cur + buffSize, Val);
//			}
//			// 最后一个节点的设定不同，因为尾端可能有备用空间，不必设初值
//			TinySTL::uninitialized_fill(finish_.first_, finish_.cur_, Val);
//		}
//		catch (...)
//		{
//			TinySTL::destory(start_, iterator(*Cur, Cur));
//			throw;
//		}
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::iterator deque<T, Alloc>::begin()
//	{
//		return start_;
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_iterator deque<T, Alloc>::begin() const
//	{
//		return start_; 
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::iterator deque<T, Alloc>::end()
//	{
//		return finish_;
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_iterator deque<T, Alloc>::end() const
//	{ 
//		return finish_;
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::reverse_iterator deque<T, Alloc>::rbegin() 
//	{
//		return reverse_iterator(end());
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_reverse_iterator deque<T, Alloc>::rbegin() const 
//	{ 
//		return const_reverse_iterator(end()); 
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::reverse_iterator deque<T, Alloc>::rend() 
//	{ 
//		return reverse_iterator(begin()); 
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_reverse_iterator deque<T, Alloc>::rend() const 
//	{
//		return const_reverse_iterator(begin());
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_iterator deque<T, Alloc>::cbegin() const 
//	{ 
//		return begin(); 
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_reverse_iterator deque<T, Alloc>::crbegin() const
//	{ 
//		return rbegin();
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_iterator deque<T, Alloc>::cend() const 
//	{
//		return end();
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_reverse_iterator deque<T, Alloc>::crend() const
//	{ 
//		return rend(); 
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::reference deque<T, Alloc>::operator[](size_type Pos)
//	{ 
//		return *(begin() + static_cast<difference_type>(Pos));
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_reference deque<T, Alloc>::operator[](size_type Pos) const
//	{ 
//		return *(begin() + static_cast<difference_type>(Pos));
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::reference deque<T, Alloc>::front()
//	{ 
//		if (!empty())
//			return *begin();
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_reference deque<T, Alloc>::front() const
//	{ 
//		if (!empty())
//			return *begin();
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::reference deque<T, Alloc>::back()
//	{ 
//		if (!empty())
//		{
//			const_iterator Tmp = finish_;
//			--finish_; 
//			return *finish_;
//		}
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::const_reference deque<T, Alloc>::back() const
//	{ 
//		if (!empty())
//		{
//			const_iterator Tmp = finish_;
//			--finish_; 
//			return *finish_;
//		}
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::size_type deque<T, Alloc>::size() const
//	{
//		return finish_ - start_;
//	}
//
//	template<class T, class Alloc>
//	typename deque<T, Alloc>::size_type deque<T, Alloc>::maxSize() const
//	{
//		return size_type(-1);
//	}
//
//	template<class T, class Alloc>
//	bool deque<T, Alloc>::empty() const
//	{
//		return finish_ == start_;
//	}

		//	template<class T, class Alloc>
//	T* deque<T, Alloc>::getANewBuck()
//	{
//		return Alloc::allocator(blockSize);
//	}
//
//	template<class T, class Alloc>
//	T** deque<T, Alloc>::getANewMap(const size_type size)
//	{
//		T** map = new T* [size]; // 动态声明二维数组
//		for (size_t i = 0; i != size; ++i)
//			map[i] = getANewBuck();
//		return map;
//	}
//
//	template<class T, class Alloc>
//	void deque<T, Alloc>::init()
//	{
//		mapSize_ = MinimumMapSize; // default 8
//		map_ = getANewMap(mapSize_);
//	}
//
//	template<class T, class Alloc>
//	void deque<T, Alloc>::push_back(const value_type & Val)
//	{
//	}
//	template<class T, class Alloc>
//	void deque<T, Alloc>::push_front(const value_type & Val)
//	{	
//		if (empty())
//			init();
//
//	}
//
//	template<class T, class Alloc>
//	deque<T, Alloc>::deque() : map_(nullptr), mapSize_(0) {}
//	template<class T, class Alloc>
//	deque<T, Alloc>::deque(size_type Count, const value_type & Val)
//	{
//		deque();
//		int i = 0;
//		for (; i != (Count >> 1); ++i)
//			(*this).push_front(Val);
//		for (; i != Count; ++i)
//			(*this).push_back(Val);
//	}
//	template<class T, class Alloc>
//	template<class InIt>
//	deque<T, Alloc>::deque(InIt First, InIt Last)
//	{
//		deque();
//		difference_type mid = First + ((Last - First) >> 1);
//		for (auto it = First + mid; it != First - 1; --it)
//			(*this).push_front(*it);
//		for (auto it = First + mid + 1; it != Last; ++it)
//			(*this).push_back(*it);
//	}
//
//	template<class T, class Alloc>
//	deque<T, Alloc>::~deque()
//	{
//
//	}
//
//}