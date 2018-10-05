# 1. Projekt - WEDI

#### Zadání:  
Cílem úlohy je vytvořit skript (tzv. wrapper), který bude spouštět textový editor.
Skript si bude pamatovat, které soubory byly v jakém adresáři prostřednictvím
skriptu wedi editovány. Pokud bude skript spuštěn bez parametrů, vybere skript
soubor, který má být editován.

##### Popis: 

•Pokud byl skriptu zadán soubor, bude editován.
• Pokud zadaný argument odpovídá existujícímu adresáři, skript z daného
adresáře vybere soubor pro editaci. Výběr souboru je následující.
– Pokud bylo v daném adresáři editováno skriptem více souborů, vybere
se soubor, který byl pomocí skriptu editován jako poslední. Editací
souboru se myslí třeba i prohlížení jeho obsahu pomocí skriptu (tj.
není nutné, aby byl soubor změněn).
– Pokud byl zadán argument -m, vybere se soubor, který byl pomocí
skriptu editován nejčastěji.
– Pokud nebyl v daném adresáři editován ještě žádný soubor, jedná se
o chybu.
• Pokud nebyl zadán adresář, předpokládá se aktuální adresář.
• Skript dokáže také zobrazit seznam všech souborů (argument -l), které
byly v daném adresáři editovány.
• Pokud byl zadán argument -b resp. -a (before, after), skript zobrazí
seznam souborů, které byly editovány před resp. po zadaném datu v čase
00:00:00.0 včetně. DATUM je formátu YYYY-MM-DD.


#### Hodnocení: 15/15b
