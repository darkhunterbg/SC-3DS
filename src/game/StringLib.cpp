#include "StringLib.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

namespace String {

	void Split(Span<char> text, Span<char> splitters, std::vector<std::string>& result, StringSplitOptions options) {

		int start = 0;
		for (int i = 0; i < text.Size(); ++i)
		{
			for (int j = 0; j < splitters.Size(); ++j) {
				if (text[i] == splitters[j]) {

					int size = i - start;

					if (options != StringSplitOptions::RemoveEmptyEntries || size > 0)
						result.push_back(std::string(text.Data() + start, (unsigned)size));

					start = i + 1;


					break;
				}
			}

		}

		int size = text.Size() - start;


		if (options != StringSplitOptions::RemoveEmptyEntries || size > 0)
			result.push_back(std::string(text.Data() + start, (unsigned)size));
	}

	void Split(Span<char> text, const char* splitter, std::vector<std::string>& result, StringSplitOptions options)
	{
		unsigned len = strnlen(splitter, 64);

		int start = 0;
		for (int i = 0; i < text.Size() - len; ++i)
		{
			if (strncmp(text.Data() + i, splitter, len) == 0) {

				int size = i - start;

				if (options != StringSplitOptions::RemoveEmptyEntries || size > 0)
					result.push_back(std::string(text.Data() + start, (unsigned)size));

				i += len - 1;
				start = i + 1;
			}
		}

		int size = text.Size() - start;


		if (options != StringSplitOptions::RemoveEmptyEntries || size > 0)
			result.push_back(std::string(text.Data() + start, (unsigned)size));
	}

	int Format(Span<char> buffer, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		unsigned result = stbsp_vsnprintf(buffer.Data(), buffer.Size(), format, args);
		va_end(args);

		return result;
	}

	int Find(Span<char> text, Span<char> symbols, unsigned offset)
	{
		for (unsigned i = offset; i < text.Size(); ++i) {
			for (int j = 0; j < symbols.Size(); ++j) {
				if (text[i] == symbols[j])
					return i;

			}

		}

		return -1;
	}
	int Find(Span<char> text, char symbol, unsigned offset)
	{
		for (unsigned i = offset; i < text.Size(); ++i) {
			if (text[i] == symbol)
				return i;
		}

		return -1;
	}

	int FindStr(std::string text, Span<char> symbols, unsigned offset)
	{
		for (unsigned i = offset; i < text.length(); ++i) {
			for (int j = 0; j < symbols.Size(); ++j) {
				if (text[i] == symbols[j])
					return i;

			}

		}

		return -1;
	}
	int FindStr(std::string text, char symbol, unsigned offset)
	{
		for (unsigned i = offset; i < text.length(); ++i) {
			if (text[i] == symbol)
				return i;
		}

		return -1;
	}
}