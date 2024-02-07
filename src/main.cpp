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
#include "Wrapper/LibFile.h"
#include "StreamPtr.h"

const std::vector<uint8_t> readFile(std::string filePath)
{
	std::ifstream in(filePath, std::ifstream::binary);
	if (!in)
	{
		throw std::runtime_error("Failed to open the input file: " + std::string(strerror(errno)));
	}

	std::filebuf* pbuf = in.rdbuf();

	std::size_t size = static_cast<std::size_t>(pbuf->pubseekoff(0, in.end, in.in));
	pbuf->pubseekpos(0, in.in);

	std::vector<uint8_t> buffer;

	buffer.resize(size);

	pbuf->sgetn(reinterpret_cast<char*>(&buffer.at(0)), size);

	return buffer;
}

HeaderKind GetHeaderKind(std::vector<uint8_t>& contents)
{
	if (contents[0] == 0x48 && contents[1] == 0x4F && contents[2] == 0xF3 && contents[3] == 0xC9)
	{ // .obs file
		return HeaderKind::OBS;
	}
	else if (contents[0] == 'p' && contents[1] == 'O' && contents[2] == 'd' && contents[3] == 'A')
	{
		return HeaderKind::OBL;
	}
	else if (contents[0] == 'a' && contents[1] == 'L' && contents[2] == 'u' && contents[3] == 'Z')
	{
		return HeaderKind::aLuZ;
	}
	else
	{ // encrypted or invalid
		uint32_t seed = 0;
		CIScript::DecryptBuffer(&seed, contents, 0xF1);

		if (contents[0] == 'a' && contents[1] == 'L' && contents[2] == 'u' && contents[3] == 'Z')
		{// script file
			return HeaderKind::aLuZ;
		}
		else if (contents[0] == 'k' && contents[1] == 'U' && contents[2] == 't' && contents[3] == 'Z')
		{ // script file with debug info
			return HeaderKind::kUtZ;
		}
		else
		{
			return HeaderKind::Unrecognized;
		}
	}
}

HeaderKind AnalyzeCompiledFile(const std::string filename, std::vector<uint8_t>& contents)
{
	contents = readFile(filename);

	return GetHeaderKind(contents);
}

int main(int argc, char** argv)
{
	argparse::ArgumentParser program("InstallScriptDecompiler");

	program.add_argument("input_file")
		.help("input file (.inx/.obs/.obl)");

	program.add_argument("output_file")
		.help("output file (.rul)");

	auto& group = program.add_mutually_exclusive_group(true);
	group.add_argument("--show-actions")
		.help("show actions (disassembly)")
		.default_value(false)
		.implicit_value(true);
	group.add_argument("--show-decompiled")
		.help("show decompilation")
		.default_value(false)
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

		if (hdrKind == HeaderKind::OBL)
		{
			LibFile libFile;
			StreamPtr filePtr(contents);
			libFile.Parse(filePtr);
			auto files = libFile.GetScriptsContent(filePtr);
			std::ofstream of(program.get("output_file"), std::ifstream::binary);
			for (auto file : files)
			{
				CIScript script(file, GetHeaderKind(file));

				// TODO: make output_file optional and output files to their respective output files by name (but replace extension to be .rul)
				
				if (program["--show-decompiled"] == true)
				{
					CDecompiler decompiler(script);
					of << decompiler;
				}
				else if (program["--show-actions"] == true)
				{
					of << script;
				}
			}
			of.close();
		}
		else
		{
			CIScript script(contents, hdrKind);

			std::ofstream of(program.get("output_file"), std::ifstream::binary);

			if (program["--show-decompiled"] == true)
			{
				CDecompiler decompiler(script);
				of << decompiler;
			}
			else if (program["--show-actions"] == true)
			{
				of << script;
			}
			of.close();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}