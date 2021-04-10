FLP - Funkcionální projekt v jazyce Haskell

Autor: Jakub Sadílek
Login: xsadil07
Datum 11.4.2021

Překlad:
make

Spuštění:
./plg-2-nka < -i | -1 | -2 > [soubor]

    -i : vypíše načtenou gramatiku
    -1 : vypíše transformovanou gramatiku
    -2 : vypíše ekvivalentní konečný automat

Úklid:
make clean

Struktura:
Makefile : soubor pro překlad pomocí 'make'
/src     : obsahuje zdrojový kód
/doc     : obsahuje stručný popis programu a testů
/test    : obsahuje testy typu vstup/výstup

---------------- POPIS PROGRAMU ----------------
Program na začátku zkontroluje správnost argumentů (parseArgs), načte vstupní pravou lineární gramatiku (PLG) ze souboru nebo z stdin (loadInput) podle zadaných argumentů programu a uloží ji do vnitřní struktury pro gramatiku (loadGrammar). Během načítáni je ošetřený nepovolený vstupní formát gramatiky.

Na začátek je vhodné zmínit definici abecedy a gramatiky podle kurzu IFJ. 'Abeceda je konečná neprázdná množina elementů, kterým říkáme symboly'. 'Gramatika je čtveřice, která se skládá z abecedy neterminálů, abecedy terminálů, konečné množiny pravidel a počátečního neterminálu.' Tedy podle těchto definic umožňuje program zpracovat gramatiku bez pravidel, jelikož prázdná množina je také množina. Neterminály a terminály nemohou být prázdné, protože jsou definované jako abeceda, nikoli přímo množina.

Konkrétně gramatika je ošetřena tak, že nesmí obsahovat jednoduchá pravidla typu 'A->B', terminály musí být v rozsahu 'A-Z' a neterminály v rozsahu 'a-z', zároveň je ošetřeno, zda počáteční symbol patří do abecedy neterminálů a zda všechny symboly v pravidlech jsou definované. Kromě toho je ošetřen i tvar pravidel, aby odpovídal tvaru PLG. Zároveň nejsou povolené žádné duplicity, jelikož se v množině nevyskytují.

Pokud po načtení gramatiky je požadováno ji opět vypsat pomocí přepínače '-i', tak je vypsána na stdout ve funkci 'showGrammar'. Pokud je nutné gramatiku transformovat pomocí přepínače '-1', tak je prvně transformována ve funkci 'transformGrammar' a poté vypsána. Pro výpis automatu pomocí přepínače '-2' je transformovaná gramatika převedena na automat ve funkci 'loadFSM' a automat je uložen do struktury 'FSM'. Poté je vypsán ve funkci 'showFSM'.

Během jakékoliv chyby během zpracování programu je vypsána chybová hláška a program je ukončen.
