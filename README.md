# passphrase-plusplus

C++, bash and python implementations of passphrase generator.

usage: passphrase [-h] [-n | --no-numbers] [-c | --no-caps] [-a | --no-altchars]

Generate some random passwords

optional arguments:
  --caps             Capitalization (default, use --no-caps to turn off)
  -c, --no-caps
  --numbers          l33t numbers (default, use --no-numbers to turn off)
  -n, --no-numbers
  --altchars         alternate characters (default, use --no-altchars to turn off)
  -a, --no-alt-chars
  --word-min         minimum length of words in the phrase, default=2
  --word-max         maximum length of words in the phrase, default=6
  --length           minimum length of the phrase, default=12
  --max-length       maximum length of the phrase, default=18
  --count            how many to phrases generate, default=5
  --verbose
  -v, --version   show version and exit
  -h, --help      show this help message and exit

passphrase.py is similar, may be more suitable for web use.
