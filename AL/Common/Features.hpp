#pragma once
#include "AL/Common.hpp"

#if AL_HAS_FEATURE(__cpp_rtti)
	#define AL_FEATURE_RTTI
#endif

#if AL_HAS_FEATURE(__cpp_concepts)
	#define AL_FEATURE_CONCEPTS
#endif
