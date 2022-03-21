#! /usr/bin/env python3

import os.path
import sys

EN_DICT = '/usr/share/hunspell/en_CA.dic'
FR_DICT = '/usr/share/hunspell/fr_CA.dic'
SW_DICT = '/usr/share/hunspell/sw_TZ.dic'

def get_dict(dict_name):
	with open(dict_name, 'r') as file:
		return file.readlines(-1)

def clean(s):
	return s.strip().split('/', 1)[0]

def process(dict_name, cpp_name):
	dict = get_dict(dict_name)
	(lang, ext) = os.path.splitext(os.path.basename(cpp_name))
	with open(cpp_name, 'w') as cpp:
		print('#include <string>', file=cpp)
		print('using std::string;', file=cpp)
		print('extern const string {}[] = {{'.format(lang), file=cpp )
		for word in dict[1:-1]:
			clean_word = clean(word)
			if (len(clean_word) > 2 and len(clean_word) < 12):
				print( '\t\"', clean_word, file=cpp, end='\",\n', sep='' )
		print('\t\"\",', file=cpp)
		print('};', file=cpp)
		print('extern const size_t {0}_size = sizeof({0});'.format(lang), file=cpp)


if (len(sys.argv) == 3):
	print(sys.argv[1], "-->", sys.argv[2])
	process(sys.argv[1], sys.argv[2])

else:
	process(EN_DICT, 'en_CA_dict.cpp')
	process(FR_DICT, 'fr_CA_dict.cpp')
	process(SW_DICT, 'sw_TZ_dict.cpp')


