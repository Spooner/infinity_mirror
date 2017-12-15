#include "FastLED.h"

#define NUM_LEDS 24

#define MODE_OFF 0
#define MODE_ON 1
#define MODE_ROTATE_SLOW 2
#define MODE_ROTATE_FAST 3
#define NUM_MODES 4

#define SLOW_REPEAT 4
#define FAST_REPEAT 6

const int BUTTON_PIN = 2;
const int LED_DATA_PIN = 6;
const int BRIGHTNESS = 16;

int last_button_state = LOW;
int button_state;
int mode = MODE_ON;
float rotate_offset = 0;
unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 50;

const CRGB LIGHTER = CRGB(150, 200, 255);
const CRGB MEDIUM = CRGB(112, 150, 200);
const CRGB DARKER = CRGB(75, 100, 127);
const CRGB DARKEST = CRGB(40, 50, 65);

CRGB leds[NUM_LEDS];

void setup() { 
  FastLED.addLeds<WS2812B, LED_DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("Starting...");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != last_button_state) {
    last_debounce_time = millis();
  }

  if ((millis() - last_debounce_time) > debounce_delay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button_state) {
      button_state = reading;

      // only toggle the LED if the new button state is HIGH
      if (button_state == HIGH) {
        mode = (mode + 1) % NUM_MODES;
        Serial.print("Switch mode: ");
        Serial.println(mode);
      }
    }
  }
  last_button_state = reading;
  
  FastLED.clear();
  
  switch (mode) {
    case MODE_OFF:
      Serial.println("Mode: Off");
      break;
      
    case MODE_ON:
      Serial.println("Mode: On");
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = LIGHTER;
      }
      break;
      
    case MODE_ROTATE_SLOW:
      Serial.println("Mode: Rotate slow");
      rotate_offset += 0.05;
      for (int i = int(rotate_offset) % SLOW_REPEAT; i < NUM_LEDS; i += SLOW_REPEAT) {
        leds[i] = LIGHTER;
      }
      for (int i = int(rotate_offset - 1) % SLOW_REPEAT; i < NUM_LEDS; i += SLOW_REPEAT) {
        leds[i] = DARKER;
      }
      break;
      
    case MODE_ROTATE_FAST:
      Serial.println("Mode: Rotate fast");
      rotate_offset += 0.2;
      for (int i = int(rotate_offset) % FAST_REPEAT; i < NUM_LEDS; i + =FAST_REPEAT) {
        leds[i] = LIGHTER;
      }
      for (int i = int(rotate_offset - 1) % FAST_REPEAT; i < NUM_LEDS; i += FAST_REPEAT) {
        leds[i] = MEDIUM;
      }
      for (int i = int(rotate_offset - 2) % FAST_REPEAT; i < NUM_LEDS; i += FAST_REPEAT) {
        leds[i] = DARKER;
      }
      for (int i = int(rotate_offset - 3) % FAST_REPEAT; i < NUM_LEDS; i += FAST_REPEAT) {
        leds[i] = DARKEST;
      }
      break;
  }
  
  FastLED.show();
  
  delay(5);
}
