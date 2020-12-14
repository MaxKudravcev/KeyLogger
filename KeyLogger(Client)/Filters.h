#pragma once
#include <vector>
#include <string>
#include <regex>

class Filters
{
public:
	Filters(std::vector<std::wstring> filters);
	bool Match(std::wstring match);

private:
	void AddFilter(std::wstring filterStr);
	std::vector<std::wregex> filters;
};

