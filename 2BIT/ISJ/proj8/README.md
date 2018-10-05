# 8. Projekt

### Zadání:
S využitím modulu ```cProfile``` zjistěte, kolikrát je volána funkce ```lev()``` ve skriptu dostupném
na https://www.fit.vutbr.cz/study/courses/ISJ/private/lev.py.  
Výsledek uložte do souboru ```lev.cProfile_output```.  
  
Pomocí ```time``` zjistěte, jak dlouho běží skript dostupný
na https://www.fit.vutbr.cz/study/courses/ISJ/private/nonpalindrom_words_existing_reversed.py na souboru staženém
z https://www.fit.vutbr.cz/study/courses/ISJ/private/words30000.txt. Upravte skript tak, aby se daný čas co možná nejvíc zkrátil
a aby bylo možné zpracovat i podstatně větší soubor stažený z https://www.fit.vutbr.cz/study/courses/ISJ/private/words.txt.
Upravený skript pojmenujte ve formatu ```login_nonpalindrom_words_existing_reversed.py```.  
Výstup z ```time ./`id -u -n`_nonpalindrom_words_existing_reversed.py words.txt``` odevzdejte jako ```words.time_output```.  
  
Změřte čas běhu skriptů dostupných na adresách https://www.fit.vutbr.cz/study/courses/ISJ/private/sekv.py a
https://www.fit.vutbr.cz/study/courses/ISJ/private/para.py a odevzdejte je jako ```sekv.time_output``` a ```para.time_output```.
Do souboru ```why.txt``` napište třípísmennou zkratku, která vysvětlí, proč se reálný čas běhu paralelní verze neblíží polovině
běhu sekvenční verze.  
Upravte skript ```para.py``` tak, aby reálný čas zpracování odpovídal zhruba polovině času běhu sekvenční verze.

#### Hodnocení: 5/5b
