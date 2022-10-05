#pragma once

namespace UE4
{
	enum class EFortCustomPartType : uint8_t
	{
		Head = 0,
		Body = 1,
		Hat = 2,
		Backpack = 3,
		Charm = 4,
		Face = 5,
		NumTypes = 6,
		EFortCustomPartType_MAX = 7
	};

	enum class ESpawnActorCollisionHandlingMethod : uint8_t
	{
		Undefined = 0,
		AlwaysSpawn = 1,
		AdjustIfPossibleButAlwaysSpawn = 2,
		AdjustIfPossibleButDontSpawnIfColliding = 3,
		DontSpawnIfColliding = 4,
		ESpawnActorCollisionHandlingMethod_MAX = 5
	};
}