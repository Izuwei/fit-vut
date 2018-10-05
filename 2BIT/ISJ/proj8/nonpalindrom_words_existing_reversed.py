#!/usr/bin/env python3

import fileinput

words = set([])
for line in fileinput.FileInput():
    words.add(line.rstrip())

palindroms = set([w for w in words if w == w[::-1]])

result = [w for w in words if w not in palindroms and w[::-1] in words]

print(sorted(result))
