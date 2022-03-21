//
// passphrase.cpp
//
//  Generate some password phrases from dictionary words
//

#include <cstdio>
#include <iostream>
#include <cstdint>
#include <cctype>
#include <cstring>
#include <string>
#include <sstream>
#include <deque>
#include <random>
#include <getopt.h>
#include <libgen.h>

using std::cout;
using std::endl;
using std::string;
using std::deque;

#if defined(OBJECT_WRAPPER)
extern const char _binary_en_CA_dic_start[];
extern const char _binary_en_CA_dic_end[];

extern const char _binary_fr_CA_dic_start[];
extern const char _binary_fr_CA_dic_end[];

extern const char _binary_sw_TZ_dic_start[];
extern const char _binary_sw_TZ_dic_end[];

#define DICT_SIZE( x ) \
	(uintptr_t)(_binary_ ## x ## _dic_end - _binary_ ## x ## _dic_start)

struct Dictionary {
	uintptr_t size;
	const char *start;
	const char *end;
	string name;
};

const Dictionary dictionary[] = {
	{ DICT_SIZE( en_CA ), _binary_en_CA_dic_start, _binary_en_CA_dic_end, "en_CA" },
	{ DICT_SIZE( fr_CA ), _binary_fr_CA_dic_start, _binary_fr_CA_dic_end, "fr_CA" },
	{ DICT_SIZE( sw_TZ ), _binary_sw_TZ_dic_start, _binary_sw_TZ_dic_end, "sw_TZ" },
	{}
};
#else

struct Dictionary {
	size_t size;
	string name;
	const string *words;
};

extern const string en_CA_dict[];
extern const size_t en_CA_dict_size;
extern const string fr_CA_dict[];
extern const size_t fr_CA_dict_size;
extern const string sw_TZ_dict[];
extern const size_t sw_TZ_dict_size;

const Dictionary dictionary[] = {
	{ en_CA_dict_size, "en_CA", en_CA_dict },
	{ fr_CA_dict_size, "fr_CA", fr_CA_dict },
	{ sw_TZ_dict_size, "sw_TZ", sw_TZ_dict },
	{}
};
#endif

std::random_device rd;
std::mt19937 gen(rd());

int caps = 0, numbers = 0, altchars = 0, verbose = 0;

static struct option opts[] = {
	{"help", no_argument, 0, '?'},
	{"usage", no_argument, 0, '?'},
	{"caps", no_argument, &caps, 1},
	{"numbers", no_argument, &numbers, 1},
	{"alt-chars", no_argument, &altchars, 1},
	{"lang", required_argument, 0, 'l'},
	{"verbose", no_argument, 0, 'V'},
	{"version", no_argument, 0, 'v'},
	{}
};

string random_word(
	std::uniform_int_distribution<> &dist, deque<string> &dict,
	bool want_apostrophe = false)
{
	string candidate;
	string::size_type apostrophe = string::npos;

	// if (dict.size() == 0)
	// 	return "";

	do {
		candidate = dict[ dist(gen) ];
		if (want_apostrophe)
			apostrophe = candidate.find( '\'' );

		string::size_type slash = candidate.rfind( '/' );
		if (slash != string::npos) {
			candidate = candidate.substr(0, slash);
		}
	}
	while ((want_apostrophe && (apostrophe == string::npos)) || 
		(candidate.size() < 2 || candidate.size() > 9));

	return candidate;
}

string phrase(
	std::uniform_int_distribution<> &dist, deque<string> &dict, bool cap, bool want_alt)
{
	if (dict.size() == 0)
		return "";

	string p = random_word(dist, dict, false);

	bool have_alt = false;
	while (p.length() < 12) {
		string next = random_word(dist, dict, want_alt ^ have_alt);
		if (cap)
			next[0] = toupper(next[0]);
		p += next;
		have_alt = p.find( '\'' ) != string::npos;
	}
	return p;
}

void help(string prog)
{
	cout << prog << " [-hcnavV] [-l en_CA | fr_CA | sw_TZ ]" << endl;
}

int main(int argc, char *argv[])
{
	string lang = "en_CA";

	cout << "Hello, world!\nReady for some passwords?" << endl;

	int ch;
	while ((ch = getopt_long(argc, argv, "acnl:vV", opts, NULL)) != -1) {
		switch (ch) {
		case 'a':
			altchars = true;
			break;
		case 'c':
			caps = true;
			break;
		case 'n':
			numbers = true;
			break;
		case 'v':
			cout << "V0.00" << endl;
			break;
		case 'V':
			verbose = true;
			break;
		case 'l':
			lang = string(optarg);
			break;
		case '?':
			help(basename(argv[0]));
			exit(1);
		case 0:
			if (optind < argc)
				help(basename(argv[0]));
			else break;
		}
	}

	if (verbose) {
		for (auto i : dictionary) {
			if (i.size == 0)
				continue;
			cout << i.name << " is : " << std::hex << std::showbase << i.size << 
				" bytes, start = " << (intptr_t)i.words << endl;
		}
	}

	Dictionary working_dictionary_;
	const Dictionary *working_dictionary = &working_dictionary_;
	for (auto d : dictionary) {
		if (d.size == 0)
			continue;
		if (d.name == lang) {
			working_dictionary_ = d;
			cout << "Got lang " << d.name <<
				", start = " << (intptr_t)d.words << endl;
			break;
		}
		cout << d.name << " != " << lang << endl;
	}
	if (!working_dictionary) {
		help(basename(argv[0]));
		exit(1);
	}

	cout << "working = " << std::dec << working_dictionary->size << endl;

	deque<string> string_dict;

	uint32_t i = 0;
	for (i = 0; i < working_dictionary->size; i++) {
		if (working_dictionary->words[i] == "")
			break;
		string_dict.push_back( working_dictionary->words[i] );
		if (i > working_dictionary->size) {
			cout << "what? i = " << i << ", size = " << working_dictionary->size << endl;
			break;
		}
	}

	cout << "done: i = " << i << ", size = " << working_dictionary->size <<
		", string_dict->size() = " << string_dict.size() << endl;
	if (string_dict.size() == 0) {
		cout << "There are no words..." << endl;
		exit(1);
	}
	cout << "There are " << std::dec << string_dict.size() << " words." << endl;
	cout << "first word  : " << string_dict[0].size() << ", " << string_dict[0] << endl;
	// cout << "second word : " << string_dict[1].size() << ", " << string_dict[1] << endl;

	std::uniform_int_distribution<> distrib(0, string_dict.size());

	cout << "Here are random words : " << endl;
	for (int i = 0; i < 6; i++)
		cout << " " << random_word( distrib, string_dict, altchars ) << endl;
	cout << "Here are random phrases : " << endl;
	for (int i = 0; i < 6; i++)
		cout << " " << phrase( distrib, string_dict, caps, altchars ) << endl;
	if (verbose) {
		cout << "caps = " << caps << ", altchars = " << altchars << ", numbers = " << numbers << endl;
	}
	return 0;
}
