#pragma once
#include "AL/Common.hpp"

namespace AL
{
	template<int S_LONG>
	struct _int32_;
	template<>
	struct _int32_<4>
	{
		typedef signed long Type;
	};
	template<>
	struct _int32_<8>
	{
		typedef signed int Type;
	};
	
	template<int S_LONG>
	struct _uint32_;
	template<>
	struct _uint32_<4>
	{
		typedef unsigned long Type;
	};
	template<>
	struct _uint32_<8>
	{
		typedef unsigned int Type;
	};

	typedef signed char                           int8;
	typedef unsigned char                         uint8;

	typedef signed short                          int16;
	typedef unsigned short                        uint16;

	typedef typename _int32_<sizeof(long)>::Type  int32;
	typedef typename _uint32_<sizeof(long)>::Type uint32;

	typedef signed long long                      int64;
	typedef unsigned long long                    uint64;

#if defined(AL_X86) || defined(AL_ARM)
	typedef int32                                 ssize_t;
	typedef uint32                                size_t;
#elif defined(AL_X86_64) || defined(AL_ARM64)
	typedef int64                                 ssize_t;
	typedef uint64                                size_t;
#endif

	typedef void                                  Void;

	typedef bool                                  Bool;
	constexpr Bool                                True = true;
	constexpr Bool                                False = false;

	typedef float                                 Float;
	typedef double                                Double;
}
