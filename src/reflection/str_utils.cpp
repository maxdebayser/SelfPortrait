#include "str_utils.h"

#include <ctype.h>

std::string normalizedTypeName(const char * argString, int* charsRead)
{
	const char* start = argString;
	while (isspace(*argString) || *argString == ',') {
		++argString;
	}

	std::string arg;

	while (*argString != '\0' && *argString != ',') {

		char curr = *argString;
		char next = *(argString+1);
		if (isspace(curr)) {
			// only push a space if it's needed to separate type tokens
			if (!(isspace(next) || next == ',' || next == '\0')) {
				arg.push_back(' '); // not any space  char
			}
		} else {
			arg.push_back(curr);

			if ((curr != '*' && curr != '&') && (next == '*' || next == '&')) {
				arg.push_back(' '); // always separate the tokens
			}
		}
		++argString;
	}
	if (charsRead != nullptr) {
		*charsRead = argString-start;
	}

	return arg;
}

std::vector<std::string> splitArgs(const char* argString) {

	std::vector<std::string> ret;

	while (*argString != '\0') {

		int pos = 0;

		std::string arg = normalizedTypeName(argString, &pos);
		argString += pos;

		if (!arg.empty()) {
			ret.push_back(arg);
		}
	}

	return std::move(ret);
}
