#ifndef CONFIG_H
#define CONFIG_H


const int RELAY_PIN = 3;
const int BTT1_PIN = 4;
const int BTT2_PIN = 5;
const int BTT3_PIN = 6;

const int NOISE_MAKER_PIN = 8;

const int BLUETOOTH_RX_PIN = 11;
const int BLUETOOTH_TX_PIN = 10;
const int BLUETOOTH_STATUS = 12;

const int SENSOR_PULLUP_PIN = 2;
const int SENSOR_UPDATE_DELAY = 100; // ms
double SENSOR_CALIB_FACTOR = 68;

class Config {
  public: 
    int frame = 0;
    boolean systemIsReady = true;
    
    int deltaT = 10; //ms
    void setDeltaT(int ms);

    int frameToMs(int frame);
    int msToFrame(int ms);
    
    //* Get current pressed button(s) signature
    //* B1: 1, B2: 2, B3: 4
    //* Return sum of all pressed
    int getPressedSignature();
    int getPressedFrames();
    int BTT1_STATE = 0;
    int BTT2_STATE = 0;
    int BTT3_STATE = 0;
    
    
    //* Save the current pressed button (and it's frame)
    //* Frame counr resets if signature change
    void updatePressedSigState();
    int PRESSED_SIG = 0;
    int PRESSED_FRAME = 0;

    void startPumpProcess(double amount);
    void stopPumpProcess();
    boolean PUMP_IS_OPEN = false;
    double PUMP_AMOUNT_DONE = 0;
    double PUMP_AMOUNT_NEED = 0;

    int EMERGENCY_COOLDOWN = 0;

    //* Bluetooth stuff
    boolean BT_CONNECTED = false;
};


#endif