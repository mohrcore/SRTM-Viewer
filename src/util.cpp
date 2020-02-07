#include "util.h"

size_t getStreamSize(std::istream & stream)
{
	stream.seekg(0, std::ios::beg);
	stream.ignore(std::numeric_limits<std::streamsize>::max());
	std::streamsize length = stream.gcount();
	stream.clear();
	stream.seekg(0, std::ios::beg);
	return length;
}