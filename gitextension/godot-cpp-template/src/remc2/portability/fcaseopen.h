#ifndef fcaseopen_h
#define fcaseopen_h

#include <stdio.h>
#include <string>
#include <vector>

FILE* fcaseopenx(char const* path, char const* mode);

static std::vector<std::string> GetTokensFromPath(const std::string &path);
#if !defined(_WIN32)
static std::string casepath(const std::string &path);
#endif

#endif
