#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct ActionFileHeader
{
	uint32_t Signature; // match 0xC9F34F48
	uint8_t CompilerVersion[12];
	uint8_t CopyrightNotice[80];
	uint16_t field_60;
	uint16_t numBBs;
	uint16_t field_64;
	uint16_t field_66;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint16_t field_74;
	uint16_t field_76;
	uint32_t field_78;
	uint32_t List4Offset;
	uint32_t VariantTableOffset;
	uint32_t ExternTableOffset;
	uint32_t PrototypesTableOffset;
	uint32_t TypedefsTableOffset;
	uint32_t field_90;
	uint32_t DebugInfoOffset;
	uint8_t field_98[64];
	uint32_t BBsTableOffset;
	uint32_t field_DC;
	uint32_t field_E0;
	uint32_t field_E4;
	uint32_t field_E8;
	uint32_t field_EC;
	uint32_t field_F0;
	uint32_t field_F4;
	uint32_t field_F8;
	uint32_t field_FC;
};
static_assert(sizeof(ActionFileHeader) == 0x100, "sizeof ActionFileHeader MUST be 0x100 bytes");

#pragma pack(pop)
