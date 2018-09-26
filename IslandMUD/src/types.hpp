#pragma once

#include <stdint.h>

namespace std
{
	template<typename T>
	using observer_ptr = T * ;
}

using character_id = size_t;
