#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "common/IDebugLog.h"  // IDebugLog
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // SKSEInterface, PluginInfo
#include <skse64/ObScript.h>
#include <skse64/PapyrusGame.cpp>


using namespace std;


string replaceconfig(string line, string chr, string to) {
	int lpos = line.find(chr);
	while (lpos != std::string::npos) {
		int spos = line.find(",");
		int pspos = 0;
		while (spos < lpos && spos != std::string::npos) {
			pspos = spos;
			spos = line.find(",", pspos + 1);
		}
		line = line.replace(pspos + 1, lpos - pspos - 1, to + " ");

		//find collision value at the end of line (only with |)
		if (chr == "|") {
			spos = line.find(",");
			pspos = 0;
			while (spos < line.size() && spos != std::string::npos) {
				pspos = spos;
				spos = line.find(",", pspos + 1);
			}
			line = line.replace(pspos + 1, line.size() - pspos - 1, to);
		}

		lpos = line.find(chr, lpos + 1);
	}
	return line;
}

bool ReadWriteCBPCConfig(std::string filepath, std::string VaginaOpeningLimit, std::string  AnusOpeningLimit, std::string CollisionSizeV, std::string CollisionSizeA, std::string PussyCollisionSize, bool femaleconfig) {
	std::vector<std::string> lines;
	std::string line;

	std::ifstream input_file(filepath);
	if (!input_file.is_open()) {
		return false;
	}

	bool pelvisnodeIsNext = false;
	bool AnalnodeIsNext = false;
	bool PussynodeIsNext = false;
	while (std::getline(input_file, line)) {
		if (line.find("VaginaOpeningLimit") != std::string::npos) {
			line = "VaginaOpeningLimit = " + VaginaOpeningLimit;
			lines.push_back(line);
		}
		else if (line.find("AnusOpeningLimit") != std::string::npos) {
			line = "AnusOpeningLimit = " + AnusOpeningLimit;
			lines.push_back(line);
		}//Pelvis Node
		else if (pelvisnodeIsNext) {
			if (femaleconfig) {
				line = replaceconfig(line, "|", CollisionSizeV);
				line = replaceconfig(line, "&", CollisionSizeV);

				//line = "0,0,-2," + CollisionSize + " | 0,0,-2," + CollisionSize;
			}
			lines.push_back(line);
			pelvisnodeIsNext = false;
		}
		else if (line.find("[NPC Pelvis [Pelv]]") != std::string::npos) {
			lines.push_back(line);
			pelvisnodeIsNext = true;
		}//Anal node
		else if (AnalnodeIsNext) {
			if (femaleconfig) {
				line = replaceconfig(line, "|", CollisionSizeA);
				line = replaceconfig(line, "&", CollisionSizeA);

				//line = "0,-6,-2," + CollisionSize + " | 0,-6,-2," + CollisionSize;
			}
			lines.push_back(line);
			AnalnodeIsNext = false;
		}
		else if (line.find("[Anal]") != std::string::npos) {
			lines.push_back(line);
			AnalnodeIsNext = true;
		}//Pussy nodes
		else if (PussynodeIsNext) {
			if (femaleconfig) {
				line = replaceconfig(line, "|", PussyCollisionSize);
				line = replaceconfig(line, "&", PussyCollisionSize);
			}
			lines.push_back(line);
			PussynodeIsNext = false;
		}
		else if (line.find("[NPC L Pussy02]") != std::string::npos || line.find("[NPC R Pussy02]") != std::string::npos || line.find("[VaginaB1]") != std::string::npos || line.find("[Clitoral1]") != std::string::npos) {
			lines.push_back(line);
			PussynodeIsNext = true;
		}
		else {
			lines.push_back(line);
		}
	}

	//Write
	std::ofstream ofs(filepath);
	for (int i = 0; i < lines.size(); ++i) {
		ofs << lines[i] << "\n";
	}
	ofs.close();

	return true;
}

bool ReadWriteCBPCConfig2(std::string filepath, std::string MaleGenitalHeadCollisionSize, std::string MaleGenitalBaseCollisionSize) {
	std::vector<std::string> lines;
	std::string line;

	std::ifstream input_file(filepath);
	if (!input_file.is_open()) {
		return false;
	}

	bool GenitalnodeIsNext = false;
	while (std::getline(input_file, line)) {
		if (GenitalnodeIsNext) {
			line = replaceconfig(line, "|", MaleGenitalBaseCollisionSize);
			line = replaceconfig(line, "&", MaleGenitalHeadCollisionSize);

			lines.push_back(line);
			GenitalnodeIsNext = false;
		}
		else if (line.find("[NPC Genitals06 [Gen06]]") != std::string::npos) {
			lines.push_back(line);
			GenitalnodeIsNext = true;
		}
		else {
			lines.push_back(line);
		}
	}

	std::ofstream ofs(filepath);
	for (int i = 0; i < lines.size(); ++i) {
		ofs << lines[i] << "\n";
	}
	ofs.close();

	return true;
}

BSFixedString CBPCUpdatePhysics2(StaticFunctionTag* base, BSFixedString MaleGenitalHeadCollisionSize, BSFixedString MaleGenitalBaseCollisionSize)
{
	std::string sMaleGenitalHeadCollisionSize = MaleGenitalHeadCollisionSize;
	std::string sMaleGenitalBaseCollisionSize = MaleGenitalBaseCollisionSize;

	std::string	runtimeDirectory = GetRuntimeDirectory();

	//Male
	bool cwriteresult = ReadWriteCBPCConfig2(runtimeDirectory + "Data\\SKSE\\Plugins\\CBPCollisionConfig.txt", sMaleGenitalHeadCollisionSize, sMaleGenitalBaseCollisionSize);
	if (!cwriteresult) {
		return BSFixedString("0");
	}

	//Female\Futa
	cwriteresult = ReadWriteCBPCConfig2(runtimeDirectory + "Data\\SKSE\\Plugins\\CBPCollisionConfig_Female.txt", sMaleGenitalHeadCollisionSize, sMaleGenitalBaseCollisionSize);
	if (!cwriteresult) {
		return BSFixedString("0");
	}

	return BSFixedString("1");
}

BSFixedString CBPCUpdatePhysics(StaticFunctionTag* base, BSFixedString isAnal, BSFixedString isGroup, BSFixedString isVaginal, BSFixedString femboy, BSFixedString VaginaOpeningLimit,
	BSFixedString AnusOpeningLimit, BSFixedString PelvisCollisionSizeV, BSFixedString PelvisCollisionSizeA, BSFixedString PussyCollisionSize, BSFixedString PussyCollisionLimit)
{
	std::string sAnal = isAnal;
	std::string sGroup = isGroup;
	std::string sVaginal = isVaginal;
	std::string sfemboy = femboy;
	std::string sVaginaOpeningLimit = VaginaOpeningLimit;
	std::string sAnusOpeningLimit = AnusOpeningLimit;
	std::string sPelvisCollisionSizeV = PelvisCollisionSizeV;
	std::string sPelvisCollisionSizeA = PelvisCollisionSizeA;
	std::string sPussyCollisionSize = PussyCollisionSize;
	std::string sPussyCollisionLimit = PussyCollisionLimit;
	std::string sCollisionSize = "";

	if (sAnal == "1" || sfemboy == "1") {
		sVaginaOpeningLimit = "0";
		sPussyCollisionSize = PussyCollisionLimit;
		sCollisionSize = sPelvisCollisionSizeA;
		if (sGroup == "1" && sVaginal == "1" && sfemboy == "0") {
			sVaginaOpeningLimit = VaginaOpeningLimit;
			sPussyCollisionSize = PussyCollisionSize;
			//sCollisionSize = sPelvisCollisionSizeA;
		}
	}
	else
	{
		sAnusOpeningLimit = "0";
		sPussyCollisionSize = PussyCollisionSize;
		//sCollisionSize = PelvisCollisionSizeV;
	}

	std::string	runtimeDirectory = GetRuntimeDirectory();

	/*bool cwriteresult = ReadWriteCBPCConfig(runtimeDirectory + "Data\\SKSE\\Plugins\\CBPCollisionConfig.txt", sVaginaOpeningLimit, sAnusOpeningLimit, sCollisionSize, false);
	if (!cwriteresult) {
		return BSFixedString("0");
	}*/

	bool cwriteresult = ReadWriteCBPCConfig(runtimeDirectory + "Data\\SKSE\\Plugins\\CBPCollisionConfig_Female.txt", sVaginaOpeningLimit, sAnusOpeningLimit, sPelvisCollisionSizeV, sPelvisCollisionSizeA, sPussyCollisionSize, true);
	if (!cwriteresult) {
		return BSFixedString("0");
	}

	return BSFixedString("1");
}

bool RegisterFuncs(VMClassRegistry* registry)
{
	registry->RegisterFunction(
		new NativeFunction10<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString, BSFixedString, BSFixedString, BSFixedString, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("CBPCUpdatePhysics", "CBPCASScript", CBPCUpdatePhysics, registry));

	registry->RegisterFunction(
		new NativeFunction2<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString>("CBPCUpdatePhysics2", "CBPCASScript", CBPCUpdatePhysics2, registry));

	return true;
}


extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "CBPC Anus Support";
		a_info->version = 0x010500;

		if (a_skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		}
		else if (a_skse->runtimeVersion != RUNTIME_VERSION_1_5_97) {
			_FATALERROR("[FATAL ERROR] Unsupported runtime version %08X!\n", a_skse->runtimeVersion);
			return false;
		}

		return true;
	}

	static SKSEPapyrusInterface* g_papyrus = NULL;

	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		_MESSAGE("[MESSAGE] CBPC Anus Support loaded");

		g_papyrus = (SKSEPapyrusInterface*)a_skse->QueryInterface(kInterface_Papyrus);

		bool bSuccess = g_papyrus->Register(RegisterFuncs);

		if (bSuccess) {
			_MESSAGE("[MESSAGE] CBPC Anus Support registered functions");
		}
		else {
			_MESSAGE("[MESSAGE] CBPC Anus Support failed to register functions");
		}

		return true;
	}
};

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
) {
	return true;
}