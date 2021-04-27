#pragma once

#include <cstring>
#include <string>
#include <vector>
#include "Span.h"
#include "stb_sprintf.h"


enum class StringSplitOptions
{
	None,
	RemoveEmptyEntries
};


namespace String {

	inline bool Equals(const char* a, const char* b, unsigned maxCompare = 0) {
		if (maxCompare == 0)
		{
			maxCompare = strlen(a);
			unsigned l2 = strlen(b);

			maxCompare = maxCompare > l2 ? maxCompare : l2;
		}


		return strncmp(a, b, maxCompare) == 0;
	}

	inline bool Equals(const std::string& a, const char* b, unsigned maxCompare = 0) {
		if (maxCompare == 0)
			maxCompare = strlen(b);

		maxCompare = maxCompare > a.length() ? maxCompare : a.length();

		return strncmp(a.data(), b, maxCompare) == 0;
	}

	inline unsigned Count(Span<char> text, char symbol) {
		unsigned result = 0;

		for (unsigned i = 0; i < text.Size(); ++i) {
			if (text[i] == symbol)
				++result;
		}

		return result;
	}

	inline bool Equals(const std::string& a, const std::string& b) {

		return a == b;
	}

	void Split(Span<char> text, Span<char> splitters, std::vector<std::string>& result, StringSplitOptions options = StringSplitOptions::None);
	void Split(Span<char> text, const char* splitter, std::vector<std::string>& result, StringSplitOptions options = StringSplitOptions::None);
	inline void SplitStr(const std::string& text, const char* splitter, std::vector<std::string>& result, StringSplitOptions options = StringSplitOptions::None) {
		Split({ text.data(), text.length() }, splitter, result, options);
	}

	inline void Split(Span<char> text, char splitter, std::vector<std::string>& result, StringSplitOptions options = StringSplitOptions::None) {
		char splitters[] = { splitter };

		Split(text, { splitters,1 }, result, options);
	}

	inline void SplitStr(const std::string& text, Span<char> splitters, std::vector<std::string>& result, StringSplitOptions options = StringSplitOptions::None) {
		Split({ text.data(), text.length() }, splitters, result, options);
	}

	inline void SplitStr(const std::string& text, char splitter, std::vector<std::string>& result, StringSplitOptions options = StringSplitOptions::None) {
		char splitters[] = { splitter };

		Split({ text.data(), text.length() }, { splitters,1 }, result, options);
	}

	int Format(Span<char> buffer, const char* format, ...);

	int Find(Span<char> text, Span<char> symbols, unsigned offset = 0);
	int Find(Span<char> text, char symbol, unsigned offset = 0);

	int FindStr(std::string text, Span<char> symbols, unsigned offset = 0);
	int FindStr(std::string  text, char symbol, unsigned offset = 0);


}