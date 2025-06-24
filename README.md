CODE BELONGS TO @The3dit0r - Tran Dang Minh Quan

I implemented the prototype, the very first code of this system but still it cannot transport water. Thanks to Quan, he did it successfully without any help, individually.
However, in final deployment, the energy supplier - 5V Pin from arduino board was insufficient for other external device; therefore, the system ran out in the middle. However, the system still can be manipulated manually through buttons, but not Arduino Serial Bluetooth mobile app

Topic 8 - IOT102

Smart Water Pouring System
-Wireless communications: Wifi/Bluetooth/LoRa: Arduino Uno, bluetooth module, LCD with I2C, YF-S401 water flow sensor, relay, pump, push button 

-Read data from sensors: Read the status of three push buttons to know the amount of to-be-poured water, i.e., small, medium, and high.

-Process the collected data: Calculate the amount of poured water based on the water flow and pouring time"- Display the water pouring progress status on an LCD

-Display/Visualize data in display device or web/mobile app: Display the total amount of consumed water over time on a mobile app

-Control the actuators - manually or automatically according to the processed data: Control the pump to automatically provide accurate amount of to-be-poured water specified on the mobile app

Code in C++
