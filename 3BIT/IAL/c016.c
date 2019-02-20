
/* c016.c: **********************************************************}
** 
** AUTOR: Jakub Sadílek
** LOGIN: xsadil07
**
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, 2014-2018
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	if (ptrht != NULL){	// Kontrola platnosti ukazatele.
		for (int i = 0; i < HTSIZE; i++)
			(*ptrht)[i] = NULL;	// Každý prvek pole inicializujeme na NULL.
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	// Zkontrolujeme platnost vstupních argumentů.
	if (ptrht == NULL || key == NULL)
		return NULL;
	// Projíždíme seznam prvků na daném indexu.
	for (tHTItem *element = (*ptrht)[hashCode(key)]; element != NULL; element = element->ptrnext){
		if (!strcmp(element->key, key))	// Pokud jsou stejné klíče, vracíme prvek.
			return element;
	}
	return NULL;
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	// Kontrola platnosti argumentů.
	if (ptrht == NULL || key == NULL)
		return;

	int index = hashCode(key);
	tHTItem *element = htSearch(ptrht, key);
	if (element == NULL){	// Pokud je NULL prvek v tabulce není.
		element = (tHTItem *)malloc(sizeof(struct tHTItem));	// Vytvoříme nový.
		element->key = (tKey)malloc(sizeof(char) * strlen(key) + 1);
		strcpy(element->key, key);	// Přiřadíme klíč.
		element->ptrnext = (*ptrht)[index];	// Vložíme na začátek seznamu.
		(*ptrht)[index] = element;
	}
	element->data = data;	// Vložíme data. Pokud prvek exist., tak se jen přepíší.
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	// Kontrola platnosti argumentů.
	if (ptrht == NULL || key == NULL)
		return NULL;

	tHTItem *element = htSearch(ptrht, key);
	if (element == NULL) // Je NULL pokud jsme prvek nenalezli.
		return NULL;
	else
		return &element->data;	// Pokud jsme ho nalezli - vracíme.
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	// Kontrola argumentů.
	if (ptrht == NULL || key == NULL)
		return;
	int index = hashCode(key);
	tHTItem *prev = NULL, *dest = NULL;
	// Projdeme všechny prvky v seznamu na daném indexu.
	for (dest = (*ptrht)[index]; dest != NULL; dest = dest->ptrnext){
		if (!strcmp(dest->key, key)){	// Pokud jsme našli shodu klíčů.
			if (prev == NULL)	// Jestli je prvek první, přímo z pole napojíme na druhý.
				(*ptrht)[index] = dest->ptrnext;
			else	// Pokud je prvek v seznamu, ale ne první, klasicky přepojíme ukazatel.
				prev->ptrnext = dest->ptrnext;
			free(dest->key);	// Uvolníme klíč a prvek.
			free(dest);
			break;
		}
		prev = dest;
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	// Kontrola argumentů.
	if (ptrht == NULL)
		return;
	
	tHTItem *rem = NULL, *prev = NULL;
	for (int i = 0; i < HTSIZE; i++){	// Pro každý index v poli projdeme celý seznam.
		for (rem = (*ptrht)[i]; rem != NULL; free(prev)){
			prev = rem;	// Posunujeme se o jeden a předchozí uvolníme.
			rem = rem->ptrnext;
			free(prev->key);
		}
	}
	htInit(ptrht);	// Inicializujeme pole do počátečního stavu.
}
