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
#include "HeaderKind.h"
#include "Frontend.h"
#include "InstallShield_Old/IScript.h"
#include "InstallShield_New/IScript.h"

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

void DecryptBuffer(uint32_t* seed, std::vector<uint8_t>& buffer, uint8_t key)
{
	for (size_t i = 0; i < buffer.size(); ++i)
	{
		uint8_t v7 = ~key ^ ~buffer[i];
		uint8_t v8 = v7 >> 1;
		if (v7 & 1)
			v8 |= 0x80;
		uint8_t v9 = v8 >> 1;
		if (v8 & 1)
			v9 |= 0x80;
		buffer[i] = v9 - (i + *seed) % 0x47;
	}
	*seed += (uint32_t)buffer.size();
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
	else if (contents[0] == 0xb8 && contents[1] == 0xc9 && contents[2] == 0x0c && contents[3] == 0x00)
	{
		return HeaderKind::INS;
	}
	else
	{ // encrypted or invalid
		uint32_t seed = 0;
		DecryptBuffer(&seed, contents, 0xF1);

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

void ProcessFile(std::ofstream& of, const std::vector<uint8_t>& file, HeaderKind hdrKind, bool showDecompiled, bool showActions, const std::string cfgFile)
{
	CFrontend* frontend = nullptr; 
	
	if (hdrKind == HeaderKind::INS)
	{
		frontend = new oldis::CIScript(file, hdrKind);
	}
	else
	{
		frontend = new newis::CIScript(file, hdrKind);
	}

	if (showDecompiled)
	{
		CDecompiler decompiler(frontend, cfgFile);
		of << decompiler;
	}
	else if (showActions)
	{
		of << *frontend;
	}
}

int main(int argc, char** argv)
{
	argparse::ArgumentParser program("InstallScriptDecompiler");

	program.add_argument("input_file")
		.help("input file (.inx/.obs/.obl)");

	program.add_argument("output_file")
		.help("output file (.rul)");

	program.add_argument("--output-cfg-file")
		.help("outputs control flow graph in .dot format to specified file")
		.default_value("");

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
		std::cout << "Starting decompilation..." << std::endl;

		std::vector<uint8_t> contents;
		HeaderKind hdrKind = AnalyzeCompiledFile(program.get<std::string>("input_file"), contents);

		switch (hdrKind)
		{
		case HeaderKind::OBL:
			{
				LibFile libFile;
				StreamPtr filePtr(contents);
				libFile.Parse(filePtr);
				auto files = libFile.GetScriptsContent(filePtr);
				std::ofstream of(program.get("output_file"), std::ifstream::binary);
				for (auto file : files)
				{
					// TODO: make output_file optional and output files to their respective output files by name (but replace extension to be .rul)
					ProcessFile(of, file, GetHeaderKind(file), program["--show-decompiled"] == true, program["--show-actions"] == true, program.get("--output-cfg-file"));
				}
				of.close();
				
			}
			break;
		case HeaderKind::Unrecognized:
			std::cerr << "The input file is unrecognized." << std::endl;
			return 1;
		default:
			{
				std::ofstream of(program.get("output_file"), std::ifstream::binary);
				
				ProcessFile(of, contents, hdrKind, program["--show-decompiled"] == true, program["--show-actions"] == true, program.get("--output-cfg-file"));
				
				of.close();
			}
		}
		std::cout << "Successful decompilation." << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}