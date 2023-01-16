#pragma once

#include <cstddef>
#include <string>

struct MemoryFontData {
	MemoryFontData () =delete;
	const char *data;
	size_t size;
};

const MemoryFontData* find_base14_font (const std::string &name);
