#include "LibFile.h"

LibFile::LibFile()
{

}

bool LibFile::Parse(StreamPtr& filePtr)
{
	std::vector<uint8_t> Signature;
	filePtr.ReadArray(Signature, 4);
	if (!(Signature[0] == 'p' && Signature[1] == 'O' && Signature[2] == 'd' && Signature[3] == 'A'))
	{
		return false;
	}

	uint32_t unknown = 0;
	filePtr.Read(unknown);
	if (unknown != 1)
	{
		return false;
	}

	uint32_t numScripts = 0;
	filePtr.Read(numScripts);

	for (size_t i = 0; i < numScripts; ++i)
	{
		ScriptInfo s;
		s.name = filePtr.ReadInsString();
		filePtr.Read(s.offset);
		filePtr.Read(s.length);
		m_scripts.push_back(s);
	}

	return true;
}

std::vector<std::vector<uint8_t>> LibFile::GetScriptsContent(StreamPtr& filePtr)
{
	std::vector<std::vector<uint8_t>> output;
	for (auto sc : m_scripts)
	{
		StreamPtr scptr(filePtr);
		scptr.JumpTo(sc.offset);
		std::vector<uint8_t> scout;
		scptr.ReadArray(scout, sc.length);
		output.push_back(scout);
	}
	return output;
}