/**
 * Autori: Jakub Sadilek (xsadil07), Adam Sedlacek (xsedla1e)
 * 
 * Projekt do IMS
 * 
*/


#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unistd.h>
#include <string.h>
#include <sstream>

using namespace std;

#define FIELD_HEIGHT 100
#define FIELD_WIDTH 100
#define CONSUM 30
#define MAX_RODENTS 4000
#define MIN_RODENTS 0

int SEED;                       // Promenna pro manualni vlozeni seedu nahodnosti
int Starved = 0;                // Pocet vyhladovelych hrabosu

enum Gender { Male, Female };                               // Pohlavi
enum Pregnancy { None, FirstWeek, SecondWeek, ThirdWeek };  // Brezost
enum Period { Spring, Summer, Autumn, Winter };             // Rocni obdobi
enum Directions { Top, Right, Down, Left };                 // Mozne smery pohybu

/**
 * Struktura reprezentuje event hubeni hrabosu
*/
typedef struct {
    int week;               // Cislo tydne
    int effectivity;        // Efektivita
} AnnihilationWeek;

/**
 * Struktura pro predavani parametru programu
 */
typedef struct {
    int interations;                              // Pocet interaci
    int rodents;                                  // Pocet hrabosu
    int predatorCnt;                              // Pocet dravcu
    int predatorKills;                            // Procet zabiti na dravce za den
    vector<AnnihilationWeek> annihilationWeeks;   // Vycet tydnu k hromadnemu hubeni
} Parameters;

/**
 * Trida reprezentuje hrabose
*/
class rodent {
    public:
        Gender gender;          // Pohlavi
        int age;                // Vek (tydny)
        Pregnancy pregnancy;    // Tehotenstvi
        int litters;            // Pocet vrhu
        int stock;              // Zasoby na zimu

        rodent(int initAge) {
            if ((rand() % 100) > 60){    // 3/5 samecci, 2/5 samicky
                this->gender = Female;
                this->pregnancy = None;
            }
            else
                this->gender = Male;

            this->age = initAge;
            this->litters = 0;
            this->stock = 250;
        }
};

/**
 * Trida reprezentuje oblast v poli o rozmeru 1m^2
*/
class fieldCell {
    public:
        vector<rodent> rodents;     // Vektro hrabosu v oblasti
        int food;                   // Pocet klasu

        fieldCell() {
            this->food = 0;         // Zadne jidlo
        }

        /**
         * Funkce rozmnozi hrabose v aktualni oblasti
        */
        void reproduction(int *population, Period actualPeriod) {    // Funkce projde vsechny samice v policku a upravi jejich pergnancy status pripadne se rozmnozi v danem policku
            if (actualPeriod == Spring || actualPeriod == Summer || actualPeriod == Autumn) {
                for (int i = 0; i < this->rodents.size(); i++) {
                    if (this->rodents[i].gender == Female) {
                        if (this->rodents[i].pregnancy == ThirdWeek) {      // Posledni stadium
                            int childs = (rand() % 9) + 1;      // 1-10 kusu v jednom vrhu

                            for (int child = 0; child < childs; child++) {
                                this->rodents.push_back(rodent(0));
                            }

                            this->rodents[i].litters++;
                            this->rodents[i].pregnancy = None;
                            *population = *population + childs;
                        }
                        else if (this->rodents[i].pregnancy == SecondWeek) {
                            this->rodents[i].pregnancy = ThirdWeek;
                        }
                        else if (this->rodents[i].pregnancy == FirstWeek) {
                            this->rodents[i].pregnancy = SecondWeek;
                        }
                        else {
                            if (this->rodents[i].age > 4) {   // Mlade muze mit mladata 4-5 tydnu po porodu
                                if ((rand() % 100) > 25 && this->rodents[i].litters < 7)
                                    this->rodents[i].pregnancy = FirstWeek;
                            }
                        }
                    }
                }
            }
        }

        int evolve(int *population, Period actualPeriod) {
            for (int i = 0; i < this->rodents.size(); i++) {    // Vsichni zase starsi
                if (this->rodents[i].age > 104) {               // Starsi nez 2 roky -> sance ze umre
                    if ((rand() % 100) > 50) {
                        this->rodents.erase(this->rodents.begin() + i);
                        *population = *population - 1;
                        i--;
                        continue;
                    }
                }
                this->rodents[i].age++;
            }

            this->reproduction(population, actualPeriod);
            return this->rodents.size();
        }

        int starvation(int *population, bool weekWinter) {
            for (int i = 0; i < this->rodents.size(); i++) {
                if (this->rodents[i].stock <= 0) {      // Hladovi
                    if ((rand() % 100) < 65) {          // Sance ze umre na vyhladoveni
                        this->rodents.erase(this->rodents.begin() + i);
                        *population = *population - 1;
                        i--;
                        Starved++;
                    }
                }
                else {
                    if (weekWinter)
                        this->rodents[i].stock -= 20;
                    else
                        this->rodents[i].stock -= 25;   // Jednotek jidla za den pres zimu
                }
            }
            return this->rodents.size();
        }
};

class fieldArea {
    public:
        int year;           // uplynule roky
        int week;           // tyden v roce
        int population;     // pocet hrabosu na poli
        int winterCounter;
        bool weekWinter;
        int predators;
        int predatorKills;
        vector<AnnihilationWeek> annihilationWeeks;
        Period period;              // rocni obdobi
        fieldCell cells[FIELD_HEIGHT][FIELD_WIDTH];
        vector<fieldCell*> activeCells;     // bunky s hrabosi

        fieldArea(int initPopulation, int initPredators, int initPredatorKills, vector<AnnihilationWeek> initAW) {
            this->year = 0;
            this->week = 0;
            this->population = initPopulation;
            this->predators = initPredators;
            this->predatorKills = initPredatorKills;
            this->winterCounter = 0;
            this->weekWinter = false;
            this->annihilationWeeks = initAW;
            this->period = Winter;

            for (int each = 0; each < initPopulation; each++) {
                int randomWidth = rand() % FIELD_HEIGHT;
                int randomHeight = rand() % FIELD_WIDTH;

                this->cells[randomHeight][randomWidth].rodents.push_back(rodent(rand() % 104));   // 104 tydnu == 2 roky == max delka zivota

                if (find(this->activeCells.begin(), this->activeCells.end(), &this->cells[randomHeight][randomWidth]) == this->activeCells.end())
                    this->activeCells.push_back(&this->cells[randomHeight][randomWidth]);
            }
        }

        void resetLitters() {
            for (int i = 0; i < this->activeCells.size(); i++) {
                for (int j = 0; j < this->activeCells[i]->rodents.size(); j++) {
                    this->activeCells[i]->rodents[j].litters = 0;
                }
            }
        }

        void timeShift() {              // Funkce nastavi cas v aktualni interaci (volat na zacatku interace)
            if (this->week == 52) {     // Prechod na novy rok
                this->week = 0;
                this->year++;

                resetLitters();
            }
            else {                      // Dalsi tyden
                this->week++;
            }

            if (this->week == 12) {      // Nove rocni obdobi
                this->period = Spring;
                this->weekWinter = false;
                return;
            }
            if (this->week == 25) {
                this->period = Summer;
                return;
            }
            if (this->week == 38) {
                this->period = Autumn;
                return;
            }
            if (this->week == 51) {
                this->winterCounter++;
                this->period = Winter;
                this->weekWinter = calcWeekWinter();
                return;
            }
        }

        double foodGrowth() {      // Simulace rustu jidla
            if (this->week > 32 || this->week < 12) return 0;       // Zima nic neroste
            if (this->week == 12 || this->week == 13) return 1;
            if (this->week == 14 || this->week == 15)  return 3;
            if (this->week > 15 && this->week <= 22)  return 5;
            if (this->week == 23)  return 7;
            if (this->week == 24)  return 10;
            if (this->week == 25)  return 12;
            if (this->week > 25 && this->week <= 30)  return 15;
            if (this->week > 30 && this->week <= 32)  return 20;
            return 0;
        }

        vector<Directions> getAvailableDirections(fieldCell *cell, int y, int x) {
            vector<Directions> availableDirections;
            
            if (y < FIELD_HEIGHT - 1) {     // Test hranic
                if (cell->food < this->cells[y + 1][x].food)   // Je tam vice jidla?
                    availableDirections.push_back(Top);       // Potom se tam mozna presune
            }
            if (y > 0) {
                if (cell->food < this->cells[y - 1][x].food)
                    availableDirections.push_back(Down);
            }
            if (x < FIELD_WIDTH - 1) {
                if (cell->food < this->cells[y][x + 1].food) 
                    availableDirections.push_back(Right);
            }
            if (x > 0) {
                if (cell->food < this->cells[y][x - 1].food)
                    availableDirections.push_back(Left);
            }

            return availableDirections;
        }

        void moveRodents(fieldCell *cell, int y, int x) {
            vector<Directions> availableDirections = getAvailableDirections(cell, y, x);

            if (availableDirections.size() == 0) {
                int randomHeight, randomWidth;

                for (int i = 0; i < cell->rodents.size(); i++) {
                    randomHeight = rand() % FIELD_HEIGHT;
                    randomWidth = rand() % FIELD_WIDTH;

                    this->cells[randomHeight][randomWidth].rodents.push_back(cell->rodents[i]);

                    if (find(this->activeCells.begin(), this->activeCells.end(), &this->cells[randomHeight][randomWidth]) == this->activeCells.end())
                        this->activeCells.push_back(&this->cells[randomHeight][randomWidth]);
                }
            }
            else {
                for (int i = 0; i < cell->rodents.size(); i++) {
                    switch(availableDirections[rand() % availableDirections.size()]) {
                        case Top:
                            this->cells[y + 1][x].rodents.push_back(cell->rodents[i]);
                            if (find(this->activeCells.begin(), this->activeCells.end(), &this->cells[y + 1][x]) == this->activeCells.end())
                                this->activeCells.push_back(&this->cells[y + 1][x]);
                            break;
                        case Down:
                            this->cells[y - 1][x].rodents.push_back(cell->rodents[i]);
                            if (find(this->activeCells.begin(), this->activeCells.end(), &this->cells[y - 1][x]) == this->activeCells.end())
                                this->activeCells.push_back(&this->cells[y - 1][x]);
                            break;
                        case Right:
                            this->cells[y][x + 1].rodents.push_back(cell->rodents[i]);
                            if (find(this->activeCells.begin(), this->activeCells.end(), &this->cells[y][x + 1]) == this->activeCells.end())
                                this->activeCells.push_back(&this->cells[y][x + 1]);
                            break;
                        case Left:
                            this->cells[y][x - 1].rodents.push_back(cell->rodents[i]);
                            if (find(this->activeCells.begin(), this->activeCells.end(), &this->cells[y][x - 1]) == this->activeCells.end())
                                this->activeCells.push_back(&this->cells[y][x - 1]);
                            break;
                    }
                }
            }
            cell->rodents.clear();
            this->activeCells.erase(remove(this->activeCells.begin(), this->activeCells.end(), cell), this->activeCells.end());
        }

        void sowField() {
            for (int i = 0; i < FIELD_HEIGHT; i++) {
                for (int j = 0; j < FIELD_WIDTH; j++) {
                    this->cells[i][j].food = (rand() % 100) + 350;
                }
            }
        }

        void harvestField() {
            for (int i = 0; i < FIELD_HEIGHT; i++) {
                for (int j = 0; j < FIELD_WIDTH; j++) {
                    this->cells[i][j].food = 0;
                }
            }
        }

        void randomKill() {
            if (this->activeCells.size() == 0)
                return;

            int cellIndex = rand() % this->activeCells.size();
            int victim = rand() % activeCells[cellIndex]->rodents.size();

            activeCells[cellIndex]->rodents.erase(activeCells[cellIndex]->rodents.begin() + victim);
            this->population--;

            if (activeCells[cellIndex]->rodents.size() == 0)
                this->activeCells.erase(this->activeCells.begin() + cellIndex);
        }

        void annihilation(int effectivity) {
            int realEffect = (rand() % 6) - 3 + effectivity;    // tolerance
            if (realEffect < 0) realEffect = 0;
            else if (realEffect > 100) realEffect = 100;

            int deaths = (this->population * realEffect) / 100;

            for (int victim = 0; victim < deaths; victim++) {
                randomKill();
            }
        }

        double calcCoeffitient() {
            if (this->period == Spring) {
                if (this->population < 10) return 0.0;
                if (this->population < 29) return 0.3;
                if (this->population < 59) return 0.6;
            }
            if (this->period == Summer) {
                if (this->population < 85) return 0.0;
                if (this->population < 180) return 0.6;
                if (this->population < 1200) return 0.8;
            }
            if (this->period == Autumn) {
                if (this->population < 280) return 0.0;
                if (this->population < 680) return 0.3;
                if (this->population < 2000) return 0.8;
            }
            if (this->period == Winter) {
                if (this->population < 85) return 0.0;
                if (this->population < 130) return 0.3;
                if (this->population < 290) return 0.6;
            }
            return 1.0;
        }

        bool calcWeekWinter() {
            if (this->winterCounter == 3) {
                if ((rand() % 100) < 45) {
                    this->winterCounter = 0;
                    return true;
                }
            }
            else if (this->winterCounter > 3) {
                if ((rand() % 100) < 80) {
                    this->winterCounter = 0;
                    return true;
                }
            }
            return false;
        }

        void evolve() {
            timeShift();

            if (this->week == 12)
                sowField();
            else if (this->week == 32)
                harvestField();

            for (int i = 0; i < this->annihilationWeeks.size(); i++) {
                if (this->annihilationWeeks[i].week == this->week)
                    annihilation(this->annihilationWeeks[i].effectivity);
            }

            int deaths = (this->predators * this->predatorKills) * calcCoeffitient();

            for (int death = 0; death < deaths; death++) {
                randomKill();
            }


            for (int i = 0; i < this->activeCells.size(); i++) {
                if (this->activeCells[i]->evolve(&this->population, this->period) == 0) {
                    this->activeCells.erase(this->activeCells.begin() + i);
                    i--;
                }
            }

            if (this->period == Winter) {   // Neni jidlo na poli       this->week > 32 || this->week < 12
                for (int i = 0; i < this->activeCells.size(); i++) {
                    if (this->activeCells[i]->starvation(&this->population, this->weekWinter) == 0) {
                        this->activeCells.erase(this->activeCells.begin() + i);
                        i--;
                    }
                }
            }

            for (int i = 0; i < FIELD_HEIGHT; i++) {
                for (int j = 0; j < FIELD_WIDTH; j++) {
                    if (this->week > 36 || this->week < 12) {
                        continue;
                    }
                    else {
                        this->cells[i][j].food = this->cells[i][j].food - ceil((CONSUM / foodGrowth()) * this->cells[i][j].rodents.size());

                        for (int z = 0; z < this->cells[i][j].rodents.size(); z++) {
                            if (this->cells[i][j].food > 5) {
                                this->cells[i][j].rodents[z].stock += 5;
                                this->cells[i][j].food -= 5;
                            }
                        }

                        if (this->cells[i][j].food < 250 && this->cells[i][j].rodents.size() != 0) {
                            moveRodents(&this->cells[i][j], i, j);
                        }
                    }

                }
            }

        }

        int fieldQualityToBGColor(int quality) {
            if (quality < 100) return 49;
            if (quality < 200) return 41;
            if (quality < 290) return 43;
            return 42;
        }

        int printField() {
            string output = "";
            string x = "42";
            for (int i = 0; i < FIELD_HEIGHT; i++) {
                for (int j = 0; j < FIELD_WIDTH; j++) {
                    int rodentsInCell = this->cells[i][j].rodents.size();
                    int fieldQuality = fieldQualityToBGColor(this->cells[i][j].food);

                    if (rodentsInCell == 0)
                        output += "\033[1;49;" + to_string(fieldQuality) + "m \033[0m";
                    else if (rodentsInCell >= 1 && rodentsInCell <= 3)
                        output += "\033[1;92;" + to_string(fieldQuality) + "m" + to_string(rodentsInCell) + "\033[0m";
                    else if (rodentsInCell >= 4 && rodentsInCell <= 6)
                        output += "\033[1;33;" + to_string(fieldQuality) + "m" + to_string(rodentsInCell) + "\033[0m";
                    else if (rodentsInCell >= 7 && rodentsInCell <= 9)
                        output += "\033[1;95;" + to_string(fieldQuality) + "m" + to_string(rodentsInCell) + "\033[0m";
                    else
                        output += "\033[1;31;" + to_string(fieldQuality) + "m+\033[0m";
                }
                output += '\n';
            }

            output += "\nPocet hrabosu: " + to_string(this->population) + "\n";
            output += "Pocet interaci: " + to_string(this->year * 52 + this->week) + "\n";
            output += "Rok: " + to_string(this->year) + "\n";
            output += "Tyden: " + to_string(this->week) + "\n";

            if (this->period == Spring) output += "Aktualni obdobi: Jaro\n";
            else if (this->period == Summer) output += "Aktualni obdobi: Leto\n";
            else if (this->period == Autumn) output += "Aktualni obdobi: Podzim\n";
            else if (this->period == Winter) {
                if (this->weekWinter)
                    output += "Aktualni obdobi: Zima (slaba)\n";
                else
                    output += "Aktualni obdobi: Zima\n";
            }

            cout << output;
            return this->population;
        }
};

void error(const char *message){
    fprintf(stderr, "%s\n", message);
    exit(1);
}

int validateNumber(char *arg, int min, int max){
    char *ptr = NULL;
    int num = strtod(arg, &ptr);

    if (strcmp(ptr, "") != 0)                   // Kontrola validniho cisla
        error("Neplatne cislo v prepinaci.");
    if (num < min || num > max)
        error("Cislo v prepinaci mimo povoleny rozsah.");    // Kontrola validnoho rozsahu
    return num;
}

vector<int> parseIntArr(char *arg, int min, int max) {
    vector<string> strArray;
    vector<int> intArray;
    string weeks = string(arg);

    if (weeks.empty())
        return intArray;

    string tmpStr = "";
    for (auto ch : weeks) {
        if (ch >= '0' && ch <= '9') {
            tmpStr += ch;
        }
        else if (ch == ',') {
            if (tmpStr.empty())
                error("Duplicitni delimiter.");

            strArray.emplace_back(tmpStr);
            tmpStr = "";
        }
        else {
            error("Cislo tydne musi byt zadano ciselnou jednotkou.");
        }
    }
    strArray.emplace_back(tmpStr);

    int tmpInt;
    for (auto each : strArray) {
        tmpInt = strtod(each.c_str(), NULL);
        if (tmpInt >= min && tmpInt <= max) {
            intArray.emplace_back(tmpInt);
        }
        else {
            error("Chybny rozsah ve vyctu cisel.");
        }
    }
    std::sort(intArray.begin(), intArray.end(), [](int &a, int &b) -> bool { return a < b; });
    
    return intArray;
}

vector<AnnihilationWeek> parseAnnihilationWeeks(vector<int> weeks, vector<int> effects) {
    if (weeks.size() != effects.size())
        error("Pocet tydnu a efektu se neshoduje.");
    
    vector<AnnihilationWeek> result;
    AnnihilationWeek tmp;

    for (int i = 0; i < weeks.size(); i++) {
        tmp.week = weeks[i];
        tmp.effectivity = effects[i];
        result.push_back(tmp);
    }

    return result;
}

Parameters parseArgs(int argc, char **argv) {
    if (argc > 13 || argc < 1)
        error("Spatny pocet argumentu.");

    Parameters params;
    vector<int> weeks;
    vector<int> effects;
    bool iSet, hSet, pSet, zSet, eSet, tSet, sSet;
    iSet = hSet = pSet = zSet = eSet = tSet = sSet = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && !iSet && i < (argc - 1)){
            params.interations = validateNumber(argv[++i], 0, 10000);
            iSet = true;
        }
        else if (strcmp(argv[i], "-h") == 0 && !hSet && i < (argc - 1)){
            params.rodents = validateNumber(argv[++i], 0, 99999999);    //2500
            hSet = true;
        }
        else if (strcmp(argv[i], "-p") == 0 && !pSet && i < (argc - 1)){
            params.predatorCnt = validateNumber(argv[++i], 0, 10000);
            pSet = true;
        }
        else if (strcmp(argv[i], "-z") == 0 && !zSet && i < (argc - 1)){
            params.predatorKills = validateNumber(argv[++i], 0, 10000);
            zSet = true;
        }
        else if (strcmp(argv[i], "-t") == 0 && !tSet && i < (argc - 1)){
            weeks = parseIntArr(argv[++i], 0, 52);
            tSet = true;
        }
        else if (strcmp(argv[i], "-e") == 0 && !eSet && i < (argc - 1)){
            effects = parseIntArr(argv[++i], 0, 100);
            eSet = true;
        }
        else if (strcmp(argv[i], "-s") == 0 && !sSet && i < (argc - 1)){
            SEED = strtod(argv[++i], NULL);
            sSet = true;
        }
        else {
            error("Neznamy nebo prebytecny parametr.");
        }
    }
    params.annihilationWeeks = parseAnnihilationWeeks(weeks, effects);

    if (!iSet) params.interations = 10000;      // Vychozi hodnoty
    if (!hSet) params.rodents = 80;
    if (!pSet) params.predatorCnt = 1;
    if (!zSet) params.predatorKills = 8;
    if (!tSet) params.annihilationWeeks.push_back({36, 50});    // Orba pred setim

    if (!sSet) SEED = time(NULL);

    return params;
}

int main(int argc, char **argv) {
    //srand(time(NULL));
    Parameters params = parseArgs(argc, argv);
    srand(SEED);

    fieldArea field(params.rodents, params.predatorCnt, params.predatorKills, params.annihilationWeeks);

    int population;
    for (int i = 0; i < params.interations; i++) {
        field.evolve();
        population = field.printField();
        std::cout << "seed: " << SEED << std::endl;
        std::cout << "vyhladovelo: " << Starved << std::endl;

        if (population > MAX_RODENTS) {
            cout << "Doslo k premnozeni." << endl;
            exit(0);
        }
        if (population <= MIN_RODENTS) {
            cout << "Doslo k vyhubeni." << endl;
            exit(0);
        }

        usleep(500000);     // 0.5s
    }
    
    return 0;
}