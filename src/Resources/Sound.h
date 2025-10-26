#pragma once

#include "Resources/Resource.h"
#include <stdint.h>
#include <vector>

struct Sound : public Resource
{
	std::vector<int16_t> samples;

	uint32_t length() const { return samples.size(); }
	uint32_t bytes() const { return samples.size() * 2; }
	const uint8_t* data() const { return reinterpret_cast<const uint8_t*>(samples.data()); }
};