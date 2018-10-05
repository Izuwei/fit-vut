#!/usr/bin/env python3
# AUTOR: Jakub Sadílek
# LOGIN: xsadil07
# DNE: 29.3.2018

def can_be_a_set_member_or_frozenset(item):
    '''Vrátí vstupní položku item, pokud může být prvkem množiny v Pythonu, v opačném případě frozenset(item).'''
    try:    # Testujeme, jestli item může být v setu.
        tmp = set([item])
    except: # Pokud nastala chyba, tak vracíme item v frozenset.
        return frozenset(item)
    else:   # Pokud nenastala chyba, tak vracíme vstup.
        return item

def all_subsets(lst):
    '''Ze vstupního seznamu vytvoří seznam odpovídající množině všech podmnožin.'''
    set1 = [[]]
    set2 = [[]]
    for item in lst: # Každé zadané písmenko.
        for each in set1:   # Každý již získaný prvek rozšíříme.
            each.append(item)
        set2.extend(set1)   # Rozšířenými prvky rozšíříme původní množinu.
        del set1[:]
        for i in range(len(set2)):  # Hluboka kopie.
            set1.append((set2[i])[:])
    return set2


def all_subsets_excl_empty(*args, exclude_empty=True):
    '''Obdoba předchozího, ale při volání dostane prvky seznamu přímo jako argumenty a navíc má volitelný parametr exclude_empty,
    který když není ve volání uveden, nebo je jeho hodnota True, vrátí výsledek bez prázdného seznamu.
    Pokud je hodnota tohoto argumentu False, je výsledek stejný jako u předchozí funkce.'''
    inputs = tuple(args)    # Z argumentů vytvoříme množinu.
    if exclude_empty == False:
        return all_subsets(inputs)  # Využijeme naší def. funkci a vrátíme výsledek pokud exclude_empty = False.
    else:
        sets=all_subsets(inputs)
        sets.remove([]) # Jinak opět Využijeme naší funkci, ale odebereme prázdnou podmnožinu.
        return sets

'''Testy k funkcím'''
assert can_be_a_set_member_or_frozenset(1) == 1
assert can_be_a_set_member_or_frozenset((1,2)) == (1,2)
assert can_be_a_set_member_or_frozenset([1,2]) == frozenset([1,2])
assert all_subsets(['a', 'b', 'c']) == [[], ['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]
assert all_subsets_excl_empty('a', 'b', 'c') == [['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]
assert all_subsets_excl_empty('a', 'b', 'c', exclude_empty = True) == [['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]
assert all_subsets_excl_empty('a', 'b', 'c', exclude_empty = False) == [[], ['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]