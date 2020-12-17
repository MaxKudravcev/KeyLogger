#include "FileService.h"
#define _CRT_SECURE_NO_WARNINGS 


FileService::FileService(std::wstring logDirectory, std::wstring fileName)
{	
	fileName = fileName + L'_' + CurrentDateTime() + L".log";
	if (CreateDirectory(logDirectory.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())

	{
		this->fout.open(logDirectory.append(fileName), std::ofstream::out | std::ofstream::app);
	}
}

std::wstring FileService::CurrentDateTime() {
	auto t = std::time(nullptr);
	tm t_m;
	localtime_s(&t_m, &t);

	std::wostringstream oss;
	oss << std::put_time(&t_m, L"%d-%m-%Y_%H-%M-%S");
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
