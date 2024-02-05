#include "LibFile.h"

LibFile::LibFile(StreamPtr& filePtr)
{
	uint32_t Signature = 0;
	filePtr.Read(Signature);
	if (Signature != 'AdOp')
	{
		throw std::runtime_error("Invalid signature for library file. Expected 'AdOp'");
	}

	uint32_t unknown = 0;
	filePtr.Read(unknown);
	if (unknown != 1)
	{
		throw std::runtime_error("Library header member 'unknown' is not set to value 1.");
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
}