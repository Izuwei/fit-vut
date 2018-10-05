#!/usr/bin/env python3
# AUTOR: Jakub Sadílek
# LOGIN: xsadil07
# DNE: 5.4.2018

import re

class Polynomial():
    '''Třída polynom, podle zadání.'''
    def __init__(self, *args, **kwargs):
        '''Inicializace koeficientů polynomu (podle typu argumentů).'''
        if len(args) == 1 and len(kwargs) == 0 and isinstance(args, tuple): # Kontrola 2. typu arg.
            self.coeffs = args[0]
        elif len(args) > 0 and len(kwargs) == 0:    # Kontrola 1. typu arg.
            self.coeffs = list(args)
        elif len(args) == 0 and len(kwargs) > 0: # Kontrola 3. typu arg.
            exp = []	# List pro exponenty.
            base = []	# List pro hodnoty exponentů.
            for each in kwargs.items():	# Cyklíme s každým parametrem.
                    exp.append(each[0][1:])	# Přidáme exponent na konec seznamu.
                    base.append(each[1])	# Přidáme hodnotu na konec seznamu.
                    
            exp, base = (list(t) for t in zip(*sorted(zip(exp, base))))	# Seřadíme hodnoty a exponenty podle hodnot exponentů.		
            
            self.coeffs = []
            i = 0	# Počítadlo pro procházení každého exponentu.
            e = 0	# Počítadlo pro procházení hodnot v původním listu.
            while i <= int(exp[-1]):	# Cyklíme pro každý exponent.
                if i == int(exp[e]):	# Pokud exponent byl v originálním listu.
                    self.coeffs.append(int(base[e]))	# Přidáme exponent na konec listu.
                    e +=1
                else:
                    self.coeffs.append(0)	# Přidáme nulu na konec listu. 
                i += 1

    def __repr__(self):   # Vypis polynomu.
        '''Reprezentace polynomu při výpisu (podle zadání).'''
        out = ""
        lenght = len(self.coeffs) - 1
        for item in self.coeffs[::-1]:    # Pro každá prvek v listě, obrácený (podle zadání).
            if item != 0:   # Koeficient 0 ignorujeme.
                out += " + %g*x^%d" % (item, lenght)
            lenght-=1
        # Zaměňujeme znaky podle mat. zákonů.
        out = out.replace("+ -", "- ")
        out = out.replace("x^1", "x")
        out = out.replace("1*", "")
        out = out.replace("*x^0", "")
        out = out.replace("x^0", "1")
        out = out.replace("*", "")
        # Odstraníme počáteční znaménka.
        if out[0:3] == " + ":
            out = out[3:]
        elif out[0:3] == " - ":
            out = out[3:]
        elif out == "":
            out = "0"
        return out

    def __eq__(self, other): # Rovnost
        '''Porovnávání polynomů.'''
        return str(self) == str(other) # Pokud řetězce polynomů jsou stejné, tak True, jinak False.

    def __add__(self, other): # Sčítání
        '''Sčítání polynomů.'''
        if len(self.coeffs) > len(other.coeffs):	# Hledáme polynom s větším exponentem.
            bigger = self.coeffs[:]
            smaller = other.coeffs[:]
        else:
            bigger = other.coeffs[:]
            smaller = self.coeffs[:]
            
        i = 0		
        while i < len(smaller):	# Cyklíme s každým prvkem kratšího polynomu.
            bigger[i] += smaller[i]	# Přičteme hodnotu kratšího polynomu do většího  
            i += 1
        return Polynomial(bigger)

    def __pow__(self, power): # Umocnění
        '''Umocňování polynomů.'''
        if power == 0:  # Kontrola exponentu.
            return "0"
        elif power == 1:
            return self

        tmp = self.coeffs[:]    # Kopirujeme koeficienty do listu.
        for interaction in range(power - 1):    # Cyklíme podle mocniny.
            result = [0]*(len(tmp) + 1)   # Vytvoříme prázdný list pro výsledek.
            for i in range(len(self.coeffs)):
                for j in range(len(tmp)):
                    result[i+j] += self.coeffs[i] * tmp[j] # Nasobíme.
            tmp = result    # Dočasně uchováme výsledek.
        return Polynomial(result) # Vracíme výsledek jako polynom.

    def derivative(self): # Derivace
        '''Derivace polynomu.'''
        if len(self.coeffs) == 1:   # Pokud polynom obsahuje pouze nejnižší koef. => 0.
            return "0"

        result = []
        counter = 0 # Pro počítání interací.
        for each in self.coeffs: # Každý prvek násobíme počtem interací což odpovídá exponentům.
            result.append(each * counter)
            counter += 1
        result = result[1:] # Odstraníme první prvek (nejnižší).
        return Polynomial(result)

    def at_value(self, *args):
        '''Vrací hodnotu polynomu pro zadané x.'''
        if len(args) == 1:  # Pokud je 1 argument vracíme hodnotu polynomu.
            return self.get_value(args[0])
        else:   # Jinak vracíme jejich rozdíl.
            return self.get_value(args[1]) - self.get_value(args[0])

    def get_value(self, number):    # Pomocná funkce k get_value.
        '''Funkce pro vypočet polynomu pro zadané x. Pomocná funkce k at_value.'''
        result = exp = 0
        for each in self.coeffs:
            result += each * (number**exp) # Vypočítáme hodnotu polynomu a přičteme.
            exp += 1    # S cyklem zvyšujeme mocninu což odpovídá řádům.
        return result

def test(): # Testy
    assert str(Polynomial(0,1,0,-1,4,-2,0,1,3,0)) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x"
    assert str(Polynomial([-5,1,0,-1,4,-2,0,1,3,0])) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x - 5"
    assert str(Polynomial(x7=1, x4=4, x8=3, x9=0, x0=0, x5=-2, x3= -1, x1=1)) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x"
    assert str(Polynomial(x2=0)) == "0"
    assert str(Polynomial(x0=0)) == "0"
    assert Polynomial(x0=2, x1=0, x3=0, x2=3) == Polynomial(2,0,3)
    assert Polynomial(x2=0) == Polynomial(x0=0)
    assert str(Polynomial(x0=1)+Polynomial(x1=1)) == "x + 1"
    assert str(Polynomial([-1,1,1,0])+Polynomial(1,-1,1)) == "2x^2"
    pol1 = Polynomial(x2=3, x0=1)
    pol2 = Polynomial(x1=1, x3=0)
    assert str(pol1+pol2) == "3x^2 + x + 1"
    assert str(pol1+pol2) == "3x^2 + x + 1"
    assert str(Polynomial(x0=-1,x1=1)**1) == "x - 1"
    assert str(Polynomial(x0=-1,x1=1)**2) == "x^2 - 2x + 1"
    pol3 = Polynomial(x0=-1,x1=1)
    assert str(pol3**4) == "x^4 - 4x^3 + 6x^2 - 4x + 1"
    assert str(pol3**4) == "x^4 - 4x^3 + 6x^2 - 4x + 1"
    assert str(Polynomial(x0=2).derivative()) == "0"
    assert str(Polynomial(x3=2,x1=3,x0=2).derivative()) == "6x^2 + 3"
    assert str(Polynomial(x3=2,x1=3,x0=2).derivative().derivative()) == "12x"
    pol4 = Polynomial(x3=2,x1=3,x0=2)
    assert str(pol4.derivative()) == "6x^2 + 3"
    assert str(pol4.derivative()) == "6x^2 + 3"
    assert Polynomial(-2,3,4,-5).at_value(0) == -2
    assert Polynomial(x2=3, x0=-1, x1=-2).at_value(3) == 20
    assert Polynomial(x2=3, x0=-1, x1=-2).at_value(3,5) == 44
    pol5 = Polynomial([1,0,-2])
    assert pol5.at_value(-2.4) == -10.52
    assert pol5.at_value(-2.4) == -10.52
    assert pol5.at_value(-1,3.6) == -23.92
    assert pol5.at_value(-1,3.6) == -23.92

if __name__ == '__main__':
    test()
