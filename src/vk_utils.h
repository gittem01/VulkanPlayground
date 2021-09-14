#pragma once

#include "vk_types.h"
#include <string>

namespace vkutil {
	bool load_image(void* engine, std::string fileName, AllocatedImage& outImage);
}
