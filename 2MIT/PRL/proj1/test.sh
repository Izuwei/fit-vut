#!/bin/bash

numbers=16;

#preklad cpp zdrojaku
mpic++ --prefix /usr/local/share/OpenMPI -o pms pms.cpp


#vyrobeni souboru s random cisly
dd if=/dev/random bs=1 count=$numbers of=numbers &> /dev/null

#spusteni
mpirun --oversubscribe --prefix /usr/local/share/OpenMPI -np 5 pms

#uklid
rm -f pms numbers
