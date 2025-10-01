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
int game_max = 25; // ennyi gyümölcs fog esni összesen
const int bit_upper = 35; // a felső, első kijelző  bit-je (ezt fogjuk léptetni, ha a következő kijelzőt szeretnénk használni)
int fruit_pos_i = 0; //gyümölcs szegmenseinek az értéke(max 3 --> felső, középső,alsó szegmens)
int fruit_screen = 0; // az első fa kiválasztásához
int fruit_pos = 0; // fák kiválasztása (melyik kijelzőn essen a gyümölcs)
int game_number = 0;  // gyümölcsök esésének száma (max 25)
uint32_t currentMillis = 0; // aktuális milliszekundom érték
uint32_t previousMillis = 0; // előző miliszekundom érték
uint32_t interval_showed = 1000; // gyümölcs szegmenseinek a frissitése
uint32_t interval_off_basket = 1; // kosár szegmensének a frissítése
int screen = 0; //gyümölcs be-kikapcsolása
int counter_win = 0; // elkapott gyümölcsök száma
int pos = 3; // kosár poziciója (közép a legelső állapot)
int previousButton = 0;   // előző gomb állapota
int basket_show = 0; // kosár be-kikapcsolása
uint32_t previousMillis_basket = 0; //kosárnak az előző milliszekundum értéke
int nyom = 0; // gomb megnyomásáanak az értéke (-1 ha balra lépünk +1 ha jobbra lépünk)
int difficulty_i = 1; // nehézségi szint alsó határa (maximuma 4)
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Felső, első kijelzőhöz tartozó számok inicializálása --> ezeket fogjuk tologatni a további 3 kijelzőn a bit_upper váltózóval 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const int com_upper[10][8] =
  {
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
void
digits_upper (int moveX, int number)
{
  for (int i = 0; com_upper[number][i] != 0; i++)
    {

      LCD_SegmentSet (com_upper[number][i], bit_upper + moveX, true);
    }

}
;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Felső kijelző törlése (moveX -> hányas kijelzőt töröljuk, number -> melyik számot töröljük)
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void
digits_clear_upper (int number, int moveX)
{
  for (int i = 0; com_upper[number][i] != 0; i++)
    {

      LCD_SegmentSet (com_upper[number][i], bit_upper + moveX, false);
    }
}
;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Az alsó kijelzó szegmenseinek definiálása a következőképpen: 
//        sorok -> a kijelző sorszáma 
//        oszlopok -> kijelző teteje, felső/középső/függőleges elem, alsó/középső/függőleges elem, kijelző alja, szegmensekhez tartozó első illetve második bit
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const int com_lower[7][6] =
  { 
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

int
fruits (int diff, uint32_t msTicks)
{ 
  if (fruit_screen == 0)
    {
      fruit_pos = random ();
      fruit_screen = 1;
    }
  if (fruit_pos_i == 0 || fruit_pos_i == 2) 
    {

      digits_lower_fruits (fruit_pos, diff, 4);
    }
  else
    {
      digits_lower_fruits (fruit_pos, diff, 5);

    }
  if (fruit_pos_i >= 3)
    {
      if (fruit_pos_i == 3 && fruit_pos == pos) // elkapott gyümölcsök számolása
        {
          counter_win++;
        }
      fruit_pos_i = 0;
      game_number++;
      fruit_pos = random ();

    }
  if (game_number >= game_max) //játék vége
    {
      return -1;
    }
  return 0;
}
;
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

void
digits_lower_fruits (int tree, int diff, int x)
{ 
  if (screen == 0)
    {
      LCD_SegmentSet (com_lower[tree][fruit_pos_i], com_lower[tree][x], true); //bekapcsoljuk
      if (currentMillis - previousMillis >= interval_showed / diff)
        {
          screen = 1;
          previousMillis = currentMillis;

        }
    }
  if (screen == 1)
    {

      LCD_SegmentSet (com_lower[tree][fruit_pos_i], com_lower[tree][x], false); //kikapcsoljuk
      if (currentMillis - previousMillis >= interval_showed / diff)
        {
          screen = 0;

          fruit_pos_i++;
          previousMillis = currentMillis;
        }
    }
}
;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// aktuális milliszekundom beállítása
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void
changeCurrentMillis (uint32_t msTicks)
{
  currentMillis = msTicks;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Az alsó szegmens(kosár) beállítása, léptetése a gombokkal
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void
basket ()
{
  int button = button_push (); // aktuális gombállapot (0 = nincs, 1 = PB1 nyomva, -1 = PB0 nyomva)
  if (button == 1 && previousButton == 0)
    {
      nyom = 1;
      previousButton = 1;
    }
  if (button == -1 && previousButton == 0)
    {
      nyom = -1;
      previousButton = 1;
    }
  if (basket_show == 0)
    {
      LCD_SegmentSet (com_lower[pos][3], com_lower[pos][5], true); // bekapcsoljuk a következö szegmenst
      if (currentMillis - previousMillis_basket >= interval_showed)
        {
          basket_show = 1;
          previousMillis_basket = currentMillis;
        }
    }
  if (basket_show == 1)
    {
      LCD_SegmentSet (com_lower[pos][3], com_lower[pos][5], false); // kikpacsoljuk az előző szegmenst
      if (currentMillis - previousMillis_basket >= interval_off_basket)
        {
          basket_show = 0;
          if (previousButton == 1)
            {
              pos += nyom;
              nyom = 0;
              previousButton = 0;
            }
          previousMillis_basket = currentMillis;
        }
    }
  if (pos < 0) //kosár végértékeit ellenőrizzük, nem engedjük tovább lépni a kijelző szélein
    pos = 0; 
  if (pos > 6)
    pos = 6;

}
;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Az alsó szegmens törlése
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void
basket_clear ()
{
  LCD_SegmentSet (com_lower[pos][3], com_lower[pos][5], false);
}
;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// A felső kijelzőre történő elkapott(jobb kijelzők)/elkapatlan(bal kijelzők) gyümölcs 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void
counter_fruit ()
{
  digits_upper (0, counter_win % 10);
  digits_upper (1, counter_win / 10);
  LCD_SegmentSet (5, 39, true);
  digits_upper (2, (game_max - counter_win) % 10);
  digits_upper (3, (game_max - counter_win) / 10);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Kettő gomb(PB0,PB1) beállítása 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int
button_push ()
{ 
  GPIO_PinModeSet (gpioPortB, 9, gpioModeInputPull, 1);  // PB0
  GPIO_PinModeSet (gpioPortB, 10, gpioModeInputPull, 1); //PB1
  bool pb0_pressed = (GPIO_PinInGet (gpioPortB, 9) == 0);  // PB0 lenyomva = 0
  bool pb1_pressed = (GPIO_PinInGet (gpioPortB, 10) == 0); // PB1 lenyomva = 0

  if (pb0_pressed && !pb1_pressed)
    {
      return -1; // Csak PB0 lenyomva
    }
  else if (pb1_pressed && !pb0_pressed)
    {
      return 1;  // Csak PB1 lenyomva
    }
  else
    {
      return 0;  // Egyik gomb sincs lenyomva, vagy mindkettő lenyomva
    }
}

;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Nehézségi szint beállítása a PB0 gombbal
// Fokozatok 1-3, ahol az 1 a könnyűt jelenti
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

uint32_t
difficulty ()
{ 
  while (1)
    {

      digits_upper (0, difficulty_i);
      if (button_push () == -1)
        {
          return difficulty_i;
        }
      sl_udelay_wait (100000);
      digits_clear_upper (difficulty_i, 0);
      if(difficulty_i>=3){
          difficulty_i = 0;
      }
      difficulty_i++;

    }

}
;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// az alsó kijelzőket (fákat) randomizálja
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int
random ()
{ 
  int max = 6;
  int min = 0;
  return (rand () % (max - min + 1)) + min;
}
;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Játék elindítása a PB1 gombbal
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void
game_start ()
{
  while (button_push () != 1)
    {

    };
  digits_clear_upper (difficulty_i, 0);
}
