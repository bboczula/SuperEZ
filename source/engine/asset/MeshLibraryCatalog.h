#pragma once

#include <algorithm>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

// Metadata only: discovering groups must not create scene objects or GPU resources.
inline std::vector<std::string> ReadMeshLibraryGroups(std::istream& input)
{
	std::vector<std::string> names;
	std::string line;
	while (std::getline(input, line))
	{
		// OBJ comments may follow a declaration on the same line.
		line.erase(line.find('#') == std::string::npos ? line.size() : line.find('#'));
		std::istringstream fields(line);
		std::string keyword;
		if (!(fields >> keyword) || keyword != "g")
			continue;
		std::string name;
		while (fields >> name)
			names.push_back(name);
	}
	std::sort(names.begin(), names.end());
	names.erase(std::unique(names.begin(), names.end()), names.end());
	return names;
}
