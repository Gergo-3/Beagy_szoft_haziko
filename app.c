//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//A játék célja
//A feladat minél több lehulló gyümölcs begyűjtése négy banánfáról. A banánfákat elhagyó gyümölcsök az alsó (alfanumerikus) kijelző négy mezeje valamelyikén eshetnek le. 
// Ezeket alul egy kosár segítségével kell elkapni.
//A játék indítása
//A játék elején lehessen állítani a potyogás sebességét (pl. a PB0 gombbal pörgessetek egy nehézségi szint számlálót, amit a program megjelenít a kijelzőn, majd PB1-re indul a játék).
//A játék menete
//Ha az egyik banánfán egy gyümölcs már nagyon érett, akkor ezt a hozzá tartozó mező legfelső vízszintes szegmensének kigyújtásával jelezzétek. 
// Ezek után a banán egy meghatározott sebességgel elkezd lefele esni az adott mező középső függőleges szegmensein (értelem szerűen először a felső szegmenset éri el, majd az alsót). 
// Ha a banán földet érésekor alatta volt a kosár, akkor sikerült elkapni. A kosarat a pozíciójának megfelelő mező alsó vízszintes szegmense jelképezi. Mozgatni pedig a PB0 ill. 
// PB1 nyomógombokkal lehet balra ill. jobbra. Összesen 25 banán elkapására van lehetőség, utána vége van a játéknak. Az eddig lehullt illetve az elkapott banánok számát a felső 
// (numerikus) kijelző egyik illetve másik oldali két-két digitjén jelezzétek (középen a kettősponttal elválasztva).
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Könyvtárak inicializálása
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "segmentlcd_individual.h"
#include "segmentlcd.h"
#include <sl_udelay.h>
#include "em_device.h"
#include "em_cmu.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Globális változók inicializálása
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile uint32_t msTicks; //1ms-ként számol
int i = 0; // játék véégt jelzi

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Minden megszaításkor növeli az msTicks értékét
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void
SysTick_Handler (void)
{
  msTicks++; 
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Függvények deklarálása
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void game (void);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Az alkalmazás indulskori inicializáló függvénye
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void
app_init (void)
{
  SegmentLCD_Init (false); // lcd kijelző normál üzemmódban induljon el és ne energiatakarékosban

  if (SysTick_Config (CMU_ClockFreqGet (cmuClock_CORE) / 1000)) //Beállítja a Systick időzítőt hogy 1 ms-onként megszakítást adjon
    while (1)
      {

      };



}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Játék fő logikája
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void game ()
{
  uint32_t game_difficulty = difficulty (); //nehézségi szint beállítása
  game_start();
  sl_udelay_wait (1000000); //1 másodperc várakozás
 while (i != -1)
    {
      changeCurrentMillis (msTicks);
      i = fruits (game_difficulty, msTicks); //gyümölcsök esése ha eléri a 25-t visszaad egy -1-t, hogy kilépjünk a ciklusból
      basket (); //kosár mozgatás
    }
  basket_clear (); //Az utolsó kosár poziciójának törlése
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


