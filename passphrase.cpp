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
#include <cstddef>
#include <string>
#include <sstream>
#include <deque>
#include <random>
#include <getopt.h>
#include <libgen.h>

#include <en_CA_dict.h>
#include <fr_CA_dict.h>
#include <sw_TZ_dict.h>

using namespace std;

#define DICT_ENTRY( x ) \
	# x, x ## _dict_size, x ## _dict_count, x ## _dict

typedef const char *charptr_t;
struct RawDictionary {
	string name;
	size_t size;
	size_t count;
	charptr_t words;
};

typedef deque<const char *> Dictionary;

const RawDictionary dictionary[] = {
	{ DICT_ENTRY( en_CA ) },
	{ DICT_ENTRY( fr_CA ) },
	{ DICT_ENTRY( sw_TZ ) },
	{}
};

random_device rd;
mt19937 gen(rd());

int caps = 1, numbers = 1, altchars = 1, verbose = 0;

static struct option opts[] = {
	{"help",         no_argument,       0,         '?'},
	{"usage",        no_argument,       0,         '?'},
	{"caps",         no_argument,       &caps,     1  },
	{"no-caps",      no_argument,       &caps,     0  },
	{"numbers",      no_argument,       &numbers,  1  },
	{"no-numbers",   no_argument,       &numbers,  0  },
	{"alt-chars",    no_argument,       &altchars, 1  },
	{"no-alt-chars", no_argument,       &altchars, 0  },
	{"lang",         required_argument, 0,         'l'},
	{"verbose",      no_argument,       &verbose,  1  },
	{"version",      no_argument,       0,         'v'},
	{}
};

/////////////////
//
// Forward function declarations
//
/////////////////
string random_word(
	uniform_int_distribution<> &dist, Dictionary &dict,
	bool want_apostrophe = false);

string phrase(
	uniform_int_distribution<> &dist, Dictionary &dict,
		bool cap = false, bool want_alt = false, bool num = false);

Dictionary parse_dict(const char *words, size_t size);


void inline help(string prog)
{
	cout << prog << " [-hcnavV] [-l en_CA | fr_CA | sw_TZ ]" << endl;
}

//////////////////////
//
//  main
//
//////////////////////
int main(int argc, char *argv[])
{
	string lang = "en_CA";

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
			cout << "V0.01-" VERSION << " built " << DATE << endl;
			exit(0);
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
			if (argc > optind) {
				if (verbose) {
					cout << optind << " : " << argv[optind] << endl;
				}
			}
			break;
		}
	}

	if (verbose) {
		cout << "Hello, world!\nReady for some passwords?" << endl;
		for (auto i : dictionary) {
			if (i.size == 0 || i.count == 0)
				continue;
			cout << i.name << " is : " << dec << i.count << " words, " << i.size <<
				" characters, start = " <<
				hex << showbase << (intptr_t)i.words << endl;
		}
	}

	const RawDictionary *working_dictionary = &dictionary[0];
	for (auto d : dictionary) {
		if (d.size == 0)
			continue;
		if (d.name == lang) {
			if (verbose)
				cout << "Got lang " << d.name << ", size = " << d.count << endl;
			break;
		}
		if (verbose)
			cout << d.name << " != " << lang << endl;
		working_dictionary++;
	}
	if (!working_dictionary) {
		help(basename(argv[0]));
		exit(1);
	}

	if (verbose) {
		cout << "working dict size = " << dec << working_dictionary->size
			<< "chars, first word : " << working_dictionary->words << endl;
	}

	Dictionary string_dict = parse_dict(working_dictionary->words,
		working_dictionary->size );

	if (string_dict.size() == 0) {
		cout << "There are no words..." << endl;
		exit(1);
	}

	if (verbose) {
		cout << "There are " << dec << string_dict.size() <<
			" words, first word : " << string_dict[0] << endl;
	}

	uniform_int_distribution<> distrib(0, string_dict.size() - 1);

	if (verbose) {
		cout << "Here are random words : " << endl;
		for (int i = 0; i < 6; i++)
			cout << " " << random_word( distrib, string_dict, altchars ) << endl;
	}

	if (verbose)
		cout << "Here are random phrases : " << endl;

	/////////
	//
	//  The big reveal: a list of random phrases
	//
	/////////
	for (int i = 0; i < 5; i++)
		cout << phrase( distrib, string_dict, caps, altchars, numbers ) << endl;

	if (verbose) {
		cout << "caps = " << caps << ", altchars = " << altchars << ", numbers = " << numbers << endl;
	}
	return 0;
}

static char replacements[] = { '%', '_', '-', '+', '*' };

static uniform_int_distribution<> replacement_dist(0, sizeof(replacements) - 1);

string random_word(
	uniform_int_distribution<> &dist, Dictionary &dict,
	bool want_apostrophe)
{
	string candidate;
	string::size_type apostrophe = string::npos;

	if (dict.size() == 0)
		return "";

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
		(candidate.size() < 2 || candidate.size() > 6));

	if (apostrophe != string::npos) {
		char replacement = replacements[ replacement_dist(gen) ];
		candidate[apostrophe] = replacement;
	}

	return candidate;
}

string phrase(
	uniform_int_distribution<> &dist, Dictionary &dict,
		bool cap, bool want_alt, bool num)
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
		have_alt = p.find_first_of( replacements ) != string::npos;
	}

	string::size_type num_pos = string::npos;
	if (num) {
		string l33t_chars = "beostzBEOSTZ";

		num_pos = p.find_first_of( l33t_chars );
		if (num_pos != string::npos) {
			switch (tolower(p[num_pos])) {
			case 'b':
				p[num_pos] = '8';
			break;
			case 'e':
				p[num_pos] = '3';
			break;
			case 'o':
				p[num_pos] = '0';
			break;
			case 's':
				p[num_pos] = '5';
			break;
			case 't':
				p[num_pos] = '7';
			break;
			case 'z':
				p[num_pos] = '2';
			break;
			}
		}
		else {
			// do we need to limit the recursion?
			return phrase(dist, dict, cap, want_alt, num );
		}
	}
	return p;
}


Dictionary parse_dict(const char *words, size_t size)
{
	Dictionary new_dict;

	size_t i = 0;
	const char *word_start = words;
	for (i = 0; i < size && *word_start != '\0'; i++, words++) {

		if (*words == '\0') {
			new_dict.push_back( word_start );
			word_start = words + 1;
		}

		if (i > size) {
			cout << "what? i = " << i << ", size = " << size << endl;
			break;
		}
	}

	return new_dict;
}
