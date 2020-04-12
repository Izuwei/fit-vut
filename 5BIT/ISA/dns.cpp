/**
 * DNS Resolver
 * 
 * Autor: Jakub Sadílek
 * Login: xsadil07
*/

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define BUFFER_LEN 65536    // 2^16 bezpecna velikost pro prijeti UDP

// Typy dotazu (qtype)
#define A 1
#define NS 2
#define CNAME 5
#define SOA 6
#define PTR 12
#define AAAA 28
#define DEFAULT_PORT 53
#define IPv6_LEN 16     // 8 * 16 = 128 bit

using namespace std;

/**
 * Struktura pro uchovani vstupnich parametru.
*/
typedef struct {
    bool r = false;
    bool x = false;
    bool six = false;
    addrinfo *server = NULL;
    int p = DEFAULT_PORT;
    string addr = "";
} arguments;

/**
 * Struktura DNS headru. Sestaveno podle knihovny arpa/nameser.h
*/
typedef struct {
    uint16_t id;
    uint8_t rd :1;
    uint8_t tc :1;
    uint8_t aa :1;
    uint8_t opcode :4;
    uint8_t qr :1;
    uint8_t rcode :4;
    uint8_t cd :1;
    uint8_t ad :1;
    uint8_t z :1;
    uint8_t ra :1;
    uint16_t total_q;
    uint16_t total_ans;
    uint16_t total_auth;
    uint16_t total_add;
} dns_header;

/**
 * https://www2.cs.duke.edu/courses/fall16/compsci356/DNS/DNS-primer.pdf
*/
typedef struct {
    uint16_t qtype;
    uint16_t qclass;
} question;

#pragma pack(push, 1)   // Bez paddingu  (vysledna velikost musi byt 10B)
typedef struct {
    uint16_t type;
    uint16_t cls;
    uint32_t ttl;
    uint16_t rd_len;
} RDATA;
#pragma pack(pop)       // Obnovime padding

typedef struct {        // https://www.ietf.org/rfc/rfc1035.txt
    uint32_t serial;
    uint32_t refresh;
    uint32_t retry;
    uint32_t expire;
    uint32_t minimum;
} SOA_DATA;

arguments args;
string output;
int sock;

/**
 * Funkce vypise chybovou hlasku na stderr a ukonci program s navratovou hodnotou 1.
 * Funkce je inspirovana z meho projektu do predmetu IPK z roku 2019.
 * @param *message Chybova hlaska, ktera se vypise.
*/
void error(const char *message){
    if (args.server != NULL)
        freeaddrinfo(args.server);
    fprintf(stderr, "%s\n", message);
    exit(1);
}

/**
 * Funkce zajisti validnost adresy DNS serveru. Vysledek ulozi do globalni promenne args.server (addrinfo).
 * @param *host Adresa serveru
 * @return true v pripade uspechu, jinak false
*/
bool getHostIP(char *host){
    struct addrinfo hints;              // Hinty pro hledanou adresu
    memset(&hints, 0, sizeof(struct addrinfo));
    char addrstr[100] = "";

    hints.ai_family = AF_UNSPEC;        // IPv4 i IPv6
    hints.ai_socktype = SOCK_DGRAM;     // Preferenci pro UDP
    hints.ai_protocol = IPPROTO_UDP;

    if ((getaddrinfo(host, NULL, &hints, &args.server)) != 0)
        return false;

    return true;
}

/**
 * Funkce validuje zadany port.
 * @param *port Ukazatel na char s portem
 * @return V pripade uspechu vrati cislo portu jako int, jinak ukonci program s chybovou hlaskou
*/
int validatePort(char *port){
    char *ptr = NULL;
    int num = strtod(port, &ptr);
    if (strcmp(ptr, "") != 0)                   // Kontrola validniho cisla
        error("Neplatne cislo portu.");
    if (num < 1 || num > 65535)
        error("Port mimo povoleny rozsah.");    // Kontrola validnoho rozsahu
    return num;
}

/**
 * Funkce otoci IPv4 adresu. Priklad: 192.168.0.20 -> 20.0.168.192.
 * Inspirovano: https://stackoverflow.com/questions/16373248/convert-ip-for-reverse-ip-lookup autor: Some programmer dude
 * @param ip String s puvodni IP adresou
 * @return Otocena vstupni adresa
*/
string reverseIPv4(string ip){
    char reversed[INET_ADDRSTRLEN];
    in_addr_t addr;

    inet_pton(AF_INET, ip.c_str(), &addr);  // Prevedeme adresu do bitove podoby (32bit)

    addr = ((addr & 0xff000000) >> 24) |
           ((addr & 0x00ff0000) >>  8) |
           ((addr & 0x0000ff00) <<  8) |
           ((addr & 0x000000ff) << 24);     // Prehazime sadu bitu podle potreby

    inet_ntop(AF_INET, &addr, reversed, sizeof(reversed));  // Prevedeme zpet do textove podoby
    return string(reversed);
}

/**
 * Funkce otoci zadanou IPv6 adresu a vrati ji jako string.
 * @param ip String s validni IPv6 adresou
 * @return Otocena IPv6 adresa
*/
string reverseIPv6(string ip){
    string reversed;
    unsigned char addr[IPv6_LEN];
    const char HEX_ARR[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };    // Pro prevod na hexa

    inet_pton(AF_INET6, ip.c_str(), &addr);     // Dekodujeme na bin podobu

    for (int i = 0, len = IPv6_LEN; i < IPv6_LEN; i++, len--){
        reversed += HEX_ARR[(int)(addr[len - 1] & 0x0f)];           // Spodni 4 bity
        reversed += '.';
        reversed += HEX_ARR[(int)((addr[len - 1] & 0xf0) >> 4)];    // Horni 4 bity (shift do spravneho radu)
        reversed += '.';
    }
    reversed.pop_back();
    return reversed;
}

/**
 * Funkce validuje adresu vuci zadanym prepinacum a upravi ji na pozdejsi operace.
 * Vsechny potrebne informace bere z globalni promenne pro argumenty (args).
 * V pripade chyby ukonci program doprovazeny chybovou hlaskou.
*/
void validateAddr(){
    struct sockaddr_in tmp;
    if (args.x){
        if (inet_pton(AF_INET, args.addr.c_str(), &tmp) == 1)           // IPv4
            args.addr = reverseIPv4(args.addr) + ".in-addr.arpa";
        else if (inet_pton(AF_INET6, args.addr.c_str(), &tmp) == 1)     // IPv6
            args.addr = reverseIPv6(args.addr) + ".ip6.arpa";
        else
            error("Pro reverzni dotaz je treba zadat validni IP adresu.");
    }
    else if ((inet_pton(AF_INET, args.addr.c_str(), &tmp) == 1) || inet_pton(AF_INET6, args.addr.c_str(), &tmp) == 1)   // Domena
        error("Pro dotaz na domenu je potreba pouzit reverzni dotaz (-x).");
}

/**
 * Funkce provede vsechny potrebne kontroly nad zadanymi vstupnimi parametry, zpracuje je, ulozi a overi konflikty.
 * Funkce je inspirovana z meho projektu do predmetu IPK z roku 2019.
 * Pri chybe ukonci program doprovazeny chybovou hlaskou.
*/
void parseArgs(int argc, char **argv){
    if (argc > 9 || argc < 4)   // Kontrola poctu
        error("Spatny pocet zadanych argumentu.");

    bool addrSet, portSet, serverSet;       // Pomocne promenne k unikatnosti parametru
    addrSet = portSet = serverSet = false;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-r") == 0 && !args.r)              // Rekurze
            args.r = true;
        else if (strcmp(argv[i], "-x") == 0 && !args.x)         // Reverzni dotaz
            args.x = true;
        else if (strcmp(argv[i], "-6") == 0 && !args.six)       // AAAA dotaz
            args.six = true;
        else if (strcmp(argv[i], "-s") == 0 && !serverSet){     // Adresa DNS serveru
            serverSet = true;
            if (getHostIP(argv[++i]) == false)
                error("Nepodarilo se najit adresu DNS serveru.");
        }
        else if (strcmp(argv[i], "-p") == 0 && !portSet){       // Cilovy port
            portSet = true;
            args.p = validatePort(argv[++i]);
        }
        else if (!addrSet){         // Dotazovana adresa
            addrSet = true;
            args.addr = argv[i];
        }
        else
            error("Neznamy nebo prebytecny argument"); 
    }
    if (args.six && args.x)     // Overeni konfliktu
        error("Neplatna kombinace parametru -x a -6.");
    else if (!serverSet)
        error("Nezadali jste adresu DNS serveru.");
    else if (!addrSet)
        error("Nezadali jste adresu pro dotaz.");
    validateAddr();             // Validace dotazovane adresy
}

/**
 * Funkce vypni DNS header.
 * @param *hdr Ukazatel na header.
*/
void fillDNShdr(dns_header *hdr){
    hdr->id = (unsigned short) htons(getpid());     // Proces ID
    hdr->qr = 0;
    hdr->opcode = 0;
    hdr->aa = 0;
    hdr->tc = 0;
    hdr->rd = args.r ? 1 : 0;       // Rekurze podle zadaneho prepinace "-r"
    hdr->ra = 0;
    hdr->z = 0;
    hdr->ad = 0;
    hdr->cd = 0;
    hdr->rcode = 0;
    hdr->total_q = htons(1);        // Mame jednu otazku
    hdr->total_ans = 0;
    hdr->total_auth = 0;
    hdr->total_add = 0;
}

/**
 * Funkce prevede domenove jmeno na DNS format. Priklad: www.google.com -> 3www6google3com
 * @param name Vstupni domenove jmeno urcene pro prevod.
 * @return Vraci preformatovanou vstupni domenu.
*/
string formatQNAME(string name){
    string result = "";
    int len = name.length();

    for (int i = 0, pos = 0; i <= len; i++){     // S kazdym pismenkem
        if (name[i] == '.' || name[i] == '\0'){  // Pokud jsme narazili na tecku nebo konec zapiseme pocet prectenych znaku
            result += pos;
            for (int j = i - pos; j < i; j++)    // Prepiseme znaky dokud se nevratime na puvodni pozici
                result += name[j];
            pos = 0;                             // Vynulujeme pozici od posledni tecky
            continue;
        }
        pos++;
    }
    return result;
}

/**
 * Funkce vyplni data v question section (za QNAME pevne dana velikost).
 * @param *data ukazatel na Qdata
*/
void fillQuestionData(question *data){
    if (args.x)                         // Reverzni dotaz
        data->qtype = htons(PTR);
    else if (args.six)                  // IPv6 addr type
        data->qtype = htons(AAAA);
    else                                // IPv4 addr type
        data->qtype = htons(A);
    data->qclass = htons(1);            // IN
}

/**
 * Funkce dekoduje zadanou DNS header a ulozi do globalni promenne "output" informace dulezite pro vypis.
 * @param *hdr Ukazatel na DNS header
*/
void decodeDnsHeader(dns_header *hdr){
    output = ntohs(hdr->aa) ? "Authoritative: Yes, " : "Authoritative: No, ";
    output += (ntohs(hdr->rd) && ntohs(hdr->ra)) ? "Recursive: Yes, " : "Recursive: No, ";      // Je rekurze povolena a nastavena?
    output += ntohs(hdr->tc) ? "Truncated: Yes\n" : "Truncated: No\n";
}

/**
 * Funkce dekoduje jmeno z paketu a vratiho ve stringu.
 * @param *buffer Ukazatel na buffer s prijatym paketem.
 * @param *reader Ukazatel na zacatek jmena, které dekodujeme. Pozor: na konci bude jeho pozice za jmenem v paketu.
*/
string decodeName(unsigned char *buffer, unsigned char **reader){
    unsigned char *prev_reader = *reader;
    string decoded_name = "";
    unsigned offset = 0, steps = 1;
    bool jumped = 0;

    while(**reader != '\0'){
        if (**reader >= 0b11000000){     // Zacina Byte 11..? Pokud ano, je to adresa
            offset = (((unsigned)*(*reader) << 8) | (unsigned)*(*reader + 1)) & 0x3FFF; // Pohlidneme jako na 2 Byty a vezmeme prvnich 14 bitu. Jak definuje RFC
            *reader = buffer + offset;   // Posuneme se na adresu danou offsetem
            jumped = true;              // Uz nebudeme pocitat o kolik jsme se posunuli, protoze jsme jinde
        }
        else {
            decoded_name += **reader;   // Nezacina 11.. takze cteme normalni znak
            (*reader)++;
        }
        if (!jumped)    // Pokud jsme neskocili, pocitame Byty o kolik jsme se posunuli v paketu
            steps++;
    }
    if (jumped)     // Prvni skok musime pricist k posunu
        steps++;

    *reader = prev_reader + steps;    // Cteci hlava bude umistena za jmenem

    int name_len = decoded_name.length();   // Dekodujeme jmeno z dns formatu \003www\006google\003com -> www.google.com
    for (int i = 0; i < name_len; i++){     // S kazdym pismenem
        int num = decoded_name[i];          // Vezmeme pocatecni cislo slova
        for (int j = 0; j < num; j++, i++)  // Predni pismeno nahradime nasledujicim
            decoded_name[i] = decoded_name[i + 1];
        decoded_name[i] = '.';              // Posledni nahrazene nahradime teckou
    }                                       // Pokracujeme znova s dalsim slovem
    if (decoded_name.compare("") == 0)      // Prazdny nazev - default <Root>, typicky pri chybe pozn Wireshark
        return "<Root>";

    decoded_name.pop_back();                // Odebereme posledni tecku
    return decoded_name;
}

/**
 * Funkce dekoduje TYPE a vrati ho v citelne podobe.
 * @param type Zakodovany typ
 * @return Typ v citelne podobe pro vypis jako string.
*/
string decodeQTYPE(uint16_t type){
    switch (ntohs(type))
        {
        case 0x0001:    // 1
            return "A";
        case 0x0002:    // 2
            return "NS";
        case 0x0005:    // 5
            return "CNAME";
        case 0x0006:    // 6
            return "SOA";
        case 0x000B:    // 11
            return "WKS";
        case 0x000C:    // 12
            return "PTR";
        case 0x000F:    // 15
            return "MX";
        case 0x0010:    // 16
            return "TXT";
        case 0x001C:    // 28
            return "AAAA";
        case 0x0021:    // 33
            return "SRV";
        case 0x00FF:    // 255
            return "ANY";
        default:        // Jine
            return "UNKNOWN";
        }
}

/**
 * Funkce dekoduje CLASS a vrati ho v citelne podobe.
 * @param type Zakodovana trida
 * @return Trida v citelne podobe pro vypis jako string.
*/
string decodeClass(uint16_t cls){
    switch (ntohs(cls))
    {
    case 0x0001:    // 1
        return "IN";
    case 0x0003:    // 3
        return "CH";
    case 0x0004:    // 4
        return "HS";
    case 0x00FF:    // 255
        return "ANY";
    default:        // Jine
        return "UNKNOWN";
    }
}

/**
 * Funkce dekoduje otazky ulozene v prijatem paketu a ulozi vystup do globalni promenne "output" pro vypis.
 * @param *buffer Ukazatel na buffer s prijatym paketem
 * @param *dns_hdr Ukazatel na DNS header
 * @param *reader Ukazatel na zacatek prvni otazky (POZOR: na konci funkce bude ukazovat na konec sekce s otazkami)
 * 
 * velikost:       N\A      2B     2B
 *              +-------+-------+--------+
 * poradi:      | QNAME | QTYPE | QCLASS |
 *              +-------+-------+--------+
*/
void decodeQuestions(unsigned char *buffer, dns_header *dns_hdr, unsigned char **reader){
    int total_q = ntohs(dns_hdr->total_q);      // Ulozime si pocet otazek v paketu

    for (int i = 0; i < total_q; i++){          // S kazdou otazkou
        output += decodeName(buffer, reader) + ", ";    // Ulozime QNAME
        output += decodeQTYPE(((question *)*reader)->qtype) + ", " + decodeClass(((question *)*reader)->qclass) + "\n";  // Ulozime QTYPE a QCLASS
        *reader += sizeof(question);    // Posuneme se za QTYPE a QCLASS (posunuti za QNAME uz za nas udelala funkce "decodeName")
    }
}

/**
 * Funkce dekoduje vsechny odpovedi a prida je do globalni promenne "output" pro vypis. Stejny format pro answers, authority, additional.
 * @param *buffer Ukazatel na buffer s prijatym paketem
 * @param *dns_hdr Ukazatel na DNS header
 * @param *reader Ukazatel na zacatek prvni odpovedi (POZOR: na konci funkce bude ukazovat na konec sekce s odpovedmi)
 * 
 * velikost:       N\A    2B     2B      4B      2B      <-
 *              +------+------+-------+-----+--------+-------+
 * poradi:      | NAME | TYPE | CLASS | TTL | RD_LEN | RDATA |
 *              +------+------+-------+-----+--------+-------+
*/
void decodeQuery(unsigned char *buffer, int count, unsigned char **reader){
    for (int i = 0; i < count; i++){                                // S kazdou odpovedi
        output += decodeName(buffer, reader) + ", ";                // Dekodujeme NAME
        output += decodeQTYPE(((RDATA *)*reader)->type) + ", ";     // Dekodujeme TYPE
        output += decodeClass(((RDATA *)*reader)->cls) + ", ";      // Dekodujeme CLASS
        output += to_string(ntohl(((RDATA *)*reader)->ttl)) + ", "; // Dekodujeme TTL
        
        if (ntohs(((RDATA *)*reader)->type) == A){           // IPv4 adresa
            struct in_addr addr;
            int rd_len = ntohs(((RDATA *)*reader)->rd_len);  // Ulozime si delku dat
            *reader += sizeof(RDATA);                       // Posuneme se na zacatek dat

            uint32_t *ipv4_num = (uint32_t *)*reader;       // Nahlidneme jako na 4B cislo a ulozime
            addr.s_addr = (*ipv4_num);                      // Ulozime a dekodujeme jako IPv4
            output += string(inet_ntoa(addr)) + "\n";
            *reader += rd_len;                              // Posuneme se za data
        }
        else if (ntohs(((RDATA *)*reader)->type) == AAAA){  // IPv6 adresa
            unsigned char ipv6_bin[INET6_ADDRSTRLEN];       // Binarni forma IPv6 adesy
            char ipv6[INET6_ADDRSTRLEN];                    // Vysledna forma pro vypis
            memset(ipv6_bin, 0, INET6_ADDRSTRLEN);          // Vynulujeme buffery
            memset(ipv6, 0, INET6_ADDRSTRLEN);

            int rd_len = ntohs(((RDATA *)*reader)->rd_len);  // Ulozime si delku dat
            *reader += sizeof(RDATA);                        // Posuneme se na zacatek dat

            memcpy(ipv6_bin, *reader, rd_len);               // Nakopirujeme data binarni adresy
            inet_ntop(AF_INET6, ipv6_bin, ipv6, INET6_ADDRSTRLEN);  // Dekodujeme na vyslednou podobu
            output += string(ipv6) + "\n";                   // Ulozime pro vypis
            *reader += rd_len;                               // Posuneme se za data
        }                                                    // Dekodovani CNAME, PTR, NS
        else if ((ntohs(((RDATA *)*reader)->type) == CNAME) || 
                (ntohs(((RDATA *)*reader)->type) == PTR) || 
                (ntohs(((RDATA *)*reader)->type) == NS)){
            *reader += sizeof(RDATA);                        // Posuneme se na zacatek dat
            output += decodeName(buffer, reader) + "\n";
        }
        else if (ntohs(((RDATA *)*reader)->type) == SOA){    // Dekodovani SOA
            *reader += sizeof(RDATA);
            output += decodeName(buffer, reader) + "\n";        // MNAME
            output += "\tResponsible Authority's mailbox: " + decodeName(buffer, reader) + "\n";         // RNAME
            output += "\tSerial number: " + to_string(ntohl(((SOA_DATA *)*reader)->serial)) + "\n";      // SERIAL
            output += "\tRefresh interval: " + to_string(ntohl(((SOA_DATA *)*reader)->refresh)) + " seconds\n";  // REFRESH
            output += "\tRetry interval: " + to_string(ntohl(((SOA_DATA *)*reader)->retry)) + " seconds\n";      // RETRY
            output += "\tExpire limit: " + to_string(ntohl(((SOA_DATA *)*reader)->expire)) + " seconds\n";       // EXPIRE
            output += "\tMinimum TTL: " + to_string(ntohl(((SOA_DATA *)*reader)->minimum)) + " seconds\n";       // MINIMUM
            *reader += sizeof(SOA_DATA);
        }
        else {                                                  // Nepodporovany typ radeji preskocime
            int rd_len = ntohs(((RDATA *)*reader)->rd_len);     // Ulozime si delku dat
            *reader += sizeof(RDATA) + rd_len;                  // Posuneme se za DNS header a RD_DATA
            output += "Not supported type\n";
        }
    }
}

/**
 * Funkce dekoduje odpoved DNS serveru a vysledek ulozi do globalni promenne "output" pro vypis.
 * @param *buffer Ukazatel na buffer s ulozenym paketem od DNS.
 * @param *dns_hdr Ukazatel na DNS header.
 * 
 *              +------------+----------+--------+-----------+------------+
 * poradi:      | DNS Header | Question | Answer | Authority | Additional |
 *              +------------+----------+--------+-----------+------------+
*/
void decodeMessage(unsigned char *buffer, dns_header *dns_hdr){
    unsigned char *reader = &buffer[sizeof(dns_header)];    // Vytvorime ukazatel pres ktery precteme celou odpoved
    decodeDnsHeader(dns_hdr);                                   // Dekodujeme DNS header
    output += "Question section (" + to_string(ntohs(dns_hdr->total_q)) + ")\n";
    decodeQuestions(buffer, dns_hdr, &reader);                  // Dekodujeme Questions
    output += "Answer section (" + to_string(ntohs(dns_hdr->total_ans)) + ")\n";
    decodeQuery(buffer, ntohs(dns_hdr->total_ans), &reader);    // Dekodujeme Answers
    output += "Authority section (" + to_string(ntohs(dns_hdr->total_auth)) + ")\n";
    decodeQuery(buffer, ntohs(dns_hdr->total_auth), &reader);   // Dekodujeme Authority
    output += "Additional section (" + to_string(ntohs(dns_hdr->total_add)) + ")\n";
    decodeQuery(buffer, ntohs(dns_hdr->total_add), &reader);    // Dekodujeme Additional
}

/**
 * Pokud nastane timeout pri zachytavani odpovedi provede se tato funkce. Uzavre socket a ukonci program s chyb. hlaskou.
*/
void alarm_handler(int sig){
    close(sock);
    error("Nepodarilo se zachytit zadnou odpoved.");
}

/**
 * Funkce odesle dotaz na DNS server, prijme odpoved a dekoduje ji.
*/
void sendAndReceive(){
    unsigned char buffer[BUFFER_LEN];       // Buffer pro praci s datagramem
    memset(buffer, 0, sizeof(buffer));

    void *dest = NULL;
    int length;

    if (args.server->ai_family == AF_INET){     // cilova IPv4 adresa
        dest = (struct sockaddr_in *)args.server->ai_addr;
        ((sockaddr_in *)dest)->sin_port = htons(args.p);
        length = sizeof(struct sockaddr_in);
    }
    else {                                      // cilova IPv6 adresa
        dest = (struct sockaddr_in6 *)args.server->ai_addr;
        ((sockaddr_in6 *)dest)->sin6_port = htons(args.p);
        length = sizeof(struct sockaddr_in6);
    }

    if ((sock = socket(args.server->ai_family, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        error("Chyba pri vytvareni socketu.");
    
    dns_header *dns_hdr = (dns_header *)buffer;
    fillDNShdr(dns_hdr);                    // Vyplnime DNS header

    unsigned char *qname = (unsigned char *)&buffer[sizeof(dns_header)];
    strcpy((char *)qname, formatQNAME(args.addr).c_str());  // Vyplnime QNAME k otazce

    question *q_data = (question *)&buffer[sizeof(dns_header) + strlen((char *)qname) + 1];
    fillQuestionData(q_data);               // Vyplnime data k otazce

    // Odesleme dotaz na server
    if (sendto(sock, buffer, sizeof(dns_header) + strlen((char *)qname) + 1 + sizeof(question), 0, (struct sockaddr *)dest, length) < 0){
        perror("");
        close(sock);
        error("Chyba pri odeslani datagramu.");
    }

    signal(SIGALRM, alarm_handler);
    alarm(3);           // Nastavime timeout, kdyby nahodou neprisla zadna odpoved
    if (recvfrom(sock, buffer, BUFFER_LEN, 0, args.server->ai_addr, &args.server->ai_addrlen) < 0){   // Prijmeme odpoved
        close(sock);
        error("Chyba pri ziskavani odpovedi.");
    }
    alarm(0);           // Zrusime timeout
    close(sock);
    decodeMessage(buffer, dns_hdr);     // Dekodujeme odpoved
}

int main(int argc, char **argv){
    setbuf(stdout, 0);          // FIXME: DEBUG
    parseArgs(argc, argv);      // Zpracujeme argumenty
    sendAndReceive();           // Odesleme, prijmeme a zpracujeme odpoved
    cout << output;             // Vypiseme vysledek
    
    if (args.server != NULL)    // Uvolnime alokovanou pamet
        freeaddrinfo(args.server);
    return 0;
}