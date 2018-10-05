# 5. Projekt

### Zadání:  
Implementujte třídu Polynomial, která bude pracovat s polynomy reprezentovanými pomocí seznamů. Například 2x^3 - 3x + 1 bude  reprezentováno jako [1,-3,0,2] (seznam začíná nejnižším řádem, i když se polynomy většinou zapisují opačně).
    
Instance třídy bude možné vytvářet několika různými způsoby:  
pol1 = Polynomial([1,-3,0,2])  
pol2 = Polynomial(1,-3,0,2)  
pol3 = Polynomial(x0=1,x3=2,x1=-3)  
  
Volání funkce print() vypíše polynom v obvyklém formátu:  
> print(pol2)  
2x^3 - 3x + 1  
  
Bude možné porovnávat vektory porovnávat:  
> pol1 == pol2  
True  

Polynomy bude možné sčítat a umocňovat nezápornými celými čísly:  
> print(Polynomial(1,-3,0,2) + Polynomial(0, 2, 1))  
2x^3 + x^2 - x + 1  
> print(Polynomial(-1, 1) ** 2)  
x^2 - 2x  + 1  

A budou fungovat metody derivative() - derivace a at_value() - hodnota polynomu pro zadané x - obě pouze vrací výsledek, nemění samotný polynom:  
> print(pol1.derivative())  
6x^2 - 3  
> print(pol1.at_value(2))  
11  
> print(pol1.at_value(2,3))  
35  
(pokud jsou zadány 2 hodnoty, je výsledkem rozdíl mezi hodnotou at_value() druhého a prvního parametru - může sloužit pro výpočet určitého integrálu, ale ten nemá být implementován)  

#### Hodnocení: 5/5b
