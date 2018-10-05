# 1. Projekt

### Zadání:
Dopište definici regulárního výrazu word_parse tak, aby do skupin rozděloval slova z řetězce, oddělovaného mezerami,
za nimiž nenásleduje konstrukce (P...), a slova, za nimiž tato konstrukce následuje (v tomto případě do další skupiny obsah závorky).  
Například ve vstupu:  
> bee(P: insect honey) dog  cat (P:milk) ant(P) ape

jsou slova 'dog' a 'ape' prvního typu,  
s 'bee' je asociováno 'insect honey'  
s 'ant' nic (prázdný řetězec)  
Mělo by být tedy možné (jak to dělá skript) získat trojice:  
>('', 'bee', 'insect honey')  
('dog', '', '')  
('', 'cat', 'milk')  
('', 'ant', '')  
('ape', '', '') 

Dopište definici regulárního výrazu punct tak, aby odpovídal pozici za čárkou nebo tečkou, s výjimkou případu,
kdy bezprostředně před znakem a za daným místem stojí číslice (a aby tedy ani následný test assert nevypisoval chybu).  

#### Hodnocení: 5/5b
