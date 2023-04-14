#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <cstdlib>

namespace TinySTL
{
	/*
	* 第二级空间分配器，以字节数为单位去分配内存空间
	* 供Allocator内部使用
	*/
	class alloc
	{
	private:
		// 只有类内const static 变量可以在类内初始化
		static const size_t __ALIGN = 8;                              // 小型区块的上调边界
		static const size_t __MAX_BYTES = 128;					      // 小型区块的上限，超过__MAX_BYTES的内存块申请，直接从操作系统new
		static const size_t __FREE_LIST_SIZE = __MAX_BYTES / __ALIGN; // free_lists的个数
		static const size_t __OBJ_NUM = 20;						      // 每次增加的节点数，即每个free_lists一次申请 20*当前负责字节数大小*2 的内存空间

	private:
		// 将申请新的内存块时计算大小的追加量(bytes >> 4)上调至8的倍数，即对齐8位
		static size_t ROUND_UP(size_t bytes)
		{
			return ((bytes)+__ALIGN - 1) & ~(__ALIGN - 1); // ~优先级最高
		}

	private:
		// free_lists节点的构造
		union obj
		{
			union obj* free_list_link; // obj可被视为一个指针，指向相同形式的另一个obj
			char client_data[1];       // obj可被视为一个指针，指向实际区块，关注的并不是client_data[1]里面的内容，而是client_data这个数组首地址
		};

		static obj* volatile free_list[__FREE_LIST_SIZE]; // 元素为obj*的数组，即16个free_list；free_list里都是volatile指针，指向obj

	private:
		// 偏移量，根据区块大小，决定使用第n号free_list，n从0开始
		static size_t FREELIST_INDEX(size_t bytes)
		{
			return ((bytes + __ALIGN - 1) / __ALIGN) - 1;
		}
		// 返回一个大小为bytes(bytes对齐8)的对象，并可能加入大小为bytes的其他区块到free_list
		static void* refill(size_t bytes); // void*：返回任意类型的指针
		// 配置一大块空间，可容纳nobjs个大小为size的区块
		// 如果配置nobjs个区块有所不便，nobjs可能会降低
		static char* chunk_alloc(size_t size, size_t& nobjs);

	public:
		static void* allocate(size_t bytes);                               // 内存空间分配
		static void deallocate(void* ptr, size_t bytes);                   // 内存空间的回收
		static void* reallocate(void* ptr, size_t old_sz, size_t new_sz);  // 将已经分配的空间大小重新分配为new_sz

	private:
		// 自建内存池
		// static：类内声明，类外初始化
		static char* start_free; // 预备pool起始位置
		static char* end_free;   // 预备pool结束位置
		static size_t heap_size; // 申请新的内存块之前已经占用的大小
	};

	// 再包装一个接口使配置器的接口能够符合STL规格
	template<class T, class Alloc>
	class simple_alloc
	{
	public:
		static T* allocate(size_t n) { return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T)); }
		static T* allocate(void) { return (T*)Alloc::allocate(sizeof(T)); }
		static void deallocate(T* p, size_t n) { if (0 != n) Alloc::deallocate(p, n * sizeof(T)); }
		static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }
	};

}

#endif
