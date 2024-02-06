#include "IScript.h"
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include "Decompiler.h"
#include <argparse/argparse.hpp>
#include <string.h>

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

HeaderKind AnalyzeCompiledFile(const std::string filename, std::vector<uint8_t>& output)
{
	output = readFile(filename);
	HeaderKind hdrKind = HeaderKind::Unrecognized;

	if (output[0] == 0x48 && output[1] == 0x4F && output[2] == 0xF3 && output[3] == 0xC9)
	{ // .obs file
		return HeaderKind::OBS;
	}
	else
	{ // encrypted or invalid
		uint32_t seed = 0;
		CIScript::DecryptBuffer(&seed, output, 0xF1);

		if (output[0] == 'a' && output[1] == 'L' && output[2] == 'u' && output[3] == 'Z')
		{// script file
			return HeaderKind::aLuZ;
		}
		else if (output[0] == 'k' && output[1] == 'U' && output[2] == 't' && output[3] == 'Z')
		{ // script file with debug info
			return HeaderKind::kUtZ;
		}
		else
		{
			return HeaderKind::Unrecognized;
		}
	}
}

int main(int argc, char** argv)
{
	argparse::ArgumentParser program("InstallScriptDecompiler");

	program.add_argument("input_file")
		.help("input file (.inx/.obs)");

	auto& group = program.add_mutually_exclusive_group();
	group.add_argument("--show-actions")
		.default_value(false)
		.help("show actions (disassembly)")
		.implicit_value(true);
	group.add_argument("--show-decompiled")
		.default_value(true)
		.help("show decompilation")
		.implicit_value(true);

	try 
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
		std::cerr << program;
		std::cerr << std::endl;
		std::cerr << "Error: " << err.what() << std::endl;
		return 1;
	}
	
	try
	{
		std::vector<uint8_t> contents;
		HeaderKind hdrKind = AnalyzeCompiledFile(program.get<std::string>("input_file"), contents);

		CIScript script(contents, hdrKind);

		if (program["--show-decompiled"] == true)
		{
			CDecompiler decompiler(script);
			std::cout << decompiler;
		}
		else if (program["--show-actions"] == true)
		{
			std::cout << script;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}