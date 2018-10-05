/**
* Vytvoril: Jakub Sadilek
* Login: xsadil07
* Dne: 2.12.2017
*
* 3. projekt IZP 2017/18
*
* Jednoducha shlukova analyza
* Unweighted pair-group average
* https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
* Ladici makra. Vypnout jejich efekt lze definici makra
* NDEBUG, napr.:
*   a) pri prekladu argumentem prekladaci -DNDEBUG
*   b) v souboru (na radek pred #include <assert.h>
*      #define NDEBUG
*/

#define TONUM	48
#define DECPOINT	10
#define CONLEN	6
#define DEFAULT_VALUE	1
#define VYNULOVAT	0

#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
* Deklarace potrebnych datovych typu:
*
* TYTO DEKLARACE NEMENTE
*
*   struct obj_t - struktura objektu: identifikator a souradnice
*   struct cluster_t - shluk objektu:
*      pocet objektu ve shluku,
*      kapacita shluku (pocet objektu, pro ktere je rezervovano
*          misto v poli),
*      ukazatel na pole shluku.
*/

struct obj_t {
	int id;
	float x;
	float y;
};

struct cluster_t {
	int size;
	int capacity;
	struct obj_t *obj;
};

/*****************************************************************
* Deklarace potrebnych funkci.
*
* PROTOTYPY FUNKCI NEMENTE
*
* IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
*
*/

/*
Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)	//DODELANO
{
	assert(c != NULL);
	assert(cap >= 0);

	c->size = 0;				//Velikost nastavime na 0.

	if (cap == 0) {				//Pokud kapacita == 0, tak "capacity" = 0 a "obj" inic. na NULL.
		c->capacity = cap;
		c->obj = NULL;
	}
	else {						//Jinak priradime kapacitu podle zadane velikosti a alokujeme velikost podle kapacity.
		c->capacity = cap;
		c->obj = malloc(cap * sizeof(struct obj_t));
	}
}

/*
Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
*/
void clear_cluster(struct cluster_t *c)	//DODELANO
{
	free(c->obj);					//Uvolnime pamet objektu.
	init_cluster(c, VYNULOVAT);		//Zavolame funkci pro inicializaci a predame ji kapacitu 0, tim bude shluk vynulovan.
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
*/
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
	// TUTO FUNKCI NEMENTE
	assert(c);
	assert(c->capacity >= 0);
	assert(new_cap >= 0);

	if (c->capacity >= new_cap)
		return c;

	size_t size = sizeof(struct obj_t) * new_cap;

	void *arr = realloc(c->obj, size);
	if (arr == NULL)
		return NULL;

	c->obj = (struct obj_t*)arr;
	c->capacity = new_cap;
	return c;
}

/*
Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
nevejde.
*/
void append_cluster(struct cluster_t *c, struct obj_t obj)	//DODELANO
{
	if (c->size >= c->capacity)		//Pokud mame malou kapacitu, tak navysime (pouzijeme jiz vytvorenou funkci).
		resize_cluster(c, c->capacity + CLUSTER_CHUNK);

	c->obj[c->size] = obj;			//Priradime objekt na konec naseho retezce objektu.
									//Inkrementaci "size" provadime az po prirazeni objektu, protoze cislujeme od nuly.
	c->size = c->size + 1;			//Inkrementujeme velikost, protoze pridavame objekt.
}

/*
Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
*/
void sort_cluster(struct cluster_t *c);

/*
Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
Shluk 'c2' bude nezmenen.
*/
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)	//DODELANO
{
	assert(c1 != NULL);
	assert(c2 != NULL);

	for (int loop = 0; loop < c2->size; loop++)	//Prirazujeme objekty z c2 do c1 podle velikosti c2.
		append_cluster(c1, c2->obj[loop]);		//Pro prirazeni pouzivame nasi naprogramovanou funkci.

	sort_cluster(c1);							//Seradime objekty podle zadane funkce.
}

/**********************************************************************/
/* Prace s polem shluku */

/*
Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
(shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)	//DODELANO
{
	assert(idx < narr);
	assert(narr > 0);

	int next = idx + 1;					//Pomocna promena pro rirazovani nasledujiciho shluku.
	clear_cluster(&carr[idx]);			//Uvolnime pam. shluku.

	for (int loop = idx; loop < narr; loop++) {
		carr[loop] = carr[next];		//Prirazujeme nasledujici prvek na pozici predchoziho (prepisujeme).
		next++;							//Zacali jsme "prepisovat" od pozice prvku k odstraneni.
	}
	narr--;								//Pocet shluku snizime o jeden odstraneny.
	return narr;
}

/*
Pocita Euklidovskou vzdalenost mezi dvema objekty.
*/
float obj_distance(struct obj_t *o1, struct obj_t *o2)	//DODELANO
{
	assert(o1 != NULL);
	assert(o2 != NULL);

	float distance;				//Pomocna prom. pro ulozeni vysledku.
	float x = o1->x - o2->x;	//Odecteme od sebe Xove a Yove souradnice objektu (podle vzorce).
	float y = o1->y - o2->y;

	x = x*x;					//Souradnice umocnime nadruhou (vzorec).
	y = y*y;

	distance = sqrtf(x + y);	//Secteme souracnice a odmocnime (vzorec).
	return distance;
}

/*
Funkce spocita prumernou pozici shluku 'c'. Vraci objekt s touto pozici.
*/
struct obj_t avg_position(struct cluster_t *c) {	//DOPLNENA FUNKCE
	struct obj_t result;							//Inic. objekt pro vysledek.

	result.x = result.y = 0.0;						//Vynulujeme hodnoty pro pozdejsi pocitani.

	for (int i = 0; i < c->size; i++) {				//Cyklime podle poctu objektu v zadanem shluku.
		result.x = result.x + c->obj[i].x;			//Scitame Xove souradnice.
		result.y = result.y + c->obj[i].y;			//Scitame Yove souradnice.
	}

	result.x = result.x / c->size;					//Soucet souradnic vydelime jejich poctem (prumer).
	result.y = result.y / c->size;
	return result;
}

/*
Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)	//DODELANO
{
	assert(c1 != NULL);
	assert(c1->size > 0);
	assert(c2 != NULL);
	assert(c2->size > 0);

	struct obj_t avg_c1 = avg_position(c1);		//Priradime prumerne pozice obou shluku.
	struct obj_t avg_c2 = avg_position(c2);

	return obj_distance(&avg_c1, &avg_c2);		//Vypocteme jejich vzdalenost a vracime vysledek.
}

/*
Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)	//DODELANO
{
	assert(narr > 0);

	float MinDistance = INT_MAX;				//Promena pro ulozeni min. vzdalenosti.
	float distance;								//Promena pro vzdalenost mezi shluky.

	for (int i = 0; i < narr; i++) {			//Porovnavame kazdy shluk s kazdym, krome sam sebe.
		for (int j = i + 1; j < narr; j++) {
			distance = cluster_distance(&carr[i], &carr[j]);	//Pocitame vzdalenost.

			if (distance < MinDistance) {		//Pokud vzdalenost < min. vzdalenost, tak ukladam pozice shluku.
				MinDistance = distance;			//Prepisujeme na aktualni min. vzdalenost.
				*c1 = i;
				*c2 = j;
			}
		}
	}
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
	// TUTO FUNKCI NEMENTE
	const struct obj_t *o1 = (const struct obj_t *)a;
	const struct obj_t *o2 = (const struct obj_t *)b;
	if (o1->id < o2->id) return -1;
	if (o1->id > o2->id) return 1;
	return 0;
}

/*
Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
	qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
	for (int i = 0; i < c->size; i++)
	{
		if (i) putchar(' ');
		printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
	}
	putchar('\n');
}

/*
Funkce kontroluje prvni radek souboru. Pokud chyba vraci hodnotu 0,
jinak vraci zadany pocet objeku. Argument je ukazatel na soubor.
*/
int check_file_format(FILE *file) {			//DOPLNENA FUNKCE
	const char count[] = "count=";			//Konstanta jako vzor pro porovnani.
	int objects = 0, character;

	for (int i = 0; (character = getc(file)) != '\n'; i++) {	//Nacitame znak po znaku prvni radek.
		if (character == EOF)				//Pokud EOF -> chyba.
			return 0;

		if (i < CONLEN) {					//Pokud cyklus < delka nasi konstanty, tak kontrolujeme znaky.
			if (character != count[i])		//Porovnavame nacteny znak s nasi konstantou (vzor).
				return 0;					//Pokud znak se lisi -> chyba.
		}
		else {
			character = character - TONUM;					//Znak prevedeme na cislo (podle ascii).
			if (character >= 0 && character <= 9)			//Pokud znak neni v intervalu -> neni cislo -> chyba.
				objects = (objects * DECPOINT) + character;	//Posuneme zvysime vys. o des. misto a pricteme.
			else
				return 0;
		}
	}
	return objects;							//Vracime pocet objektu.
}

/*
Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)	//DODELANO
{
	assert(arr != NULL);

	FILE *file;
	if ((file = fopen(filename, "r")) == NULL) {		//Otevreme pro cteni. Pokud nastala chyba v otevreni, tak NULL.
		fprintf(stderr, "Chyba pri otevirani souboru %s.\n", filename);	//Piseme chybu a koncime.
		*arr = NULL;
		return 0;
	}

	int objects = check_file_format(file);				//Zkontrolujeme spravnost prvniho radku.
	if (objects == 0) {									//Pokud chyba, tak koncime.
		fclose(file);
		fprintf(stderr, "Spatne formovana data v souboru.\n");
		*arr = NULL;
		return 0;
	}
	//Alokujeme pam. pro vsechny objekty v souboru.
	struct cluster_t *arr_clust = (struct cluster_t*)malloc(objects * sizeof(struct cluster_t));
	if (arr_clust == NULL) {							//Pokud chyba, tak koncime.
		fclose(file);
		fprintf(stderr, "Chyba pameti.\n");
		*arr = NULL;
		return 0;
	}
	*arr = arr_clust;									//Priradime ukazatel na 1. shluk v poli.

	int act_obj;
	for (act_obj = 0; act_obj < objects; act_obj++) {			//Nacitame objekty do alok. pameti.
		int id;
		float x, y;
		fscanf(file, "%d%f%f", &id, &x, &y);					//Skenujeme parametry a ukladame do prom.

		if (x < 0.0 || x > 1000.0 || y < 0.0 || y > 1000.0) {	//Kontrolujeme spravnost parametru.
			free(arr_clust);									//Pokud chyba -> konec.
			fclose(file);
			*arr = NULL;
			fprintf(stderr, "Spatne zadane souradnice.\n");
			return 0;
		}

		for (int i = 0; i < act_obj; i++) {				//Zkontrolujeme duplicity ID.
			if (arr_clust[i].obj->id == id) {			//Pokud jsme nasli shodu -> error.
				free(arr_clust);
				fclose(file);
				fprintf(stderr, "Hodnota ID nesmi byt duplicitni.\n");
				*arr = NULL;
				return 0;
			}
		}

		struct cluster_t cluster;				//Vytvorime shluk (podle zadani 1 objekt == 1 shluk).
		init_cluster(&cluster, 1);				//Inicializujeme pro jeden obj.

		struct obj_t object;					//Vytvorime objekt a priradime do nej naskenovane param.
		object.id = id;
		object.x = x;
		object.y = y;

		append_cluster(&cluster, object);		//Vlozime obj. do vytvoreneho shluku.
		arr_clust[act_obj] = cluster;			//Vlozime shluk do pole shluku.
	}
	fclose(file);								//Zavreme soubor.
	return act_obj;								//Vracime pocet nactenych objektu.
}

/*
Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
	printf("Clusters:\n");
	for (int i = 0; i < narr; i++)
	{
		printf("cluster %d: ", i);
		print_cluster(&carr[i]);
	}
}

int main(int argc, char *argv[])	//DODELANO
{
	struct cluster_t *clusters;
	int N;												//Prom. pro arg., ktery udava pocet shluku.

	switch (argc) {										//Kontrolujeme volitelny arg.
	case 2:												//Pokud neni, tak defaultne N = 1.
		N = DEFAULT_VALUE;
		break;
	case 3:												//Pokud je, tak ho priradime do N.
		if ((N = strtol(argv[2], NULL, 10)) > 0)		//Pokud je N > 0, tak pokr., jinak error.
			break;
		else {
			fprintf(stderr, "Argument N musi byt > 0.\n");
			return 1;
		}
	default:											//Pokud jsou spatne zadane arg., tak error.
		fprintf(stderr, "Spatne argumenty programu.\n");
		return 1;
	}

	int total = load_clusters(argv[1], &clusters);		//Nacteme vsechny shluky ze souboru.
	if (clusters == NULL)								//Pokud NULL, tak je inic. chyba a koncime.
		return 1;
	else if (N > total) {								//Zadano je vic nez mame v souboru -> error.
		fprintf(stderr, "V souboru je malo objektu.\n");
		return 1;
	}

	while (N < total) {		//Podle N shlukujeme objekty ve shluku. Rozdil mezi nimi == pocet slouceni.
		int c1, c2;

		find_neighbours(clusters, total, &c1, &c2);		//Najdeme nejblizsi shluky.
		merge_clusters(&clusters[c1], &clusters[c2]);	//Sloucime je do c1.
		total = remove_cluster(clusters, total, c2);	//Odstranime c2. Total je nizsi o 1.
	}
	print_clusters(clusters, total);					//Tiskneme shluky (celkem "total").

	for (int loop = 0; loop < total; loop++)			//Uvolnime pam. obj. v kazdem shluku.
		clear_cluster(&clusters[loop]);

	free(clusters);										//Uvolnime pam. retezce shluku.
	return 0;
}