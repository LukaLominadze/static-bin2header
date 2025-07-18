
/* Copyright © 2017-2022 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the bin2header project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "include/bin2header.h"

using namespace std;


const string version = "0.3.1";
const string appname = "Binary to Header";
string executable;


/** Prints app name & version. */
void printVersion() {
	cout << "\n" << appname << " version " << version << endl;
#ifdef WIN32
	cout << "Copyright (C) 2017-2022 Jordan Irwin <antumdeluge@gmail.com>" << endl;
#else
	cout << "Copyright © 2017-2022 Jordan Irwin <antumdeluge@gmail.com>" << endl;
#endif
}


/** Prints usage information. */
void printUsage() {
	printVersion();
	cout << "\n  Usage:\n\t" << executable << " [options] <file>" << endl;
	cout << "\n  Options:" << endl;
	cout << "\t-h, --help\t\tPrint help information & exit." << endl;
	cout << "\t-v, --version\t\tPrint version information & exit." << endl;
	cout << "\t-o, --output\t\tOutput file name." << endl;
	cout << "\t-n, --hname\t\tHeader name. Default is file name with \".\" replaced by \"_\"." << endl;
	cout << "\t-s, --chunksize\t\tRead buffer chunk size (in bytes)." << endl;
	cout << "\t\t\t\t  Default: 1048576 (1 megabyte)" << endl;
	cout << "\t-d, --nbdata\t\tNumber of bytes to write per line." << endl;
	cout << "\t\t\t\t  Default: 12" << endl;
	cout << "\t-c, --datacontent\tShow data content as comments." << endl;
	cout << "\t-f  --offset\t\tPosition offset to begin reading file (in bytes)." << endl;
	cout << "\t\t\t\t  Default: 0" << endl;
	cout << "\t-l  --length\t\tNumber of bytes to process (0 = all)." << endl;
	cout << "\t\t\t\t  Default: 0" << endl;
	cout << "\t-p  --pack\t\tStored data type bit length (8/16/32)." << endl;
	cout << "\t\t\t\t  Default: 8" << endl;
	cout << "\t-e  --swap\t\tSet endianess to big endian for 16 & 32 bit data types." << endl;
	cout << "\t    --stdvector\t\tAdditionally store data in std::vector for C++." << endl;
	cout << "\t    --eol\t\tSet end of line character (cr/lf/crlf)." << endl;
	cout << "\t\t\t\t  Default: lf" << endl;
}


/** Prints message to stderr & exits program.
 *
 *  @tparam int code
 *      Program exit code.
 *  @tparam string msg
 *      Message to print.
 *  @tparam bool show_usage
 *      If `true` will print usage info.
 */
void exitWithError(const int code, const string msg, const bool show_usage) {
	cerr << "\nERROR: " << msg << endl;
	if (show_usage) printUsage();
}

/** Prints message to stderr & exits program.
 *
 *  @tparam int code
 *      Program exit code.
 *  @tparam string msg
 *      Message to print.
 */
void exitWithError(const int code, const string msg) {
	exitWithError(code, msg, false);
}


/** Program entry point.
 *
 *  @tparam int argc
 *      Number of command line arguments.
 *  @tparam char** argv
 *      Command line arguments.
 */
int Convert(const filesystem::path& fin, const filesystem::path& fout, vector<const char*> _args) {
	string program = "bin2header";

	vector<string> owned_args;
	owned_args.push_back(program);                // program name
	owned_args.push_back("--output=" + fout.string()); // output file, as an example
	owned_args.push_back(fin.string());         // source file

	_args.insert(_args.begin(), owned_args[0].c_str());
	_args.push_back(owned_args[1].c_str());
	_args.push_back(owned_args[2].c_str());

	cxxopts::Options options(program, "Convert binary files to C/C++ headers");
	options.add_options()
			("h,help", "")
			("v,version", "")
			("o,output", "", cxxopts::value<string>())
			("n,hname", "", cxxopts::value<string>())
			("s,chunksize", "", cxxopts::value<unsigned int>())
			("d,nbdata", "", cxxopts::value<unsigned int>())
			("c,datacontent", "")
			("f,offset", "", cxxopts::value<unsigned long>())
			("l,length", "", cxxopts::value<unsigned long>())
			("p,pack", "", cxxopts::value<unsigned int>())
			("e,swap", "")
			("stdvector", "")
			("eol", "", cxxopts::value<string>());

	cxxopts::ParseResult args;
	try {
		args = options.parse(_args.size(), _args.data());
	} catch (const cxxopts::OptionParseException& e) {
		return -1;
	}


	if (args["help"].as<bool>()) {
		printUsage();
		return 0;
	} else if (args["version"].as<bool>()) {
		printVersion();
		return 0;
	}

	// string stream for concatenating error messages
	stringstream ss;

	if (args.count("chunksize") > 0) {
		setChunkSize(args["chunksize"].as<unsigned int>());
	}

	if (args.count("nbdata") > 0) {
		setNumberDataPerLine(args["nbdata"].as<unsigned int>());
	}

	if (args["datacontent"].as<bool>()) {
		setShowDataContent(true);
	}

	if (args.count("offset") > 0) {
		setReadOffset(args["offset"].as<unsigned long>());
	}

	if (args.count("pack") > 0) {
		setOutputBitLength(args["pack"].as<unsigned int>());
	}

	if (args.count("length") > 0) {
		setReadLength(args["length"].as<unsigned long>());
	}

	if (args.count("swap") > 0) {
		setSwapEndianess();
	}

	if (args.count("eol") > 0) {
		setEol(args["eol"].as<string>());
	}

	// too many input files
	// XXX: should we allow multiple inputs?
	#if 0	/* Multiple switches */
	if (argc > 2) {
		exitWithError(E2BIG, "Too many input files specified", true);
	}
	#endif

	if (_args.size() < 2) {
		// FIXME: correct error return code
		return -1;
	}

	// only remaining argument should be input file
	string source_file = normalizePath(_args[_args.size() -1]);

	// check if source file exists
	if (access(source_file.c_str(), F_OK) == -1) {
		// clear stringstream
		ss.str("");
		ss << "File \"" << source_file << "\" does not exist";
		std::cout << ss.str() << std::endl;
		return -1;
	}

	string hname = "";
	if (args.count("hname") > 0) {
		hname = args["hname"].as<string>();
	}

	string target_file = "";
	if (args.count("output") > 0) {
		target_file = args["output"].as<string>();
	}

	convert(fin.string(), fout.string(), hname, false);
}

const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64Encode(const std::vector<uint8_t>& bytes) {
	std::string encoded;
	int val = 0, valb = -6;

	for (uint8_t c : bytes) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}

	if (valb > -6)
		encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);

	while (encoded.size() % 4)
		encoded.push_back('=');

	return encoded;
}

std::vector<uint8_t> parseCppArray(const std::string& cpp) {
	std::regex hexRegex(R"(0x[0-9a-fA-F]+)");
	std::sregex_iterator iter(cpp.begin(), cpp.end(), hexRegex);
	std::sregex_iterator end;

	std::vector<uint8_t> result;
	while (iter != end) {
		std::string match = iter->str();
		result.push_back(static_cast<uint8_t>(std::stoi(match, nullptr, 16)));
		++iter;
	}
	return result;
}

std::string cppToYamlBinary(const std::string& cpp_code, const std::string& var_name) {
	std::vector<uint8_t> data = parseCppArray(cpp_code);

	std::ostringstream out;
	out << var_name << ":\n";
	for (size_t i = 0; i < data.size(); ++i) {
		if (i % 16 == 0) out << "  - "; // start new line
		else out << ", ";
		out << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
		if (i % 16 == 15 && i + 1 < data.size()) out << "\n";
	}
	out << "\n";
	return out.str();
}