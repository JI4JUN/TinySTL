#ifndef _REVERSE_ITERATOR_H_
#define _REVERSE_ITERATOR_H_

#include "Iterator.h"

namespace TinySTL
{
	/*
	* 实现了反向迭代器
	* 包括衍生部分bidirectional_iterator和random_access_iterator
	*/
	template <class BidIt>
	class reverse_iterator
	{
	public:
		// 逆向迭代器的5种associated types和其对应的正向迭代器相同
		using iterator_category = typename TinySTL::iterator_traits<BidIt>::iterator_category;
		using value_type        = typename TinySTL::iterator_traits<BidIt>::value_type;
		using difference_type   = typename TinySTL::iterator_traits<BidIt>::difference_type;
		using pointer           = typename TinySTL::iterator_traits<BidIt>::pointer;
		using reference         = typename TinySTL::iterator_traits<BidIt>::reference;

		using iterator_type     = BidIt;
		using self              = reverse_iterator<BidIt>;

	protected:
		// BidIt base_;    // 反转后的最前端
		BidIt current_{}; // 对应的正向迭代器

	public:
		// 默认构造函数
		reverse_iterator() = default;
		// 构造函数，接受一个正向迭代器，explicit阻止隐式的类型转换，只能被显式调用
		explicit reverse_iterator(iterator_type Right) : current_(Right) {}
		// 拷贝构造函数
		template<class Other>
		reverse_iterator(const self& Right) : current_(Right.current_) {}

	public:
		inline BidIt base() const { return current_; } // 取出对应的正向迭代器
		// pointer-like classes
		inline reference operator*() const // 对逆向迭代器取值，即将对应的正向迭代器退一位取值，返回引用
		{ 
			BidIt Tmp = current_;
			return *--Tmp;
		};

		inline pointer operator->() const // 同上，返回指针
		{
			// return &(operator*());

			BidIt Tmp = current_;
			--Tmp;
			return Tmp.operator->();
		}

		inline reverse_iterator& operator++() // 前置++
		{
			--current_;
			return *this;
		}

		inline reverse_iterator operator++(int) // 后置++
		{
			reverse_iterator Tmp = *this;
			--current_;
			return Tmp;
		}

		inline reverse_iterator operator--() // 前置--
		{
			++current_;
			return *this;
		}

		inline reverse_iterator operator--(int) // 后置--
		{
			reverse_iterator Tmp = *this;
			++current_;
			return Tmp;
		}

		inline reverse_iterator operator+(const difference_type Off) const
		{
			return reverse_iterator(current_ - Off);
		}

		inline reverse_iterator& operator+=(const difference_type Off)
		{
			current_ -= Off;
			return *this;
		}

		inline reverse_iterator& operator-(const difference_type Off) const
		{
			return reverse_iterator(current_ + Off);
		}

		inline reverse_iterator& operator-=(const difference_type Off)
		{
			current_ += Off;
			return *this;
		}

		inline reference operator[](const difference_type Off) const
		{
			return current_[static_cast<difference_type>(-Off - 1)];
		}

		constexpr const BidIt& GetCurrent() const { return current_; }

	public:
		// relatrional operator
		template <class BidIt1, class BidIt2>
		friend bool operator==(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right);

		template <class BidIt1, class BidIt2>
		friend bool operator!=(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right);

		template <class BidIt1, class BidIt2>
		friend bool operator<(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right);

		template <class BidIt1, class BidIt2>
		friend bool operator>(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right);

		template <class BidIt1, class BidIt2>
		friend bool operator<=(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right);

		template <class BidIt1, class BidIt2>
		friend bool operator>=(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right);

	public:
		template <class BidIt1, class BidIt2>
		friend auto operator-(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right)
			-> decltype(Right.GetCurrent() - Left.GetCurrent());

		template <class BidIt>
		friend inline reverse_iterator<BidIt> operator+(
			typename reverse_iterator<BidIt>::difference_type Off, const reverse_iterator<BidIt>& Right);
	};

	template <class BidIt1, class BidIt2>
	bool operator==(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right)
	{
		return Left.GetCurrent() == Right.GetCurrent();
	}

	template <class BidIt1, class BidIt2>
	bool operator!=(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right)
	{
		return Left.GetCurrent() != Right.GetCurrent();
	}

	template <class BidIt1, class BidIt2>
	bool operator<(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right)
	{
		return Left.GetCurrent() > Right.GetCurrent();
	}

	template <class BidIt1, class BidIt2>
	bool operator>(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right)
	{
		return Left.GetCurrent() < Right.GetCurrent();
	}

	template <class BidIt1, class BidIt2>
	bool operator<=(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right)
	{
		return Left.GetCurrent() >= Right.GetCurrent();
	}

	template <class BidIt1, class BidIt2>
	bool operator>=(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right)
	{
		return Left.GetCurrent() <= Right.GetCurrent();
	}

	template <class BidIt1, class BidIt2>
	auto operator-(const reverse_iterator<BidIt1>& Left, const reverse_iterator<BidIt2>& Right) -> decltype(Right.GetCurrent() - Left.GetCurrent())
	{
		return Right.GetCurrent() - Left.GetCurrent();
	}
	template <class BidIt>
	reverse_iterator<BidIt> operator+(typename reverse_iterator<BidIt>::difference_type Off, const reverse_iterator<BidIt>& Right)
	{
		return Right + Off;
	}
}

#endif // !_REVERSE_ITERATOR_H_
