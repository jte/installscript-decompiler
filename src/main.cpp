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
		throw std::runtime_error("Failed to open the action file: " + std::string(strerror(errno)));
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
	try
	{
		const std::vector<uint8_t> contents = readFile("output.obs");
		
		
		CIScript script(contents);
		CDecompiler decompiler(script);


		//std::cout << script.GetStruct(0);
		//std::cout << script.GetStruct(1);
		//script.PrintPrototypes();
		//std::cout << script;
		std::cout << decompiler;
		std::cout << std::endl;
		char dummy;
		std::cin >> dummy;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}