#include "printer.h"
#include "menu.h"
#include "config.h"


void Page::getLines(Config &cfg, String * str)  {
  str[0] = "Default Page";
  str[1] = "Not implemented";
}

void MenuPage::getLines(Config &cfg, String * str)  {
  str[0] = "Smart Water";
  str[1] = "Pouring - Grp. 2";
}

void ButtonPressedPage::getLines(Config &cfg, String * str) {
  str[0] = "Hold to confirm ";

  int bttSig = cfg.getPressedSignature();
  int sigFrame = cfg.getPressedFrames();

  double timePassed = cfg.frameToMs(sigFrame) / 1000.0;
  
  int remain = min(3, 4 - timePassed);

  String tminus = "Hold for ";
  tminus += String(remain);
  tminus += "s to:";

  str[0] = tminus;

  switch (bttSig) {
    case 1: {
      str[1] = "Pump 100.0ml";
      break;
    }

     case 2: {
      str[1] = "Pump 300.0ml";
      break;
     }

     case 4: {
      str[1] = "Pump 500.0ml";
      break;
     }

     case 7: {
      str[0] = "EMERGENCY Break";
      str[1] = "No action needed";
      break;
     }

     default: {    
      str[0] = "Error: Select";
      str[1] = "1 button only";

      break;
    }
  }
}

void PumpingPage::getLines(Config &cfg, String * str) {

  if (cfg.EMERGENCY_COOLDOWN) {
    str[0] = "EMERGENCY Break";
    str[1] = "Pump is stopped";
    return;
  }

  double need = cfg.PUMP_AMOUNT_NEED;
  double done = min(need, cfg.PUMP_AMOUNT_DONE);

  double perf = need ? (done * 100.0) / (need * 1.0) : 0;

  str[0] = "Status: " + roundToStr(perf, 2) + "%";
  str[1] = String((int)done) + "/" + String((int)need) + "ml";
}

void StatusPage::getLines(Config &cfg, String * str)  {  
  
  if (cfg.systemIsReady) {
    str[0] = "Status     READY";
  } else {
    str[0] = "Status      BUSY";
  }

  String btStatus = "Bluetooth";

  if (cfg.BT_CONNECTED) {
    btStatus += "    CON";
  } else {
    btStatus += "  N_CON";
  }

  str[1] = btStatus;
}

void EmergencyPage::getLines(Config &cfg, String * str) {
  str[0] = "EMERGENCY BREAK";
  str[1] = "PUMP STOPPED";
}


void PageManager::changePage(Page * page, unsigned int frame) {
  this->currentPage = page;
  this->frameRemain = frame;
}

void PageManager::changePage() {
  this->currentPage = this->defaultPage;
  this->frameRemain = 1;
}

void PageManager::printPage(Printer &printer, Config &config) {

  if (frameRemain <= 0) {
    this->changePage();
    return;
  }

  String lines[2];
  this->currentPage->getLines(config, lines);

  printer.clear();

  printer.setCursor(0, 0);
  printer.print(lines[0]);

  printer.setCursor(0, 1);
  printer.print(lines[1]);

  if (this->currentPage == this->defaultPage) {
    // Default page, should be persistant
  } else {
    this->frameRemain--;
  }
}


String roundToStr(double value, int precision) {
  double scale = pow(10, precision);  
  double rounded = round(value * scale) / scale;

  return String(rounded);
}