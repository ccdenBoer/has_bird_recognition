//
// Created by Ewout on 8-6-2023.
//
#pragma once
#ifndef HAS_BIRD_RECOGNTION_INCLUDE_SDRAMALLOCATOR_H_
#define HAS_BIRD_RECOGNTION_INCLUDE_SDRAMALLOCATOR_H_

#include <SDRAM.h>
#include <cstdlib> //size_t, malloc, free
#include <new> // bad_alloc, bad_array_new_length
#include <memory>

template <class T>
struct SDRAMAllocator
{
  typedef T value_type;
  SDRAMAllocator() noexcept = default; //default ctor not required by C++ Standard Library

  // A converting copy constructor:
  template<class U> SDRAMAllocator(const SDRAMAllocator<U>&) noexcept { // NOLINT(google-explicit-constructor)
	SDRAM.begin();
  }

  template<class U> bool operator==(const SDRAMAllocator<U>&) const noexcept
  {
	return true;
  }
  template<class U> bool operator!=(const SDRAMAllocator<U>&) const noexcept
  {
	return false;
  }
  T* allocate(size_t n) const;
  void deallocate(T* p, size_t) const noexcept;
};

template <class T>
T* SDRAMAllocator<T>::allocate(const size_t n) const
{
  if (n == 0)
  {
	return nullptr;
  }
  if (n > static_cast<size_t>(-1) / sizeof(T))
  {
	throw std::bad_array_new_length();
  }

  static int alignment = 16;
  void *original = SDRAM.malloc((n * sizeof(T))+alignment);
  if (!original) { throw std::bad_alloc(); }

  void *aligned = reinterpret_cast<void*>((reinterpret_cast<std::size_t>(original) & ~(std::size_t(alignment-1))) + alignment);
  *(reinterpret_cast<void**>(aligned) - 1) = original;

  return static_cast<T*>(aligned);
}

template<class T>
void SDRAMAllocator<T>::deallocate(T * const p, size_t) const noexcept
{
  SDRAM.free(*(reinterpret_cast<void**>(p) - 1));
}

#endif //HAS_BIRD_RECOGNTION_INCLUDE_SDRAMALLOCATOR_H_
