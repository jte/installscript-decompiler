#include "ActionFile.h"
#include "Action.h"
#include "Struct.h"
#include "IScript.h"
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include "FuncPrologAction.h"
#include "BasicBlock.h"

const std::vector<uint8_t> readFile(std::wstring filePath)
{
	std::ifstream in(filePath, std::ifstream::binary);

	if (!in)
	{
		throw std::runtime_error("Failed to open the action file");
	}

	std::filebuf* pbuf = in.rdbuf();

	std::size_t size = static_cast<std::size_t>(pbuf->pubseekoff(0, in.end, in.in));
	pbuf->pubseekpos(0, in.in);

	std::vector<uint8_t> buffer;

	buffer.resize(size);

	pbuf->sgetn(reinterpret_cast<char*>(&buffer.at(0)), size);

	return buffer;
}

CBasicBlock* TranslateBBs(uint32_t address, CIScript::ScriptBasicBlock bb)
{
	CBasicBlock* irbb = new CBasicBlock(address);
	for (const auto& acts : bb)
	{
		auto s(acts->ToStatement());
		s.SetOwner(irbb);
		irbb->AddStatement(s);
	}
	return irbb;
}

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const std::vector<uint8_t> contents = readFile(L"output.obs");
		
		CIScript script(contents);
		COptimizer optimizer;
		for (const auto& proto : script.GetPrototypes())
		{
			optimizer.AddFunctionPrototype(proto);
		}
		const auto& bbs = script.GetBasicBlocks();
		CFunction* currFunction = nullptr;
		size_t label = 0;
		for (auto bb = bbs.cbegin(); bb != bbs.cend(); ++bb)
		{
			size_t index = std::distance(bbs.cbegin(), bb);
			if (dynamic_cast<CFuncPrologAction*>((*bb)[0]))
			{
				currFunction = &optimizer.GetFunction(index);
				label = 0;
			}
			// detect func epilogue ?
			currFunction->AddBasicBlock(TranslateBBs(label, *bb));
			label++;
		}
		for (const auto& e : script.GetExterns())
		{
			ArgType type = (ArgType)-1;
			if (e.type == 1)
				type = ArgType::VariantArg;
			else if (e.type == 2)
				;//??
			else if (e.type == 3)
				;//??
			CVariable* gvar = new CVariable(type, e.address);
			gvar->SetName(e.name);
			optimizer.AddGlobalVariable(gvar);
		}

		//std::cout << script.GetStruct(0);
		//std::cout << script.GetStruct(1);
		//script.PrintPrototypes();
		//std::cout << script;
		std::cout << optimizer;
		std::cout << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}