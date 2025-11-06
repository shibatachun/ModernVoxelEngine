#include <vma/vk_mem_alloc.h>
template<class T>
constexpr const T& raptor_min(const T& a, const T& b) {
	return (a < b) ? a : b;
}

template<class T>
constexpr const T& raptor_max(const T& a, const T& b) {
	return (a < b) ? b : a;
}

#define VMA_MAX raptor_max
#define VMA_MIN raptor_min
#define VMA_USE_STL_CONTAINERS 0
#define VMA_USE_STL_VECTOR 0
#define VMA_USE_STL_UNORDERED_MAP 0
#define VMA_USE_STL_LIST 0

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
