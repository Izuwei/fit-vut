# Scanner síťových služeb
### Zadání:
1) Vytvořte jednoduchý síťový TCP, UDP skener v C/C++. Program oskenuje zadanou IP adresu a porty. Na standardní výstup vypíše, v jakém stavu se porty nacházejí (otevřený, filtrovaný, uzavřený)   
2) Vytvořte relevantní manuál/dokumentaci k projektu  
  
### Upřesnění:
Aplikace oskenuje zvolené porty na daném síťovém zařízení. Pakety musí být odeslané pomocí BSD sockets. Odchytávat odpovědi můžete např. pomocí knihovny libpcap.  
  
**TCP skenování:**  
Posílá pouze SYN pakety. Neprovádí tedy kompletní 3-way-handshake. Pokud přijde odpověď RST - port je označen jako uzavřený. Pokud po daný časový interval nepřijde ze skenovaného portu odpověď, je nutno ověřit dalším paketem a teprve potom port označit jako filtrovaný. Pokud na daném portu běží nějáká služba, je port označen jako otevřený. Více viz RFC 793.  
  
**UDP skenování:**  
U UDP skenování můžete uvažovat, že daný počítač při zavřeném portu odpoví ICMP zprávou typu 3, kódu 3 (port unreachable). Ostatní porty považujte za otevřené.  
  
**Volání programu:**  
  
./ipk-scan {-i <interface>} -pu <port-ranges> -pt <port-ranges> [<domain-name> | <IP-address>]  
  
kde:  
-pt, pu port-ranges - skenované tcp/udp porty, povolený zápis např. -pt 22 nebo -pu 1-65535 nebo -pt 22,23,24  
domain-name | ip address - doménové jméno, nebo IP adresa skenovaného stroje  
-i eth0, kde argument představuje identifikátor rozhraní. Tento parametr je volitelný, v případě jeho nepřítomnosti se zvolí první IEEE 802 interface, který má přidělenou neloopbackovou IP adresu.  
  
**Příklad chování:**  
./ipk-scan -pt 21,22,143 -pu 53,67 localhost  
  
Interesting ports on localhost (127.0.0.1):  
PORT     STATE  
21/tcp	 closed  
22/tcp 	 open  
143/tcp	 filtered  
53/udp	 closed  
67/udp	 open  
