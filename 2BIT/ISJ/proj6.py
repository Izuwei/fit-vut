#!/usr/bin/env python3
# AUTOR: Jakub Sadilek
# LOGIN: xsadil07
# DNE: 19.4.2018

import operator
from itertools import permutations
from fractions import Fraction

def first_nonrepeating(string):
    '''Funkce vrati prvni neopakujici znak ve stringu, jinak none.'''
    if type(string) != str or string.isspace(): # Kontrola vstupu.
        return None
    order = [] # Pro uchovani poradi znaku.
    counter = {} # Pocitadlo znaku.
    for char in string: # Cyklime s kazdym znakem v retezci.
        if char in counter: # Pokud znak byl jiz nalezen, zvysime pocitadlo, jinak pridame novy znak.
            counter[char] += 1
        else:
            counter[char] = 1 
            order.append(char)
    for char in order:  # Pokud je vyskyt znaku == 1, vratime znak, jinak none.
        if counter[char] == 1:
            return char
    return None

operations = dict() # List znaku operaci s prirazenou funkcialitou.
operations['+'] = operator.add
operations['-'] = operator.sub
operations['/'] = operator.truediv
operations['*'] = operator.mul

def combine4(numbers, exp_result):
    '''Dostane ctverici 4 kladnych celych cisel a ocekavany vysledek a vrati setrideny seznam unikatnich reseni matematických hadanek s vysledkem operaci +, -, *, / nad 4 cisly.'''
    results = find_results(exp_result, ([Fraction(x) for x in numbers])) # Vsechny nalezene vysledky
    to_ret = [] # Promena, kterou vratime ve finalni podobe podle zadani.
    for each in results:
        if each.startswith("(") and each.endswith(")"): # Odstranime poc. a kon. zavorky.
            each = each[1:-1]
        to_ret.append(each) # Kazdy prvek pridame do vysledneho retezce.
    if len(to_ret) == 0: # Pokud list je prazny vratime ho, jinak vratime polozky bez duplikaci.
        return to_ret
    else:
        return set(to_ret)

def find_results(result, numbers):
    '''Funkce rekurzivne vyhleda vysledek. K funkci combine4.'''
    if len(numbers) == 1: # Podminka ukonceni rekurze.
        if numbers[0] == result:
            yield str(result)
        return

    for num1, num2 in permutations(numbers, 2): # Kombinuje par cisel s operaci, pak znovu.
        for symbol, operation in operations.items():
            try: # Pokus operace s danymi cisli, pokud nastala chyba (napr: deleni nulou) skocime na zacatek cyklu, jinak pokracujeme.
                product = operation(num1, num2)
            except ZeroDivisionError:
                continue
            subnum = list(numbers) # Vytvorime list hodnot, odstranime prvni dve (a,b), pote pridame jejich kombinaci.
            subnum.remove(num1)
            subnum.remove(num2)
            subnum.append(product)
            for solution in find_results(result, subnum): # Rozsirime product (jednou).
                yield solution.replace(str(product), "({0}{1}{2})".format(num1, symbol, num2), 1)

print (combine4([6, 6, 5, 2], 36))