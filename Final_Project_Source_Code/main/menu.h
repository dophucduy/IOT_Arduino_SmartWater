#include "printer.h"
#include "config.h"

#ifndef MENU_H
#define MENU_H


class Page {
  // friend class PageManager;
  
  public:   
    virtual void getLines(Config &cfg, String * str);

    Page() {}
};

class MenuPage: public Page {
  public:
    void getLines(Config &cfg, String * str);
};

class StatusPage: public Page {
  public:
    void getLines(Config &cfg, String * str);
};


class ButtonPressedPage: public Page {
  public:
    void getLines(Config &cfg, String * str);
};

class PumpingPage: public Page {
  public:
    void getLines(Config &cfg, String * str);
};

class EmergencyPage: public Page {
  public:
    void getLines(Config &cfg, String * str);
};

class PageManager {
  public:
    Page * defaultPage = new Page();
    Page * currentPage = this->defaultPage;
    int frameRemain = 0;

    void changePage(Page * page, unsigned int frame);

    void changePage(); 

    void printPage(Printer &printer, Config &config);
};

#endif
