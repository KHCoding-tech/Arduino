/*
  Fade

  This example shows how to fade an LED on pin 9 using the analogWrite()
  function.

  The analogWrite() function uses PWM, so if you want to change the pin you're
  using, be sure to use another PWM capable pin. On most Arduino, the PWM pins
  are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Fade/
*/

const int leds[] = {11, 10, 9, 6}; // LED pins
const int numLeds = 4;
const int fadeDelay = 10; // ms between brightness changes

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < numLeds; i++) {
    // Fade current LED in
    for (int brightness = 0; brightness <= 255; brightness++) {
      analogWrite(leds[i], brightness);
      delay(fadeDelay);
    }
    // Fade current LED out
    for (int brightness = 255; brightness >= 0; brightness--) {
      analogWrite(leds[i], brightness);
      delay(fadeDelay);
    }
  }
}

