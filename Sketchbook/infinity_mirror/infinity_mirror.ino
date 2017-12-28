#include "FastLED.h"

#define NUM_LEDS 24

#define MODE_OFF 0
#define MODE_ALL_ON 1
#define MODE_CLAP 2
#define MODE_RAIN 3
#define MODE_RAINBOW 4
#define MODE_ROTATING_RAINBOW 5
#define NUM_MODES 6

#define FAST_REPEAT 8

const int BUTTON_PIN = 2;
const int LED_DATA_PIN = 6;
const int BRIGHTNESS = 20;

int last_button_state = LOW;
int button_state;
int mode = MODE_ALL_ON;
float rotate_offset = 0;
unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 50;

const CRGB LIGHTER = CRGB(150, 200, 255);

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(BUTTON_PIN, INPUT);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  Serial.println("Starting...");
}

void loop() {
  check_button();
  if (mode != MODE_OFF) {
    draw();
  }
  delay(20);
}

void check_button() {
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
        FastLED.clear();
      }
    }
  }
  last_button_state = reading;
}

void draw() {
  int offset;

  switch (mode) {
    case MODE_ALL_ON:
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = LIGHTER;
      }
      break;

    case MODE_CLAP:
      FastLED.clear();
      rotate_offset += 0.1;
      offset = int(rotate_offset) % 12;
      if (offset >= 6) {
        offset = 11 - offset;
      }
      leds[offset] = leds[11 - offset] = leds[12 + offset] = leds[23 - offset] = LIGHTER;
      break;

    case MODE_RAIN:
      rotate_offset += 1;
      // fade
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].subtractFromRGB(1);
      }
      // create new every so often
      if (rotate_offset > 10) {
        rotate_offset -= 10;
        leds[random(NUM_LEDS)].setHSV(random(256), 255, 255);
      }
      break;

    case MODE_RAINBOW:
      rotate_offset += 0.5;
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].setHSV(int(rotate_offset) % 256, 200, 255);
      }
      break;

    case MODE_ROTATING_RAINBOW:
      FastLED.clear();
      rotate_offset += 0.2;
      for (int i = int(rotate_offset) % FAST_REPEAT; i < NUM_LEDS; i += FAST_REPEAT) {
        leds[i].setHSV(int(rotate_offset) % 256, 200, 255);
      }
      for (int i = int(rotate_offset - 1) % FAST_REPEAT; i < NUM_LEDS; i += FAST_REPEAT) {
        leds[i].setHSV(int(rotate_offset) % 256, 200, 200);
      }
      for (int i = int(rotate_offset - 2) % FAST_REPEAT; i < NUM_LEDS; i += FAST_REPEAT) {
        leds[i].setHSV(int(rotate_offset) % 256, 200, 150);
      }
      for (int i = int(rotate_offset - 3) % FAST_REPEAT; i < NUM_LEDS; i += FAST_REPEAT) {
        leds[i].setHSV(int(rotate_offset) % 256, 200, 100);
      }
      for (int i = int(rotate_offset - 4) % FAST_REPEAT; i < NUM_LEDS; i += FAST_REPEAT) {
        leds[i].setHSV(int(rotate_offset) % 256, 200, 50);
      }
      break;
  }
  FastLED.show();
}
