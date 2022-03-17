//
// passphrase.cpp
//
//  Generate some password phrases from dictionary words
//

#include <cstdio>
#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>
#include <deque>
#include <random>

using std::cout;
using std::endl;
using std::string;
using std::deque;

extern const char _binary_en_CA_dic_start[];
extern const char _binary_en_CA_dic_end[];

extern const char _binary_fr_CA_dic_start[];
extern const char _binary_fr_CA_dic_end[];

extern const char _binary_sw_TZ_dic_start[];
extern const char _binary_sw_TZ_dic_end[];

#define DICT_SIZE( x ) \
	(uintptr_t)(_binary_ ## x ## _dic_end - _binary_ ## x ## _dic_start)

typedef struct {
	uintptr_t size;
	const char *start;
	const string name;
} Dictionary;

const Dictionary dictionary[] = {
	{ DICT_SIZE( en_CA ), _binary_en_CA_dic_start, "en_CA" },
	{ DICT_SIZE( fr_CA ), _binary_fr_CA_dic_start, "fr_CA" },
	{ DICT_SIZE( sw_TZ ), _binary_sw_TZ_dic_start, "sw_TZ" },
};

std::random_device rd;
std::mt19937 gen(rd());

string random_word(
	std::uniform_int_distribution<> &dist, deque<string> &dict,
	bool want_apostrophe = false)
{
	string candidate;
	if (want_apostrophe) {
		string::size_type apostrophe;
		do {
			candidate = dict[ dist(gen) ];
			apostrophe = candidate.find( "'" );
		}
		while (apostrophe == string::npos);
	}
	else candidate = dict[ dist(gen) ];

	string::size_type slash = candidate.rfind( "/" );
	if (slash != string::npos) {
		return candidate.substr(0, slash);
	}
	return candidate;
}

string phrase(
	std::uniform_int_distribution<> &dist, deque<string> &dict)
{
	string p = random_word(dist, dict, true);

	while (p.length() < 12) {
		p += random_word(dist, dict);
	}
	return p;
}

int main(int argc, char *argv[])
{
	cout << "Hello, world!\nReady for some passwords?" << endl;

	for (auto i : dictionary) {
		cout << i.name << " is : " << std::hex << std::showbase << i.size << " bytes" << endl;
	}

	deque<string> string_dict;

	const char *word_start = _binary_en_CA_dic_start;
	for (auto w = _binary_en_CA_dic_start;
		w < _binary_en_CA_dic_end; w++) {
		if (*w == '\n') {
			string_dict.push_back( string( word_start, w - word_start ) );
			word_start = w + 1;
		}
	}

	cout << "There are " << std::dec << string_dict.size() << " words." << endl;

	std::uniform_int_distribution<> distrib(0, string_dict.size());

	cout << "Here are random words : " << endl;
	for (int i = 0; i < 6; i++)
		cout << " " << random_word( distrib, string_dict ) << endl;
	cout << "Here are random phrases : " << endl;
	for (int i = 0; i < 6; i++)
		cout << " " << phrase( distrib, string_dict ) << endl;
	return 0;
}
