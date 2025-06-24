#include <string.h>
#include <LiquidCrystal_I2C.h>

#include "printer.h"

Printer::Printer() {
  this->buffer = new char*[2];
  this->buffer[0] = new char[16];
  this->buffer[1] = new char[16];

  this->oldBuffer = new char*[2];
  this->oldBuffer[0] = new char[16];
  this->oldBuffer[1] = new char[16];

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 16; j++) {
      this->buffer[i][j] = ' ';
      this->oldBuffer[i][j] = ' ';
    }
  }


  this->x = 0;
  this->y = 0;
}

Printer::~Printer() {

}

void Printer::clearLine(int line){
  if (line != 0 && line != 1) {
    return;
  }

  for (int i = 0; i < 16; i++) {
    this->buffer[line][i] = ' ';
  }
}

void Printer::clear() {
  this->clearLine(0);
  this->clearLine(1);
}

void Printer::print(String inp) {
  int y = this->y;

  for (int i = 0; i < inp.length(); i++) {
    int x = this->x + i;
    if (x > 15) return;

    this->buffer[y][x] = inp[i];
  }
}

// void Printer::print(char * inp) {
 
// }

void Printer::setCursor(int x, int y) {
  this->x = x;
  this->y = y;
}

boolean Printer::bufferIsDifferent(int x, int y) {
  if (x > 15 || x < 0) return false;
  if (y != 0 && y != 1) return false;  

  char oc = this->oldBuffer[y][x];
  char c = this->buffer[y][x];

  return oc != c;
}

void Printer::updateFrame(LiquidCrystal_I2C lcd) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 16; j++) {
      if (!this->bufferIsDifferent(j, i)) {
        continue;
      }

      lcd.setCursor(j, i);
      lcd.print(this->buffer[i][j]);
      
      this->oldBuffer[i][j] = this->buffer[i][j];
    }
  }
}
