#include "IScript.h"
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include "Decompiler.h"
#include <argparse/argparse.hpp>

const std::vector<uint8_t> readFile(std::string filePath)
{
	std::ifstream in(filePath, std::ifstream::binary);
	if (!in)
	{
		char errMsg[256] = { 0 };
		strerror_s(errMsg, errno);
		throw std::runtime_error("Failed to open the action file: " + std::string(errMsg));
	}

	std::filebuf* pbuf = in.rdbuf();

	std::size_t size = static_cast<std::size_t>(pbuf->pubseekoff(0, in.end, in.in));
	pbuf->pubseekpos(0, in.in);

	std::vector<uint8_t> buffer;

	buffer.resize(size);

	pbuf->sgetn(reinterpret_cast<char*>(&buffer.at(0)), size);

	return buffer;
}

int main(int argc, char** argv)
{
	//try
	//{
		std::vector<uint8_t> contents = readFile("output.obs");
		HeaderKind hdrKind = HeaderKind::Unrecognized;

		if (contents[0] == 0x48 && contents[1] == 0x4F && contents[2] == 0xF3 && contents[3] == 0xC9)
		{ // .obs file
			hdrKind = HeaderKind::OBS;
		}
		else 
		{ // encrypted or invalid
			uint32_t seed = 0;
			CIScript::DecryptBuffer(&seed, contents, 0xF1);

			if (contents[0] == 'a' && contents[1] == 'L' && contents[2] == 'u' && contents[3] == 'Z')
			{// script file
				hdrKind = HeaderKind::aLuZ;
			}
			else if (contents[0] == 'k' && contents[1] == 'U' && contents[2] == 't' && contents[3] == 'Z')
			{ // script file with debug info
				hdrKind = HeaderKind::kUtZ;
			}
			else
			{
				hdrKind = HeaderKind::Unrecognized;
			}
		}
		

		CIScript script(contents, hdrKind);
		CDecompiler decompiler(script);

		//std::cout << script;
		std::cout << decompiler;

	//}
	//catch (std::exception& e)
	//{
	//	std::cerr << "Exception: " << e.what() << std::endl;
	//	return 1;
	//}
	return 0;
}