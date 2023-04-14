#ifndef _UTILITY_H_
#define _UTILITY_H_

namespace TinySTL
{
	// 交换的模板类
	template<class T>
	inline void swap(T& left, T& right)
	{
		T temp = left;
		left = right;
		right = temp;
	}

	// pair的模板(类模板)
	template<class T1, class T2>
	struct pair
	{
	public:
		using first_type  = T1;
		using second_type = T2;

	public:
		T1 first;
		T2 second;

	public:
		pair() = default;                                                                        // 1.默认构造函数

		// pair<other1, other2>模板类
		template <class other1, class other2>
		pair(const pair<other1, other2>& right) :first(right.first), second(right.second) {} // 2.拷贝构造函数
		pair(const pair&) = default;

		pair(const T1& Val1, const T2& Val2) :first(Val1), second(Val2) {}                     // 3.初始化构造函数

		template <class other1, class other2>
		inline pair& operator=(const pair<other1, other2>& right) // 重载赋值操作符
		{
			if (this != &right)
			{
				first = right.first;
				second = right.second;
			}
			return *this; // 返回对象本身的引用
		}
		inline pair& operator=(const pair& right)
		{
			if (this != &right) // 判断是否为同一个对象
			{
				first = right.first;
				second = right.second;
			}
			return *this; // 返回对象本身的引用
		}

		inline void swap(pair& right)
		{
			if (this != &right)
			{
				TinySTL::swap(first, right.first);
				TinySTL::swap(second, right.second);
			}
		}

	public:
		template <class T1, class T2>
		friend inline void swap(const pair<T1, T2>& left, const pair<T1, T2>& right);

		template <class T1, class T2>
		friend inline bool operator==(const pair<T1, T2>& left, const pair<T1, T2>& right);

		template <class T1, class T2>
		friend inline bool operator!=(const pair<T1, T2>& left, const pair<T1, T2>& right);

		template <class T1, class T2>
		friend inline bool operator<(const pair<T1, T2>& left, const pair<T1, T2>& right);

		template <class T1, class T2>
		friend inline bool operator>(const pair<T1, T2>& left, const pair<T1, T2>& right);
	
		template <class T1, class T2>
		friend inline bool operator<=(const pair<T1, T2>& left, const pair<T1, T2>& right);
	
		template <class T1, class T2>
		friend inline bool operator>=(const pair<T1, T2>& left, const pair<T1, T2>& right);
	
	};

	//template<class T1, class T2>
	//inline TinySTL::pair<T1, T2>& TinySTL::pair<T1, T2>::operator=(const pair<T1, T2>& pr)
	//{
	//	if (this != &pr) // 判断是否为同一个对象
	//	{
	//		first = pr.first;
	//		second = pr.second;
	//	}

	//	return *this; // 返回对象本身的引用
	//}

	template <class T1, class T2>
	void swap(const pair<T1, T2>& left, const pair<T1, T2>& right)
	{
		left.swap(right);
	}

	template <class T1, class T2>
	bool operator==(const pair<T1, T2>& left, const pair<T1, T2>& right)
	{
		return left.first == right.first && left.second == right.second;
	}

	template <class T1, class T2>
	bool operator!=(const pair<T1, T2>& left, const pair<T1, T2>& right)
	{
		return !(left == right);
	}

	template <class T1, class T2>
	bool operator<(const pair<T1, T2>& left, const pair<T1, T2>& right)
	{
		return left.first < right.first || (!(right.first < left.first) && left.second < right.second);
	}

	template <class T1, class T2>
	bool operator>(const pair<T1, T2>& left, const pair<T1, T2>& right)
	{
		return right < left;
	}

	template <class T1, class T2>
	bool operator<=(const pair<T1, T2>& left, const pair<T1, T2>& right)
	{
		return !(right < left);
	}

	template <class T1, class T2>
	bool operator>=(const pair<T1, T2>& left, const pair<T1, T2>& right)
	{
		return !(left < right);
	}

	// make_pair模板函数
	template <class T1, class T2>
	pair<T1, T2> make_pair(const T1& val1, const T2& val2)
	{
		return pair<T1, T2>(val1, val2);
	}
}

#endif // !_UTILITY_H_
