#ifndef PTR_WRAPPER_H
#define PTR_WRAPPER_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <queue>
#include <cstdarg>
#include <vector>
#include <list>
#include <stdexcept>
#include <map>
#include <stdint.h>
#include <pthread.h>

#include "api.h"

// ******************************************************************************************
//   C hacks
// ******************************************************************************************

#ifdef UNSAFE_MODE
	#define restrict

	#undef UINT64_C
	#undef UINT32_C

	#define UINT64_C(A) (A)
	#define UINT32_C(A) (A)
#endif

// ******************************************************************************************
//   wrappers
// *************i*****************************************************************************

#define DIM1(A)           (size_t)(A)
#define DIM2(A,B)         DIM1(A), (size_t)(B)
#define DIM3(A,B,C)       DIM2(A,B), (size_t)(C)
#define DIM4(A,B,C,D)     DIM3(A,B,C), (size_t)(D)
#define DIM5(A,B,C,D,E)   DIM4(A,B,C,D), (size_t)(E)
#define DIM6(A,B,C,D,E,F) DIM5(A,B,C,D,E), (size_t)(F)

namespace locality {

template<typename T, size_t N>
class PtrWrapper;

/**
	base class for ptr wrapper
	contains some common functions
	pointer \T = type, \N = arity
	tracks the base pointer and name, even if the pointer moves
	tracks the unified index,
	assuming that multidim arrays are placed in memory sequentially

	\type is NOT \T
*/
template<typename T, size_t N>
class PtrWrapperBase
{
protected:
	T* ptr;
	std::string name;
	std::vector<size_t> size;
	size_t offset;

	size_t global_array_offset;

	PtrWrapperBase(T* _ptr, const std::string& _name, std::vector<size_t> _size
		, size_t _global_array_offset, size_t _offset):
		ptr(_ptr),
		name(_name),
		size(_size),
		offset(_offset),
		global_array_offset(_global_array_offset)
	{}

	PtrWrapperBase(std::string _name):
		ptr(NULL),
		name(_name),
		size(0),
		offset(0),
		global_array_offset(0)
	{}

#ifdef UNSAFE_MODE
public:
#endif

	PtrWrapperBase():
		ptr(NULL),
		name(""),
		offset(0),
		global_array_offset(0)
	{}

public:
	/*operator T* () const
	{
		return ptr + offset;
	}*/

	std::string GetName()
	{
		return name;
	}

	bool operator == (const PtrWrapperBase<T, N>& cmp)
	{
		return ptr == cmp.ptr;
	}


/**
is used to carry plain pointers for some retarded C code
*/
	PtrWrapper<T, 1> plain()
	{
		return PtrWrapper<T, 1> (ptr, name, -1, global_array_offset, offset);
	}

	PtrWrapper<T, N> operator + (size_t val) const
	{
		return PtrWrapper<T, N>(ptr, name, size, global_array_offset, offset + val);
	}

	PtrWrapper<T, N> operator - (size_t val) const
	{
		return PtrWrapper<T, N>(ptr, name, size, global_array_offset, offset - val);
	}

	PtrWrapperBase<T, N> operator ++ ()
	{
		offset++;
		return *this;
	}

	PtrWrapperBase<T, N> operator -- ()
	{
		offset--;
		return *this;
	}

	PtrWrapperBase<T, N> operator ++ (int)
	{
		offset++;
		return *this - 1;
	}

	PtrWrapperBase<T, N> operator -- (int)
	{
		offset--;
		return *this + 1;
	}

	void Delete()
	{
		delete[] ptr;
		ptr = NULL;
	}
};

// ******************************************************************************************

/**
	temlpate for N-dim pointer wrapper around PLAIN N-dim c arrays
	it is stored in linear memory, just keeps size for easier addressing
*/
template<typename T, size_t N>
class PtrWrapper : public PtrWrapperBase<T, N>
{
#ifdef UNSAFE_MODE
public:
#endif
	PtrWrapper()
	{}


protected:
	static std::vector<size_t> Form(int n, va_list vl)
	{
		std::vector<size_t> dims;

		for(int i = 0; i < n; i++)
		{
			size_t val = va_arg(vl, size_t);
			dims.push_back(val);
		}

		return dims;
	}

public:

/**
	form new array from exisiting ptr, dims will be taken from va_list
	dims count must be N-1, ignore 1st dim
*/
/*	PtrWrapper(T* _ptr, const std::string& _name, size_t _global_array_offset, ...):
		PtrWrapperBase<T, N>(_ptr, _name, std::vector<size_t>(), _global_array_offset, 0)
	{
		va_list vl;
		va_start(vl, _global_array_offset);
		
		this->size = Form(N, vl);

		va_end(vl);
	}
*/
	PtrWrapper(T* _ptr, const std::string& _name, std::vector<size_t> _size, size_t _global_array_offset, size_t _offset):
		PtrWrapperBase<T, N>(_ptr, _name, _size, _global_array_offset, _offset)
	{}

/**
	allocate new array dims will be taken from va_list
	dims count must be N with type of size_t
*/
	PtrWrapper(std::string _name, ...):
		PtrWrapperBase<T, N>(_name)
	{
		va_list vl;
		va_start(vl, _name);

		this->size = Form(N, vl);
		
		va_end(vl);
		
		int mult = 1;

		for(size_t i = 0; i < N; i++)
			mult *= this->size[i];

		this->ptr = new T[mult];

		locality::profiler::NotifyAllocation(mult * sizeof(T));

		std::cout << "allocated " << (mult * sizeof(T)) << " bytes for " << _name << std::endl;

		this->global_array_offset = utils::NextGlobalOffset(mult);
		std::cout << "global_array_offset " << this->global_array_offset << std::endl;
	}

/*	PtrWrapper<T, N>& operator = (const PtrWrapper<T, N>& new_ptr)
	{
		std::string new_name = new_ptr.name;

		this->ptr = new_ptr.ptr;
		this->offset = new_ptr.offset;
		this->size = new_ptr.size;
		this->global_array_offset = new_ptr.global_array_offset;
		
		this->name = new_name;

		return *this;
	}*/

	const PtrWrapper<T, N-1> operator[](size_t n) const
	{
		int mult = 1;

		for(size_t i = 1; i < N; i++)
			mult *= this->size[i];

		std::vector<size_t> new_size(this->size.begin() + 1, this->size.end());

		return PtrWrapper<T, N-1>(this->ptr, this->name, new_size
			, this->global_array_offset, this->offset + mult * n);
	}

//	T operator[](int n) const
//	{
//		return this->ptr[n + this->offset];
//	}
};

// ******************************************************************************************

/**
	specified template for 1-dim array
	writes all requests in the file 'arrays/<<var_name>>'
*/
template<typename T>
class PtrWrapper<T, 1> : public PtrWrapperBase<T, 1>
{
#ifdef UNSAFE_MODE
public:
#endif
	PtrWrapper()
	{}

	
public:
	PtrWrapper(T* _ptr, const std::string& _name, std::vector<size_t> _size, size_t _global_array_offset, size_t _offset):
		PtrWrapperBase<T, 1>(_ptr, _name, _size, _global_array_offset, _offset)
	{}

	PtrWrapper(T* _ptr, const std::string& _name, size_t _size, size_t _global_array_offset, size_t _offset):
		PtrWrapperBase<T, 1>(_ptr, _name, std::vector<size_t>(1), _global_array_offset, _offset)
	{
		this->size[0] = _size;
	}

	PtrWrapper(std::string _name, size_t _size):
		PtrWrapperBase<T, 1>(new T[_size], _name, std::vector<size_t>(1), 0, 0)
	{
		this->size[0] = _size;
		
		locality::profiler::NotifyAllocation(_size * sizeof(T));

		std::cout << "allocated " << _size * sizeof(T) << " for " << _name << std::endl;
		std::cout << _name << "  ::  " << this->ptr << std::endl;

		this->global_array_offset = utils::NextGlobalOffset(_size);
		std::cout << "global_array_offset : " << this->global_array_offset << std::endl;
	}

/*	PtrWrapper<T, 1>& operator = (const PtrWrapper<T, 1>& new_ptr)
	{
		std::string new_name = new_ptr.name;

		this->name = new_name;
		this->ptr  = new_ptr.ptr;
		this->offset  = new_ptr.offset;

		return *this;
	}*/

	/*T operator *() const
	{
		return this->ptr[this->offset];
	}*/

/*	PtrWrapper<T, 1>& operator = (T* ptr)
	{
		this->ptr = ptr;
	}*/

	T* GetPtr() const
	{
		return this->ptr + this->offset;
	}

/*	T& operator[](int n)
	{
		size_t index  = n + this->offset;

		LocGetLogger->AddVal(this->name, index, this->ptr + index
			, this->global_array_offset, sizeof(T));

		return this->ptr[index];
	}*/

	T& operator[](size_t n) const
	{
		if(n >= this->size[0])
			throw std::out_of_range(this->name + " size: " + utils::ToString(this->size[0]) + " index: " + utils::ToString(n));

		size_t index = n + this->offset;

		locality::profiler::NotifyAccess(this->name, index, this->ptr + index
			, this->global_array_offset, sizeof(T));

		return this->ptr[index];
	}

	PtrWrapper<T, 2> To2D(size_t x)
	{
		return PtrWrapper<T, 2>(this->ptr, this->name, this->global_array_offset, x);
	}

	PtrWrapper<T, 3> To3D(size_t x, size_t y)
	{
		return PtrWrapper<T, 3>(this->ptr, this->name, this->global_array_offset, x, y);
	}

	PtrWrapper<T, 4> To2D(size_t x, size_t y, size_t z)
	{
		return PtrWrapper<T, 4>(this->ptr, this->name, this->global_array_offset, x, y, z);
	}

};

} /*namepace*/

#endif /*PTR_WRAPPER_H*/
