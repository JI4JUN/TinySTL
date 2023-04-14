#include "pch.h"
#include "CppUnitTest.h"
#include "../TinySTL/Utility.h"
#include "../TinySTL/ReserverseIterator.h"
#include "../TinySTL/Algorithm.h"
#include "../TinySTL/UninitializedFunctions.h"
#include "../TinySTL/Deque.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			TinySTL::pair<int, int> pair1(3, 4);
			TinySTL::pair<int, int> pair2(5, 4);
			const wchar_t mess[] = L"pair1小于pair2";
			Assert::IsTrue(pair1 < pair2, mess);
		}

		TEST_METHOD(TestMethod2)
		{
			TinySTL::pair<int, int> pair1(3, 4);
			TinySTL::pair<int, int> pair2(3, 4);
			Assert::IsTrue(pair1 == pair2, L"pair1等于pair2");
		}

		TEST_METHOD(TestHeap)
		{
			// make_heap() sort_heap()
			std::string str1 = "54321";
			TinySTL::make_heap(str1.begin(), str1.end());
			TinySTL::sort_heap(str1.begin(), str1.end());
			std::string str2 = "12345";
			Assert::IsTrue(str1 == str2, L"排序错误");

			std::vector<int> vec1 = { 4,5,8,2,3,9,7,1 };
			TinySTL::make_heap(vec1.begin(), vec1.end());
			TinySTL::sort_heap(vec1.begin(), vec1.end());
			std::vector<int> vec2 = { 1,2,3,4,5,7,8,9 };
			Assert::IsTrue(vec1 == vec2, L"排序错误");

			// is_heap()
			std::vector<int> vec3 = { 4,5,8,2,3,9,7,1 };
			TinySTL::make_heap(vec3.begin(), vec3.end());
			Assert::IsTrue(TinySTL::is_heap(vec3.begin(), vec3.end()), L"vec1不是堆");
		}

		TEST_METHOD(TestSort)
		{
			std::vector<int> vec1 = { 4,5,8,2,3,9,7,1 };
			TinySTL::sort(vec1.begin(), vec1.end());
			std::vector<int> vec2 = { 1,2,3,4,5,7,8,9 };
			Assert::IsTrue(vec1 == vec2, L"插入排序错误");

			std::vector<int> vec3 = { 4,5,8,2,2,2,7,1 };
			TinySTL::sort(vec3.begin(), vec3.end());
			std::vector<int> vec4 = { 1,2,2,2,4,5,7,8 };
			Assert::IsTrue(vec3 == vec4, L"插入排序错误（存在相同）");

			// 45个元素
			std::vector<int> vec5 = { 4,5,8,2,2,2,7,1,9,12,45,15,13,65,14,2,3,7,77,94,12,35,32,3,46,9,12,12,7,75,55,1,13,22,36,54,55,61,42,78,2,4,68,99,35 };
			TinySTL::sort(vec5.begin(), vec5.end());
			std::vector<int> vec6 = { 1,1,2,2,2,2,2,3,3,4,4,5,7,7,7,8,9,9,12,12,12,12,13,13,14,15,22,32,35,35,36,42,45,46,54,55,55,61,65,68,75,77,78,94,99 };
			Assert::IsTrue(vec5 == vec6, L"快速排序错误(存在相同)");
		}

		TEST_METHOD(TestUninitializedFunctions)
		{
			/*
			* ********************************************************************
			* test uninitialized_copy()
			* ********************************************************************
			*/
			const char *v[] = {"This", "is", "an", "example"};
			auto sz = std::size(v);

			if(void *pbuf = _aligned_malloc(sizeof(std::string) * sz, alignof(std::string)))
			{
				try
				{
					auto first = static_cast<std::string*>(pbuf);
					auto last = TinySTL::uninitialized_copy(std::begin(v), std::end(v), first);
					
					std::vector<std::string> strVec{};
					for (auto it = first; it != last; ++it)
						strVec.emplace_back(*it);

					std::vector<std::string> verifiVec{"This", "is", "an", "example"};
					Assert::IsTrue(strVec == verifiVec, L"uninitialized_copy()错误");

					TinySTL::destroy(first, last);
				}
				catch(...) {}
			   _aligned_free(pbuf);
			}
			/*
			* ********************************************************************
			*/

			/*
			* ********************************************************************
			* test uninitialized_copy_n()
			* ********************************************************************
			*/
			std::vector<std::string> v2 = {"This", "is", "an", "example"};
 
			std::string* p;
			std::size_t sz2;
			std::tie(p, sz2) = std::get_temporary_buffer<std::string>(v2.size());
			sz2 = std::min(sz2, v2.size());
 
			TinySTL::uninitialized_copy_n(v2.begin(), sz2, p);
 
			std::vector<std::string> verifiVec2{};
			for (std::string* i = p; i != p+sz2; ++i) {
				verifiVec2.emplace_back(*i);
				i->~basic_string<char>();
			}

			Assert::IsTrue(v2 == verifiVec2, L"uninitialized_copy_n()错误");

			std::return_temporary_buffer(p);
			/*
			* ********************************************************************
			*/

			/*
			* ********************************************************************
			* test uninitialized_fill()
			* ********************************************************************
			*/
			std::string* p2;
			std::size_t sz3;
			std::tie(p2, sz3) = std::get_temporary_buffer<std::string>(4);
 
			TinySTL::uninitialized_fill(p2, p2+sz3, "Example");

			std::vector<std::string> strVec2{};
			for (std::string* i = p2; i != p2+sz3; ++i) {
				strVec2.emplace_back(*i);
				i->~basic_string<char>();
			}
			std::vector<std::string> verifiVec3{ "Example","Example","Example","Example" };
			Assert::IsTrue(strVec2 == verifiVec3, L"uninitialized_fill()错误");

			std::return_temporary_buffer(p2);
			/*
			* ********************************************************************
			*/

			/*
			* ********************************************************************
			* test uninitialized_fill_n()
			* ********************************************************************
			*/
			std::string* p3;
			std::size_t sz4;
			std::tie(p3, sz4) = std::get_temporary_buffer<std::string>(4);
			std::uninitialized_fill_n(p3, sz4, "Example");
 
			std::vector<std::string> strVec3{};
			for (std::string* i = p3; i != p3+sz4; ++i) {
				strVec3.emplace_back(*i);
				i->~basic_string<char>();
			}
			Assert::IsTrue(strVec3 == verifiVec3, L"uninitialized_fill_n()错误");

			std::return_temporary_buffer(p3);
		}

		TEST_METHOD(TestDeque)
		{
			/*
			* ********************************************************************
			* test Deque
			* ********************************************************************
			*/
			int arr1[] = { 1, 2, 3, 4 };
			int arr2[] = { 1, 2, 3, 4 };
			TinySTL::deque<int> d1(std::begin(arr1), std::end(arr1));
			TinySTL::deque<int> d2(std::begin(arr2), std::end(arr2));
			Assert::IsTrue(d1 == d2, L"Deque错误");
		}
	};
}
