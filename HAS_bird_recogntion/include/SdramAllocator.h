//
// Created by Ewout on 8-6-2023.
//

#ifndef HAS_BIRD_RECOGNTION_INCLUDE_SDRAMALLOCATOR_H_
#define HAS_BIRD_RECOGNTION_INCLUDE_SDRAMALLOCATOR_H_

#include <cstdlib>
#include <SDRAM.h>

template<typename T>
class SdramAllocator {
 public:
  using value_type = T;

  SdramAllocator() noexcept = default;

  template<typename U>
  explicit SdramAllocator(const SdramAllocator<U> &) noexcept {}

  T *allocate(std::size_t n) {
	if (n > std::size_t(-1) / sizeof(T)) {
	  throw std::bad_alloc();
	}
	if (auto p = static_cast<T *>(SDRAM.malloc(n * sizeof(T)))) {
	  return p;
	}
	throw std::bad_alloc();
  }

  void deallocate(T *p, std::size_t) noexcept {
	SDRAM.free(p);
  }
};

template<typename T>
class MyAllocator {
 public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using size_type = std::size_t;

  MyAllocator() noexcept {}
  template<typename U>
  MyAllocator(const MyAllocator<U> &) noexcept {}

  pointer allocate(size_type n) {
	return static_cast<pointer>(SDRAM.malloc(n * sizeof(T)));
  }

  void deallocate(pointer p, size_type n) noexcept {
	(void *)(n);
	SDRAM.free(p);
  }
};

#endif //HAS_BIRD_RECOGNTION_INCLUDE_SDRAMALLOCATOR_H_
