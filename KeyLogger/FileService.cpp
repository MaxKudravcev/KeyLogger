#include "FileService.h"

FileService::FileService(std::wstring logDirectory, std::wstring fileName)
{	
	if (CreateDirectoryW(logDirectory.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
		fileName = logDirectory + L"/" + fileName + L"_" + CurrentDateTime() + L".log";
	else
		fileName = fileName + L"_" + CurrentDateTime() + L".log";
	
	fout.open(fileName, std::ios_base::out || std::ios_base::app);
}

std::wstring FileService::CurrentDateTime() {
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::wostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
	std::wstring str = oss.str();
	return str;
}

void FileService::Append(std::wstring str)
{
	fout << str;
}

FileService::~FileService()
{
	fout.flush();
	fout.close();
}
