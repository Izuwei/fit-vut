#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/*
Vytvoril: Jakub Sadilek
Dne: 22.10.2017
Login: xsadil07
*/

//Funkce f_add prida pocatecni pismeno do retezce "Enable".
//*city - pole, ze ktereho dostaneme znak; *characters - pole, do ktereho ulozime znak;
//*index udava pozici na kterou muzeme zapsat; shoda je pozice znaku v city, ktery zapiseme

char *f_add(char *city, char *characters, int *index, int shoda) {
	char *p;					//Ukazatel na pismeno, ktere priradime.
	p = &city[shoda];
	characters[*index] = *p;
	*index = *index + 1;		//Zvysime pozici na retezci pro moznou dalsi upravu.
	return characters;			//Vracime retezec.
}

//Funkce f_check zkontroluje, zda retezec "Enable" jiz neobsahuje daný znak, aby
//nedochazelo k duplicitam. Pouziva se predfunkci f_add.
//*city - pole, ze ktereho ziskame znak; *characters - pole, ve kterem hledame znak; shoda - udava pozici znaku

bool f_check(char *city, char *characters, int shoda) {
	char *p;
	p = &city[shoda];						//Nacteme znak, ktery potrebujeme do ukazatele.
	unsigned int floop = 0;					//Pomocna promena do cyklu. Pouzivame ji jen v tehle funkci.

	while (floop < strlen(characters)) {
		if (*p == characters[floop])		//Cyklus projde retezec a zkontroluje, zda-li nedojde k duplicitam.
			return false;
		else floop++;
	}							//Funkce vraci hodnotu TRUE, jestlize retezec neobsahuje dany znak. Jinak FALSE.
	return true;
}

//Funkce f_order seradi znaky v poli "Enable" podle abecedy a vypise je.

void f_order(char *characters) {		//Characters je vstupni pole, ktere seradime a nasledne vytiskneme.
	int c, d = 0, length;				//Pomocne promene do cyklu.
	char *ukazatel;
	char result[43];
	char ch;
	length = strlen(characters);
	ukazatel = characters;

	for (ch = 'A'; ch <= 'Z'; ch++)		//Cyklus projde znaky A - Z. Tj. od nejmensiho 'A' do nejvyssiho 'Z'.
	{
		for (c = 0; c < length; c++)	//Cyklus projde delku retezce.
		{
			if (*ukazatel == ch)		//Pokud znak A - Z je shodny, tak ho ulozi do retezce "result".
			{
				result[d] = *ukazatel;
				d++;					//Posuneme se na dalsi byte vysledku, pokud jsme zapsali.
			}
			ukazatel++;					//Posuneme se na dalsi byte retezce, ze ktereho cteme.
		}
		ukazatel = characters;
	}
	result[d] = '\0';					//Na konci vysledneho retezce zapiseme '\0' pro ukonceni.
	printf("Enable: %s\n", result);		//Vypiseme Enable.
	return;
}

//Funkce f_uppercase zmeni nazvy na velka pismena, at uz jsou mala ci velka.
//Dostaneme tak vlastnost programu "case insensitive".

void f_uppercase(char *ch) {		//ch znaci vstupni pole 
	int i = 0;
	while (ch[i] != '\0') {
		ch[i] = toupper(ch[i]);		//Menime znak po znaku na velka pismena, dokud \0.
		i++;
	}
}

//Funkce f_presearch slouzi pro vypsani jedineho mozneho mesta, ke kteremu smeruji argumenty.

void f_presearch(char *find, char *city, int *found) { //"*find" slouzi k ulozeni mozneho vysledku.
	*found = *found + 1;				//"found", je promena, ktera nam znaci, kolik mest je mozne vypsat.
	strcpy(find, city);					//Pokud je mozne vypsat pouze 1. Vypiseme ulozene mesto ve "find".
}

//f_read cte stdin po znacich a uklada vysledek do pole "city"

int f_read(char *city) {
	int inc = 0;								//Udava pozici v poli.
	while ((city[inc] = getchar()) != EOF) {	//Cteme dokud EOF
		if (city[inc] == '\n')					//Pokud narazime na konec radku, vyskocime z cyklu.
			break;
		else if (inc < 100)						//Cteme max. 100 znaku, jinak vyskocime s cyklu.
			inc++;
		else break;
	}
	city[inc] = '\0';							//Ukoncim pole.
	return *city;
}

//Pokud uzivatel zada argument "-h", vypiseme nasledujici texty.

void f_help(void) {
	printf("Vytvoril: Jakub Sadilek\n");
	printf("Login: xsadil07\n");
	printf("Dne: 22.10.2017\n");
	printf("Vystup Enable je abecedne serazeny.\n");
	printf("Argumenty i databaze muze obsahovat velka i mala pismena (case insensitive).\n");
	printf("Databaze mest muze byt libovolne velka.\n");
}

int main(int argc, char **argv) {
	char city[101];				//Promena, do ktere budeme nacitat mesta.
	char characters[43] = "";	//Slouzi k ulozeni znaku "Enable".
	char find[101] = "";		//Slouzi k ulozeni mozneho vysledku, pokud shoda je jen jedna.
	char same[101] = "";		//Pro docasne ulozeni identity argv a mesta.
	int found = 0;				//Patri k funkci f_presearch. Znaci pocet moznych vysledku.
	int arglen;					//Delka nazvu mesta. Slouzi k porovnavani s hodnotami z klavesnice.
	int notfound = 0;			//pokud nenajdeme shodu, inkrementujeme. Pokud  pocet neshod == pocet adres, zobrazime "Not Found".
	int loop = 0;				//Pocet cyklu (udava pocet adres).
	int index = 0;				//K funkci f_add.
	bool identita = false;		//Pokud argv == city -> true.

	if (argc > 2) {
		fprintf(stderr, "You must enter only one argument.\n");	//Pokud je zadano vice argumentu, piseme error.
		return 1;
	}

	while (f_read(city)) {				//Nacitame adresy.
		f_uppercase(city);				//Prevadime na velka pismena.
		if (argv[1] != NULL)
			f_uppercase(argv[1]);

		if (argv[1] == NULL) {			//pokud neni zadana adresa vypiseme poc. pismena
			if (f_check(city, characters, 0) == true) {		//Kontroluji duplicity.
				f_add(city, characters, &index, 0);			//Pridavam znak.
				continue;
			}
			else continue;
		}
		else if (strcmp("-H", argv[1]) == 0) {	//Pokud je zadano "-h", vypisujeme.
			f_help();
			return 0;
		}
		else if (strcmp(argv[1], city) == 0) {	//Porovnavam adresu s arg., pokud shoda -> ukladam.
			identita = true;
			strcpy(same, city);					//Ukladam nazev mesta.
			loop++;
			continue;
		}
		//----------ZJISTUJEME ENABLE ZNAK POKUD SHODA---------
		arglen = strlen(argv[1]);						//Delka argumentu.
		int shoda = 0;
		for (int i = 0; argv[1][i] == city[i]; i++) {	//Dokud shoda, tak porovnavam.
			shoda++;
		}

		if (shoda == arglen) {						//Pokud shoda je stejna s delkou arg, tak ukladam dalsi znak.
			f_presearch(find, city, &found);		//V pripade, ze by mesto bylo jediny vysledek, tak ukladam nazev.
			if (f_check(city, characters, shoda) == true)
				f_add(city, characters, &index, shoda);
		}
		else notfound++;						//Pokud neshoda -> inkrementuji.
		loop++;									//Pocitam cykly.
	}
	//-------TISKNEME--------
	if (loop == notfound && argv[1] != NULL) {	//Pokud neshoda == pocet cyklu -> neshoda.
		printf("Not found.\n");
		return 0;
	}
	else if (found >= 1 && identita == true) {	//1 identita a vice moznych vysledku.
		printf("Found: %s\n", same);
		f_order(characters);
	}
	else if (found == 0 && identita == true) {	//Pouze 1 identita.
		printf("Found: %s\n", same);
	}
	else if (found == 1) {						//Pokud shoda je jen jedna, zobrazuji ulozene mesto.
		printf("Found: %s\n", find);
	}
	else f_order(characters);					//Jinak vypisuji "Enable".
	return 0;
}