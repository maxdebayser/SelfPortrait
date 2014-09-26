/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <string>
#include <vector>


std::string normalizedTypeName(const char * argString, int* charsRead = nullptr);

/* Transforms an argument string such as "int, const char*"
 * into a list of parameter type string like ["int", "const char *"].
 * The tokens are always separated by spaces
 */

std::vector<std::string> splitArgs(const char* argString);


#endif /* STR_UTILS_H */
