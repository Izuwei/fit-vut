
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**  AUTOR: Jakub Sadílek
**  Login: xsadil07
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int errflg;
int solved;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->First = NULL;    // Inicializace všech ukazatelů na default. hodnoty.
    L->Act = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	for (L->Act = L->First; L->First != NULL; L->Act = L->First){
        L->First = L->First->rptr;  // Projdeme všechny prvky a uvolňujeme.
        free(L->Act);
    }
    L->Act = NULL;  // Inicializujeme defaultní hodnoty.
    L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr element = malloc(sizeof(struct tDLElem));    // Alok. prvek.
    if (element != NULL) {      // Kontrola alokace.
        element->data = val;    // Přiřadíme data.
        element->lptr = NULL;   // První prvek nemá nikoho po levici = NULL.
        if (L->First == NULL){  // Pokud je seznam prázdný, tak:
            element->rptr = NULL;   // Seznam má 1 prvek -> ukaz. doprava = NULL.
            L->Last = element;      // Prvek je sám, proto je první i poslední.
        }
        else {
            element->rptr = L->First;   // Vzájemně provážeme prvky.
            L->First->lptr = element;
        }
        L->First = element; // Odkážeme na náš první prvek.
    }
    else    // Pokud selhala alokace -> error.
        DLError();
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr element = malloc(sizeof(struct tDLElem));    // Alok. prvek.
    if (element != NULL) {      // Kontrola alokace.
        element->data = val;    // Přiřadíme data.
        element->rptr = NULL;   // Poslední prvek nemá nikoho po pravici = NULL.
        if (L->First == NULL){  // Pokud je seznam prázdný, tak:
            element->lptr = NULL;   // Seznam má 1 prvek -> ukaz. doleva = NULL.
            L->First = element;     // Prvek je sám, proto je první i poslední.
        }
        else {
            element->lptr = L->Last;   // Vzájemně provážeme prvky.
            L->Last->rptr = element;
        }
        L->Last = element; // Odkážeme na náš poslední prvek.
    }
    else    // Pokud selhala alokace -> error.
        DLError();
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First;  // Pouze odkážeme ukazatel na první prvek.
    // Return netřeba ve voidu.
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->Last; // Pouze odkážeme ukazatel na poslední prvek.
    // Return netřeba ve voidu.
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL)   // Test prázdného seznamu.
        DLError();
    else                    // Předáváme hodnotu.
        *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL || L->Last == NULL)    // Kontrola prázdného seznamu.
        DLError();
    else                                        // Předáváme hodnotu.
        *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if (L->First != NULL){  // Kontrola prázdného seznamu.
        if (L->First == L->Act) // Kontrola jestli není první prvek aktivní.
            L->Act = NULL;
        tDLElemPtr tmp = L->First;  // Uložíme si adresu k uvolnění.
        if (L->First == L->Last){   // Kontrola jestli není pouze jeden prvek.
            L->First = NULL;
            L->Last = NULL;
        }
        else {
            L->First = L->First->rptr;  // Prvním prvkem se stává druhý prvek.
            L->First->lptr = NULL;  // První prvek nemá nikoho po levici.
        }
        free(tmp);  // Uvolníme první prvek.
    }
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
    if (L->First != NULL || L->Last != NULL){  // Kontrola prázdného seznamu.
        if (L->Last == L->Act) // Kontrola jestli není poslední prvek aktivní.
            L->Act = NULL;
        tDLElemPtr tmp = L->Last;  // Uložíme si adresu k uvolnění.
        if (L->First == L->Last){  // Kontrola jestli není pouze jeden prvek.
            L->First = NULL;
            L->Last = NULL;
        }
        else {
            L->Last = L->Last->lptr;  // Posledním prvkem je předposlední prvek.
            L->Last->rptr = NULL;     // Poslední prvek nemá nikoho po pravici.
        }
        free(tmp);  // Uvolníme poslední prvek.
    }
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    if (L->Act != NULL && L->Act != L->Last){   // Kontrola aktivního prvku.
        tDLElemPtr tmp = L->Act->rptr;  // Uložíme adresu pro uvolnění.
        if (tmp->rptr == NULL){     // Kontrola jestli akt. se nestane posledním.
            L->Act->rptr = NULL;
            L->Last = L->Act;
        }
        else {
            L->Act->rptr = tmp->rptr;   // Provážeme seznam.
            tmp->rptr->lptr = L->Act;
        }
        free(tmp);  // Uvolníme paměť.
    }
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if (L->Act != NULL && L->Act != L->First){   // Kontrola aktivního prvku.
        tDLElemPtr tmp = L->Act->lptr;  // Uložíme adresu pro uvolnění.
        if (tmp->lptr == NULL){     // Kontrola jestli akt. se nestane prvním.
            L->Act->lptr = NULL;
            L->First = L->Act;
        }
        else {
            L->Act->lptr = tmp->lptr;   // Provážeme seznam.
            tmp->lptr->rptr = L->Act;
        }
        free(tmp);  // Uvolníme paměť.
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act != NULL){            // Kontrola aktivního prvku.
        tDLElemPtr element = malloc(sizeof(struct tDLElem));
        if (element == NULL)        // Kontrola alokace.
            DLError();
        else{
            element->data = val;    // Přiřazujeme data a provážem seznam.
            element->lptr = L->Act;
            element->rptr = L->Act->rptr;
            if (L->Last == L->Act)  // Pokud poslední byl aktivní, poslední se posouvá.
                L->Last = element;
            else
                element->rptr->lptr = element;
            L->Act->rptr = element;
        }
    }
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act != NULL){            // Kontrola aktivního prvku.
        tDLElemPtr element = malloc(sizeof(struct tDLElem));
        if (element == NULL)        // Kontrola alokace.
            DLError();
        else{
            element->data = val;    // Přiřazujeme data a provážem seznam.
            element->rptr = L->Act;
            element->lptr = L->Act->lptr;
            if (L->First == L->Act)  // Pokud první byl aktivní, první se posouvá.
                L->First = element;
            else
                element->lptr->rptr = element;
            L->Act->lptr = element;
        }
    }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
    if (L->Act == NULL) // Kontrola aktivního prvku.
        DLError();
    else                // Vracíme hodnotu.
        *val = L->Act->data;
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if (L->Act != NULL) // Kontrola aktivního prvku a přepíšem hodnotu.
        L->Act->data = val;
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL){    // Kontrola aktivního prvku.
        if (L->Act == L->Last)  // Kontrola, jestli aktivní není poslední.
            L->Act = NULL;
        else                // Pokud není, tak posuneme posuneme.
            L->Act = L->Act->rptr;
    }
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL){    // Kontrola aktivního prvku.
        if (L->Act == L->First)
            L->Act = NULL;  // Pokud aktivní je první, tak deaktivujeme.
        else                // Jinak posunujeme aktivitu.
            L->Act = L->Act->lptr;
    }
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
    // Ternární operátor, pokud pravda vracíme TRUE=1, jinak FALSE=0.
	return (L->Act != NULL) ? TRUE : FALSE;
}

/* Konec c206.c*/
