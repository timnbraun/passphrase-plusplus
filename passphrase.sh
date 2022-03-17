#! /bin/sh
DICT=/usr/share/hunspell/en_CA.dic
for i in `seq 4`; do head -n $(($(od -N 2 /dev/urandom|cut -d' ' -f2 -s)%51244)) ${DICT} | tail -n1; done
