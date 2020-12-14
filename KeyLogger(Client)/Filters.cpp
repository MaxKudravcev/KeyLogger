#include "Filters.h"

Filters::Filters(std::vector<std::wstring> filters)
{
	for (std::wstring filter : filters)
	{
		AddFilter(filter);
	}
}

bool Filters::Match(std::wstring match)
{
	std::wsmatch m;

	for (std::wregex regExp : filters)
	{
		if (std::regex_search(match, m, regExp))
			return true;
	}

	return false;
}

void Filters::AddFilter(std::wstring filterStr)
{
	std::wregex regExp(filterStr, std::regex_constants::icase);
	filters.push_back(regExp);
}
