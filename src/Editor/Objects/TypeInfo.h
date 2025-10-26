#pragma once

#include <string>
#include <vector>
#include <stdint.h>

#include "math/math3d.h"

struct EnumValue
{
	const char* name;
	uint8_t value;
};

using EnumInfo = std::vector<EnumValue>;

struct TypeInfoMember
{
	enum class Type
	{
		Bool,
		Integer,
		Float,
		Vec3,
		String,
		Angle,
		Orientation,
		Color,
		Model,
		Sound,
		Enum,
		ModelEnum
	};

	const char* name;
	Type type;
	const EnumInfo* enumInfo;
	size_t offset;
	size_t offset2;

	static constexpr size_t SizeOf(Type type)
	{
		switch (type)
		{
			case Type::Bool: return sizeof(bool);
			case Type::Integer: return sizeof(int32_t);
			case Type::Float: return sizeof(float);
			case Type::Vec3: return sizeof(vec3);
			case Type::String: return sizeof(std::string);
			case Type::Orientation: return sizeof(mat3);
			case Type::Color: return sizeof(vec3);
			case Type::Model: return sizeof(std::string);
			case Type::Sound: return sizeof(std::string);
			case Type::Enum: return sizeof(uint8_t);
			case Type::ModelEnum: return sizeof(uint8_t);

			default: return 1;
		}
	}
};

struct TypeInfo
{
	const char* name;
	const TypeInfo* parentTypeInfo;
	std::vector<TypeInfoMember> members;
};