#pragma once

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

class FileService
{
public:
	FileService(std::wstring logDirectory, std::wstring fileName);
	void Append(std::wstring str);
	~FileService();
private:
	std::wofstream fout;
	std::wstring CurrentDateTime();
};

