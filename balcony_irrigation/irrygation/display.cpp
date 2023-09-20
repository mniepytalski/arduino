#include "display.h"
#include <Arduino.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header


hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip  

Display::Display() {
}

void Display::init() {
  if (lcdStatus==1 ) {
    lcdStatus = lcd.begin(LCD_COLS, LCD_ROWS);
    if(lcdStatus) {
      hd44780::fatalError(lcdStatus); // does not return
    }
  }  
}

void Display::print(int col, int row, int digit) {
  if (lcdStatus!=1 ) {
    lcd.setCursor(col, row);  
    lcd.print(digit);
  }
}

void Display::print(int col, int row, char *text) {
  if (lcdStatus!=1 ) {
    lcd.setCursor(col, row);  
    lcd.print(text);
  }
}