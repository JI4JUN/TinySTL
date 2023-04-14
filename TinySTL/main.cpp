#include <iostream>

#include <vector>
#include <deque>
#include <list>

//#include "Utility.h"
#include "ReserverseIterator.h"
#include "Algorithm.h"
#include "Iterator.h"
#include "UninitializedFunctions.h"
#include "TypeTraits.h"
#include "Deque.h"
#include "Vector.h"
#include "List.h"
#include "Stack.h"
#include "Queue.h"
#include "Slist.h"
#include "Rbtree.h"
#include "Functional.h"

template<class T>
void showItems(T& container)
{
	for (auto n : container)
		std::cout << n << ' ';
	std::cout << std::endl;
}

int main()
{
	//TinySTL::_Rb_tree<int, int, TinySTL::identity<int>, TinySTL::less<int>> itree;
	//std::cout << itree.size() << std::endl;
	//itree.insert_unique(10); // _Rb_tree_rebalance
	//std::cout << "1 : " << itree.size() << std::endl;
	//itree.insert_unique(7);  // _Rb_tree_rebalance
	//std::cout << "2 : "<< itree.size() << std::endl;
	//itree.insert_unique(8);  // _Rb_tree_rebalance
	//std::cout << "3 : "<< itree.size() << std::endl;
	//							// _Rb_tree_rotate_left
	//							// _Rb_tree_rotate_right
	//itree.insert_unique(15); // _Rb_tree_rebalance
	//std::cout << "4 : " << itree.size() << std::endl;
	//itree.insert_unique(5);  // _Rb_tree_rebalance
	//std::cout << "5 : "<< itree.size() << std::endl;
	//itree.insert_unique(6);  // _Rb_tree_rebalance
	//std::cout << "6 : "<< itree.size() << std::endl;
	//							// _Rb_tree_rotate_left
	//							// _Rb_tree_rotate_right
	//itree.insert_unique(11); // _Rb_tree_rebalance
	//std::cout << "7 : "<< itree.size() << std::endl;
	//							// _Rb_tree_rotate_right
	//							// _Rb_tree_rotate_left
	//itree.insert_unique(13); // _Rb_tree_rebalance
	//std::cout << "8 : "<< itree.size() << std::endl;
	//itree.insert_unique(12);
	//std::cout << "9 : "<< itree.size() << std::endl;
	//showItems(itree);

	//auto iter1 = itree.begin();
	//auto iter2 = itree.end();
	//TinySTL::_Rb_tree_base_iterator rbtite;
	//for (; iter1 != iter2; ++iter1)
	//{
	//	rbtite = TinySTL::_Rb_tree_base_iterator(iter1); // 向上转型没问题
	//	std::cout << *iter1 << '(' << rbtite._M_node->_M_color << ") ";
	//}
	//std::cout << std::endl;



	//int arr1[] = { 1, 4, 2, 5, 33, 12, 23, 27 };
	//TinySTL::slist<int> slist1(std::begin(arr1), std::end(arr1));
	//showItems(slist1);
	//std::cout << slist1.size() << std::endl;
	//slist1.push_front(9);
	//slist1.push_front(10);
	//showItems(slist1);
	//TinySTL::slist<int>::iterator ite = slist1.begin();
	//ite = TinySTL::find(slist1.begin(), slist1.end(), 33);
	//std::cout << *ite << std::endl;
	//slist1.push_front(9);
	//slist1.push_front(10);
	//slist1.sort();
	//showItems(slist1);
	//slist1.unique();
	//showItems(slist1);

	//TinySTL::queue<int> q;
	//q.push(1);
	//q.push(2);
	//q.push(3);
	//std::cout << q.size() << std::endl;

	//int arr1[] = { 1, 4, 2, 5, 33, 12, 23, 27 };
	//TinySTL::priority_queue<int> ipg(std::begin(arr1), std::end(arr1));
	//std::cout << ipg.size() << std::endl;
	//for (int i = 0; i < ipg.size(); ++i)
	//	std::cout << ipg.top() << ' ';
	//std::cout << std::endl;
	//while (!ipg.empty())
	//{
	//	std::cout << ipg.top() << ' ';
	//	ipg.pop();
	//}
	//std::cout << std::endl;

	//int arr1[] = { 1, 4, 2, 5, 33, 12, 23, 27 };
	//TinySTL::vector<int> vec1(std::begin(arr1), std::end(arr1));
	//TinySTL::make_heap(vec1.begin(), vec1.end());
	//showItems(vec1);
	//vec1.push_back(7);
	//TinySTL::push_heap(vec1.begin(), vec1.end());
	//showItems(vec1);
	//TinySTL::pop_heap(vec1.begin(), vec1.end());
	//vec1.pop_back();
	//showItems(vec1);
	//TinySTL::sort(vec1.begin(), vec1.end());
	//showItems(vec1);

	//TinySTL::stack<int> stack1;
	//stack1.push(1);
	//stack1.push(2);
	//stack1.push(3);
	//std::cout << stack1.top() << std::endl;
	//std::cout << stack1.top() << std::endl;
	//stack1.pop();
	//std::cout << stack1.top() << std::endl;

	//TinySTL::list<int> list1(10,1);
	//showItems(list1);
	//int arr1[] = { 1, 4, 3, 2, 4, 6, 2, 1, 7, 2 };
	//TinySTL::list<int> list2(std::begin(arr1), std::end(arr1));
	//showItems(list2);
	//list2.push_back(0);
	//list2.push_front(0);
	//showItems(list2);
	//list2.pop_front();
	//list2.pop_back();
	//showItems(list2);
	//int arr2[] = { 1, 4, 3, 2, 4, 6, 2, 1, 7, 2 };
	//TinySTL::list<int> list3(std::begin(arr2), std::end(arr2));
	//list3.sort();
	//showItems(list3);

	//int arr1[] = { 1, 2, 3, 4 };
	//TinySTL::vector<int> vec1(std::begin(arr1), std::end(arr1));
	//vec1.push_back(5);
	//vec1.pop_back();
	//for (int n : vec1)
	//	std::cout << n << ' ';
	//std::cout << std::endl;
	//std::cout << vec1.size() << std::endl;
	//std::cout << vec1.capacity() << std::endl;
	//vec1.insert(vec1.end(), 2, 6);
	//for (int n : vec1)
	//	std::cout << n << ' ';
	//std::cout << std::endl;
	//std::cout << vec1.size() << std::endl;
	//std::cout << vec1.capacity() << std::endl;
	//vec1.push_back(5);
	//vec1.push_back(5);
	//vec1.push_back(5);
	//vec1.push_back(5);
	//vec1.push_back(5);
	//vec1.push_back(5);
	//vec1.push_back(5);
	//vec1.push_back(5);
	//std::cout << vec1.size() << std::endl;
	//std::cout << vec1.capacity() << std::endl;
	//vec1.resize(1);
	//std::cout << vec1.size() << std::endl;
	//std::cout << vec1.capacity() << std::endl;
	//TinySTL::vector<int> vec2(vec1);
	//vec2.push_back(2);
	//showItems(vec2);
	//std::cout << (vec1 == vec2) << std::endl;
	//std::cout << (vec1 < vec2) << std::endl;

	//int arr1[] = { 1, 2, 3, 4 };
	//int arr2[] = { 1, 2, 3, 4, 5 };
	//TinySTL::deque<int> d1(std::begin(arr1), std::end(arr1));
	//TinySTL::deque<int> d2(std::begin(arr2), std::end(arr2));
	//bool isEqual = d1 == d2;
	//std::cout << isEqual << std::endl;
	//std::cout << d2[2] << std::endl;
	//d2.push_back(5);
	//d2.push_front(0);
	//d2.pop_back();
	//d2.pop_front(); 
	//d2.insert(++d2.begin(), 100);
	//d2.insert(++d2.begin());
	//d2.erase(++d2.begin());
	//d2.resize(2);
	//std::cout << *(d2.begin() + 4) << std::endl;
	//d2.erase(d2.begin(), d2.begin() + 4);
 //   for (TinySTL::deque<int>::iterator it = d2.begin(); it!=d2.end(); ++it)
	//	std::cout << ' ' << *it;
	//std::cout << std::endl;
	//std::cout << d2.empty() << std::endl;

	//int arr[] = { 7, 5, 16, 8 };
	//TinySTL::deque<int> d(std::begin(arr), std::end(arr));
	//for (int n : d)
	//	std::cout << n << ' ';
	//std::cout << std::endl;
	//d.push_back(1);
	//for (int n : d)
	//	std::cout << n << ' ';
	//std::cout << std::endl;
	//d.push_front(2);
	//for (int n : d)
	//	std::cout << n << ' ';
	//std::cout << std::endl;

	//std::cout << TinySTL::bool_constant<true>() << std::endl;
	//std::cout << TinySTL::is_integral<int>() << std::endl;
	//std::cout << std::is_integral<const int>() << std::endl;

	//std::string* p;
 //   std::size_t sz;
 //   std::tie(p, sz) = std::get_temporary_buffer<std::string>(4);
 //
 //   TinySTL::uninitialized_fill(p, p+sz, "Example");
 //
 //   for (std::string* i = p; i != p+sz; ++i) {
 //       std::cout << *i << '\n';
 //       i->~basic_string<char>();
 //   }
 //   std::return_temporary_buffer(p);

 //   std::vector<std::string> v = {"This", "is", "an", "example"};
 //
 //   std::string* p;
 //   std::size_t sz;
 //   std::tie(p, sz) = std::get_temporary_buffer<std::string>(v.size());
 //   sz = std::min(sz, v.size());
 //
 //   TinySTL::uninitialized_copy_n(v.begin(), sz, p);
 //
 //   for (std::string* i = p; i != p+sz; ++i) {
 //       std::cout << *i << ' ';
 //       i->~basic_string<char>();
 //   }
 //   std::return_temporary_buffer(p);

	//std::vector<int> vec1{ 1,2,3,4,5 };

 //   if(void *pbuf = _aligned_malloc(sizeof(int) * vec1.size(), alignof(int)))
 //   {
 //       try
 //       {
 //           auto first = static_cast<int*>(pbuf);
 //           auto last = TinySTL::uninitialized_copy(std::begin(vec1), std::end(vec1), first);
 //
 //           for (auto it = first; it != last; ++it)
	//				std::cout << *it << '_';
 //           std::cout << '\n';
 //
 //           TinySTL::destroy(first, last);
 //       }
 //       catch(...) {}
 //      _aligned_free(pbuf);
 //   }

 //   const char *v[] = {"This", "is", "an", "example"};
 //   auto sz = std::size(v);
	//std::cout << typeid(std::begin(v)).name() << std::endl;
	//std::cout << *(std::begin(v)) << std::endl;
	//std::cout << *(std::end(v)-1) << std::endl;
	//std::cout << sizeof(std::string) * sz << std::endl;
	//std::cout << sizeof(v) << std::endl;
	//std::cout << sz << std::endl;

 //   if(void *pbuf = _aligned_malloc(sizeof(std::string) * sz, alignof(std::string)))
 //   {
 //       try
 //       {
 //           auto first = static_cast<std::string*>(pbuf);
 //           auto last = TinySTL::uninitialized_copy(std::begin(v), std::end(v), first);
 //
 //           for (auto it = first; it != last; ++it)
	//				std::cout << *it << '_';
 //           std::cout << '\n';
 //
 //           TinySTL::destroy(first, last);
 //       }
 //       catch(...) {}
 //      _aligned_free(pbuf);
 //   }

	//std::cout << *first << std::endl;
	//TinySTL::_destroy(first, last);

	//int a2 = 1;
	//int* const p2 = &a2;

	//std::cout << TinySTL::is_pointer_v<int* const> << std::endl;
	//std::cout << TinySTL::is_pointer_v<const int*> << std::endl;
	//std::cout << TinySTL::To_address(p2) << std::endl;

	//std::vector<int> vec5 = { 4,5,8,2,2,2,7,1,9,12,45,15,13,65,14,2,3,7,77,94,12,35,32,3,46,9,12,12,7,75,55,1,13,22,36,54,55,61,42,78,2,4,68,99,35 };
	//TinySTL::sort(vec5.begin(), vec5.end());
	//for (auto n : vec5)
	//	std::cout << n << ",";
	//std::cout << std::endl;
	//std::cout << vec5.size() << std::endl;

	//int a[8] = { 4,5,8,2,3,9,7,1 };
	//int* p1 = a;
	//int* p2 = p1 + 8;
	//std::cout << *p2 << std::endl;
	//TinySTL::make_heap(p1, p2);
	//for (int i = 0; i < 8; ++i)
	//	std::cout << a[i] << " ";
	//std::cout << std::endl;

	//TinySTL::sort_heap(p1, p2);
	//for (int i = 0; i < 8; ++i)
	//	std::cout << a[i] << " ";
	//std::cout << std::endl;

	//std::vector<int> vec1 = { 4,5,8,2,3,9,7,1 };
	//TinySTL::make_heap(vec1.begin(), vec1.end());
	//for (auto c : vec1)
	//	std::cout << c << " ";
	//std::cout << std::endl;
	//TinySTL::sort_heap(vec1.begin(), vec1.end());
	//for (auto c : vec1)
	//	std::cout << c << " ";
	//std::cout << std::endl;
	//std::vector<int> vec2 = { 1,2,3,4,5,7,8,9 };
	//TinySTL::sort_heap(vec.begin(), vec.end(), [](int a, int b) { return a < b; });
	//for (auto c : vec)
	//	std::cout << c << " ";
	//std::cout << std::endl;

	system("pause");
	return 0;
}