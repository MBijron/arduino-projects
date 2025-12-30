#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

const int ledPin = 12;  // Pin you want to control (port 12)


void setup() {
  pinMode(ledPin, OUTPUT);  // Set port 12 as output
  pinMode(13, INPUT);   // or INPUT_PULLUP
  
  Serial.begin(9600);
  
  setupLight();
}

void loop() {
  Serial.println("wawaweewa");
}

void setupLight() {
  digitalWrite(ledPin, HIGH);

  enterSleep(true);
}

void enterSleep(bool awake) {
  // Prepare to enter sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  if(awake) {
    ADCSRA &= ~(1 << ADEN);
  }

  // Disable all peripherals to save power
  power_all_disable();

  // Enter sleep mode
  sleep_mode();

  if(awake) {
    sleep_disable();
  
    power_all_enable();
    ADCSRA |= (1 << ADEN);
  }
}

ISR(WDT_vect) {
}
