# 2. Projekt - The Senate Bus Problem

### Zadání:

Implementujte v jazyce C modifikovaný synchronizační problém The Senate Bus Problem.
Osoby (riders) přicházejí postupně na zastávku a čekají na autobus (bus). V okamžiku příjezdu
autobusu nastoupí všechny čekající osoby. Kapacita autobusu je limitována. Pokud je na nástupišti
více osob, než je kapacita autobusu, čekají osoby, které se již do autobusu nevešly, na další autobus.
Pokud stojí na zastávce autobus a nastupují osoby přítomné na zastávce, další příchozí osoby vždy
čekají na další autobus. Po nastoupení osob autobus odjíždí. Pokud při příjezdu na zastávku nikdo
nečeká, autobus odjíždí prázdný.
Příklad: Kapacita autobusu je 10, čekajících osob je 8. Přijíždí autobus, osoby nastupují, přicházejí
další 3 osoby. Nastoupí pouze 8 čekajících osob, autobus odjíždí a nově příchozí 3 osoby čekají na zastávce.

### Detaily:
>• Každé osobě odpovídá jeden proces rider.  
• Autobus je reprezentován procesem bus. V systému je právě jeden autobus.  

### Spuštění:
```$ ./proj2 R C ART ABT```  
kde:  
>• R je počet procesů riders; A > 0.  
• C je kapacita autobusu; C > 0.  
• ART je maximální hodnota doby (v milisekundách), po které je generován nový proces rider;  
ART >= 0 && ART <= 1000.  
• ABT je maximální hodnota doby (v milisekundách), po kterou proces bus simuluje jízdu;  
ABT >= 0 && ABT <= 1000.  
• Všechny parametry jsou celá čísla  

#### Hodnocení: 15/15b
