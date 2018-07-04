/*
      Padawan Shield Demo
          (v1.0 - 20/05/15)
          (for Arduino 1.0.1 and later)

  Copyright 2015 RoboCore (FranÃ§ois) ( http://www.RoboCore.net )

  ------------------------------------------------------------------------------
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  ------------------------------------------------------------------------------

*/


#define DEBUG_SENSORS
//#define SOFTWARE_LED_RESET

// Conversion Factor for the LM35 sensor
//  (sensor has 10mV/oC resolution)
#if defined(__AVR_ATmega328P__)          // UNO
#define CONVERSION_FACTOR_LM35   0.1075269
#define CONVERSION_FACTOR_BOARD  1
#define ANALOG_REFERENCE         INTERNAL
#elif defined (__AVR_ATmega2560__)       // Mega 2560
#define CONVERSION_FACTOR_LM35   0.1075269
#define CONVERSION_FACTOR_BOARD  1
#define ANALOG_REFERENCE         INTERNAL1V1
#elif defined (__AVR_ATmega32U4__)       // Leonardo
#define CONVERSION_FACTOR_LM35   0.2502444
#define CONVERSION_FACTOR_BOARD  2.3272727
#define ANALOG_REFERENCE         INTERNAL // 2.56V
#else                                    // other (not supported)
#define CONVERSION_FACTOR_LM35   1
#define CONVERSION_FACTOR_BOARD  1
#define ANALOG_REFERENCE         DEFAULT
#endif

#define DEBOUNCE 25  // [ms]
#define FILTER_FACTOR_LM35 0.8 // 0 <= k <= 1

// ------------------------------------------------
// PINS *****

const int pinLM35 = A0;
const int pinLDR = A1;
const int pinPot = A2;
const int pinBtn = 2;

#define SELECT_LED      HIGH
#define SELECT_DISPLAY   LOW
const int pinSelect = A3;

const int pinRelay = 9;

// LED RGB
const int pinRGB_R = 5;
const int pinRGB_G = 3;
const int pinRGB_B = 6;

// LED array
const int pinLED1 = A4;
const int pinLED2 = A5;
const int pinLED3 = 7;
const int pinLED4 = 8;

// 7 segments display
#define DISPLAY_A 0
#define DISPLAY_B 1
#define DISPLAY_C 2
#define DISPLAY_D 3
#define DISPLAY_E 4
#define DISPLAY_F 5
#define DISPLAY_G 6
const int *pin7seg[7] = { &pinLED1 , &pinLED2 , &pinLED3 , &pinLED4 , &pinRGB_R , &pinRGB_G , &pinRGB_B };

// ------------------------------------------------
// 7 segments display *****

#define ALPHANUMERIC_NONE 0x00

#define ALPHANUMERIC_0 ((1 << DISPLAY_A) | (1 << DISPLAY_B) | (1 << DISPLAY_C) | (1 << DISPLAY_D) | (1 << DISPLAY_E) | (1 << DISPLAY_F))
#define ALPHANUMERIC_1 ((1 << DISPLAY_B) | (1 << DISPLAY_C))
#define ALPHANUMERIC_2 ((1 << DISPLAY_A) | (1 << DISPLAY_B) | (1 << DISPLAY_D) | (1 << DISPLAY_E) | (1 << DISPLAY_G))
#define ALPHANUMERIC_3 ((1 << DISPLAY_A) | (1 << DISPLAY_B) | (1 << DISPLAY_C) | (1 << DISPLAY_D) | (1 << DISPLAY_G))
#define ALPHANUMERIC_4 ((1 << DISPLAY_B) | (1 << DISPLAY_C) | (1 << DISPLAY_F) | (1 << DISPLAY_G))
#define ALPHANUMERIC_5 ((1 << DISPLAY_A) | (1 << DISPLAY_C) | (1 << DISPLAY_D) | (1 << DISPLAY_F) | (1 << DISPLAY_G))
#define ALPHANUMERIC_6 ((1 << DISPLAY_A) | (1 << DISPLAY_C) | (1 << DISPLAY_D) | (1 << DISPLAY_E) | (1 << DISPLAY_F) | (1 << DISPLAY_G))
#define ALPHANUMERIC_7 ((1 << DISPLAY_A) | (1 << DISPLAY_B) | (1 << DISPLAY_C))
#define ALPHANUMERIC_8 ((1 << DISPLAY_A) | (1 << DISPLAY_B) | (1 << DISPLAY_C) | (1 << DISPLAY_D) | (1 << DISPLAY_E) | (1 << DISPLAY_F) | (1 << DISPLAY_G))
#define ALPHANUMERIC_9 ((1 << DISPLAY_A) | (1 << DISPLAY_B) | (1 << DISPLAY_C) | (1 << DISPLAY_D) | (1 << DISPLAY_F) | (1 << DISPLAY_G))

// ------------------------------------------------
// Variables *****

float value_LM35;
int value_LDR;
int value_Pot;
float temperature;

#define STATE_LED_1  0
#define STATE_LED_2  1
#define STATE_LED_3  2
#define STATE_LED_4  3
#define STATE_RGB_R  4
#define STATE_RGB_G  5
#define STATE_RGB_B  6
byte state_Select = SELECT_LED;
byte state_Display = ALPHANUMERIC_NONE;
byte state_LEDs = 0x00;
unsigned long stop_time_select;

byte state_Relay = LOW;

unsigned long stop_time_refresh;

// ------------------------------------------------
// Function prototypes *****

int newAnalogRead(int pin);
boolean ReadButton(void);
void SetDisplay(byte character);
void SetLEDs(byte state);

void _ResetPins(void);
void _SetPin(int *pin, byte state);

// ------------------------------------------------
// ------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("Configuring");

  pinMode(pinLM35, INPUT);
  pinMode(pinLDR, INPUT);
  pinMode(pinPot, INPUT);
  pinMode(pinBtn, INPUT);
  pinMode(pinSelect, OUTPUT);
  pinMode(pinRelay, OUTPUT);
  for (int i = DISPLAY_A ; i <= DISPLAY_G ; i++) {
    pinMode(*pin7seg[i], OUTPUT);
  }

  digitalWrite(pinSelect, state_Select);
  digitalWrite(pinRelay, state_Relay);
  for (int i = DISPLAY_A ; i <= DISPLAY_G ; i++) {
    digitalWrite(*pin7seg[i], LOW);
  }

  analogReference(ANALOG_REFERENCE); // use 1.1V

  state_Display = ALPHANUMERIC_8; // TESTE

  Serial.println("\tdone");
}

// ------------------------------------------------
// ------------------------------------------------

void loop() {
  state_LEDs = 0x00; // reset

  // read button
  if (ReadButton()) {
    if (state_Relay == LOW)
      state_Relay = HIGH;
    else
      state_Relay = LOW;

    digitalWrite(pinRelay, state_Relay);
#ifdef DEBUG_SENSORS
    Serial.print("Relay: ");
    (state_Relay == HIGH ? Serial.println("HIGH") : Serial.println("LOW"));
#endif

    while (ReadButton()) {
      /* wait for release */
    }
  }

  // read potentiometer
  value_Pot = newAnalogRead(pinPot);
#ifdef DEBUG_SENSORS
  Serial.print("Pot: ");
  Serial.print(value_Pot);
#endif
  if (value_Pot > 700) {
    state_LEDs |= (1 << STATE_RGB_R);
  } else if (value_Pot > 400) {
    state_LEDs |= (1 << STATE_RGB_B);
  } else if (value_Pot > 50) {
    state_LEDs |= (1 << STATE_RGB_G);
  }

  // read LDR
  value_LDR = newAnalogRead(pinLDR);
#ifdef DEBUG_SENSORS
  Serial.print("\tLDR: ");
  Serial.print(value_LDR);
#endif
  if (value_LDR >= 900) {
    state_LEDs |= (1 << STATE_LED_1);
  }
  if (value_LDR >= 650) {
    state_LEDs |= (1 << STATE_LED_2);
  }
  if (value_LDR >= 450) {
    state_LEDs |= (1 << STATE_LED_3);
  }
  if (value_LDR >= 200) {
    state_LEDs |= (1 << STATE_LED_4);
  }

  // read LM35
  value_LM35 = FILTER_FACTOR_LM35 * value_LM35 + (1.0 - FILTER_FACTOR_LM35) * analogRead(pinLM35);
  temperature = value_LM35 * CONVERSION_FACTOR_LM35;
#ifdef DEBUG_SENSORS
  Serial.print("\tLM35: ");
  Serial.print(value_LM35, 2);
  Serial.print("\t>> Temperature: ");
  Serial.println(temperature, 2);
#endif
  int temp = (int)temperature % 10;
  if (temp <= 0) {
    state_Display = ALPHANUMERIC_0;
  } else if (temp == 1) {
    state_Display = ALPHANUMERIC_1;
  } else if (temp == 2) {
    state_Display = ALPHANUMERIC_2;
  } else if (temp == 3) {
    state_Display = ALPHANUMERIC_3;
  } else if (temp == 4) {
    state_Display = ALPHANUMERIC_4;
  } else if (temp == 5) {
    state_Display = ALPHANUMERIC_5;
  } else if (temp == 6) {
    state_Display = ALPHANUMERIC_6;
  } else if (temp == 7) {
    state_Display = ALPHANUMERIC_7;
  } else if (temp == 8) {
    state_Display = ALPHANUMERIC_8;
  } else {
    state_Display = ALPHANUMERIC_9;
  }

  // switch select
  if (micros() > stop_time_select) {
    if (state_Select == SELECT_LED) {
      SetDisplay(state_Display);
    } else {
      SetLEDs(state_LEDs);
    }

    stop_time_select = micros() + 5000; // update (5us)
  }
}

// ------------------------------------------------
// ------------------------------------------------

// New AnalogRead function for board compatibility
int newAnalogRead(int pin) {
  float value = analogRead(pin) * CONVERSION_FACTOR_BOARD;
  if (value > 1023)
    value = 1023;
  return (int)value;
}

// ------------------------------------------------

// Read the button
//  (returns TRUE if pressed)
boolean ReadButton(void) {
  boolean res = false;
  if (digitalRead(pinBtn) == HIGH) {
    delay(DEBOUNCE);
    if (digitalRead(pinBtn) == HIGH) {
      res = true;
    }
  }

  return res;
}

// ------------------------------------------------

void SetDisplay(byte character) {
#ifdef SOFTWARE_LED_RESET
  _ResetPins();
#endif

  digitalWrite(pinSelect, SELECT_DISPLAY);
  state_Select = SELECT_DISPLAY; // update

  byte mask;
  for (int i = DISPLAY_A ; i <= DISPLAY_G ; i++) {
    mask = (1 << i);
    _SetPin(pin7seg[i], (character & mask));
  }
}


// ------------------------------------------------

// Set the desired LED state
void SetLEDs(byte state) {
#ifdef SOFTWARE_LED_RESET
  _ResetPins();
#endif

  digitalWrite(pinSelect, SELECT_LED);
  state_Select = SELECT_LED; // update

  _SetPin(&pinLED1, (state & 0x01));
  _SetPin(&pinLED2, (state & 0x02));
  _SetPin(&pinLED3, (state & 0x04));
  _SetPin(&pinLED4, (state & 0x08));

  _SetPin(&pinRGB_R, (state & 0x10));
  _SetPin(&pinRGB_G, (state & 0x20));
  _SetPin(&pinRGB_B, (state & 0x40));
}

// ------------------------------------------------
// ------------------------------------------------

// "Private" function
// Reset pins
void _ResetPins(void) {
  for (int i = DISPLAY_A ; i <= DISPLAY_G ; i++) {
    digitalWrite(*pin7seg[i], LOW);
  }
  delayMicroseconds(1500); // give some time for capacitors to discharge (1us)
}

// ------------------------------------------------

// "Private" function
// Set a pin state
void _SetPin(const int *pin, byte state) {
  // check for null pointer
  if (pin == NULL)
    return;

  if ((state == HIGH) || (state != 0)) {
    digitalWrite(*pin, HIGH);
  } else {
    digitalWrite(*pin, LOW);
  }
}

// ------------------------------------------------
// ------------------------------------------------







