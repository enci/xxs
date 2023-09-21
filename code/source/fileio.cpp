#include "fileio.hpp"
#include <fstream>
#include <cassert>
#include "log.hpp"

using namespace xxs;
using namespace std;

vector<char> fileio::read_binary_file(const std::string& filename)
{
	const auto path = get_path(filename);
	ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		log::error("File {} with full path {} was not found!", filename, path);
		return vector<char>();	
	}
	const streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	if (file.read(buffer.data(), size))
		return  buffer;
	assert(false);
	return vector<char>();
}

string fileio::read_text_file(const string& filename)
{
	const auto path = get_path(filename);	
	ifstream file(path);
	if (!file.is_open())
	{
		log::error("File {} with full path {} was not found!", filename, path);
		return string();
	}
	file.seekg(0, std::ios::end);
	const size_t size = file.tellg();
	string buffer(size, '\0');
	file.seekg(0);
	file.read(&buffer[0], size);
	return buffer;
}

string fileio::get_path(const string& filename)
{
    // You can implement this function to take different paths
    // into account depending on the platform and configuration.
    return filename;
}
