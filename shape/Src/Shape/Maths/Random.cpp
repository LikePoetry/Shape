#include "hzpch.h"
#include "Random.h"

#define SUPPORT_RANDOM_DEVICE

namespace Shape
{
	Random32 Random32::Rand = Random32(Random32::RandSeed());
	Random64 Random64::Rand = Random64(Random64::RandSeed());

	uint32_t Random32::RandSeed()
	{
		std::random_device randDevice;
		return randDevice();
	}

	uint64_t Random64::RandSeed()
	{
		//#if SUPPORT_RANDOM_DEVICE
		std::random_device randDevice;
		uint64_t value = randDevice();
		return (value << 32) | randDevice();
		//#else
		//		return uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		//#endif
	}
}