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

with open(DICT, 'r') as file:
	dict = file.readlines(-1)

def clean():
	s = choice(dict)
	return s.strip().split('/', 1)[0]

def goodchoice(want_apostrophe = False):
	w = clean()
	while (len(w) > 6) or (len(w) < 2) or (want_apostrophe ^ ("'" in w)):
		w = clean()
	return w

def goodword(altchars, caps):
	p = goodchoice()
	while (len(p) < 12):
		new_word = goodchoice(altchars and (not "'" in p))
		p =  p + (new_word.capitalize() if caps else new_word)
	return p.replace("'", choice( ALTCHARS )) if altchars else p

# get a digit somehow - t to 7, b to 8, a to 4, s to 5,e to 3 
def l33tphrase(altchars, caps):
	w = goodword(altchars, caps)
	if 't' in w:
		return w.replace('t', '7', 1) 
	if 'b' in w:
		return w.replace('b', '8', 1) 
	if 'a' in w:
		return w.replace('a', '4', 1) 
	if 's' in w:
		return w.replace('s', '5', 1) 
	return w.replace('e', '3', 1)


parser = argparse.ArgumentParser(description='Generate some random passwords')
parser.add_argument('-n', '--numbers', help='l33t numbers', action='store_true', default=True)
parser.add_argument('--no-numbers', help='no l33t numbers', action='store_false', dest='numbers')
parser.add_argument('-c', '--caps', help='Capitalization', action='store_true', default=True)
parser.add_argument('--no-caps', help='no capitalization', action='store_false', dest='caps')
parser.add_argument('-a', '--altchars', help='alternate characters', action='store_true', default=True)
parser.add_argument('--no-altchars', help='no alternate characters', action='store_false', dest='altchars')
args = parser.parse_args()


for i in range(5):
	print( l33tphrase(args.altchars, args.caps) if args.numbers else goodword(args.altchars, args.caps) )
