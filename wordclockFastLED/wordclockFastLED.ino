/**
   wordclockFastLED.ino

   @autor    Bruno Merz
   @version  7.0
   @updated  04.09.2022
**/

// Implemented in WordclockFastLED.cpp

extern void wordclockSetup();
extern void wordclockLoop();

void setup() {
  wordclockSetup();
}

void loop() {
  wordclockLoop();
}
