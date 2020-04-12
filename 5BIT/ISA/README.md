## Programování síťové služby - DNS resolver

Vytvořte komunikující aplikaci podle konkrétní vybrané specifikace pomocí síťové knihovny BSD sockets. Projekt bude vypracován v jazyce C/C++. Projekt musí být přeložitelný a spustitelný na serveru merlin.fit.vutbr.cz pod operačním systémem GNU/Linux. Program by však měl být přenositelný. Hodnocení projektů může probíhat na jiném počítači s nainstalovaným OS GNU/Linux, včetně jiných architektur než Intel/AMD, jiných distribucí, jiných verzí knihoven apod. Pokud vyžadujete minimální verzi knihovny (dostupnou na serveru merlin), jasně tuto skutečnost označte v dokumentaci a README.

Napište program dns, který bude umět zasílat dotazy na DNS servery a v čitelné podobě vypisovat přijaté odpovědi na standardní výstup. Sestavení a analýza DNS paketů musí být implementována přímo v programu dns. Stačí uvažovat pouze komunikaci pomocí UDP.

Není-li v jiné části zadání specifikováno jinak, je při vytváření programu povoleno použít hlavičkové soubory pro práci se sokety a další obvyklé funkce používané v síťovém prostředí (jako je netinet/*, sys/*, arpa/* apod.), knihovnu pro práci s vlákny (pthread), signály, časem, stejně jako standardní knihovnu jazyka C (varianty ISO/ANSI i POSIX), C++ a STL. Jiné knihovny nejsou povoleny.

Spuštění aplikace
Použití: dns [-r] [-x] [-6] -s server [-p port] adresa

Pořadí parametrů je libovolné. Popis parametrů:
-r: Požadována rekurze (Recursion Desired = 1), jinak bez rekurze.
-x: Reverzní dotaz místo přímého.
-6: Dotaz typu AAAA místo výchozího A.
-s: IP adresa nebo doménové jméno serveru, kam se má zaslat dotaz.
-p port: Číslo portu, na který se má poslat dotaz, výchozí 53.
adresa: Dotazovaná adresa.
Podporované typy dotazů

Program dns se v odpovědích musí vypořádat se záznamy typu CNAME. Není požadována podpora DNSSEC.

Výstup aplikace

Na standardní výstup vypište informaci o tom, zda je získaná odpověď autoritativní, zda byla zjištěna rekurzivně a zda byla odpověď zkrácena. Dále vypište všechny sekce a záznamy obdržené v odpovědi.

Pro každou sekci v odpovědi vypište její název a počet získaných záznamů. Pro každý záznam vypište jeho název, typ, třídu, TTL a data.

Ukázka možného výstupu:

$ dns -r -s kazi.fit.vutbr.cz www.fit.vut.cz
Authoritative: No, Recursive: Yes, Truncated: No
Question section (1)
  www.fit.vut.cz., A, IN
Answer section (1)
  www.fit.vut.cz., A, IN, 14400, 147.229.9.26
Authority section (0)
Additional section (0)
$ dns -r -s kazi.fit.vutbr.cz www.ietf.org
Authoritative: No, Recursive: Yes, Truncated: No
Question section (1)
  www.ietf.org., A, IN
Answer section (3)
  www.ietf.org., CNAME, IN, 300, www.ietf.org.cdn.cloudflare.net.
  www.ietf.org.cdn.cloudflare.net., A, IN, 300, 104.20.1.85
  www.ietf.org.cdn.cloudflare.net., A, IN, 300, 104.20.0.85
Authority section (0)
Additional section (0)
