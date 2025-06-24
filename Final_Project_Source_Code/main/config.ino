#include "config.h"



void Config::setDeltaT(int ms) {
  if (ms > 2000 || ms < 10) return;

  this->deltaT = ms;
}


int Config::getPressedSignature() {
  int a = 0;

  if (!this->BTT1_STATE) {
    a += 1;
  }

  if (!this->BTT2_STATE) {
    a += 2;
  }

  if (!this->BTT3_STATE) {
    a += 4;
  }

  return a;
}

int Config::getPressedFrames() {
  return this->PRESSED_FRAME;
}

void Config::updatePressedSigState() {
  int newSig = this->getPressedSignature();

  if (newSig != this->PRESSED_SIG || this->PUMP_IS_OPEN) {
    this->PRESSED_FRAME = 0;
  } else if (newSig != 0) {
    this->PRESSED_FRAME++;
  }

  this->PRESSED_SIG = newSig;
}

void Config::startPumpProcess(double amount) {
  if (this->PUMP_IS_OPEN) return;

  
  this->PUMP_IS_OPEN = true;
  this->PUMP_AMOUNT_NEED = amount;
  this->PUMP_AMOUNT_DONE = 0.0;
  //? Reset the pulse for the sensor
  digitalWrite(RELAY_PIN, HIGH);
}

void Config::stopPumpProcess() {
  this->PUMP_IS_OPEN = false;
  digitalWrite(RELAY_PIN, LOW);
}

int Config::frameToMs(int frame) {
  return frame * this->deltaT;
}

int Config::msToFrame(int ms) {
  return (int) (ms / this->deltaT);
}