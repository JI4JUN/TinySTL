#include "Alloc.h"

namespace TinySTL
{
	char* alloc::start_free = 0;
	char* alloc::end_free = 0;
	size_t alloc::heap_size = 0;

	alloc::obj* volatile alloc::free_list[alloc::__FREE_LIST_SIZE] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	void* alloc::allocate(size_t bytes) // bytes必须大于0
	{
		if (bytes > alloc::__MAX_BYTES) return malloc(bytes); // 返回值是一个指针，指向一段可用内存的起始位置

		//size_t index = FREELIST_INDEX(bytes);
		//obj* list = free_list[index];

		obj* volatile* my_free_list = nullptr; // 原来是obj** my_free_list
		obj* result = nullptr;

		my_free_list = free_list + FREELIST_INDEX(bytes);
		result = *my_free_list;

		// 如果没有合适大小的空间，需要从内存池中取出空间
		if (result == nullptr) return refill(ROUND_UP(bytes));

		// 如果list不为nullptr，即list还有空间，则从free_list中取出合适大小的区块
		//free_list[index] = list->free_list_link;
		*my_free_list = result->free_list_link;

		return result;
	}

	void alloc::deallocate(void* ptr, size_t bytes)
	{
		if (bytes > alloc::__MAX_BYTES)
			free(ptr);
		else
		{
			obj* node = static_cast<obj*>(ptr); // static_cast:(低风险转换)*空指针转换为任何目标类型的指针*
			obj* volatile* my_free_list = nullptr;
			my_free_list = free_list + FREELIST_INDEX(bytes);
			node->free_list_link = *my_free_list;
			*my_free_list = node;
		}
	}

	void* alloc::reallocate(void* ptr, size_t old_sz, size_t new_sz)
	{
		deallocate(ptr, old_sz);
		ptr = allocate(new_sz);

		return ptr;
	}

	void* alloc::refill(size_t bytes)
	{
		size_t nobjs = alloc::__OBJ_NUM; // 预设20个区块，但不一定够
		char* chunk = chunk_alloc(bytes, nobjs); // nobjs为引用传递
		obj* volatile* my_free_list = nullptr;
		obj* result = nullptr;
		obj* current_obj = nullptr;
		obj* next_obj = nullptr;

		if (1 == nobjs)	return chunk; // 实际取出的空间只够一个对象使用
		// 以下开始将所得区块挂上free_list，即在chunk内创建free_list
		my_free_list = free_list + FREELIST_INDEX(bytes);
		// 保存区块头
		result = (obj*)chunk;
		*my_free_list = next_obj = (obj*)(chunk + bytes); // chunk是char*，实际上+bytes就是越过了一个块，指向了第二个块的起始地址
		for (int i = 1; ; ++i) // 从i=1开始是因为第一块已经分出去，没必要重复切割过程
		{
			current_obj = next_obj;
			next_obj = (obj*)((char*)next_obj + bytes); // 所谓切割就是把指针所指处转型为obj，然后用next_obj指向它
			if (nobjs - 1 == i) // 最后一个
			{
				current_obj->free_list_link = nullptr;
				break;
			}
			else
				current_obj->free_list_link = next_obj;
		}

		return result;
	}

	char* alloc::chunk_alloc(size_t size, size_t& nobjs)
	{
		char* result = nullptr;
		size_t total_bytes = size * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes) // 预备pool空间足以满足20块的需求
		{
			result = start_free;
			start_free += total_bytes; // 调整(降低)预备pool水位

			return result;
		}
		else if (bytes_left >= size)   // 预备pool空间只足以满足一块(含)以上的需求
		{
			nobjs = bytes_left / size;
			total_bytes = size * nobjs;
			result = start_free;
			start_free += total_bytes;

			return result;
		}
		else                            // 预备pool空间不足以满足一块需求
		{
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4); // 2倍是因为有一半作为预备pool
			if (bytes_left > 0) // 如果预备pool还有空间
			{
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left); // 定位出应转移至第几号free_list
				((obj*)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj*)start_free;
			}
			start_free = (char*)malloc(bytes_to_get);
			if (start_free == nullptr) // 如果malloc失败
			{
				obj* volatile* my_free_list = nullptr;
				obj* p = nullptr;

				for (int i = size + alloc::__ALIGN; i <= alloc::__MAX_BYTES; i += alloc::__ALIGN)
				{
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != nullptr) // 如果该free_list有可用区块，则只给出一块给预备pool
					{
						*my_free_list = p->free_list_link;
						start_free = (char*)p;
						end_free = start_free + i;

						return chunk_alloc(size, nobjs);
					}
				}
				// 至此，意味着山穷水尽
				end_free = nullptr;
				// start_free = (char*)malloc_alloc::allocate(bytes_to_get);
			}
			// 至此，表示已成功取得很多memory
			heap_size += bytes_to_get;            // 累计总分配额
			end_free = start_free + bytes_to_get; // 调整尾端

			return chunk_alloc(size, nobjs);
		}
	}
}