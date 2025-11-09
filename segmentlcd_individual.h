

#ifndef SRC_SEGMENTLCD_INDIVIDUAL_H_
#define SRC_SEGMENTLCD_INDIVIDUAL_H_

#include <stdbool.h>
#include <em_lcd.h>
#include "em_gpio.h"
#include <stdint.h>
#include <stdlib.h>

void digits_upper(int,int);
void digits_clear_upper(int);
int fruits(int);
void digits_lower_fruits(int,int,int);
void changeCurrentMillis(uint32_t);
void basket();
void basket_clear();
void counter_fruit();
int button_push();
uint32_t difficulty();
int random();
void game_start();

#endif 
