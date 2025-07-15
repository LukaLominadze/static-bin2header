#pragma once

/* Copyright © 2017-2022 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the bin2header project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "convert.h"
#include "cxxopts.hpp"
#include "paths.h"

#include <cerrno>
#include <cctype> // toupper
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#ifdef _WIN32
#include <io.h>
#define access _access
#define F_OK 0
#else
#include <unistd.h>
#endif

using namespace std;

/** Prints app name & version. */
extern void printVersion();


/** Prints usage information. */
extern void printUsage();


/** Prints message to stderr & exits program.
 *
 *  @tparam int code
 *      Program exit code.
 *  @tparam string msg
 *      Message to print.
 *  @tparam bool show_usage
 *      If `true` will print usage info.
 */
extern void exitWithError(const int code, const string msg, const bool show_usage);

/** Prints message to stderr & exits program.
 *
 *  @tparam int code
 *      Program exit code.
 *  @tparam string msg
 *      Message to print.
 */
extern void exitWithError(const int code, const string msg);

/** Program entry point.
 *
 *  @tparam int argc
 *      Number of command line arguments.
 *  @tparam char** argv
 *      Command line arguments.
 */
extern int Convert(const filesystem::path& fin, const filesystem::path& fout, vector<const char*> _args = vector<const char*>());

extern std::string base64Encode(const std::vector<uint8_t>& bytes);

extern std::vector<uint8_t> parseCppArray(const std::string& cpp);

extern std::string cppToYamlBinary(const std::string& cpp_code, const std::string& var_name);