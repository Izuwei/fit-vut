//	Autor: Jakub Sadilek (xsadil07)
//
//	Projekt byl vypracovan v kostre programu pro demonstracni ucely fitkitu3 dodaneho od pana Bidla,
//	ze ktereho jsem z velke casti cerpal informace. Zmeny spocivaly hlavne ve funkcionalite
//	vysledneho programu a ovladani potrebnych PINu na desce.
//
//	Muj autorsky podil na kodu cca 70%, zbytek ponechan z puvodniho dema.
//
//	Datum posledni zmeny: 16.12.2019
//
//       An example for demonstrating basic principles of FITkit3 usage.
//
//	IMP Project
//
// It includes GPIO - inputs from button press/release, outputs for LED control,
// timer in output compare mode for generating periodic events (via interrupt
// service routine) and speaker handling (via alternating log. 0/1 through
// GPIO output on a reasonable frequency). Using this as a basis for IMP projects
// as well as for testing basic FITkit3 operation is strongly recommended.
//
//            (c) 2019 Michal Bidlo, BUT FIT, bidlom@fit.vutbr.cz
////////////////////////////////////////////////////////////////////////////
/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1)<<(x & GPIO_PIN_MASK)))

/* Mapping of LEDs and buttons to specific port pins: */
// Note: only D9, SW3 and SW5 are used in this sample app
#define LED_D9  0x20      // Port B, bit 5
#define LED_D10 0x10      // Port B, bit 4
#define LED_D11 0x8       // Port B, bit 3
#define LED_D12 0x4       // Port B, bit 2

/*
 * Tlacitka
*/
#define BTN_SW2 0x400     // Port E, bit 10
#define BTN_SW3 0x1000    // Port E, bit 12
#define BTN_SW4 0x8000000 // Port E, bit 27
#define BTN_SW5 0x4000000 // Port E, bit 26
#define BTN_SW6 0x800     // Port E, bit 11

#define SPK 0x10          		// Speaker is on PTA4
#define APorts 0x3F000FD0		// Porty k ovladani displeje a SPK
#define SEGMENTS 0x39000640		// Segmenty displeje (bez tecky)

/* Pozice **/
#define FIRST 0x100
#define SECOND 0x800
#define	THIRD 0x80
#define	FOURTH 0x2000000

/* Segmenty (v log. 0 -> zaple)*/
#define ZERO 0x5800000
#define ONE 0x2D000440
#define TWO 0x14800040
#define THREE 0xC800040
#define FOUR 0x2C800400
#define FIVE 0xC800200
#define SIX 0x4800200
#define SEVEN 0x2D800040
#define EIGHT 0x4800000
#define NINE 0xC800000

#define DOT 0x4000000	// v log.1 = vypla

int segmentNumbers[] = { ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE };
int pressed_up = 0, pressed_down = 0, pressed_right = 0, pressed_left = 0, pressed_stop = 0;
int beep_flag = 0, beep_count = 0, countdown_flag = 0;
unsigned int seconds = 0;
unsigned int compare = 1000;

int blink_mode = 0, blink_status = 0, blink_count = 0;

/* A delay function */
void delay(unsigned long long int bound) {
  for (unsigned long long int i=0 ; i < bound; i++);
}

/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK;

    /* Set corresponding PTB pins (connected to LED's) for GPIO functionality */
    PORTB->PCR[5] = PORT_PCR_MUX(0x01); // D9
    PORTB->PCR[4] = PORT_PCR_MUX(0x01); // D10
    PORTB->PCR[3] = PORT_PCR_MUX(0x01); // D11
    PORTB->PCR[2] = PORT_PCR_MUX(0x01); // D12

    /* Nastaveni PINu pro segremnty displeje jako vstupne-vystupni (viz. dokumentace) */
    PORTA->PCR[8] = PORT_PCR_MUX(0x01);
    PORTA->PCR[6] = PORT_PCR_MUX(0x01);
    PORTA->PCR[7] = PORT_PCR_MUX(0x01);
    PORTA->PCR[10] = PORT_PCR_MUX(0x01);
    PORTA->PCR[11] = PORT_PCR_MUX(0x01);
    PORTA->PCR[9] = PORT_PCR_MUX(0x01);
    PORTA->PCR[27] = PORT_PCR_MUX(0x01);
    PORTA->PCR[26] = PORT_PCR_MUX(0x01);
    PORTA->PCR[24] = PORT_PCR_MUX(0x01);
    PORTA->PCR[29] = PORT_PCR_MUX(0x01);
    PORTA->PCR[28] = PORT_PCR_MUX(0x01);
    PORTA->PCR[25] = PORT_PCR_MUX(0x01);

    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // SW2
    PORTE->PCR[12] = PORT_PCR_MUX(0x01); // SW3
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // SW4
    PORTE->PCR[26] = PORT_PCR_MUX(0x01); // SW5
    PORTE->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    PORTA->PCR[4] = PORT_PCR_MUX(0x01);  // Speaker

    /* Change corresponding PTB port pins as outputs */
    PTB->PDDR = GPIO_PDDR_PDD(0x3C);     // LED ports as outputs
    PTA->PDDR = GPIO_PDDR_PDD(APorts);
    PTB->PDOR |= GPIO_PDOR_PDO(0x3C);    // turn all LEDs OFF
    PTA->PDOR = GPIO_PDOR_PDO(~APorts);  // Speaker off, segments off, beep_flag is false
}

/* Obsluha preruseni timeru */
void LPTMR0_IRQHandler(void)
{
    LPTMR0_CMR = compare;                // !! the CMR reg. may only be changed while TCF == 1
    LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;   // writing 1 to TCF tclear the flag

    if (blink_mode) {					 // Jsme ve stavu blikani
    	blink_status = !blink_status;	 // Invertujeme stav bliknuti 1->0 ; 0->1
    	blink_count--;					 // Snizime pocitadlo bliknuti
    	return;
    }

    PTB->PDOR |= GPIO_PDOR_PDO(0x3C);	 // Vypneme LED diody
    PTA->PDOR &= GPIO_PDOR_PDO(~SPK);	 // Vypneme speaker
    countdown_flag = 1;					 // Inicializujeme odpocet 1s pro obsluhu
}

/* Inicializace timeru */
void LPTMR0Init(int count)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK; // Enable clock to LPTMR
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;   // Turn OFF LPTMR to perform setup
    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK   // LPO feeds directly to LPT
                 | LPTMR_PSR_PCS(1)) ;   // use the choice of clock
    LPTMR0_CMR = count;                  // Set compare value
    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK    // Clear any pending interrupt (now)
                 | LPTMR_CSR_TIE_MASK    // LPT interrupt enabled
                );
    NVIC_EnableIRQ(LPTMR0_IRQn);         // enable interrupts from LPTMR0
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;
}

/* Funkce zobrazi aktualne nastaveny cas stopek na displeji */
void displayTime(int seconds) {
	int dispSec = seconds % 60;		// Pocet sekund
	PTA->PDOR = GPIO_PDOR_PDO(FOURTH | segmentNumbers[dispSec % 10]);	// Jednotkova cast sekund
	delay(500);
	PTA->PDOR = GPIO_PDOR_PDO(THIRD | segmentNumbers[dispSec / 10]);	// Desitkova cast sekund
	delay(500);

	int dispMin = seconds / 60;		// Pocet minut
	PTA->PDOR = GPIO_PDOR_PDO((SECOND | segmentNumbers[dispMin % 10]) & ~DOT);	// Jednotkova cast minut
	delay(500);
	PTA->PDOR = GPIO_PDOR_PDO(FIRST | segmentNumbers[dispMin / 10]);	// Desitkova cast minut
	delay(500);
}

/* Funkce spusti zablikani segmentovahe displeje */
void displayBlink() {
	LPTMR0_CMR = 250;	// Stav displeje se zmeni po 0,25s
	compare = 250;
	blink_mode = 1;		// Inicializace blikani
	blink_count = 4;	// Pocet zmen stavu
	LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;	// Zapnuti timeru
}

int main(void)
{
    MCUInit();
    PortsInit();
    LPTMR0Init(1000);	// Inicializace timeru

    while (1) {
        /* Zobrazeni casu na displeji nebo vypnuti displeje */
    	if (blink_status) {
    		PTA->PDOR = GPIO_PDOR_PDO(~(SEGMENTS | ~DOT));	// Vypnuti displeje
    		delay(500);
    	}
    	else {
    		displayTime(seconds);							// Zobrazeni casu
    	}

    	/* Tlacitko nahoru: zvyseni casu o 1s */
        if (!pressed_up && !(GPIOE_PDIR & BTN_SW5) && !beep_flag) {
        	if (seconds < 6000)		// MAX 99min 59s
        		seconds++;

            pressed_up = 1;
        }
        else if (GPIOE_PDIR & BTN_SW5) pressed_up = 0;

        /* Tlacitko dolu: snizeni casu o 1s */
        if (!pressed_down && !(GPIOE_PDIR & BTN_SW3) && !beep_flag) {
        	if (seconds > 0)		// MIN 0min 0s
        		seconds--;

            pressed_down = 1;
        }
        else if (GPIOE_PDIR & BTN_SW3) pressed_down = 0;

        /* Tlacitko doprava: spusteni odpocitavani po 1s */
        if (!pressed_right && !(GPIOE_PDIR & BTN_SW2) && !(LPTMR0_CSR & LPTMR_CSR_TEN_MASK)) {
        	if (seconds > 0) {			// Cas musi byt minimalne 1s
        		LPTMR0_CMR = 1000;		// Nastaveni snizovani po 1s
        		compare = 1000;
        		LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;	// Zapnuti timeru
        	}
        	else {						// Jinak zablikame displejem pro upozorneni uzivatele
        		displayBlink();
        	}

        	pressed_right = 1;
        }
        else if (GPIOE_PDIR & BTN_SW2) pressed_right = 0;

        /* Tlacitko doleva: spusteni zrychleneho odpocitavani po 0,5s */
        if (!pressed_left && !(GPIOE_PDIR & BTN_SW4) && !(LPTMR0_CSR & LPTMR_CSR_TEN_MASK)) {
        	if (seconds > 0) {			// Cas musi byt minimalne 1s
        		LPTMR0_CMR = 500;		// Nastaveni snizovani po 0,5s
        		compare = 500;
        		LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;	// Zapnuti timeru
        	}
        	else {						// Jinak zablikame displejem pro upozorneni uzivatele
        	    displayBlink();
        	}

        	pressed_left = 1;
        }
        else if (GPIOE_PDIR & BTN_SW4) pressed_left = 0;

        /* Zastaveni odpocitavani a obnoveni vychoziho stavu */
        if (!pressed_stop && !(GPIOE_PDIR & BTN_SW6)) {
        	LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;	// Vypnuti stopek
        	seconds = 0;						// Reset casu na 00:00
        	PTB->PDOR |= GPIO_PDOR_PDO(0x3C);	// Vypnuti ledek
        	PTA->PDOR &= GPIO_PDOR_PDO(~SPK);	// Vypnuti bzucaku
            beep_flag = 0;						// Signalizacni flagy na 0 (vychozi hodnoty)
            beep_count = 0;
            pressed_stop = 1;
            blink_mode = 0;
            blink_status = 0;
            blink_count = 0;
        }
        else if (GPIOE_PDIR & BTN_SW6) pressed_stop = 0;

        /* Obsluha odpoctu */
        if (countdown_flag)
        {
        	if (seconds > 0)		// Snizeni casu pokud neni 00:00
        		seconds--;

        	if (seconds == 0) {		// Cas vyprsel -> spusteni signalizace na 1s
        		beep_flag = 1;
        		beep_count++;
        	}

        	if (beep_count == 2) {	// Druha iterace -> vypnuti signalizace
        		beep_flag = 0;		// Vypnuti signalizace
        		beep_count = 0;		// Reset pomocne promenne
        		LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;    // Vypnuti timeru
        	}

        	countdown_flag = 0;		// Opet zapne timerHandler v dalsi interaci
        }

        /* Signalizace */
        if (beep_flag) {
        	PTA->PDOR |= GPIO_PDOR_PDO(SPK);	// Bzuceni
        	PTB->PDOR &= GPIO_PDOR_PDO(~0x3C);	// Zapnuti LED diod
        }

        /* Vypnuti signalizace blikani displeje */
        if (blink_mode && blink_count == 0) {
        	blink_mode = 0;
        	LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;    // Vypnuti timeru
        }
    }

    return 0;
}
