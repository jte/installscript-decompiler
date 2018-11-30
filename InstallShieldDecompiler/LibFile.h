#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "StreamPtr.h"

/**
struct LibFileHeader
{
	uint32_t Signature; // match 'AdOp'
	uint32_t unknown; // match 1
	uint32_t numFiles;
	struct ScriptInfo
	{
		InsString name;
		uint32_t offset;
		uint32_t length;
	} libs[numFiles];
};
*/

class LibFile
{
private:
	struct ScriptInfo
	{
		std::string name;
		uint32_t offset;
		uint32_t length;
	};
	std::vector<ScriptInfo> m_scripts;
public:
	LibFile(StreamPtr& filePtr);
};