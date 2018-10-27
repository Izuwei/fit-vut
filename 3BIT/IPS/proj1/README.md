# 1. Projekt - psed

### Zadání:  
Vytvořte program založený na vláknech, který bude provádět paralelní sed. Výsledná binárka "psed" bude mít "N" vstupních parametrů:  

```psed RE1 REPL1 [ RE2 REPL2 ] [ RE3  REPL3 ] ...```  

RE: regulární výráz (uvažujeme regulární výrazy užívané v C++11)

Minimální počet parametrů je 1, maximální není omezen.  
  
### Program psed funguje následovně:  
1) Spustí se "k" vláken, každé pro jeden regulární výraz. Vlákna čekají, až jim bude předáno řízení hlavním vláknem.  
2) Hlavní vlákno načte řádek ze standardního vstupu.  
3) Předá řízení pracovním vláknům, kde každé z nich provede náhradu řádku ze vstupu pomocí regulárního výrazu "REi". Výsledek bude "REPLi". Pokud na vstupu žádný řádek nebyl, jsou vlákna korektně ukončena a celý program končí.  
4) Pracovní vlákna paralelně provedou náhradu regulárního výrazu a vytisknou výsledek na standardní výstup, a to v pořadí daném parametry vstupu - tedy výsledek RE1 je vytisknut první, následuje RE2 atd..  
5) Pracovní vlákna předají řízení hlavnímu vláknu.  
6) Pokračuje se bodem 2.  

### Možnost použití následujících komponent z jazyka C++:  
- Regulární výrazy z C++1 (#include <regex>)  
- Vlákna a zámky z C++11 (#include <thread>, #include <mutex>)  
- Pole proměnné velikosti z C++1 (#include <vector>) pro uchování threadů a zámků.  
- Funkce int read_input_line(char *) a char *to_cstr(std::string a) z předpřipravené kostry.  

Projekt byl vypracován ve dvojici.  

#### Hodnocení 10/10b
