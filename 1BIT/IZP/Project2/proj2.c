#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define pozice_konst	k-1			//Udava pozici konstant ze zadani ve funkci taylor_tan.
#define implicitni_vyska	1.5		//Udava vysku pristroje, pokud neni zadana.
#define tolerance	1e-10			//Udava toleranci na 10 desetinnych mist ve vunkci tan_presne.

/*
Vytvoril: Jakub Sadilek
Dne:10.11.2017
Login: xsadil07
*/

enum { HELP, TANM, MA, MAB, CXMAB, CXMA, ERROR };	//Pro prehlednost udava mozne vysledky z funkce zkontroluj_args.

//Funkce vraci absolutni hodotu zadaneho cisla.
double f_abs(double hodnota) {
	return hodnota >= 0 ? hodnota : -hodnota;
}

//Funkce pocita tangens pomoci taylorova polynomu. Uhel udava uhel, interace udava pocet cyklu.
double taylor_tan(double uhel, unsigned int interace) {
	long double vysledek = uhel;								//Priradime uhel (podle vzorce) jako prvni interaci.
	const unsigned long long int ConstCitatel[12] = { 1, 2, 17, 62, 1382, 21844,
		929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604 };	//Konst. hodnoty v citateli, podle vzorce. (tezce odvoditelne)
	const unsigned long long int ConstJmenovatel[12] = { 3, 15, 315, 2835, 155925, 6081075,
		638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375 };	//Konst. hodnoty ve jmenovateli podle vzorce.
	long double umocneny_uhel = uhel;							//Slouzi k vypoctu umocneni uhlu.

	for (unsigned int k = 1; k < interace; k++) {				//Cyklime podle zadanych interaci. O jednu mene, ta se pricte na konci funkce.
		umocneny_uhel = umocneny_uhel * uhel * uhel;						//Umocnime uhel v citateli.
		long double citatel = ConstCitatel[pozice_konst] * umocneny_uhel;	//Vynasobime uhel konstantou.
		vysledek = citatel / ConstJmenovatel[pozice_konst] + vysledek;		//Podelime konst. ve jmenovateli a pricteme k nasemu vysledku.
	}

	return vysledek;
}

//Funkce vypocita tangens pomoci zretezenych zlomku. Uhel udava uhel, interace udava rozvoj zlomku.
double cfrac_tan(double uhel, unsigned int interace) {
	long double vysledek = 1.0;					//Pro zacatek zadame vysledku pocatecni hodnotu, aby se cyklus provedl (nema specificky vyznam).
	double citatel = uhel * uhel;				//Citatel umocnime na druhou (podle vzorce).
	double a = interace * 2 - 1;				//'a' reprezencuje cast rozvoje zlomku 'A' tudiz cela cisla ve jmenovateli.

	for (int k = interace; k > 0; k--) {		//Opakujeme podle zadaheno poctu interaci. Opet cyklime od zadu rozvoje.
		vysledek = a - (citatel / vysledek);	//Vypocteme jeden zlomek rozvoje.
		a = a - 2;								//Dekrementujeme mocninu o 2.
	}
	return uhel / vysledek;						//Posledni (prvni) cast zlomku podelime a vratime vysledek. (vzorec)
}

//Funkce vytiskne hodnoty tan. ze vsech implementaci dle zadani. Uhel znaci zadany uhel, min a max interace znaci v jakem intervalu budeme vypisovat. 
void print_tan(double uhel, int min_interace, int max_interace) {
	for (int cyklus = 1; cyklus <= max_interace; cyklus++) {		//Cyklime podle max. poctu zadanych interaci.
		if (cyklus >= min_interace && cyklus <= max_interace)		//Pokud jsme i v intermalu min interaci -> tiskneme.
			printf("%d %e %e %e %e %e\n", cyklus, tan(uhel), taylor_tan(uhel, cyklus), f_abs(tan(uhel) - taylor_tan(uhel,
				cyklus)), cfrac_tan(uhel, cyklus), f_abs(tan(uhel) - cfrac_tan(uhel, cyklus)));
	}		//Vypisujeme: cislo interace, tan z math.h, taylor_tan, rozdil mezi taylor_tan a math.h, cfrac_tan, rozdil mezi cfrac_tan a math.h
}

//Funkce vraci tangens s presnosti na 10 desetinnych mist. Uhel znaci zadany uhel.
long double tan_presne(double uhel) {
	long double tan_uhlu = uhel;
	long double mezivysledek = 0;	//Promena pro docasny vysledek, ktery potrebujeme pro porovnani. Poc. hodnotu zadavame jen, aby se cyklus provedl, nema vyznam na vypocet.
	int retezec = 1;				//Retezec znaci rozvoj zlomku. Jdeme od nejnizsiho po nejvetsi.

	while (f_abs(tan_uhlu - mezivysledek) > tolerance) {	//Porovnavame vypocty. Pokud nejsme v toleranci tak opakujeme.
		mezivysledek = tan_uhlu;							//Uchovame docasny vysledek pro porovnani.
		tan_uhlu = cfrac_tan(uhel, retezec);				//Vypocteme tangens.
		retezec++;											//Pokud nejsme v toleranci, tak zvysujeme rozvoj a tim zvysujeme presnost.
	}
	return tan_uhlu;										//Vracime tangens s presnosti na 10 desetinnych mist (tolerance).
}

//Funkce vypocita vzdalenost objektu od naseho pristroje. Uhel znaci zadany uhel alpha a vyska znaci zadanou vysku pristoroje.
double f_vzdalenost(double uhel, double vyska) {
	double delka;
	long double tan_alpha = tan_presne(uhel);		//Vypocteme tangens uhlu.
	delka = vyska / tan_alpha;						//Podle goniometricke funkce vypocitame vzdalenost.

	return delka;
}

//Funkce vypocita vysku mereneho objektu, Uhel znaci zadany uhel, vzdalenost znaci vzdalenost od naseho pristroje.
double f_vyska(double uhel, double vzdalenost) {
	double vyska;
	double tan_uhel = tan_presne(uhel);				//Vypocteme tangens zadaneho uhlu.
	vyska = tan_uhel * vzdalenost;					//Podle goniometricke funkce vypocteme vysku.

	return vyska;
}

//Funkce zkontroluje jestli zadane argumenty jsou v poradku a odpovidaji zadani. Jinak ERROR.
int zkontroluj_args(int argc, char **argv) {		//Argc udava pocet argumentu, argv udava zadane argumenty.
	if (argc == 1)									//Pokud neni zadan zadny argument, tak error.
		return ERROR;
	else if (strcmp(argv[1], "--tan") == 0 && argc == 5) {		//Jestli je pocatek --tan a poc. arg. je 5, je mozna shoda.
		long int n = strtol(argv[3], NULL, 10);
		long int m = strtol(argv[4], NULL, 10);
		double uhel_alpha = strtod(argv[2], NULL);
		if (0 < n && n <= m && m < 14 && 0.0 < uhel_alpha && uhel_alpha <= 1.4)	//Kontrolujeme jestli zadane intervaly a uhel odpovida zadani.
			return TANM;														//Jestli ano provedeme vypocet, jiank error.
		else return ERROR;
	}
	else if (strcmp(argv[1], "-m") == 0) {						//Jestli zacatek zacina -m, je mozna shoda se zadanim.
		double uhel_alpha = strtod(argv[2], NULL);
		if (argc == 3 || argc == 4) {							//Kontrolujeme poc. argumentu dle zadani, jestli neodpovida -> error.
			if (0.0 < uhel_alpha && uhel_alpha <= 1.4) {		//Kontrolujeme uhel, jestli odpovida zadani. Pokud ne -> error.
				if (argc == 4) {								//Kontrolujeme, jestli je zadan i dalsi uhel, pokud ne -> vypocet vzdalenosti.
					double uhel_beta = strtod(argv[3], NULL);
					if (0.0 < uhel_beta && uhel_beta <= 1.4)	//Pokud je zadan dalsi uhel, kontrolujeme jeho velikost.
						return MAB;								//Pokud odpovida -> vypocet vzdalenosti a vysky.
					else return ERROR;							//Neodpovida zadani -> error.
				}
				else return MA;
			}
			else return ERROR;
		}
		else return ERROR;
	}
	else if (strcmp(argv[1], "-c") == 0 && strcmp(argv[3], "-m") == 0) {	//Pokud je zadano -c je zadana i vyska pristroje.
		if (argc == 5 || argc == 6) {				//Kontrolujeme zadany pocet arg. jestli odpovida zadani, jinak error.
			double uhel_alpha = strtod(argv[4], NULL);
			double vyska_pristroje = strtod(argv[2], NULL);	//Kontrolujeme velikosti uhlu a vysky jestli odpovida zadani, jestli ne -> error.
			if (0.0 < uhel_alpha && uhel_alpha <= 1.4 && 0.0 < vyska_pristroje && vyska_pristroje <= 100) {
				if (argc == 6) {					//Pokud je zadano 6 arg., tak je zadany i dalsi uhel, jestli neni, tak vypiseme vzdalenost. 
					double uhel_beta = strtod(argv[5], NULL);	//Pokud je zadan dalsi uhel, tak vypocteme vzdalenost i vysku.
					if (0.0 < uhel_beta && uhel_beta <= 1.4)	//Kontrolujeme uhel, jestli odpovida zadani, jinak error.
						return CXMAB;
					else return ERROR;
				}
				else return CXMA;
			}
			else return ERROR;
		}
		else return ERROR;
	}
	else if (strcmp(argv[1], "--help") == 0)		//Pokud je zadano --help, tak budeme vypisovat text pro pomoc.
		return HELP;
	else return ERROR;								//Pokud neni nalezana zadna shoda, tak vracime error.
}

//Funkce vypise vsechyn vypocty tangens v zadanem intervalu. **argv je retezec zadany uzivatelem dle zadani.
void vypis_TANM(char **argv) {
	double uhel_alpha = strtod(argv[2], NULL);
	int min_interace = atoi(argv[3]);
	int max_interace = atoi(argv[4]);
	print_tan(uhel_alpha, min_interace, max_interace);
}

//Funkce vypise vzdalenost mereneho objektu. **argv je retezec zadany uzivatelem dle zadani.
void vypis_MA(char **argv) {
	double uhel_alpha = strtod(argv[2], NULL);
	printf("%.10e\n", f_vzdalenost(uhel_alpha, implicitni_vyska));	//Vyska pristroje neni zadana, tudiz implicitne bereme 1.5.
}

//Funkce vypise vzdalenost a vysku mereneho objektu. **argv je retezec zadany uzivatelem dle zadani.
void vypis_MAB(char **argv) {
	double uhel_alpha = strtod(argv[2], NULL);
	double uhel_beta = strtod(argv[3], NULL);
	double vzdalenost = f_vzdalenost(uhel_alpha, implicitni_vyska);
	printf("%.10e\n%.10e\n", vzdalenost, (f_vyska(uhel_beta, vzdalenost) + implicitni_vyska));
}

//Funkce vypise vzdalenost mereneho objektu. **argv je retezec zadany uzivatelem dle zadani.
void vypis_CXMA(char **argv) {
	double uhel_alpha = strtod(argv[4], NULL);
	double vyska_pristroje = strtod(argv[2], NULL);					//Narozdil od funkce MA mame zadanou vysku pristroje.
	printf("%.10e\n", f_vzdalenost(uhel_alpha, vyska_pristroje));	//Vypocteme vzdalenost od objektu.
}

//Funkce vypise vzdalenost a vysku mereneho objektu. **argv je retezec zadany uzivatelem dle zadani.
void vypis_CXMAB(char **argv) {
	double vyska_pristroje = strtod(argv[2], NULL);					//Narozdil od funkce MAB mame zadanou vysku pristroje.
	double uhel_alpha = strtod(argv[4], NULL);
	double vzdalenost = f_vzdalenost(uhel_alpha, vyska_pristroje);
	double uhel_beta = strtod(argv[5], NULL);
	printf("%.10e\n%.10e\n", vzdalenost, (f_vyska(uhel_beta, vzdalenost) + vyska_pristroje));
}

//Funkce vypisuje instrukce pro praci s programem podle zadani.
void vypis_pomoc(void) {
	printf("Vytvoril: Jakub Sadilek\n");
	printf("Login: xsadil07\n");
	printf("Dne: 10.11.2017\n");
	printf("Argumenty zadavejte:\n--tan A N M\n-m A\n-m A B\n-c X -m A\n-c X -m A B\n");
	printf("A - uhel alfa, B - uhel beta, N a M je interval pro zobrazeni, X vyska pristroje.");
	printf("Argumenty malymi pismeny musite psat takto. Za argumenty velkymi pismeny dosadte cislice.\n");
	printf("A, B, X - desetina cisla\nN, M - cela cisla (N <= M)\n");
}

int main(int argc, char *argv[]) {
	switch (zkontroluj_args(argc, argv)) {	//Kontrolujeme zadane hodnoty jestli odpovidaji zadani a nasledne podle nich vracime typ vypoctu.
	case TANM:
		vypis_TANM(argv);					//Vypiseme tangens vsech vypoctu podle zadani.
		break;
	case MA:
		vypis_MA(argv);						//Vypiseme vzdalenost objektu.
		break;
	case MAB:
		vypis_MAB(argv);					//Vypiseme vzdalenost a vysku objektu.
		break;
	case CXMA:
		vypis_CXMA(argv);					//Vypiseme to stejne jako "MA", akorat mame zadanou vysku pristroje.
		break;
	case CXMAB:
		vypis_CXMAB(argv);					//Vypiseme to stejne jako "MAB", akorat mame zadanou vysku pristroje.
		break;
	case HELP:
		vypis_pomoc();						//Vypiseme napovedu.
		break;
	case ERROR:								//Pokud je spatne zadany retezec, tak vypisujeme chybovou hlasku a koncime.
		fprintf(stderr, "Zadal si spatne argumenty. Pro pomoc zadej '--help'.\n");
		return 1;
	}
	return 0;
}