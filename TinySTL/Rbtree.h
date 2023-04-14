#ifndef _RBTREE_H_
#define _RBTREE_H_

#include "Iterator.h"
#include "Algorithm.h"
#include "ReserverseIterator.h"

namespace TinySTL
{
	using _Rb_tree_color_type = bool;

	const _Rb_tree_color_type _S_rb_tree_red   = false; // 红色为0
	const _Rb_tree_color_type _S_rb_tree_black = true;  // 黑色为1

	struct _Rb_tree_node_base
	{
		using _color_type = _Rb_tree_color_type;
		using _Base_ptr   = _Rb_tree_node_base*;

		_color_type _M_color; // 节点颜色，非红即黑
		_Base_ptr _M_parent;  // 指向父节点的指针
		_Base_ptr _M_left;    // 指向左节点
		_Base_ptr _M_right;	  // 指向右节点

		// 寻找以x为根节点的RB树的最小节点
		static _Base_ptr _S_minimum(_Base_ptr __x)
		{
			while (__x->_M_left != nullptr) // 一直向左走，就会找最小值
				__x = __x->_M_left;         // 这是二叉搜索树的特性
			return __x;
		}

		// 寻找以x为根节点的RB树的最大值节点
		static _Base_ptr _S_maximum(_Base_ptr __x)
		{
			while (__x->_M_right != nullptr) // 一直向右走，就会找最大值
				__x = __x->_M_right;         // 这是二叉搜索树的特性
			return __x;
		}
	}; 

	template<class _Value>
	struct _Rb_tree_node : public _Rb_tree_node_base
	{
		using _Link_type = _Rb_tree_node<_Value>*;
		_Value _M_value_field; // 节点值
	};

	struct _Rb_tree_base_iterator
	{
		using _Base_ptr         = _Rb_tree_node_base::_Base_ptr;
		using iterator_category = bidirectional_iterator_tag;
		using difference_type   = ptrdiff_t;

		_Base_ptr _M_node;

		// 找到递增序列的后继
		void _M_increment()
		{
			if (_M_node->_M_right != nullptr) // 如果右子树非空，状况（1）
			{	// 找到右子树的最左端子孙节点
				_M_node = _M_node->_M_right;
				while (_M_node->_M_left != nullptr) // 一直往左子树走到底
				{
					_M_node = _M_node->_M_left; 
				}
			}
			else // 右子树为空，状况（2）
			{
				_Base_ptr __y = _M_node->_M_parent; // 找出父节点
				while (_M_node == __y->_M_right) // 如果现行节点本身是个右子节点
				{	// 沿着父节点上溯，直到其为父节点的左孩子，或者达到根节点
					_M_node = __y; 
					__y = __y->_M_parent;
				}
				if (_M_node->_M_right != __y) // 若此时的右子节点不等于此时的父节点
					_M_node = __y; // 状况（3）此时的父节点即为解答
								   // 否则此时的node为解答，状况（4）
			}
			// 以上判断“若此时的右子节点不等于此时的父节点”，是为了应付一种特殊情况：我们欲寻找根节点
			// 的下一个节点，而恰巧根节点无右子节点。以上特殊做法需要配合RB-tree根节点与特殊之header
			// 之间的特殊关系
		}

		// 找到递增序列的前驱
		void _M_decrement()
		{
			// 如果是红节点，且父节点的父节点等于自己，状况（1），右子节点即为解答
			if (_M_node->_M_color == _S_rb_tree_red && _M_node->_M_parent->_M_parent == _M_node)
			{
				_M_node = _M_node->_M_right;
				// 以上情况发生于node为header时（即node为end()时）
				// header之右子节点即mostright，指向整棵树的max节点
			}
			else if (_M_node->_M_left != nullptr) // 如果有左子节点，状况（2）
			{
				_Base_ptr __y = _M_node->_M_left; 
				while (__y->_M_right != nullptr) // 当__y有右子节点时
					__y = __y->_M_right; // 一直往右子节点走到底
				_M_node = __y;
			}
			else // 既非根节点，亦无左子节点
			{
				_Base_ptr __y = _M_node->_M_parent; // 状况（3），找到父节点
				while (_M_node == __y->_M_left) // 当现行节点身为左子节点
				{
					_M_node = __y; // 一直交替往上走，直到现行节点不为左子节点
					__y = __y->_M_parent;
				}

				// 找到前驱，即为其父节点
				_M_node = __y;
			}
		}
	};

	// RB-tree的正规迭代器
	template<class _Value, class _Ref, class _Ptr>
	struct _Rb_tree_iterator : public _Rb_tree_base_iterator
	{
		using value_type     = _Value;
		using reference      = _Ref;
		using pointer        = _Ptr;
		using iterator       = _Rb_tree_iterator<_Value, _Value&, _Value*>;
		using const_iterator = _Rb_tree_iterator<_Value, const _Value&, const _Value*>;
		using _Self          = _Rb_tree_iterator<_Value, _Ref, _Ptr>;
		using _Link_type     = _Rb_tree_node<_Value>*;

		_Rb_tree_iterator() {}
		_Rb_tree_iterator(_Link_type __x) { _M_node = __x; }
		_Rb_tree_iterator(const iterator& __it) { _M_node = __it._M_node; }

		reference operator*() const { return static_cast<_Link_type>(_M_node)->_M_value_field; }
		pointer operator->() const { return &(operator*()); }

		_Self& operator++() 
		{ 
			_M_increment(); 
			return *this;
		}

		_Self& operator++(int)
		{
			_Self tmp = *this;
			_M_increment();
			return tmp;
		}

		_Self& operator--()
		{
			_M_decrement(); 
			return* this;
		}

		_Self& operator--(int)
		{
			_Self tmp = *this;
			_M_decrement();
			return tmp;
		}
	};
	
	inline bool operator==(const _Rb_tree_base_iterator& __x, const _Rb_tree_base_iterator& __y)
	{
		return __x._M_node == __y._M_node;
	}

	inline bool operator!=(const _Rb_tree_base_iterator& __x, const _Rb_tree_base_iterator& __y)
	{
		return !(__x._M_node == __y._M_node);
	}

	// 全局函数
	// 新节点必为红节点。如果插入处之父节点亦为红节点，就违反红黑树规则，需要做树形旋转
	// 将以__x为根的子树左旋
	inline void _Rb_tree_rotate_left(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root)
	{
		_Rb_tree_node_base* __y = __x->_M_right; // 令__y为旋转点的右子节点
		__x->_M_right = __y->_M_left; // __x的右子节点指向__x->_M_right的左子节点
		if (__y->_M_left != nullptr)
			__y->_M_left->_M_parent = __x;
		__y->_M_parent = __x->_M_parent;

		// 令__y完全顶替__x的地位，必须将__x对其父节点的关系完全接受过来
		if (__x == __root) // __x为根节点
			__root = __y;
		else if (__x == __x->_M_parent->_M_left) // __x为其父节点的左子节点
			__x->_M_parent->_M_left = __y;
		else // __x为其父节点的右子节点
			__x->_M_parent->_M_right = __y;
		__y->_M_left = __x;
		__x->_M_parent = __y;
	}

	// 将以__x为根的子树右旋
	inline void _Rb_tree_rotate_right(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root)
	{
		_Rb_tree_node_base* __y = __x->_M_left; // 令__y为旋转点的左子节点
		__x->_M_left = __y->_M_right; // __x的左子节点指向__x->_M_left的右子节点
		if (__y->_M_right != nullptr)
			__y->_M_right->_M_parent = __x;
		__y->_M_parent = __x->_M_parent;

		if (__x == __root)
			__root = __y;
		else if (__x == __x->_M_parent->_M_right)
			__x->_M_parent->_M_right = __y;
		else
			__x->_M_parent->_M_left = __y;
		__y->_M_right = __x;
		__x->_M_parent = __y;
	}

	//插入结点__x之后的调整,使其符合RB树的定义,
	//调整采用按子树逐层上溯处理,直至整棵树合法为止
	inline void _Rb_tree_rebalance(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root)
	{
		__x->_M_color = _S_rb_tree_red; // 新节点必为红
		while (__x != __root && __x->_M_parent->_M_color == _S_rb_tree_red) // 父节点为红
		{
			if (__x->_M_parent == __x->_M_parent->_M_parent->_M_left) // 父节点为祖父节点的左节点
			{
				_Rb_tree_node_base* __y = __x->_M_parent->_M_parent->_M_right; // __y为伯父节点
				if (__y && __y->_M_color == _S_rb_tree_red) // 伯父节点存在且为红
				{
					__x->_M_parent->_M_color = _S_rb_tree_black; // 更改父节点为黑
					__y->_M_color = _S_rb_tree_black; // 更改伯父节点为黑
					__x->_M_parent->_M_parent->_M_color = _S_rb_tree_red; // 更改祖父节点为红
					__x = __x->_M_parent->_M_parent; // 再次循环从其祖父节点开始
				}
				else // 无伯父节点，或伯父节点为黑
				{	// 此时__x和__x的父节点都为红色，伯父节点为黑
					if (__x == __x->_M_parent->_M_right) // 如果新节点为父节点的右子节点
					{									 // 调整，将__x调整为左孩子交由后面处理
						__x = __x->_M_parent;
						_Rb_tree_rotate_left(__x, __root);
					}
					// 此时__x必为左孩子，且__x，__x的父节点均为红色，__x伯父节点为黑
					__x->_M_parent->_M_color = _S_rb_tree_black;
					__x->_M_parent->_M_parent->_M_color = _S_rb_tree_red;
					_Rb_tree_rotate_right(__x->_M_parent->_M_parent, __root);
					// 此时__x的父节点已经为黑色，可保证整棵树符合RB树的定义，完全可以跳出循环
				}
			}
			else // 父节点为祖父节点的右节点
			{
				_Rb_tree_node_base* __y = __x->_M_parent->_M_parent->_M_left; // __y为伯父节点
				if (__y && __y->_M_color == _S_rb_tree_red) // 伯父节点存在且为红
				{
					__x->_M_parent->_M_color = _S_rb_tree_black; // 更改父节点为黑
					__y->_M_color = _S_rb_tree_black; // 更改伯父节点为黑
					__x->_M_parent->_M_parent->_M_color = _S_rb_tree_red; // 更改祖父节点为红
					__x = __x->_M_parent->_M_parent; // 再次循环从其祖父节点开始
				}
				else // 无伯父节点，或伯父节点为黑
				{
					if (__x == __x->_M_parent->_M_left) // 如果新节点为父节点的左子节点
					{
						__x = __x->_M_parent;
						_Rb_tree_rotate_right(__x, __root);
					}
					// 此时__x必为右孩子，且__x，__x的父节点均为红色，__x伯父节点为黑
					__x->_M_parent->_M_color = _S_rb_tree_black;
					__x->_M_parent->_M_parent->_M_color = _S_rb_tree_red;
					_Rb_tree_rotate_left(__x->_M_parent->_M_parent, __root);
				}
			}
		}
		// while结束，根节点永远为黑
		__root->_M_color = _S_rb_tree_black;
	}

	// 删除节点z并调整该树，使其符合RB树的定义
	inline _Rb_tree_node_base* _Rb_tree_rebalance_for_erase(_Rb_tree_node_base* __z,
															_Rb_tree_node_base*& __root,
															_Rb_tree_node_base*& __leftmost,
															_Rb_tree_node_base*& __rightmost)
	{
		_Rb_tree_node_base* __y = __z;            // 待删除结点
		_Rb_tree_node_base* __x = nullptr;		  // 待删除结点或其后继结点的子节点
		_Rb_tree_node_base* __x_parent = nullptr; // 待删除结点的父节点
		if (__y->_M_left == nullptr)       // __z最多有一个非null子节点. __y = __z
			__x = __y->_M_right;           // __x可能是null
		else if (__y->_M_right == nullptr) // __z只有一个非null子节点. __y = __z
			__x = __y->_M_left;            // __x不是null
		else                               // __z有两个非null子节点
		{		
			// 为了删除以后方便调整,RB树只删除没有孩子或只有一个孩子的结点
			// 如果需要删除的结点具有双孩子,需要找到该结点的后继(一定无双孩子)
			// 然后用后继代替该节点,同时染上该节点的颜色,调整因此,实际删除
			// 的结点永远无双孩子
			__y = __y->_M_right;
			while (__y->_M_left != nullptr) // 让__y找到__z的后继节点
				__y = __y->_M_left;
			__x = __y->_M_right;            // x可能为null
		}

		if (__y != __z) // __z具有双子节点
		{  // 用y代替z
			__z->_M_left->_M_parent = __y;
			__y->_M_left = __z->_M_left;
			if (__y != __z->_M_right)
			{
				__x_parent = __y->_M_parent;
				if (__x) // x不为null
					__x->_M_parent = __y->_M_parent;
				__y->_M_parent->_M_left = __x;
				__y->_M_right = __z->_M_right;
				__z->_M_right->_M_parent = __y;
			}
			else
				__x_parent = __y;

			if (__root == __z)
				__root = __y;
			else if (__z->_M_parent->_M_left == __z)
				__z->_M_parent->_M_left = __y;
			else
				__z->_M_parent->_M_right = __y;

			__y->_M_parent = __z->_M_parent;
			TinySTL::swap(__y->_M_color, __z->_M_color);
			__y = __z;
		}
		else  // __z不具有双子节点(具有一个或没有子节点)
		{
			__x_parent = __y->_M_parent;
			if (__x)
				__x->_M_parent = __y->_M_parent;
			if (__root == __z)
				__root = __x;
			else if (__z->_M_parent->_M_left == __z)
				__z->_M_parent->_M_left = __x;
			else
				__z->_M_parent->_M_right = __x;

			if (__leftmost == __z) // z已经是整棵树的最小值
			{
				if (__z->_M_right == nullptr) // z没有右子节点，即z没有子节点
					__leftmost = __z->_M_parent;
				else
					__leftmost = _Rb_tree_node_base::_S_minimum(__x);
			}
			
			if (__rightmost == __z) // z已经是整棵树的最大值
			{
				if (__z->_M_left == nullptr) // z没有左子节点，即z没有子节点
					__rightmost = __z->_M_parent;
				else
					__rightmost = _Rb_tree_node_base::_S_maximum(__x);
			}
		}
		// 删除完毕,需要调整,和rebalance一样是按子树逐层上溯处理,直到整棵树合法为止

		// 如果删除的结点为红色结点，不需要调整
		if (__y->_M_color != _S_rb_tree_red)
		{
			//如果x是根结点或者红色结点,只需最后调整结点颜色为黑色即可使整棵树满足RB树的定义
			while (__x != __root && (__x == nullptr || __x->_M_color == _S_rb_tree_black))
			{
				if (__x == __x_parent->_M_left)
				{
					// w为被删节点的兄弟，由于删除操作，w子树比x子树多一个黑结点
					_Rb_tree_node_base* __w = __x_parent->_M_right;
					if (__w->_M_color == _S_rb_tree_red)
					{
						// 通过调整将w变为黑色,交由下面处理
						__w->_M_color = _S_rb_tree_black;
						__x_parent->_M_color = _S_rb_tree_red;
						_Rb_tree_rotate_left(__x_parent, __root);
						__w = __x_parent->_M_right;
					}

					// 此时w一定为黑色,而且仍然比x子树多一个黑结点
					if ((__w->_M_left == nullptr || __w->_M_left->_M_color == _S_rb_tree_black) &&
						(__w->_M_right == nullptr || __w->_M_right->_M_color == _S_rb_tree_black)) // w的两个子节点均为黑色
					{
						// 将w染成红色,此时w子树减少了一个黑结点,和x子树黑结点数目相同
						// 但__x_parent子树比其兄弟子树少一个结点,因此令x=__x_parent,
						// 交由下次循环处理
						__w->_M_color = _S_rb_tree_red;
						__x = __x_parent;
						__x_parent = __x_parent->_M_parent;
					}
					else // w为黑色,其孩子一红一黑或者同为红色
					{
						if (__w->_M_right == nullptr || __w->_M_right->_M_color == _S_rb_tree_black) // w孩子节点左红右黑
						{
							if (__w->_M_left)
								__w->_M_left->_M_color = _S_rb_tree_black;

							__w->_M_color = _S_rb_tree_red;
							_Rb_tree_rotate_right(__w, __root);
							__w = __x_parent->_M_right;
							// 此处w仍然为黑色,w子树的黑结点仍然比x子树多1,但w的孩子节点为左黑右红,交由下面处理
						}

						// 此时此处w一定为黑色,w子树的黑结点一定比x子树多1,w的右孩子节点一定为红色
						__w->_M_color = __x_parent->_M_color;
						__x_parent->_M_color = _S_rb_tree_black;
						if (__w->_M_right)
							__w->_M_right->_M_color = _S_rb_tree_black;
						_Rb_tree_rotate_left(__x_parent, __root);
						// 至此处可保证整棵树符合RB树的定义
						break;
					}
				}
				else
				{
					_Rb_tree_node_base* __w = __x_parent->_M_left;
					if (__w->_M_color == _S_rb_tree_red)
					{
						__w->_M_color = _S_rb_tree_black;
						__x_parent->_M_color = _S_rb_tree_red;
						_Rb_tree_rotate_right(__x_parent, __root);
						__w = __x_parent->_M_left;
					}
					if ((__w->_M_right == 0 || __w->_M_right->_M_color == _S_rb_tree_black) &&
						(__w->_M_left == 0 || __w->_M_left->_M_color == _S_rb_tree_black))
					{
						__w->_M_color = _S_rb_tree_red;
						__x = __x_parent;
						__x_parent = __x_parent->_M_parent;
					}
					else
					{
						if (__w->_M_left == 0 || __w->_M_left->_M_color == _S_rb_tree_black)
						{
							if (__w->_M_right) 
								__w->_M_right->_M_color = _S_rb_tree_black;
							__w->_M_color = _S_rb_tree_red;
							_Rb_tree_rotate_left(__w, __root);
							__w = __x_parent->_M_left;
						}
						__w->_M_color = __x_parent->_M_color;
						__x_parent->_M_color = _S_rb_tree_black;
						if (__w->_M_left)
							__w->_M_left->_M_color = _S_rb_tree_black;
						_Rb_tree_rotate_right(__x_parent, __root);
						break;
					}
				}
			}
			if (__x)
				__x->_M_color = _S_rb_tree_black;
		}
		return __y;
	}

	// 为了避免基类为空，我们任意
	// 将rbtree的一个数据成员移动到基类中
	template<class _Tp, class _Alloc>
	struct _Rb_tree_base
	{
		using allocator_type = _Alloc;
		allocator_type get_allocator() const { return allocator_type(); }

		_Rb_tree_base(const allocator_type&) : _M_header(_M_get_node()) {}
		~_Rb_tree_base() { _M_put_node(_M_header); }

	protected:
		_Rb_tree_node<_Tp>* _M_header; // 不存储数据元素,只存储指向根节点,最小节点和最大结点的指针

		using _Alloc_type = simple_alloc<_Rb_tree_node<_Tp>, _Alloc>;

		_Rb_tree_node<_Tp>* _M_get_node() { return _Alloc_type::allocate(1); }
		void _M_put_node(_Rb_tree_node<_Tp>* __p) { _Alloc_type::deallocate(__p, 1); }
	};

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc = TinySTL::alloc>
	class _Rb_tree : protected _Rb_tree_base<_Value, _Alloc>
	{
		using _Base           = _Rb_tree_base<_Value, _Alloc>;

	protected:
		using _Base_ptr       = _Rb_tree_node_base*;
		using _Rb_tree_node   = _Rb_tree_node<_Value>;
		using _Color_type     = _Rb_tree_color_type;

	public:
		using key_type        = _Key;
		using value_type      = _Value;
		using pointer         = value_type*;
		using const_pointer   = const pointer;
		using reference       = value_type&;
		using const_reference = const reference;
		using _Link_type      = _Rb_tree_node*;
		using size_type       = size_t;
		using difference_type = ptrdiff_t;

		using allocator_type = typename _Base::allocator_type;
		allocator_type get_allocator() const { return _Base::get_allocator(); }

	protected:
		using _Base::_M_get_node;
		using _Base::_M_put_node;
		using _Base::_M_header;

	protected:
		_Link_type _M_create_node(const value_type& __x) // 产生一个新节点
		{
			_Link_type __tmp = _M_get_node();
			try
			{
				construct(&__tmp->_M_value_field, __x);
			}
			catch (...)
			{
				_M_put_node(__tmp);
			}

			return __tmp;
		}

		_Link_type _M_clone_node(_Link_type __x) // 克隆一个节点
		{
			_Link_type __tmp = _M_create_node(__x->_M_value_field);
			__tmp->_M_color = __x->_M_color;
			__tmp->_M_left = nullptr;
			__tmp->_M_right = nullptr;
			return __tmp;
		}

		void destroy_node(_Link_type __p)
		{
			destroy(&__p->_M_value_field);
			_M_put_node(__p);
		}

	protected:
		size_type _M_node_count; // 追踪记录树的大小（节点数量）
		_Compare _M_key_compare; // 节点间的键值大小比较标准 function object

		_Link_type& _M_root() const // _M_header和_M_root互为父节点
		{
			return (_Link_type&)(_M_header->_M_parent);
		}

		_Link_type& _M_leftmost() const // _M_header->_M_left指向最小值,即最左端结点
		{
			return (_Link_type&)(_M_header->_M_left);
		}

		_Link_type& _M_rightmost() const // _M_header->_M_right指向最小值,即最左端结点
		{
			return (_Link_type&)(_M_header->_M_right);
		}

		static _Link_type& _S_left(_Link_type __x)
		{
			return (_Link_type&)(__x->_M_left);
		}

		static _Link_type& _S_right(_Link_type __x)
		{
			return (_Link_type&)(__x->_M_right);
		}

		static _Link_type& _S_parent(_Link_type __x)
		{
			return (_Link_type&)(__x->_M_parent);
		}

		static reference _S_value(_Link_type __x)
		{
			return __x->_M_value_field;
		}

		static const _Key& _S_key(_Link_type __x)
		{
			return _KeyOfValue()(_S_value(__x));
		}

		static _Color_type& _S_color(_Link_type __x)
		{
			return (_Color_type&)(__x->_M_color);
		}

		static _Link_type& _S_left(_Base_ptr __x)
		{
			return (_Link_type&)(__x->_M_left);
		}

		static _Link_type& _S_right(_Base_ptr __x)
		{
			return (_Link_type&)(__x->_M_right);
		}

		static _Link_type& _S_parent(_Base_ptr __x)
		{
			return (_Link_type&)(__x->_M_parent);
		}

		static reference _S_value(_Base_ptr __x)
		{
			return ((_Link_type)__x)->_M_value_field;
		}

		static const _Key& _S_key(_Base_ptr __x)
		{
			return _KeyOfValue()(_S_value((_Link_type)__x));
		}

		static _Color_type& _S_color(_Base_ptr __x)
		{
			return (_Color_type&)(((_Link_type)__x)->_M_color);
		}

		static _Link_type _S_minimum(_Link_type __x)
		{
			return (_Link_type)_Rb_tree_node_base::_S_minimum(__x);
		}

		static _Link_type _S_maximum(_Link_type __x)
		{
			return (_Link_type)_Rb_tree_node_base::_S_maximum(__x);
		}

	public:
		using iterator               = _Rb_tree_iterator<value_type, reference, pointer>;
		using const_iterator         = _Rb_tree_iterator<value_type, const_reference, const_pointer>;
		using const_reverse_iterator = reverse_iterator<const_iterator>;
		using reverse_iterator       = reverse_iterator<iterator>;

	private:
		// v为要插入的值，x为要插入的位置，y为x的父节点
		iterator _M_insert(_Base_ptr __x, _Base_ptr __y, const value_type& __v);
		_Link_type _M_copy(_Link_type __x, _Link_type __p); // 复制__x子树到__P子树
		void _M_erase(_Link_type __x); // 删除节点x的子树，且不需要rebalance

	public:
		// 构造函数和析构函数
		_Rb_tree() : _Base(allocator_type()), _M_node_count(0), _M_key_compare()  // base类构造时会创建_M_header
		{
			_M_empty_initialize();
		}

		_Rb_tree(const _Compare& __comp) : _Base(allocator_type()), _M_node_count(0), _M_key_compare(__comp)
		{
			_M_empty_initialize();
		}

		_Rb_tree(const _Compare& __comp, const allocator_type& __a) : _Base(__a), _M_node_count(0), _M_key_compare(__comp)
		{
			_M_empty_initialize();
		}

		// 拷贝构造函数
		_Rb_tree(const _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __x) :
			_Base(__x.get_allocator()), _M_node_count(__x._M_node_count), _M_key_compare(__x._M_key_compare)
		{
			if (__x._M_root() == nullptr)
				_M_empty_initialize();
			else
			{
				_S_color(_M_header) = _S_rb_tree_red;
				_M_root() = _M_copy(__x._M_root(), _M_header);
				_M_leftmost() = _S_minimum(_M_root());
				_M_rightmost() = _S_maximum(_M_root());
			}
		}

		~_Rb_tree()
		{ 
			clear();
		}

		_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>&
		operator=(const _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __x);

	private:
		void _M_empty_initialize() // 空树的构建
		{
			_S_color(_M_header) = _S_rb_tree_red; // 令header为红色，用来区分header和root（在iterator.operator++中）
			_M_root() = nullptr;
			_M_leftmost() = _M_header;  // 令header的左子节点为自己
			_M_rightmost() = _M_header; // 令header的右子节点为自己
		}

	public:
		_Compare key_comp() const { return _M_key_compare; }
		iterator begin() { return _M_leftmost(); }
		const_iterator begin() const { return _M_leftmost(); }
		iterator end() { return _M_header; }
		const_iterator end() const { return _M_header; }
		reverse_iterator rbegin() { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
		reverse_iterator rend() { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const { return reverse_iterator(begin()); }
		bool empty() const { return _M_node_count == 0; }
		size_type size() const { return _M_node_count; }
		size_type max_size() const { return size_type(-1); }
		void swap(_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __x)
		{
			TinySTL::swap(_M_header, __x._M_header);
			TinySTL::swap(_M_node_count, __x._M_node_count);
			TinySTL::swap(_M_key_compare, __x._M_key_compare);
		}

	public:
		// 插入新值，节点键值不允许重复，若重复则插入无效
		// 返回值是个pair，第一个元素是个RB-tree迭代器，指向新增节点
		// 第二个元素表示插入是否成功
		TinySTL::pair<iterator, bool> insert_unique(const value_type& __v);

		// 插入新值，节点键值可以重复
		// 返回值是个RB-tree迭代器，指向新增节点
		iterator insert_equal(const value_type& __v);

		// 不可重复插入,先在指定位置插入,若指定位置插入不合法,
		// 再试图寻找合法位置插入
		iterator insert_unique(iterator __position, const value_type& __v);
		
		// 可重复插入
		iterator insert_equal(iterator __position, const value_type& __v);

		template <class InIt>
		void insert_unique(InIt __first, InIt __last);
		template <class InIt>
		void insert_equal(InIt __first, InIt __last);

		void erase(iterator __position);
		size_type erase(const key_type& __x);
		void erase(iterator __first, iterator __last);
		void erase(const key_type& __first, const key_type& __last);

		void clear() // 将树清空
		{
			if (_M_node_count != 0)
			{
				_M_erase(_M_root());
				_M_root() = nullptr;
				_M_leftmost() = _M_header;
				_M_rightmost() = _M_header;
				_M_node_count = 0;
			}
		}

	public:
		iterator find(const key_type& __k);
		const_iterator find(const key_type& __k) const;
		size_type count(const key_type& __k) const;
		iterator lower_bound(const key_type& __k);
		const_iterator lower_bound(const key_type& __k) const;
		iterator upper_bound(const key_type& __k);
		const_iterator upper_bound(const key_type& __k) const;
		pair<iterator,iterator> equal_range(const key_type& __k);
		pair<const_iterator,const_iterator> equal_range(const key_type& __k) const;

	public:
		// 验证该RB-tree是否合法
		bool __rb_verify() const;
	};

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::_M_insert(_Base_ptr __x_, _Base_ptr __y_, const value_type& __v)
	{
		_Link_type __x = (_Link_type)__x_; // x为要插入的位置
		_Link_type __y = (_Link_type)__y_; // y为x的父节点
		_Link_type __z;

		// key_compare是键值大小比较准则，是个function object（仿函数）
		// __y == _M_header：插入位置为root
		// __x != nullptr： 插入位置为null节点
		// _M_key_compare(_KeyOfValue()(__v), _S_key(__y))：插入点为其父节点的左子节点
		if (__y == _M_header || __x != nullptr || _M_key_compare(_KeyOfValue()(__v), _S_key(__y)))
		{
			__z = _M_create_node(__v); // 产生一个新节点
			_S_left(__y) = __z; // 这使得当y即为header时，leftmost() = z
			if (__y == _M_header)
			{
				_M_root() = __z;
				_M_rightmost() = __z;
			}
			else if (__y == _M_leftmost()) // 如果y为最右节点
				_M_leftmost() = __z;
		}
		else
		{
			__z = _M_create_node(__v); // 产生一个新节点
			_S_right(__y) = __z; // 令新节点成为插入点之父节点y的右子节点
			if (__y == _M_rightmost())
				_M_rightmost() = __z; // 维护rightmost()，使它永远指向最右节点
		}
		_S_parent(__z) = __y;    // 设定新节点的父节点
		_S_left(__z) = nullptr;  // 设定新节点的左子节点
		_S_right(__z) = nullptr; // 设定新节点的有子节点
		_Rb_tree_rebalance(__z, _M_header->_M_parent);
		++_M_node_count;
		return iterator(__z); // 返回一个迭代器，指向新增节点
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::_Link_type 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::_M_copy(_Link_type __x, _Link_type __p)
	{
		// 整棵树所有的右子树都递归复制,所有的左子树都直接复制
		// structural copy.  __x and __p must be non-null	
		_Link_type __top = _M_clone_node(__x);
		__top->_M_parent = __p;
		try
		{
			if (__x->_M_right)
				__top->_M_right = _M_copy(_S_right(__x), __top); // 递归复制x的右子树
			__p = __top;
			__x = _S_left(__x);
			// 直接复制x的左子树
			while (__x != nullptr)
			{
				_Link_type __y = _M_clone_node(__x);
				__p->_M_left = __y;
				__y->_M_parent = __p;
				if (__x->_M_right)
					__y->_M_right = _M_copy(_S_right(__x), __y); // 继续递归复制右子树
				__p = __y;
				__x = _S_left(__x); // x指向其左孩子，循环复制其左子树
			}
		}
		catch (...)
		{
			_M_erase(__top);
		}
		return __top;
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline void _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::_M_erase(_Link_type __x)
	{
		while (__x != nullptr)
		{
			_M_erase(_S_right(__x)); // 递归删除右子树
			// 循环删除左子树
			_Link_type __y = _S_left(__x);
			destroy_node(__x);
			__x = __y;
		}
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::
	operator=(const _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __x)
	{
		if (*this != __x)
		{
			clear(); // 先将需要赋值的树清空
			_M_node_count = 0;
			_M_key_compare = __x._M_key_compare; // 比较函数赋值
			if (__x._M_root() == nullptr)
			{
				_M_root() = nullptr;
				_M_leftmost() = _M_header;
				_M_rightmost() = _M_header;
			}
			else
			{
				_M_root() = _M_copy(__x._M_root(), _M_header); // 复制整棵树
				_M_leftmost() = _S_minimum(_M_root());
				_M_rightmost() = _S_maximum(_M_root());
				_M_node_count = __x._M_node_count;
			}
		}
		return *this;
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline TinySTL::pair<typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator, bool>
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::insert_unique(const value_type& __v)
	{
		_Link_type __y = _M_header;
		_Link_type __x = _M_root(); // 从根节点开始
		bool __comp = true;
		while (__x != nullptr) // 从根节点开始，往下寻找适当的插入点
		{
			__y = __x;
			__comp = _M_key_compare(_KeyOfValue()(__v), _S_key(__x)); // v键值小于目前节点的键值
			// v小于x的键值时向左子树移动,否则向右子树移动,因此,如果已有重复
			// 键值存在,x最终一定位于重复键值所在节点的右子树
			__x = __comp ? _S_left(__x) : _S_right(__x);
		}
		// 离开while循环之后，y所指即插入点的父节点（此时的它必为叶节点）
		iterator __j = iterator(__y); // 令迭代器j指向插入点的父节点y
		if (__comp) // 如果离开while循环时comp为true，表示插入点是其父节点的左孩子
		{
			if (__j == begin()) // 如果插入点的父节点为最左节点
				return TinySTL::pair<iterator, bool>(_M_insert(__x, __y, __v), true);
			else // 否者，插入点的父节点不为最左节点
				--__j; // 得到其父节点的直接前驱结点,若插入x,即x成为的直接前驱结点
					   // 调整j，回头准备测试
		}
		if (_M_key_compare(_S_key(__j._M_node), _KeyOfValue()(__v))) // 插入点的父节点的直接前驱结点小于插入新值
			return TinySTL::pair<iterator, bool>(_M_insert(__x, __y, __v), true); // 此时，j为插入x后，x的直接前驱结点
		
		// 到这里，表示新值一定与树中键值重复，不插入新值
		return TinySTL::pair<iterator, bool>(__j, false);
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::insert_equal(const value_type& __v)
	{
		_Link_type __y = _M_header;
		_Link_type __x = _M_root(); // 从根节点开始
		while (__x != nullptr) // 寻找合适的插入点（一定为null），同时记录插入点的父节点
		{
			__y = __x;
			__x = _M_key_compare(_KeyOfValue()(__v), _S_key(__x)) ? _S_left(__x) : _S_right(__x);
		}
		return _M_insert(__x, __y, __v);
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::insert_unique(iterator __position, const value_type& __v)
	{
		if (__position._M_node == _M_header->_M_left) // begin()，__position为最左端
		{
			if (size() > 0 && _M_key_compare(_KeyOfValue()(__v), _S_key(__position._M_node)))
			{	// 第一个参数需要为非null
				return _M_insert(__position._M_node, __position._M_node, __v);
			}
			else
				return insert_unique(__v).first;
		}
		else if (__position._M_node == _M_header) // end()
		{
			if (_M_key_compare(_S_key(_M_rightmost()), _KeyOfValue()(__v)))
			{
				return _M_insert(0, __position._M_node, __v);
			}
			else
				return insert_unique(__v).first;
		}
		else
		{
			iterator __before = __position;
			--__before; // 到插入位置的直接前驱
			if (_M_key_compare(_S_key(__before._M_node), _KeyOfValue()(__v)) &&
				_M_key_compare(_KeyOfValue()(__v), _S_key(__position._M_node)))
			{
				if (_S_right(__before._M_node) == 0)
					return _M_insert(0, __before._M_node, __v);
				else
					return _M_insert(__position._M_node, __position._M_node, __v);
			}
			else
				return insert_unique(__v).first;
		}
	}
	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::insert_equal(iterator __position, const value_type& __v)
	{
		if (__position._M_node == _M_header->_M_left)   // begin()
		{
			if (size() > 0 && !_M_key_compare(_S_key(__position._M_node), _KeyOfValue()(__v)))
				return _M_insert(__position._M_node, __position._M_node, __v);
			// first argument just needs to be non-null
			else
				return insert_equal(__v);
		}
		else if (__position._M_node == _M_header)    // end()
		{
			if (!_M_key_compare(_KeyOfValue()(__v), _S_key(_M_rightmost())))
				return _M_insert(0, _M_rightmost(), __v);
			else
				return insert_equal(__v);
		}
		else
		{
			iterator __before = __position;
			--__before;
			if (!_M_key_compare(_KeyOfValue()(__v), _S_key(__before._M_node))
				&& !_M_key_compare(_S_key(__position._M_node), _KeyOfValue()(__v)))
			{
				if (_S_right(__before._M_node) == 0)
					return _M_insert(0, __before._M_node, __v);
				else
					return _M_insert(__position._M_node, __position._M_node, __v);
				// first argument just needs to be non-null
			}
			else
				return insert_equal(__v);
		}	
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline void _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::erase(iterator __position)
	{
		_Link_type __y = (_Link_type)_Rb_tree_rebalance_for_erase(__position._M_node,
																  _M_header->_M_parent,
																  _M_header->_M_left,
																  _M_header->_M_right);
		destroy_node(__y);
		--_M_node_count;
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::size_type 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::erase(const key_type& __x)
	{
		TinySTL::pair<iterator, iterator> __p = equal_range(__x);
		size_type __n = static_cast<size_type>(TinySTL::distance(__p.first, __p.second));
		erase(__p.first, __p.second);
		return __n;
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline void _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::erase(iterator __first, iterator __last)
	{
		if (__first == begin() && __last == end())
			clear();
		else
			while (__first != __last)
			{
				erase(__first);
				++__first;
			}
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline void _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::erase(const key_type& __first, const key_type& __last)
	{
		while (__first != __last)
			erase(*__first++);
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::find(const key_type& __k)
	{
		_Link_type __y = _M_header; // Last node which is not less than __k
		_Link_type __x = _M_root;   // current node

		while (__x != nullptr)
		{
			if (!_M_key_compare(_S_key(__x), __k)) // k <= x的键值时
			{
				__y = __x;
				__x = _S_left(__x);
			}
			else // k > x的键值时
				__x = _S_right(__x);
		}

		// 至此,x为k的可插入点,y为x的直接前驱,如果k存在,则j不可能为end()
		// 而且应有j指向结点的键值和k相等
		iterator __j = iterator(__y);
		return (__j == end() || _M_key_compare(__k, _S_key(__j._M_node))) ? end() : __j;
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::const_iterator 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::find(const key_type& __k) const
	{
		_Link_type __y = _M_header; // Last node which is not less than __k
		_Link_type __x = _M_root;   // current node

		while (__x != nullptr)
		{
			if (!_M_key_compare(_S_key(__x), __k)) // k <= x的键值时
			{
				__y = __x;
				__x = _S_left(__x);
			}
			else // k > x的键值时
				__x = _S_right(__x);
		}

		// 至此,x为k的可插入点,y为x的直接前驱,如果k存在,则j不可能为end()
		// 而且应有j指向结点的键值和k相等
		const_iterator __j = const_iterator(__y);
		return (__j == end() || _M_key_compare(__k, _S_key(__j._M_node))) ? end() : __j;
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::size_type
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::count(const key_type& __k) const
	{
		pair<const_iterator, const_iterator> __p = equal_range(__k);
		size_type __n = static_cast<size_type>(distance(__p.first, __p.second));
		
		return __n;
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::lower_bound(const key_type& __k)
	{
		_Link_type __y = _M_header;
		_Link_type __x = _M_root();

		while (__x != nullptr) // 当x的键值等于k时，寻找更小的键值等于k的x
		{
			if (!_M_key_compare(_S_key(__x), __k)) // x的键值大于k
			{
				__y = __x;
				__x = _S_left(__x);
			}
			else
				__x = _S_right(__x);
		}

		return iterator(__y);
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::const_iterator
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::lower_bound(const key_type& __k) const
	{
		_Link_type __y = _M_header;
		_Link_type __x = _M_root();

		while (__x != nullptr) // 当x的键值等于k时，寻找更小的键值等于k的x
		{
			if (!_M_key_compare(_S_key(__x), __k)) // x的键值大于k
			{
				__y = __x;
				__x = _S_left(__x);
			}
			else
				__x = _S_right(__x);
		}

		return const_iterator(__y);
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::upper_bound(const key_type& __k)
	{
		_Link_type __y = _M_header;
		_Link_type __x = _M_root();

		while (__k != nullptr)
		{
			if (_M_key_compare(__k, _S_key())) // k小于x的键值
			{
				__y = __x;
				__x = _S_left(__x);
			}
			else
				__x = _S_right(__x);
		}

		return iterator(__y);
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::const_iterator 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::upper_bound(const key_type& __k) const
	{
		_Link_type __y = _M_header;
		_Link_type __x = _M_root();

		while (__k != nullptr)
		{
			if (_M_key_compare(__k, _S_key())) // k小于x的键值
			{
				__y = __x;
				__x = _S_left(__x);
			}
			else
				__x = _S_right(__x);
		}

		return const_iterator(__y);
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline pair<typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator,
				typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator> 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::equal_range(const key_type& __k)
	{
		return pair<iterator, iterator>(lower_bound(__k), upper_bound(__k));
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline pair<typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::const_iterator, 
				typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::const_iterator> 
	_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::equal_range(const key_type& __k) const
	{
		return pair<const_iterator, const_iterator>(lower_bound(__k), lower_bound(__k));
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	template<class InIt>
	inline void _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::insert_unique(InIt __first, InIt __last)
	{
		for (; __first != __last; ++__first)
			insert_unique(*__first);
	}

	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	template<class InIt>
	inline void _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::insert_equal(InIt __first, InIt __last)
	{	
		for ( ; __first != __last; ++__first)
			insert_equal(*__first);
	}

	// 计算[__node, __root]之间的黑色结点个数，采用递归上溯法
	inline int __black_count(_Rb_tree_node_base* __node, _Rb_tree_node_base* __root)
	{
		if (__node == nullptr)
			return 0;
		else
		{
			int __bc = __node->_M_color == _S_rb_tree_black ? 1 : 0;

			if (__node == __root)
				return __bc;
			else
				return __bc + __black_count(__node->_M_parent, __root);
		}
	}
	
	template<class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
	inline bool _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::__rb_verify() const
	{
		if (_M_node_count == 0 || begin() == end()) // 当结点个数为0，或header结点的left和right相等
			return _M_node_count == 0 && begin() == end() &&
		           _M_header->_M_left == _M_header && _M_header->_M_right == _M_header;
		
		int __len = __black_count(_M_leftmost, _M_root()); // 叶子节点到根节点路径的黑色结点数目
		for (const_iterator __it = begin(); __it != end(); ++__it) // 遍历整棵树
		{
			_Link_type __x = (_Link_type)__it._M_node;
			_Link_type __L = _S_left(__x);  // __x的左孩子
			_Link_type __R = _S_right(__x); // __x的右孩子

			// 检验红色结点是否有红色孩子
			if (__x->_M_color == _S_rb_tree_red)
			{
				if ((__L && __L->_M_color == _S_rb_tree_red) || (__R && __R->_M_color == _S_rb_tree_red))
					return false;
			}

			// 检验键值是否不大于其左孩子
			if (__L && _M_key_compare(_S_key(__x), _S_key(__L)))
				return false;

			// 检验键值是否不小于其右孩子
			if (__R && _M_key_compare( _S_key(__R),_S_key(__x)))
				return false;

			// 检验叶子节点到根结点路径的黑色结点数目是否等于__len
			if (!__L && !__R && __black_count(__x, _M_root()) != __len)
				return false;
		}

		// 检验最大、小值指针指向是否正确
		if (_M_leftmost() != _Rb_tree_node_base::_S_minimum(_M_root()))
			return false;
		if (_M_rightmost() != _Rb_tree_node_base::_S_maximum(_M_root()))
			return false;

		return true;
	}
}

#endif // !_RBTREE_H_

