#ifndef _DEQUE_H_
#define _DEQUE_H_

#include "Allocator.h"
#include "ReserverseIterator.h"
#include "Algorithm.h"
#include "UninitializedFunctions.h"

namespace TinySTL
{	
	// 全局函数，计算deque每个区段大小(缓冲区可容纳的元素个数)，size：模板类占用大小
	inline size_t dequeBuffSize(size_t size)
	{
		return size < 512 ? static_cast<size_t>(512 / size) : static_cast<size_t>(1);
	}

	/*
	* ***********************************
	* class deque_iterator
	* ***********************************
	*/
	template <class T, class Ref, class Ptr>
	struct deque_iterator
	{
	public:
		using iterator          = deque_iterator<T, T&, T*>;
		using const_iterator    = deque_iterator<T, const T&, const T*>;

	public:
		using iterator_category = TinySTL::random_access_iterator_tag;
		using value_type        = T;
		using difference_type   = ptrdiff_t;
		using pointer           = Ptr;
		using reference         = Ref;

		using mapPointer        = T**;
		using self              = deque_iterator;

	public:
		T*         cur_;   // 此迭代器所指的缓冲区的当前元素(空元素)
		T*         first_; // 此迭代器所指的缓冲区的头部
		T*         last_;  // 此迭代器所指的缓冲区的尾部，最后一个元素的后一个位置
		mapPointer node_;  // 指向管控中心(map)

	private:
		//static constexpr size_t buffSize() = dequeBuffSize(sizeof(T));
		static  size_t buffSize()
		{
			return dequeBuffSize(sizeof(T));
		}

	public:
		deque_iterator() : cur_(nullptr), first_(nullptr), last_(nullptr), node_(nullptr) {}
		deque_iterator(T* Cur, mapPointer Curnode) : cur_(Cur), first_(*Curnode), last_(*Curnode + buffSize()), node_(Curnode) {}
		deque_iterator(const iterator& Right) : cur_(Right.cur_), first_(Right.first_), last_(Right.last_), node_(Right.node_) {}

	public:
		reference operator*() const
		{
			return *cur_;
		}

		reference operator->() const
		{
			return &(operator*());
		}

		difference_type operator-(const self& Right) const
		{
			// 计算两个迭代器之间的完整缓冲区段数
			return static_cast<difference_type>(
				buffSize() * (node_ - Right.node_ - 1) + 
				(cur_ - first_)					     + 
				(Right.last_ - Right.cur_));
		}

		self& operator++() // 前置++
		{
			++cur_;
			if (cur_ == last_) // 超出该缓冲区范围，移动到下一个缓冲区
			{
				setNode(node_ + 1);
				cur_ = first_;
			}
			return *this;
		}

		self operator++(int) // 后置++
		{
			self Tmp = *this;
			++*this;
			return Tmp;
		}

		self& operator--() // 前置--
		{
			if (cur_ == first_)
			{
				setNode(node_ - 1);
				cur_ = last_;
			}
			--cur_;
			return *this;
		}

		self operator--(int) // 后置--
		{
			self Tmp = *this;
			--*this;
			return Tmp;
		}

		// deque迭代器为随机迭代器具有以下方法
		self& operator+=(difference_type Off)
		{
			difference_type Pos = Off + (cur_ - first_);
			difference_type Buffsize = static_cast<difference_type>(buffSize());
			if (Pos >= 0 && Pos < Buffsize)
			{	// 向后移动未超出目前所在区域
				cur_ += Pos;
			}
			else
			{	// 超出当前所在区域，先计算需要前移/后移的区段数
				difference_type nodePos = Pos > 0 ? Pos / Buffsize : -(-Pos - 1 / Buffsize) - 1;
				setNode(node_ + nodePos);
				// 计算迭代器当前正确指向的位置
				cur_ = first_ + (Pos - nodePos * Buffsize);
			}
			return *this;
		}

		self operator+(difference_type Off) const
		{
			self Tmp = *this;
			return Tmp += Off;
		}

		self& operator-=(difference_type Off)
		{
			return *this += -Off;
		}

		self operator-(difference_type Off) const
		{
			self Tmp = *this;
			return Tmp -= Off;
		}

		reference operator[](difference_type Pos) const
		{
			return *(*this + Pos);
		}

		bool operator==(const self& Right) const
		{
			return cur_ == Right.cur_;
		}
		
		bool operator!=(const self& Right) const
		{
			return !(*this == Right);
		}

		// 按迭代器所在区域头指针在中控器中的存储位置进行比较
		// 若在同一区域，再比较迭代器确指指针
		bool operator<(const self& Right) const
		{
			return (node_ == Right.node_) ? cur_ < Right.cur_ : node_ < Right.node_;
		}

		bool operator>(const self& Right) const
		{
			return Right < *this;
		}

		bool operator<=(const self& Right) const
		{
			return !(Right < *this);
		}

		bool operator>=(const self& Right) const
		{
			return !(*this < Right);
		}

		void setNode(mapPointer newNode)
		{
			// 设置迭代器所在的缓冲区
			first_ = *newNode;
			last_ = first_ + buffSize();
			node_ = newNode;
		}
	};

	/*
	* ***********************************
	* class deque_base
	* ***********************************
	*/
	template <class T, class Alloc = TinySTL::allocator<T>>
	class deque_base
	{
	public:
		using iterator       = deque_iterator<T, T&, T*>;
		using const_iterator = deque_iterator<T, const T&, const T*>;

		using allocator_type = Alloc;
		allocator_type getAllocator() const { return allocator_type(); }

		using node_alloc_type = TinySTL::allocator<T>;
		using map_alloc_type  = TinySTL::allocator<T*>;

	protected:
		T**      map_;     // 中控器，一个指向T类型数组指针的数组
		size_t   mapSize_; // map内有多少个指针
		iterator start_;   // 起始迭代器
		iterator finish_;  // 结束迭代器

	public:
		deque_base(const allocator_type&, size_t numElements) : map_(), mapSize_(0), start_(), finish_()
		{
			initiailizeMap(numElements);
		}
		deque_base(const allocator_type& Alloc) : map_(), mapSize_(0), start_(), finish_() {}
		
		~deque_base();

	protected:
		// 初始化中控器
		void initiailizeMap(size_t numElements);
		// 依次创建缓冲区
		void createNodes(T** nodeBegin, T** nodeEnd);
		// 依次删除缓冲区
		void destroyNodes(T** nodeBegin, T** nodeEnd);

		// 每次分配、回收一个区段(缓冲区)
		T* allocateNode()
		{
			return node_alloc_type::allocate(dequeBuffSize(sizeof(T)));
		}
		void deallocateNode(T* buff)
		{
			node_alloc_type::deallocate(buff, dequeBuffSize(sizeof(T)));
		}
		// 中控器内存的分配与回收，实际上是T*类型数组的分配与回收
		T** allocateMap(size_t mapSize)
		{
			return map_alloc_type::allocate(mapSize);
		}
		void deallocateMap(T** map, size_t mapSize)
		{
			map_alloc_type::deallocate(map, mapSize);
		}

		// 默认中控器大小为8,即可创建8个区段
		enum { s_initialMapSize = 8 };
	};

	template<class T, class Alloc>
	TinySTL::deque_base<T, Alloc>::~deque_base()
	{
		if (map_)
		{
			// 先回收各个缓冲区的内存
			destroyNodes(start_.node_, finish_.node_ + 1);
			// 再回收中控器的内存
			deallocateMap(map_, mapSize_);
		}
	}

	template<class T, class Alloc>
	void deque_base<T, Alloc>::initiailizeMap(size_t numElements)
	{
		// 计算所需的区段数目，从而决定中控器的大小
		// 需要节点数 = (元素个数 / 每个缓冲区可容纳的元素个数) + 1
		// 如果刚好整除，会多配一个
		size_t numNodes = numElements / dequeBuffSize(sizeof(T)) + 1;
		
		// 一个map最少管理8个节点，最多是所需节点数+2(前后各预留一个，扩充时可用)
		mapSize_ = TinySTL::max(static_cast<size_t>(s_initialMapSize), numNodes + 2);
		map_ = allocateMap(mapSize_);

		// 使用中控器的中间部分，可以保证前后都可以继续扩充缓冲区
		T** nodeBegin = map_ + ((mapSize_ - numNodes) >> 1);
		T** nodeEnd = nodeBegin + numNodes;

		try
		{
			createNodes(nodeBegin, nodeEnd);
		}
		catch (...)
		{
			deallocateMap(map_, mapSize_);
			map_ = nullptr;
			mapSize_ = 0;
		}

		// 设置起始、终止迭代器的指向
		start_.setNode(nodeBegin);
		finish_.setNode(nodeEnd - 1);
		start_.cur_ = start_.first_;
		finish_.cur_ = finish_.first_ + numElements % dequeBuffSize(sizeof(T));
	}

	template<class T, class Alloc>
	void deque_base<T, Alloc>::createNodes(T** nodeBegin, T** nodeEnd)
	{
		T** nodeCur;
		try
		{
			for (nodeCur = nodeBegin; nodeCur < nodeEnd; ++nodeCur)
				*nodeCur = allocateNode();
		}
		catch (...)
		{
			destroyNodes(nodeBegin, nodeEnd);
		}
	}

	template<class T, class Alloc>
	void deque_base<T, Alloc>::destroyNodes(T ** nodeBegin, T ** nodeEnd)
	{
		for (T** nodeCur = nodeBegin; nodeCur < nodeEnd; ++nodeCur)
			deallocateNode(*nodeCur);
	}

	/*
	* ***********************************
	* class deque
	* ***********************************
	*/
	template <class T, class Alloc = TinySTL::allocator<T>>
	class deque : protected deque_base<T, Alloc>
	{
	private:
		using base_            = deque_base<T, Alloc>;

	public:
		using value_type      = T;
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;
		using pointer         = value_type*;
		using const_pointer   = const value_type*;
		using reference       = value_type&;
		using const_reference = const value_type&;

		using allocator_type  = typename base_::allocator_type;
		allocator_type getAllocator() const { return base_::allocator_type(); }

	public:
		// iterator
		using iterator               = typename base_::iterator;
		using const_iterator         = typename base_::const_iterator;
		using const_reverse_iterator = TinySTL::reverse_iterator<const_iterator>;
		using reverse_iterator       = TinySTL::reverse_iterator<iterator>;

	protected:
		// Internal typedefs
		using mapPointer = T**;
		//static constexpr size_t buffSize() = dequeBuffSize(sizeof(T));
		static size_t buffSize()
		{
			return dequeBuffSize(sizeof(T));
		}

	protected:
		using base_::initiailizeMap;
		using base_::createNodes;
		using base_::destroyNodes;

		using base_::allocateNode;
		using base_::deallocateNode;
		using base_::allocateMap;
		using base_::deallocateMap;

		using base_::map_;
		using base_::mapSize_;
		using base_::start_;
		using base_::finish_;

	public:
		explicit deque(const allocator_type& Alloc = allocator_type()) : base_(Alloc, 0) {}
		deque(const deque& Other) : base_(Other.getAllocator(), Other.size())
		{
			TinySTL::uninitialized_copy(Other.begin(), Other.end(), start_);
		}
		deque(size_type Count, const value_type& Val, const allocator_type& Alloc = allocator_type()) : base_(Alloc, Count)
		{
			fillInitialize(Val);
		}
		explicit deque(size_type Count) : base_(allocator_type(), Count)
		{
			fillInitialize(value_type());
		}
		// Check whether it's an integral type.  If so, it's not an iterator
		template <class InIt>
		deque(InIt First, InIt Last, const allocator_type& Alloc = allocator_type()) : base_(Alloc)
		{	// 根据类别做不同的处理
			initialize_dispatch(First, Last, TinySTL::is_integral<InIt>());
		}
		template <class Integer>
		void initialize_dispatch(Integer numElements, Integer Val, TinySTL::true_type);
		template <class InIt>
		void initialize_dispatch(InIt First, InIt Last, TinySTL::false_type);
		// 一次析构每个对象元素，回收内存交由基类处理
		~deque()
		{
			TinySTL::destroy(start_, finish_);
		}

	public:
		deque& operator=(const deque& Right)
		{
			const size_type len = size();
			if (this != &Right)
			{
				if (len >= Right.size())
					erase(TinySTL::copy(Right.begin(), Right.end(), start_), finish_);
				else
				{
					const_iterator mid = Right.begin() + static_cast<difference_type>(len);
					TinySTL::copy(Right.begin(), mid, start_);
					insert(finish_, mid, Right.end());
				}
			}
			return *this;
		}

	protected:
		// Precondition: start_ and finish_ have already been initialized,
		// but none of the deque's elements have yet been constructed
		void fillInitialize(const value_type& Val);

		template <class InIt>
		void rangeInitialize(InIt First, InIt Last, TinySTL::input_iterator_tag);
		template <class FwdIt>
		void rangeInitialize(FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag);

	protected:
		void fill_assign(size_type Count, const value_type& Val);
		template<class Integer>
		void assign_dispatch(Integer Count, Integer Val, TinySTL::true_type);
		template<class InIt>
		void assign_dispatch(InIt First, InIt Last, TinySTL::false_type);
		template<class InIt>
		void assign_aux(InIt First, InIt Last, TinySTL::input_iterator_tag);
		template<class FwdIt>
		void assign_aux(FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag);

		void push_back_aux(const value_type& Val);
		void push_back_aux();
		void push_front_aux(const value_type& Val);
		void pop_back_aux();
		void pop_front_aux();

		template<class InIt>
		void insert(iterator Pos, InIt First, InIt Last, TinySTL::input_iterator_tag);
		template<class FwdIt>
		void insert(iterator Pos, FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag);
		iterator insert_aux(iterator Pos, const value_type& Val);
		void insert_aux(iterator Pos, size_type Count, const value_type& Val);
		template<class FwdIt>
		void insert_aux(iterator Pos, FwdIt First, FwdIt Last, size_type Count);

		iterator reserve_elements_at_front(size_type Count);
		iterator reserve_elements_at_back(size_type Count);

		void new_elements_at_front(size_type new_elements);
		void new_elements_at_back(size_type new_elements);

	protected:
		// Allocation of _M_map and nodes	
		// Makes sure the _M_map has space for new nodes.  Does not actually
		// add the nodes.  Can invalidate _M_map pointers.  (And consequently,
		// deque iterators.)
		void reserve_map_at_back(size_type NeedNumNodes = 1);
		void reserve_map_at_front(size_type NeedNumNodes = 1);

		// 重新分配中控器
		void reallocateMap(size_type NeedNumNodes, bool Backward);

	public:
		// Iterators:
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

		// capacity:
		size_type size() const;
		size_type max_size() const;
		void resize(size_type newSize, const value_type& Val);
		void resize(size_type newSize);
		bool empty() const;

		// Modifiers:
		void assign(size_type Count, const value_type& Val);
		template <class InIt>
		void assign(InIt First, InIt Last);
		void push_back(const value_type& Val);
		void push_back();
		void push_front(const value_type& Val);
		void pop_back();
		void pop_front();
		iterator insert(iterator Pos, const value_type& Val);
		iterator insert(iterator Pos);
		void insert(iterator Pos, size_type Count, const value_type& Val);
		template<class InIt>
		void insert(iterator Pos, InIt First, InIt Last);
		template<class Integer>
		void insert_dispatch(iterator Pos, Integer Count, Integer Val, true_type);
		template<class InIt>
		void insert_dispatch(iterator Pos, InIt First, InIt Last, false_type);
		void fill_insert(iterator Pos, size_type Count, const value_type& Val);

		iterator erase(iterator Pos);
		iterator erase(iterator First, iterator Last);
		void clear();

		void swap(deque& Other);

	//public:
	//	// Non-member functions overloads
	//	template <class T, class Alloc>
	//	friend bool operator==(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator!=(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator<(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator<=(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator>(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator>=(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	};

//////////////////////////////////////////// 实现 //////////////////////////////////////////////////////////

	template<class T, class Alloc>
	inline void deque<T, Alloc>::reallocateMap(size_type NeedNumNodes, bool AddAtFront)
	{
		size_type OldNumNodes = finish_.node_ - start_.node_ + 1; // 已经分配的节点数
		size_type NewNumNodes = OldNumNodes + NeedNumNodes;       // 加上新需要的节点后的总结点数

		mapPointer newStart = nullptr;
		if (mapSize_ > 2 * NewNumNodes)
		{	// 中控器现有容量比需要容量的2倍还多，只需要将中控器中的元素向一端挪动以腾出足够容量即可
			newStart = map_ + ((mapSize_ - NewNumNodes) >> 1) + (AddAtFront ? NeedNumNodes : 0);
			
			if (newStart < start_.node_) // 从尾端
				TinySTL::copy(start_.node_, finish_.node_ + 1, newStart);
			else                         // 从前端
				TinySTL::copy_backward(start_.node_, finish_.node_ + 1, newStart + OldNumNodes);
		}
		else
		{	// 否则，需要为中控器重新分配内存并且复制原有的中控器数据
			size_type newMapSize = mapSize_ + TinySTL::max(mapSize_, NeedNumNodes) + 2;
			// 配置一块新空间，给新map使用
			mapPointer newMap = allocateMap(newMapSize); 
			newStart = newMap + ((newMapSize - NewNumNodes) >> 1 ) + (AddAtFront ? NeedNumNodes : 0);
			// 把原map的内容拷贝过来
			TinySTL::copy(start_.node_, finish_.node_ + 1, newStart);
			// 释放原map
			deallocateMap(map_, mapSize_);
			// 设定新map的起始地址与大小
			map_ = newMap;
			mapSize_ = newMapSize;
		}
		// 重新设定迭代器start_和finish_
		start_.setNode(newStart);
		finish_.setNode(newStart + OldNumNodes - 1);
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::reserve_map_at_back(size_type NeedNumNodes) // NeedNumNodes默认为1
	{
		if (NeedNumNodes + 1 > mapSize_ - static_cast<size_type>(finish_.node_ - map_)) // 判断中控器后面剩余空间是否小于需要补充的空间数+1
			reallocateMap(NeedNumNodes, false); //重新分配一个新的map
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::reserve_map_at_front(size_type NeedNumNodes)
	{
		if (NeedNumNodes > static_cast<size_type>(start_.node_ - map_))
			reallocateMap(NeedNumNodes, true);
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::fill_assign(size_type Count, const value_type& Val)
	{
		if (Count > size())
		{
			TinySTL::fill(begin(), end(), Val);
			insert(end(), Count - size(), Val);
		}
		else
		{
			erase(begin() + Count, end());
			TinySTL::fill(begin(), end(), Val);
		}
	}

	// Called only if finish_.cur_ == finish_.last_ - 1.
	template<class T, class Alloc>
	inline void deque<T, Alloc>::push_back_aux(const value_type& Val)
	{
		value_type CopyVal = Val;
		reserve_map_at_back(); // 为中控器后面补充空间，若符合某种条件则必须重换一个map
		// 在最后一个区段后面再分配一个区段
		*(finish_.node_ + 1) = allocateNode();
		try
		{
			TinySTL::construct(finish_.cur_, CopyVal); // 在新分配的区段上创建对象，调整deque相关状态
			finish_.setNode(finish_.node_ + 1);        // 改变finish_，令其指向新节点
			finish_.cur_ = finish_.first_;             // 设定finish_的状态
		}
		catch (...)
		{
			deallocateNode(*(finish_.node_ + 1));
			throw;
		}
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::push_back_aux()
	{
		reserve_map_at_back(); 
		*(finish_.node_ + 1) = allocateNode();
		try
		{
			TinySTL::construct(finish_.cur_, value_type());
			finish_.setNode(finish_.node_ + 1);
			finish_.cur_ = finish_.first_;
		}
		catch (...)
		{
			deallocateNode(*(finish_.node_ + 1));
			throw;
		}

	}

	// 和push_back_aux实现大同小异
	// Called only if start_.cur_ == start_.first_.
	template<class T, class Alloc>
	inline void deque<T, Alloc>::push_front_aux(const value_type& Val)
	{
		value_type CopyVal = Val;
		reserve_map_at_front(); // 为中控器前面补充空间，若符合某种条件则必须重换一个map
		*(start_.node_ - 1) = allocateNode();
		try
		{
			start_.setNode(start_.node_ - 1);		  // 改变start_，令其指向新节点
			start_.cur_ = start_.last_ - 1;			  // 设定start_的状态
			TinySTL::construct(start_.cur_, CopyVal); // 在新分配的区段上创建对象，调整deque相关状态
		}
		catch (...)
		{
			++start_;
			deallocateNode(*(start_.node_ - 1));
			throw;
		}
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::assign(size_type Count, const value_type& Val)
	{
		fill_assign(Count, Val);
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void deque<T, Alloc>::assign(InIt First, InIt Last)
	{
		using Integral = typename TinySTL::is_integral<InIt>::type;
		assign_dispatch(First, Last, Integral());
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::push_back(const value_type & Val)
	{
		if (finish_.cur_ != finish_.last_ - 1)
		{	// 结束迭代器所指位置之后，该区段尚有空间
			TinySTL::construct(finish_.cur_, Val);
			++finish_.cur_;
		}
		else // 已到最后一个区段的区段末尾，需另行处理
			push_back_aux(Val);
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::push_back()
	{
		if (finish_.cur_ != finish_.last_ - 1)
		{
			TinySTL::construct(finish_.cur_, value_type());
			++finish_.cur_;
		}
		else
		{
			push_back_aux();
		}
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::push_front(const value_type& Val)
	{
		if (start_.cur_ != start_.first_)
		{	// 起始迭代器所指位置之前，该区段尚有空间
			TinySTL::construct(start_.cur_ - 1, Val);
			--start_.cur_;
		}
		else // 已到第一个区段的区段的头部，需另行处理
			push_front_aux(Val);
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::pop_back_aux()
	{	
		deallocateNode(finish_.first_);		// 释放最后一个缓冲区
		finish_.setNode(finish_.node_ - 1); // 调整finish的状态，使其指向上一个缓冲区的最后一个元素
		finish_.cur_ = finish_.last_ - 1;    // 上一个缓冲区的最后一个元素
		TinySTL::destroy(finish_.cur_);		// 析构该元素
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::pop_front_aux()
	{
		TinySTL::destroy(start_.cur_);
		deallocateNode(start_.first_);
		start_.setNode(start_.node_ + 1);
		start_.cur_ = start_.first_;
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::insert_aux(iterator Pos, const value_type& Val)
	{
		difference_type index = Pos - start_;
		size_type length = this->size();
		value_type val_copy = Val;
		if (static_cast<size_type>(index) < (length >> 1))
		{
			push_front(front());
			iterator front1 = start_;
			++front1;
			iterator front2 = front1;
			++front2;
			Pos = start_ + index;
			iterator Pos1 = Pos;
			++Pos1;
			TinySTL::copy(front2, Pos1, front1);
		}
		else
		{
			push_back(back());
			iterator back1 = finish_;
			--back1;
			iterator back2 = back1;
			--back2;
			Pos = start_ + index;
			TinySTL::copy_backward(Pos, back2, back1);
		}
		*Pos = val_copy;
		return Pos;
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::insert_aux(iterator Pos, size_type Count, const value_type& Val)
	{
		const difference_type elems_before = Pos - start_;
		size_type length = this->size();
		value_type val_copy = Val;
		if (elems_before < static_cast<difference_type>(length >> 1))
		{	// 在start之前扩容n个元素，将插入位置之前的元素前移
			iterator new_start = reserve_elements_at_front(Count);
			iterator old_start = start_;
			Pos = old_start + elems_before;
			try
			{
				if (elems_before >= static_cast<difference_type>(Count))
				{	// 插入位置之前的元素多于需要插入的元素，旧有元素移动
					// 需要分两次进行，一次是向空白内存复制，一次是向已有对象赋值
					iterator start_n = old_start + static_cast<difference_type>(Count);
					TinySTL::uninitialized_copy(start_, start_n, new_start);
					start_ = new_start;
					TinySTL::copy(start_n, Pos, old_start);
					// 新元素的插入只需要一次进行，都是向已有对象赋值
					TinySTL::fill(Pos - static_cast<difference_type>(Count), Pos, val_copy);
				}
				else 
				{	// 插入位置之前的元素少于需要插入的元素，旧有元素移动
					// 旧有元素移动一次就可以完成，而新元素插入需要两次
					TinySTL::uninitialized_copy_fill(start_, Pos, new_start, start_, val_copy);
					// 第二次插入新元素
					TinySTL::fill(old_start, Pos, val_copy);
				}
			}
			catch (...)
			{
				destoryNodes(new_start.node_, start_.node_);
				throw;
			}
		}
		else
		{	// 在finish之后扩容n个元素，将插入位置之前的元素后移
			iterator new_finish = reserve_elements_at_back(Count);
			iterator old_finish = finish_;
			const difference_type elems_after = static_cast<difference_type>(length) - elems_before;
			Pos = finish_ - elems_after;
			try
			{
				if (elems_after > static_cast<difference_type>(Count))
				{
					iterator finish_n = finish_ - static_cast<difference_type>(Count);
					TinySTL::uninitialized_copy(finish_n, finish_, finish_);
					finish_ = new_finish;
					TinySTL::copy_backward(Pos, finish_n, old_finish);
					TinySTL::fill(Pos, Pos + static_cast<difference_type>(Count), val_copy);
				}
				else
				{
					TinySTL::uninitialized_fill_copy(finish_, Pos + static_cast<difference_type>(Count), val_copy, Pos, finish_);
					finish_ = new_finish;
					TinySTL::fill(Pos, old_finish, val_copy);
				}
			}
			catch (...)
			{
				destoryNodes(finish_.node_ + 1, new_finish.node_ + 1);
				throw;
			}
		}
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::reserve_elements_at_front(size_type Count)
	{
		size_type vacancies = start_.cur_ - start_.first_;
		if (Count > vacancies) // 当前空余的空间不足
			new_elements_at_front(Count - vacancies);
		return start_ - static_cast<difference_type>(Count);
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::reserve_elements_at_back(size_type Count)
	{
		size_type vacancies = finish_.last_ - finish_.cur_ - 1;
		if (Count > vacancies) // 当前空余的空间不足
			new_elements_at_back(Count - vacancies);
		return finish_ + static_cast<difference_type>(Count);
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::new_elements_at_front(size_type new_elements)
	{	// 根据需要扩充的元素扩充相应数量的中控器单元
		size_type new_nodes = (new_elements + buffSize() - 1) / buffSize();
		reserve_map_at_front(new_nodes);

		// 真正进行每个区段的分配
		size_type i;
		try
		{
			for (i = 1; i <= new_elements; ++i)
				*(start_.node_ - i) = allocateNode();
		}
		catch (...)
		{
			for (size_type j = 1; j < i; ++j)
				deallocateNode(*(start_.node_ - j));
			throw;
		}
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::new_elements_at_back(size_type new_elements)
	{	// 根据需要扩充的元素扩充相应数量的中控器单元
		size_type new_nodes = (new_elements + buffSize() - 1) / buffSize();
		reserve_map_at_back(new_nodes);

		// 真正进行每个区段的分配
		size_type i;
		try
		{
			for (i = 1; i <= new_elements; ++i)
				*(finish_.node_ + i) = allocateNode();
		}
		catch (...)
		{
			for (size_type j = 1; j < i; ++j)
				deallocateNode(*(finish_.node_ + j));
			throw;
		}
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::pop_back()
	{
		if (finish_.cur_ != finish_.first_)
		{	//最后一个元素不是最后一个区段的第一个元素
			--finish_.cur_;
			TinySTL::destroy(finish_.cur_);
		}
		else
			// 最后缓冲区没有任何元素
			pop_back_aux();
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::pop_front()
	{
		if (start_.cur_ != start_.last_ - 1)
		{	// 第一缓冲区有一个或多个元素
			TinySTL::destroy(start_.cur_);
			++start_.cur_;
		}
		else
			// 第一缓冲区仅有一个元素
			pop_front_aux();
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::insert(iterator Pos, const value_type& Val)
	{
		if (Pos.cur_ == start_.cur_)
		{
			push_front(Val);
			return start_;
		}
		else if (Pos.cur_ == finish_.cur_)
		{
			push_back(Val);
			iterator Tmp = finish_;
			--Tmp;
			return Tmp;
		}
		else
		{
			return insert_aux(Pos, Val);
		}
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::insert(iterator Pos)
	{
		return insert(Pos, value_type());
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::insert(iterator Pos, size_type Count, const value_type& Val)
	{
		fill_insert(Pos, Count, Val);
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void deque<T, Alloc>::insert(iterator Pos, InIt First, InIt Last)
	{
		using Integral = typename TinySTL::is_integral<InIt>::type;
		insert_dispatch(Pos, First, Last, Integral());
	}

	template<class T, class Alloc>
	template<class Integer>
	inline void deque<T, Alloc>::insert_dispatch(iterator Pos, Integer Count, Integer Val, TinySTL::true_type)
	{ 
		fill_insert(Pos, static_cast<size_type>(Count), static_cast<value_type>(Val));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void deque<T, Alloc>::insert_dispatch(iterator Pos, InIt First, InIt Last, TinySTL::false_type)
	{
		insert(Pos, First, Last, TinySTL::iterator_category(First));
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::fill_insert(iterator Pos, size_type Count, const value_type& Val)
	{
		if (Pos.cur_ == start_.cur_)       // 插入点是deque的开头
		{
			iterator new_start = reserve_elements_at_front(Count);
			try
			{
				TinySTL::uninitialized_fill(new_start, start_, Val);
				start_ = new_start;
			}
			catch (...)
			{
				destoryNodes(new_start.node_, start_.node_);
				throw;
			}
		}
		else if (Pos.cur_ == finish_.cur_) // 插入点是deque的末尾
		{
			iterator new_finish = reserve_elements_at_back(Count);
			try
			{
				TinySTL::uninitialized_fill(finish_, new_finish, Val);
				finish_ = new_finish;
			}
			catch (...)
			{
				destoryNodes(finish_.node_ + 1, new_finish.node_ + 1);
				throw;
			}
		}
		else
		{
			insert_aux(Pos, Count, Val);
		}
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::resize(size_type newSize, const value_type& Val)
	{
		//while (size() < newSize)
		//{
		//	push_back(Val);
		//}

		//while (size() > newSize)
		//{
		//	pop_back();
		//}
		const size_type len = size();
		if (newSize < len)
		{
			erase(start_ + newSize, finish_);
		}
		else
		{
			insert(finish_, newSize - len, Val);
		}
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::resize(size_type newSize)
	{
		//const size_type len = size();
		//while (len < newSize)
		//{
		//	push_back();
		//}

		//while (len > newSize)
		//{
		//	pop_back();
		//}
		resize(newSize, value_type());
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::erase(iterator Pos)
	{
		iterator next = Pos;
		++next;
		difference_type index = Pos - start_;
		if (static_cast<size_type>(index) < (this->size() >> 1))
		{	// 前方元素比较少，使用copy_backward(),尽可能减少元素移动次数
			TinySTL::copy_backward(start_, Pos, next);
			pop_front();
		}
		else
		{
			TinySTL::copy(next, finish_, Pos);
			pop_back();
		}
		return start_ + index;
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::erase(iterator First, iterator Last)
	{
		if (First == start_ && Last == finish_)
		{
			clear();
			return finish_;
		}
		else
		{
			difference_type elems_count = Last - First;
			difference_type elems_before = First - start_;
			if (static_cast<size_type>(elems_before) < (size() - elems_count) >> 1)
			{	// 前方元素比较少，使用copy_backward(),尽可能减少元素移动次数
				TinySTL::copy_backward(start_, First, Last);
				iterator new_start = start_ + elems_count;
				// 析构多余元素
				TinySTL::destroy(start_, new_start);
				// 回收未用区段
				destroyNodes(new_start.node_, start_.node_);
				start_ = new_start;
			}
			else
			{	// 后元素比较少，使用copy(),尽可能减少元素移动次数
				TinySTL::copy(Last, finish_, First);
				iterator new_finish = finish_ - elems_count;
				TinySTL::destroy(new_finish, finish_);
				destroyNodes(new_finish.node_ + 1, finish_.node_ + 1);
				finish_ = new_finish;
			}
			return start_ + elems_before;
		}
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::clear()
	{	// 依次析构每个区段的对象元素,然后回收析构完的区段
		for (mapPointer dctNode = start_.node_ + 1; dctNode < finish_.node_; ++dctNode)
		{	// 对完整区段进行统一处理
			TinySTL::destroy(*dctNode, *dctNode + buffSize());
			deallocateNode(*dctNode);
		}

		if (start_.node_ != finish_.node_)
		{	// 所有元素分布在不少于两个区段上,对尚未处理的第一个和最后一个区段进行处理
			TinySTL::destroy(start_.cur_, start_.last_);    // 第一个
			TinySTL::destroy(finish_.first_, finish_.cur_); // 最后一个
			deallocateNode(finish_.first_);
		}
		else
		{
			destroy(start_.cur_, finish_.cur_);
		}
		finish_ = start_;
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::swap(deque<T, Alloc>& Other)
	{
		TinySTL::swap(mapSize_, Other.mapSize_);
		TinySTL::swap(map_, Other.map_);
		TinySTL::swap(start_, Other.start_);
		TinySTL::swap(finish_, Other.finish_);
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void deque<T, Alloc>::rangeInitialize(InIt First, InIt Last, TinySTL::input_iterator_tag)
	{
		initiailizeMap(0); // 默认创建8个节点
		try
		{
			for (; First != Last; ++First)
				push_back(*First);
		}
		catch (...)
		{
			clear();
			throw;
		}
	}

	template<class T, class Alloc>
	template<class FwdIt>
	inline void deque<T, Alloc>::rangeInitialize(FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag)
	{
		size_type Diff = static_cast<size_type>(TinySTL::distance(First, Last));
		initiailizeMap(Diff);
		mapPointer curNode = nullptr;
		try
		{
			for (curNode = start_.node_; curNode < finish_.node_; ++curNode)
			{
				FwdIt mid = First;
				TinySTL::advance(mid, buffSize());
				TinySTL::uninitialized_copy(First, mid, *curNode);
				First = mid;
			}
			TinySTL::uninitialized_copy(First, Last, finish_.first_);
		}
		catch (...)
		{
			TinySTL::destroy(start_, iterator(*curNode, curNode));
			throw;
		}
	}

	template<class T, class Alloc>
	template<class Integer>
	inline void deque<T, Alloc>::assign_dispatch(Integer Count, Integer Val, true_type)
	{
		fill_assign(static_cast<size_type>(Count), static_cast<value_type>(Val));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void deque<T, Alloc>::assign_dispatch(InIt First, InIt Last, false_type)
	{	//根据迭代器不同采取不同的方法,以取得最佳效率
		assign_aux(First, Last, iterator_category(InIt));
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void deque<T, Alloc>::assign_aux(InIt First, InIt Last, TinySTL::input_iterator_tag)
	{
		iterator cur = begin();
		for (; First != Last && cur != end(); ++cur, ++First)
		{
			*cur = *First;
		}
		if (First == Last)
		{
			erase(cur, end());
		}
		else
		{
			insert(end(), First, Last);
		}
	}

	template<class T, class Alloc>
	template<class FwdIt>
	inline void deque<T, Alloc>::assign_aux(FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag)
	{
		difference_type Diff = TinySTL::distance(First, Last);
		if (static_cast<size_type>(Diff) > size())
		{
			FwdIt mid = First;
			TinySTL::advance(mid, static_cast<difference_type>(size()));
			TinySTL::copy(First, mid, begin());
			insert(end(), mid, Last);
		}
		else
		{
			erase(copy(First, Last, begin()), end());
		}
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void deque<T, Alloc>::insert(iterator Pos, InIt First, InIt Last, TinySTL::input_iterator_tag)
	{
		TinySTL::copy(First, Last, inserter(*this, Pos));
	}

	template<class T, class Alloc>
	template<class FwdIt>
	inline void deque<T, Alloc>::insert(iterator Pos, FwdIt First, FwdIt Last, TinySTL::forward_iterator_tag)
	{
		size_type Diff = static_cast<size_type>(TinySTL::distance(First, Last));
		if (Pos.cur_ == start_.cur_) // 插入点是deque的开头
		{
			iterator new_start = reserve_elements_at_front(Diff);
			try
			{
				TinySTL::uninitialized_copy(First, Last, new_start);
				start_ = new_start;
			}
			catch (...)
			{
				deallocateNode(new_start.node_, start_.node_);
				throw;
			}
		}
		else if (Pos.cur_ == finish_.cur_) // 插入点是deque的末尾
		{
			iterator new_finish = reserve_elements_at_back(Diff);
			try
			{
				TinySTL::uninitialized_copy(First, Last, finish_);
				finish_ = new_finish;
			}
			catch (...)
			{
				destoryNodes(finish_.node_ + 1, new_finish.node_ + 1);
				throw;
			}
		}
		else
			insert_aux(Pos, First, Last, Diff);
	}

	template<class T, class Alloc>
	template<class FwdIt>
	inline void deque<T, Alloc>::insert_aux(iterator Pos, FwdIt First, FwdIt Last, size_type Count)
	{
		const difference_type elems_before = Pos - start_;
		size_type length = this->size();
		if (elems_before < static_cast<difference_type>(length >> 1))
		{
			iterator new_start = reserve_elements_at_front(Count);
			iterator old_start = start_;
			Pos = old_start + elems_before;
			try
			{
				if (elems_before >= static_cast<difference_type>(Count))
				{
					iterator start_n = old_start + static_cast<difference_type>(Count);
					TinySTL::uninitialized_copy(start_, start_n, new_start);
					start_ = new_start;
					TinySTL::copy(start_n, Pos, old_start);
					TinySTL::copy(First, Last, Pos - static_cast<difference_type>(Count));
				}
				else
				{
					FwdIt mid = First;
					TinySTL::advance(mid, static_cast<difference_type>(Count) - elems_before);
					TinySTL::uninitialized_copy_copy(start_, Pos, First, mid, new_start);
					start_ = new_start;
					TinySTL::copy(mid, Last, old_start);
				}
			}
			catch (...)
			{
				destoryNodes(new_start.node_, start_.node_);
				throw;
			}
		}
		else
		{
			iterator new_finish = reserve_elements_at_back(Count);
			iterator old_finish = finish_;
			const difference_type elems_after = static_cast<difference_type>(length) - elems_before;
			Pos = old_finish - elems_after;
			try
			{
				if (elems_after > static_cast<difference_type>(Count))
				{
					iterator finish_n = old_finish - static_cast<difference_type>(Count);
					TinySTL::uninitialized_copy(finish_n, finish_, finish_);
					finish_ = new_finish;
					TinySTL::copy_backward(Pos, finish_n, old_finish);
					TinySTL::copy(First, Last, Pos);
				}
				else
				{
					FwdIt mid = First;
					TinySTL::advance(mid, elems_after);
					TinySTL::uninitialized_copy_copy(mid, Last, Pos, finish_, finish_);
					finish_ = new_finish;
					TinySTL::copy(First, mid, Pos);
				}
			}
			catch (...)
			{
				destoryNodes(finish_.node_ + 1, new_finish.node_ + 1);
				throw;
			}
		}
	}

	template<class T, class Alloc>
	template<class Integer>
	inline void deque<T, Alloc>::initialize_dispatch(Integer numElements, Integer Val, true_type)
	{
		initiailizeMap(numElements);
		fillInitialize(Val);
	}

	template<class T, class Alloc>
	template<class InIt>
	inline void deque<T, Alloc>::initialize_dispatch(InIt First, InIt Last, false_type)
	{
		rangeInitialize(First, Last, TinySTL::iterator_category(First));
	}

	template<class T, class Alloc>
	inline void deque<T, Alloc>::fillInitialize(const value_type& Val)
	{
		mapPointer Cur = nullptr;
		try
		{   // 对完整的缓冲器统一处理
			for (Cur = start_.node_; Cur < finish_.node_; ++Cur)
			{
				TinySTL::uninitialized_fill(*Cur, *Cur + buffSize(), Val);
			}
			// 最后一个节点的设定不同，因为尾端可能有备用空间，不必设初值
			TinySTL::uninitialized_fill(finish_.first_, finish_.cur_, Val);
		}
		catch (...)
		{
			TinySTL::destroy(start_, iterator(*Cur, Cur));
			throw;
		}
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::begin()
	{
		return start_;
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_iterator deque<T, Alloc>::begin() const
	{
		return start_; 
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::iterator deque<T, Alloc>::end()
	{
		return finish_;
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_iterator deque<T, Alloc>::end() const
	{ 
		return finish_;
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::reverse_iterator deque<T, Alloc>::rbegin() 
	{
		return reverse_iterator(end());
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_reverse_iterator deque<T, Alloc>::rbegin() const 
	{ 
		return const_reverse_iterator(end()); 
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::reverse_iterator deque<T, Alloc>::rend() 
	{ 
		return reverse_iterator(begin()); 
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_reverse_iterator deque<T, Alloc>::rend() const 
	{
		return const_reverse_iterator(begin());
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_iterator deque<T, Alloc>::cbegin() const 
	{ 
		return begin(); 
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_reverse_iterator deque<T, Alloc>::crbegin() const
	{ 
		return rbegin();
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_iterator deque<T, Alloc>::cend() const 
	{
		return end();
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_reverse_iterator deque<T, Alloc>::crend() const
	{ 
		return rend(); 
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::reference deque<T, Alloc>::operator[](size_type Pos)
	{ 
		return *(begin() + static_cast<difference_type>(Pos));
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_reference deque<T, Alloc>::operator[](size_type Pos) const
	{ 
		return *(begin() + static_cast<difference_type>(Pos));
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::reference deque<T, Alloc>::front()
	{ 
		if (!empty())
			return *begin();
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_reference deque<T, Alloc>::front() const
	{ 
		if (!empty())
			return *begin();
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::reference deque<T, Alloc>::back()
	{ 
		if (!empty())
		{
			const_iterator Tmp = finish_;
			--finish_; 
			return *finish_;
		}
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::const_reference deque<T, Alloc>::back() const
	{ 
		if (!empty())
		{
			const_iterator Tmp = finish_;
			--finish_; 
			return *finish_;
		}
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::size_type deque<T, Alloc>::size() const
	{
		return finish_ - start_;
	}

	template<class T, class Alloc>
	inline typename deque<T, Alloc>::size_type deque<T, Alloc>::max_size() const
	{
		return size_type(-1) / sizeof(T);
	}

	template<class T, class Alloc>
	inline bool deque<T, Alloc>::empty() const
	{
		return finish_ == start_;
	}

	template<class T, class Alloc>
	bool operator==(const deque<T,Alloc>& Left, const deque<T,Alloc>& Right)
	{
		return Left.size() == Right.size() && TinySTL::equal(Left.begin(), Left.end(), Right.begin());
	}

	template<class T, class Alloc>
	bool operator!=(const deque<T,Alloc>& Left, const deque<T,Alloc>& Right)
	{
		return !(Left == Right);
	}

	template<class T, class Alloc>
	bool operator<(const deque<T,Alloc>& Left, const deque<T,Alloc>& Right)
	{
		return TinySTL::compare(Left.begin(), Left.end(), Right.begin(), Right.end());
	}

	template<class T, class Alloc>
	bool operator<=(const deque<T,Alloc>& Left, const deque<T,Alloc>& Right)
	{
		return !(Right < Left);
	}

	template<class T, class Alloc>
	bool operator>(const deque<T,Alloc>& Left, const deque<T,Alloc>& Right)
	{
		return Right < Left;
	}

	template<class T, class Alloc>
	bool operator>=(const deque<T,Alloc>& Left, const deque<T,Alloc>& Right)
	{
		return !(Left < Right);
	}

	//template <class T, class Alloc = TinySTL::allocator<T>>
	//class deque
	//{
	//public:
	//	// 成员类型
	//	using value_type             = T;
	//	using allocator_type         = Alloc;
	//	using size_type              = size_t;
	//	using difference_type        = ptrdiff_t;
	//	using pointer                = typename Alloc::pointer;
	//	using const_pointer          = typename Alloc::const_pointer;
	//	using reference              = T&;
	//	using const_reference        = const T&;

	//	using iterator               = deque_iterator<T>;
	//	using const_iterator         = deque_iterator<const T>;
	//	using reverse_iterator       = TinySTL::reverse_iterator<iterator>;

	//	using map_point              = T**;

	//	enum { BLOCK_SIZE = blockSize };

	//private:
	//	iterator start_;	// 迭代器
	//	iterator last_;		// 迭代器
	//	map_point map_;     // 迭代器数组(map)指针
	//	size_type mapSize_; // map内有多少个指针

	//private:
	//	static constexpr size_t bytes = sizeof(value_type);
	//	static constexpr int MinimumMapSize = 8;

	//public:
	//	// 缓冲区元素个数
	//	static constexpr size_t blockSize = bytes <= 1 ? 16 :
	//								 bytes <= 2 ? 8  :
	//								 bytes <= 4 ? 4  :
	//								 bytes <= 8 ? 2  : 1;
	//	
	//private:
	//	void init();
	//	T** getANewMap(const size_type size);
	//	T* getANewBuck();

	//public:
	//	// 构造函数
	//	deque();
	//	explicit deque(size_type Count, const value_type& Val = value_type());
	//	template <class InIt>
	//	deque(InIt First, InIt Last);
	//	deque(const deque& Right);
	//	// 析构函数
	//	~deque();

	//	deque& operator=(const deque& Other);

	//	// Iterators:
	//	iterator begin() noexcept;
	//	const_iterator begin() const noexcept;
	//	iterator end() noexcept;
	//	const_iterator end() const noexcept;
	//	reverse_iterator rbegin() noexcept;
	//	reverse_iterator rend() noexcept;
	//	const_iterator cbegin() const noexcept;
	//	const_iterator cend() const noexcept;

	//	// capacity:
	//	size_type size() const noexcept;
	//	size_type max_size() const noexcept;
	//	void resize(size_type Newsize);
	//	void resize(size_type Newsize, const value_type& Val);
	//	bool empty() const noexcept;
	//	void shrink_to_fit();

	//	// Element access:
	//	reference operator[] (size_type Pos) const;
	//	const_reference operator[](size_type Pos) const noexcept;
	//	reference at(size_type Pos);
	//	const_reference at (size_type Pos) const;
	//	reference front();
	//	const_reference front() const;
	//	reference back();
	//	const_reference back() const;

	//	// Modifiers:
	//	void assign(size_type Count, const T& Val);
	//	template <class InIt>
	//	void assign(InIt First, InIt Last);
	//	void push_back(const value_type& Val);
	//	void push_front(const value_type& Val);
	//	void pop_back();
	//	void pop_front();
	//	iterator insert(const_iterator Pos, const value_type& Val);
	//	iterator insert(const_iterator Pos, size_type Count, const value_type& Val);
	//	iterator erase(const_iterator Pos);
	//	iterator erase(const_iterator First, const_iterator Last);
	//	void swap(deque& Other);
	//	void clear() noexcept;

	//public:
	//	// Non-member functions overloads
	//	template <class T, class Alloc>
	//	friend bool operator==(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator!=(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator<(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator<=(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator>(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);
	//	template <class T, class Alloc>
	//	friend bool operator>=(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right);

	//	template <class T, class Alloc>
	//	friend void swap(deque<T, Alloc>& Left, deque<T, Alloc>& Right);
	//};

	//template<class T, class Alloc>
	//bool operator==(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right)
	//{
	//	return Left.size() != Right.size() && TinySTL::equal(Left.begin(), Left.end(), Right.begin());
	//}

	//template<class T, class Alloc>
	//bool operator!=(const deque<T,Alloc>& Left, const deque<T,Alloc>& Right)
	//{
	//	return !(Left == Right);
	//}

	//template<class T, class Alloc>
	//bool operator<(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right)
	//{
	//	return TinySTL::compare(Left.begin(), Left.end(), Right.begin(), Right.end());
	//}

	//template<class T, class Alloc>
	//bool operator<=(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right)
	//{
	//	return !(Right < Left);
	//}

	//template<class T, class Alloc>
	//bool operator>(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right)
	//{
	//	return Right < Left;
	//}

	//template<class T, class Alloc>
	//bool operator>=(const deque<T, Alloc>& Left, const deque<T, Alloc>& Right)
	//{
	//	return !(Left < Right);
	//}
}

#endif // !_DEQUE_H_

