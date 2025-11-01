//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Könyvtárak deklarálása
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "segmentlcd_individual.h"
#include <stdbool.h>
#include <em_lcd.h>
#include "em_gpio.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Globális változók deklarálása
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define FRUIT_MAX 25 // ennyi gyümölcs fog esni összesen
#define SCREEN_UPPER_BIT 35 // a felső, első kijelző  bit-je (ezt fogjuk léptetni, ha a következő kijelzőt szeretnénk használni)
int fruit_position_row = 0; //gyümölcs szegmenseinek az értéke(max 3 --> felső, középső,alsó szegmens)
int fruit_position_column_first = 0; // az első fa kiválasztásához
int fruit_position_column = 0; // fák kiválasztása (melyik kijelzőn essen a gyümölcs)
int fruit_number = 0;  // gyümölcsök esésének száma (max 25)
uint32_t currentMillis = 0; // aktuális milliszekundom érték
uint32_t previousMillis = 0; // előző miliszekundom érték
uint32_t interval_showed = 1000; // gyümölcs szegmenseinek a frissitése
uint32_t interval_off_basket = 1; // kosár szegmensének a frissítése
bool fruit_active = 0; //gyümölcs be-kikapcsolása
int fruit_caught = 0; // elkapott gyümölcsök száma
int basket_position_column = 3; // kosár poziciója (közép a legelső állapot)
int previousButton = 0;   // előző gomb állapota
bool basket_visible = 0; // kosár be-kikapcsolása
uint32_t previousMillis_basket = 0; //kosárnak az előző milliszekundum értéke
int button_current = 0; // gomb megnyomásáanak az értéke (-1 ha balra lépünk +1 ha jobbra lépünk)
int difficulty_i = 1; // nehézségi szint alsó határa (maximuma 4)
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Felső, első kijelzőhöz tartozó számok inicializálása --> ezeket fogjuk tologatni a további 3 kijelzőn a bit_upper váltózóval 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const int com_upper[10][8] = {
		{ 6, 7, 5, 3, 1, 2, 0 }, // -->0
		{ 5, 2, 0 }, // -->1
		{ 7, 5, 4, 3, 1, 0 }, // -->2
		{ 7, 5, 4, 2, 1, 0 }, // -->3
		{ 6, 4, 5, 2, 0 }, // -->4
		{ 6, 7, 1, 2, 4, 0 }, // -->5
		{ 7, 6, 1, 2, 3, 4, 0 }, // -->6
		{ 7, 5, 2, 0 }, //-->7
		{ 5, 1, 2, 3, 4, 7, 6, 0 }, //-->8
		{ 5, 1, 2, 4, 7, 6, 0 } // --> 9
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// A felső kijelző beüzemelése, melyik kijelzőn(moveX) és hányas szám(number) jelenjen meg  
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void digits_upper(int moveX, int number) {
	digits_clear_upper(moveX);
	for (int i = 0; com_upper[number][i] != 0; i++) {
		LCD_SegmentSet(com_upper[number][i], SCREEN_UPPER_BIT + moveX, true);
	}

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Felső kijelző törlése (moveX -> hányas kijelzőt töröljuk, number -> melyik számot töröljük)
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void digits_clear_upper(int moveX) {
	//for (int i = 0; com_upper[number][i] != 0; i++) {
	//
	//	LCD_SegmentSet(com_upper[number][i], SCREEN_UPPER_BIT + moveX, false);
	//}
	for (unsigned i = 0; i < 8; i++)
	{
		LCD_SegmentSet(i, SCREEN_UPPER_BIT + moveX, false);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Az alsó kijelzó szegmenseinek definiálása a következőképpen: 
//        sorok -> a kijelző sorszáma 
//        oszlopok -> kijelző teteje, felső/középső/függőleges elem, alsó/középső/függőleges elem, kijelző alja, szegmensekhez tartozó első illetve második bit
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const int com_lower[7][6] = {
		{ 1, 3, 5, 7, 13, 14 }, // 1 kijelzo
		{ 1, 3, 5, 7, 15, 16 }, // 2 kijelzo
		{ 1, 3, 5, 7, 17, 18 }, // 3 kijelzo
		{ 1, 3, 5, 7, 19, 28 }, // 4 kijelzo
		{ 0, 3, 4, 7, 29, 30 }, // 5 kijelzo
		{ 0, 3, 4, 7, 31, 32 }, // 6 kijelzo
		{ 1, 3, 5, 7, 33, 34 }, // 7 kijelzo
		};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Gyümölcsök esését éllítjuk be, azaz ehhez a fentebbi mátrixból válogatjuk ki a 0,1,2,4,5 oszlopkat úgy hogy a 0 és a 2 oszlophoz a 4 oszlopot(bit1 értéket tároljuk) rendeljük, 
//a többihez az 5-ös oszlopot(bit2-t tároljuk). Illetve leállítjuk a játékot ha elértük a game_max változó értéket, jelen esetben 25 játékot engedünk. 
//Itt számoljuk még az elkapott gyümölcsöket a kosárral
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int fruits(int diff, uint32_t msTicks) {
	if (fruit_position_column_first == 0) {
		fruit_position_column = random();
		fruit_position_column_first = 1;
	}
	if (fruit_position_row == 0 || fruit_position_row == 2) {
		digits_lower_fruits(fruit_position_column, diff, 4);
	} else {
		digits_lower_fruits(fruit_position_column, diff, 5);
	}
	if (fruit_position_row >= 3) {
		if (fruit_position_row == 3 && fruit_position_column == basket_position_column) // elkapott gyümölcsök számolása
			{
				fruit_caught++;
			}
		fruit_position_row = 0;
		fruit_number++;
		fruit_position_column = random();
	    counter_fruit (); //végeredmény kiiratása a felső kijelzőkre

	}
	if (fruit_number >= FRUIT_MAX) //játék vége
	{
		return -1;
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Függvények deklarálása
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
 int
 fruit_rand ()
 {

 int rand = random ();

 return rand;
 }
 */
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// A gyümölcs szegmenseinek az esését állítjuk be 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void digits_lower_fruits(int tree, int diff, int x) {
	if (fruit_active == 0) {
		LCD_SegmentSet(com_lower[tree][fruit_position_row], com_lower[tree][x],
		true); //bekapcsoljuk
		if (currentMillis - previousMillis >= interval_showed / diff) {
			fruit_active = 1;
			previousMillis = currentMillis;

		}
	}
	if (fruit_active == 1) {

		LCD_SegmentSet(com_lower[tree][fruit_position_row], com_lower[tree][x],
		false); //kikapcsoljuk
		if (currentMillis - previousMillis >= interval_showed / diff) {
			fruit_active = 0;

			fruit_position_row++;
			previousMillis = currentMillis;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// aktuális milliszekundom beállítása
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void changeCurrentMillis(uint32_t msTicks) {
	currentMillis = msTicks;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Az alsó szegmens(kosár) beállítása, léptetése a gombokkal
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void basket() {
	int currentbutton = button_push(); // aktuális gombállapot (0 = nincs, 1 = PB1 nyomva, -1 = PB0 nyomva)

	int change = 0;

	if (currentbutton != previousButton)
	{
		change = currentbutton;
	}

	if (basket_visible == 0) {
		LCD_SegmentSet(com_lower[basket_position_column][3],
		com_lower[basket_position_column][5], true); // bekapcsoljuk a következö szegmenst
		basket_visible = 1;
	}
	if (basket_visible == 1) {
		LCD_SegmentSet(com_lower[basket_position_column][3],
		com_lower[basket_position_column][5], false); // kikpacsoljuk az előző szegmenst
		basket_position_column += change;
		LCD_SegmentSet(com_lower[basket_position_column][3],
		com_lower[basket_position_column][5], true); // bekapcsoljuk az előző szegmenst
	}
	if (basket_position_column < 0) //kosár végértékeit ellenőrizzük, nem engedjük tovább lépni a kijelző szélein
		basket_position_column = 0;
	if (basket_position_column > 6)
		basket_position_column = 6;

	previousButton = currentbutton;

}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Az alsó szegmens törlése
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void basket_clear() {
	LCD_SegmentSet(com_lower[basket_position_column][3], com_lower[basket_position_column][5],
	false);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// A felső kijelzőre történő elkapott(jobb kijelzők)/elkapatlan(bal kijelzők) gyümölcs 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void counter_fruit() {
	digits_upper(0, fruit_number % 10);
	digits_upper(1, fruit_number / 10);
	LCD_SegmentSet(5, 39, true);
	digits_upper(2, fruit_caught % 10);
	digits_upper(3, fruit_caught / 10);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Kettő gomb(PB0,PB1) beállítása 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int button_push() {
	GPIO_PinModeSet(gpioPortB, 9, gpioModeInputPull, 1);  // PB0
	GPIO_PinModeSet(gpioPortB, 10, gpioModeInputPull, 1); //PB1
	bool pb0_pressed = (GPIO_PinInGet(gpioPortB, 9) == 0);  // PB0 lenyomva = 0
	bool pb1_pressed = (GPIO_PinInGet(gpioPortB, 10) == 0); // PB1 lenyomva = 0

	if (pb0_pressed && !pb1_pressed) {
		return -1; // Csak PB0 lenyomva
	} else if (pb1_pressed && !pb0_pressed) {
		return 1;  // Csak PB1 lenyomva
	} else {
		return 0;  // Egyik gomb sincs lenyomva, vagy mindkettő lenyomva
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Nehézségi szint beállítása a PB0 gombbal
// Fokozatok 1-3, ahol az 1 a könnyűt jelenti
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

uint32_t difficulty() {
	while (1) {

		digits_upper(0, difficulty_i);
		if (button_push() == -1) {
			return difficulty_i;
		}
		sl_udelay_wait(100000);
		digits_clear_upper(difficulty_i);
		if (difficulty_i >= 3) {
			difficulty_i = 0;
		}
		difficulty_i++;

	}

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// az alsó kijelzőket (fákat) randomizálja
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int random() {
	int max = 6;
	int min = 0;
	return (rand() % (max - min + 1)) + min;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Játék elindítása a PB1 gombbal
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void game_start() {
	while (button_push() != 1) {

	};
	digits_clear_upper(difficulty_i);
	counter_fruit();
}
