#! /usr/bin/env python3

#* 
#* passPhraseGenerator - Generate pronounceable passwords
#  This program uses a random series of short dictionary words
#
#  tnb  09/09/01 port to c#, command line
#  tnb  16/07/08 convert to using dictionary
#  tnb	18/02/23 add command line options for camel-case and l33t numbers
#*/

from random import choice
import argparse

DICT = '/usr/share/hunspell/en_CA.dic'
ALTCHARS = '-+_%'

parser = argparse.ArgumentParser(description='Generate some random passwords')
parser.add_argument('--numbers', help='l33t numbers', action='store_true', default=True)
parser.add_argument('-n', '--no-numbers', help='no l33t numbers', action='store_false', dest='numbers')
parser.add_argument('--caps', help='Capitalization', action='store_true', default=True)
parser.add_argument('-c', '--no-caps', help='no capitalization', action='store_false', dest='caps')
parser.add_argument('--altchars', help='alternate characters', action='store_true', default=True)
parser.add_argument('-a', '--no-altchars', help='no alternate characters', action='store_false', dest='altchars')
parser.add_argument('--count', help='how many passwords do you want', default=5, type=int)
parser.add_argument('--length', help='minimum password length', default=12, 
	dest='phrase_min', type=int)
parser.add_argument('--max-length', help='maximum password length', default=18, 
	dest='phrase_max', type=int)
parser.add_argument('--word-max', help='maximum word length', default=6, type=int)
parser.add_argument('--word-min', help='minimum word length', default=2, type=int)
args = parser.parse_args()

if (args.word_min > args.word_max):
	print('Impossible options, word_min', args.word_min, '> word_max', args.word_max)
	exit(1)

if (args.phrase_min > args.phrase_max):
	print('Impossible options, phrase_min', args.phrase_min, '> phrase_max', args.phrase_max)
	exit(1)

with open(DICT, 'r') as file:
	dict = file.readlines(-1)

def clean():
	s = choice(dict)
	return s.strip().split('/', 1)[0]

def random_word(want_apostrophe = False):
	w = clean()
	while ((len(w) > args.word_max) or
		  (len(w) < args.word_min) or
		  (want_apostrophe ^ ("'" in w))):
		w = clean()
	return w

def phrase(altchars, caps):
	p = random_word()
	while (len(p) < args.phrase_min):
		new_word = random_word(altchars and (not "'" in p))
		p =  p + (new_word.capitalize() if caps else new_word)
		if ((((args.phrase_min - len(p)) < args.word_min) and
			len(p) < args.phrase_min) or 
			(len(p) > args.phrase_max)):
			# start over
			# print('reset: min', args.phrase_min, 'len(p)', len(p), 'word_min', args.word_min)
			# print('  p', p, 'phrase_max', args.phrase_max)
			p = random_word()
	return p.replace("'", choice( ALTCHARS )) if altchars else p

# get a digit somehow - t to 7, b to 8, a to 4, s to 5,e to 3 
def l33tphrase(altchars, caps):
	w = phrase(altchars, caps)
	if 't' in w:
		return w.replace('t', '7', 1) 
	if 'b' in w:
		return w.replace('b', '8', 1) 
	if 'a' in w:
		return w.replace('a', '4', 1) 
	if 's' in w:
		return w.replace('s', '5', 1) 
	return w.replace('e', '3', 1)



for i in range(args.count):
	print( l33tphrase(args.altchars, args.caps) if args.numbers else phrase(args.altchars, args.caps) )
