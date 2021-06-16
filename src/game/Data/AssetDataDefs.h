#pragma once

#include <string>
#include "../MathLib.h"
#include "../Span.h"

enum class AnimationType : uint8_t
{
	Init,
	Death,
	GroundAttackInit,
	AirAttackInit,
	SpecialAbility1,
	GroundAttackRepeat,
	AirAttackRepeat,
	SpecialAbility2,
	GroundAttackToIdle,
	AirAttackToIdle,
	SpecialAbility3,
	Walking,
	Other,
	BurrowInit,
	ConstructHarvest,
	IsWorking,
	Landing,
	LiftOff,
	Burrow,
	UnBorrow,
};

#pragma pack(push,1)
struct AtlasDef {
	char name[32];
	uint16_t index;

	std::string GetAtlasName() const {
		return std::string(name) + "_" + std::to_string(index);
	}
};
struct ImageDef {
	char name[32];
	uint16_t frameStart;
	uint16_t frameCount;
	Vector2Int16 size;
	uint16_t colorMaskOffset;
};


struct ImageFrameDef {
	Vector2Int16 offset;
	Vector2Int16 size;
	Vector2Int16 atlasOffset;
	uint8_t atlasId;
};

struct AnimClipDef {
	uint16_t spriteId;
	uint32_t instructionStart;
	uint8_t instructionCount;
	AnimationType type;

	inline uint32_t InstructionEnd() const { return instructionStart + instructionCount; }
};

struct SpriteDef {
	char name[32];
	int16_t imageId;
	uint16_t animStart;
	uint8_t animCount;
	bool isRotating;

	const Span<AnimClipDef> GetClips() const;
};

union InstructionParams {
	uint8_t bytes[4];
	uint16_t shorts[2];
};

struct AnimInstructionDef {
	uint8_t id;
	InstructionParams param = {};
};

#pragma pack(pop)