#pragma once

#include <string>
#include <vector>

namespace xxs::fileio
{
	std::vector<char> read_binary_file(const std::string& filename);
	std::string read_text_file(const std::string& filename);
}