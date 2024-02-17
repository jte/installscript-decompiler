#pragma once

#include <cstdint>

enum class HeaderKind
{
	Unrecognized,
	OBS,
	aLuZ,
	kUtZ,
	OBL
};

#pragma pack(push, 1)
struct ActionFileHeaderOBS
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
	uint16_t numGlobalVarNumbers;
	uint16_t field_76;
	uint32_t field_78;
	uint32_t StringTableOffset;
	uint32_t VariantTableOffset;
	uint32_t ExternTableOffset;
	uint32_t PrototypesTableOffset;
	uint32_t TypedefsTableOffset;
	uint32_t AddressResolveTableOffset;
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
static_assert(sizeof(ActionFileHeaderOBS) == 0x100, "sizeof ActionFileHeaderOBS MUST be 0x100 bytes");

struct ActionFileHeaderaLuZ
{
	uint32_t Signature; // match "aLuZ"
	uint16_t _unknown1;
	uint8_t CopyrightNotice[80];
	uint16_t _unknown2;
	uint32_t _unknown3;
	uint32_t _unknown4;
	uint32_t _unknown5;
	uint32_t _unknown6;
	uint32_t VariablesTableOffset;
	uint32_t PrototypesTableOffset;
	uint32_t TypedefsTableOffset;
	uint32_t BBsTableOffset;
	uint32_t DebugInfoOffset;
};
static_assert(sizeof(ActionFileHeaderaLuZ) == 124, "sizeof ActionFileHeaderaLuZ MUST be 0x100 bytes");


#pragma pack(pop)
