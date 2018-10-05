# 4. Projekt

### Zadání:
can_be_a_set_member_or_frozenset(item) - vrátí vstupní položku item, pokud tato může být prvkem množiny v Pythonu,
v opačném případě frozenset(item)  
Můžete si zkusit:  
    assert can_be_a_set_member_or_frozenset(1) == 1  
    assert can_be_a_set_member_or_frozenset((1,2)) == (1,2)  
    assert can_be_a_set_member_or_frozenset([1,2]) == frozenset([1,2])  
V Pythonu nemůže být prvkem množiny jiná množina. Měli byste vědět proč.  
      
all_subsets(lst) - na vstupu dostane seznam a pouze s použitím vestavěných funkcí (tedy bez použití "import")
z něj vytvoří seznam, odpovídající množině všech podmnožin, tedy např.  
    assert all_subsets(['a', 'b', 'c']) == [[], ['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]  
Návod: Množina všech podmnožin prázdného seznamu (prázdné množiny) má jeden prvek - prázdný seznam (množinu).
Přidejte jej na začátku do vytvářeného výstupního seznamu.
Pro každý další prvek platí, že množina všech podmnožin obsahuje všechny podmnožiny, které daný prvek
neobsahují (předchozí stav), a podmnožiny, které daný prvek obsahují (do nichž byl daný prvek přidán).
Tedy když jsem narazil ve výše uvedém příkladě na prvek 'c', přidal jsem jej nejprve k prázdnému seznamu,
potom k seznamům ['a'], ['b'] a nakonec k ['a', 'b']).  
Poznámka: Lze to samozřejmě dělat i jinak, ale aby vyšel assert, použijte prosím popsaný postup.
  
all_subsets_excl_empty - obdoba předchozího, ale při volání dostane prvky seznamu přímo jako argumenty a navíc má volitelný parametr
exclude_empty, který, když není ve volání uveden, nebo je jeho hodnota True, vrátí výsledek bez prázdného seznamu.
Pokud je hodnota tohoto argumentu False, je výsledek stejný jako u předchozí funkce.   
Takže např.:  
    assert all_subsets_excl_empty('a', 'b', 'c') == [['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]  
    assert all_subsets_excl_empty('a', 'b', 'c', exclude_empty = True) == [['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'],
    ['a', 'b', 'c']]  
    assert all_subsets_excl_empty('a', 'b', 'c', exclude_empty = False) == [[], ['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'],
    ['b', 'c'], ['a', 'b', 'c']]  
#### Hodnocení: 5/5b  
