#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "printer.h"
#include "menu.h"


LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

//! On the document, this should be SoftwareSerial(RX, TX)
//! But somehow in this situation, it's not the case
//! Either the document is outdated or the printed labels on the
//! module itself is incorrect!
SoftwareSerial BtSerial =  SoftwareSerial(BLUETOOTH_RX_PIN, BLUETOOTH_TX_PIN);

Config state = Config();
Printer printer = Printer();
PageManager pageManager = PageManager();


//! NOTE TO SELF: 
//! Do not under any circumstances forget the pointer (*).
// It will do some type conversion and you will not like it.
// It's C++, it will do Static Binding.

//# Page declaration
Page * MENU_PAGE = new MenuPage();
Page * STATUS_PAGE = new StatusPage();
Page * BUTTON_PRESSED_PAGE = new ButtonPressedPage();
Page * PUMP_PAGE = new PumpingPage();
Page * EMERGENCY_PAGE = new EmergencyPage();

int SENSOR_PULSE_COUNT = 0;
void flowISR() {
  SENSOR_PULSE_COUNT++;
}

void setup() {
  pageManager.defaultPage = STATUS_PAGE; 


  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  BtSerial.begin(9600);

  pageManager.changePage(MENU_PAGE, state.msToFrame(5000));

  pinMode(BTT1_PIN, INPUT);
  pinMode(BTT2_PIN, INPUT);
  pinMode(BTT3_PIN, INPUT);
  pinMode(BLUETOOTH_STATUS, INPUT);

  pinMode(NOISE_MAKER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  pinMode(SENSOR_PULLUP_PIN, INPUT_PULLUP);
  
  BtSerial.println("Ready!");
}

void interrupt() {
  attachInterrupt(digitalPinToInterrupt(SENSOR_PULLUP_PIN), flowISR, RISING);
}

void noInterrupt() {
  detachInterrupt(digitalPinToInterrupt(SENSOR_PULLUP_PIN));
}

void loop() {
  delay(state.deltaT);
  // # On frame start
  handleBluetoothCon();                             // Check for connection
  updateCalibrationFactor();                        //? DEBUG: Update calibration factor

  // External event listener
  onFrameStart();  

  // # During frame
  listenToButtonState();                            // Button state change listener.
  handleBluetoothInput();                           // Handle input from bluetooth.
  handlePages();                                    // Choose the appropriate current page
  handleNoiseMaker();                               // Noise maker
  handleRelay();                                    // Handle relay
  
  
  // Handles everything else
  pageManager.printPage(printer, state);            // Update LCD buffer
  // writeToSerial(50);                             // Write serial (max 2 messages per 1s)

  // # Advance to next frame 
  state.frame++;

  // External event listener
  onFrameEnd();
}


void onFrameStart() {
  
}

void onFrameEnd() {
  printer.updateFrame(lcd);
}

void listenToButtonState() {
  int BTT1 = digitalRead(BTT1_PIN);
  state.BTT1_STATE = BTT1;

  int BTT2 = digitalRead(BTT2_PIN);
  state.BTT2_STATE = BTT2;

  int BTT3 = digitalRead(BTT3_PIN);
  state.BTT3_STATE = BTT3;

  // Serial.println("Button 1: " + String(BTT1));
  // Serial.println("Button 2: " + String(BTT2));
  // Serial.println("Button 3: " + String(BTT3));

  // state.PRESSED_FRAME++;  
  state.updatePressedSigState();
}

// int rateLimit = 0;
// void writeToSerial(int afterExecPauseForFrame) {
//   if (rateLimit > 0) {
//     rateLimit--;
//     return;
//   }

//   Serial.println("Button state: ");

//   Serial.print(" " + String(state.BTT1_STATE));
//   Serial.print(" " + String(state.BTT2_STATE));
//   Serial.print(" " + String(state.BTT3_STATE) + "\n");

//   rateLimit = afterExecPauseForFrame;
// }

void handlePages() {
  if (state.EMERGENCY_COOLDOWN) {
    pageManager.changePage(EMERGENCY_PAGE, 1);
    return;
  }

  if (state.PUMP_IS_OPEN) {
    pageManager.changePage(PUMP_PAGE, state.msToFrame(3000));
    return;
  }

  int bttSig = state.getPressedSignature();
  int pressedFrame = state.getPressedFrames();

  if (bttSig != 0) {
    pageManager.changePage(BUTTON_PRESSED_PAGE, 1);
  }
}

void handleNoiseMaker() {
  // int bttSig = state.getPressedSignature();
  int pressedFrame = state.getPressedFrames();
  
  boolean doNoise = pressedFrame == 1;

  digitalWrite(NOISE_MAKER_PIN, doNoise ? 1 : 0);
}

void handleRelay() {
  int pressedFrame = state.getPressedFrames();
  int pressedSig = state.getPressedSignature();

  double timePassed = state.frameToMs(pressedFrame) / 1000.0;

  if (state.EMERGENCY_COOLDOWN > 0) {
    state.EMERGENCY_COOLDOWN--;
    return;
  }

  if (state.PUMP_IS_OPEN) {
    if (pressedSig == 7) {
      state.stopPumpProcess(); 
      state.EMERGENCY_COOLDOWN = state.msToFrame(3000);
      return;
    }

    if (state.PUMP_AMOUNT_NEED > state.PUMP_AMOUNT_DONE) {
      onPumpingProcess(); // Handle pump counter (if it is open)
    } else {
      state.stopPumpProcess();
      BtSerial.println("Done!");
    }

    return;
  }

  if (timePassed >= 3) {
    int i = 0;
    switch (pressedSig) {
      case 1:  { i = 100; break; }
      case 2:  { i = 300; break; }
      case 4:  { i = 500; break; }
      default: { i = 0; break; }
    }

    if (i == 0) return;
    state.startPumpProcess(i);
    int SENSOR_PULSE_COUNT = 0; // Reset
  }
}


void handleBluetoothCon() {
  state.BT_CONNECTED = !!digitalRead(BLUETOOTH_STATUS);
}


void handleBluetoothInput() {
  if (BtSerial.available()) {
    int input = getClientInput();
    

    switch (input) {
      case -1: {
        // Invalid input, accepts only integer (100 - 2000)
        BtSerial.println("Invalid (100-2000ml only)");
        break;
      }

      case 2001: {
        // Upper limit exceeded, max: 2000
        BtSerial.println("Invalid! Max: 2000ml");
        break;
      }

      case -99: {
        // Lower limit exceeded, min: 100ml!
        BtSerial.println("Invalid! Min: 100ml!");
        break;
      }

      default: {
        if (state.PUMP_IS_OPEN) {
          // Pump is busy, system is not ready
          BtSerial.println("Pump busy!");
        } else {
          // Pump is ready
          state.startPumpProcess(input);
          int SENSOR_PULSE_COUNT = 0; // Reset
          BtSerial.println("Pumping " + String(input) + "ml!");
        }
      }
    }

  }
}

int getClientInput() {
  String inputString = ""; 

  while (BtSerial.available()) {
    char receivedChar = BtSerial.read();  
    inputString += receivedChar;  
    delay(10);
  }


  if (inputString.length() > 0) {
    int inputNumber = inputString.toInt();  
    if (inputNumber == 0 && inputString != "0") {
      return -1; 
    } 

    if (inputNumber > 2000) {
      return 2001;
    }

    if (inputNumber < 100) {
      return -99;
    }

    return inputNumber; 
  }
  
  return -1;  
}

void onPumpingProcess() {
  if (!state.PUMP_IS_OPEN) return;

  double timeInSeconds = state.deltaT / 1000.0;

  noInterrupt();
  double pulseCount = SENSOR_PULSE_COUNT;
  SENSOR_PULSE_COUNT = 0;
  interrupt();

  double flowRate = pulseCount / (double)SENSOR_CALIB_FACTOR;
  // Serial.println(String(flowRate) + " " + String(pulseCount));

  state.PUMP_AMOUNT_DONE += flowRate;
}



void updateCalibrationFactor() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // Read input until newline
    input.trim();  // Remove leading/trailing spaces

    char* endPtr;
    double newFactor = strtod(input.c_str(), &endPtr);  // Convert string to double

    // Check if conversion was successful and ensure entire string was a number
    if (endPtr == input.c_str() || *endPtr != '\0') {
      Serial.println("Error: Invalid input. Please enter a valid number.");
      return;
    }

    // Validate range
    if (newFactor == -1) {
      Serial.print("Calibration factor set to: ");
      Serial.println(SENSOR_CALIB_FACTOR);
      return;
    } else if (newFactor < 1 || newFactor > 500) {
      Serial.println("Error: Value out of range. Enter a number between 1 and 500.");
      return;
    }

    // Update calibration factor
    SENSOR_CALIB_FACTOR = newFactor;
    Serial.print("Calibration factor set to: ");
    Serial.println(SENSOR_CALIB_FACTOR);
  }
}