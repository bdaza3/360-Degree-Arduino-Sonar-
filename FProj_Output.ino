#include <FastLED.h>


#define DATA_PIN 6
#define NUM_LEDS 8
bool detected = false;
unsigned long currentMillis = 0;
int toneFrequency = 440;


unsigned long previousMillis = 0;


unsigned long toneStartMillis = 0;
bool isTonePlaying = false;
int toneDuration = 500; // half a second


bool isDelayRunning = false;
CRGB leds[NUM_LEDS];


// constants won't change. They're used here to set pin numbers:


// variables will change:


#define SERIAL_PORT Serial1
int buzzerPin = 7;
int prevValue = -1; // initialize with an invalid previous value to detect the first change


void setup() {
  // initialize the LED pin as an output:
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  Serial.begin(9600);
  SERIAL_PORT.begin(9600);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}


void loop() {
  currentMillis = millis();


  if (SERIAL_PORT.available() > 0) {  
    int incomingByte = SERIAL_PORT.read();
    Serial.println(incomingByte);
    fill_solid(leds, NUM_LEDS, CRGB::Black);


    if (incomingByte != prevValue) {
      // Play the tone for half a second when there's a change
      tone(buzzerPin, toneFrequency);
      isTonePlaying = true;
      toneStartMillis = currentMillis;


      prevValue = incomingByte; // Update the previous value


      if (incomingByte <= 45) {
        leds[0] = CRGB::Red;
      } else if (incomingByte <= 90) {
        leds[1] = CRGB::Red;
      } else if (incomingByte <= 135) {
        leds[2] = CRGB::Red;
      } else if (incomingByte <= 180) {
        leds[3] = CRGB::Red;
      } else if (incomingByte <= 225) {
        leds[4] = CRGB::Red;
      } else if (incomingByte <= 270) {
        leds[5] = CRGB::Red;
      } else if (incomingByte <= 315) {
        leds[6] = CRGB::Red;
      } else if (incomingByte <= 360) {
        leds[7] = CRGB::Red;
      }


      FastLED.show();
    }
  }


  // Check if the tone should stop
  if (isTonePlaying && (currentMillis - toneStartMillis >= toneDuration)) {
    noTone(buzzerPin);        
    isTonePlaying = false;    
  }
}
