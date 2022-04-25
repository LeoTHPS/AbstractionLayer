#pragma once
#include "AL/Common.hpp"

#if __has_feature(__cpp_rtti)
	#define AL_FEATURE_RTTI
#endif

#if __has_feature(__cpp_concepts)
	#define AL_FEATURE_CONCEPTS
#endif
