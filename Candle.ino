#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

const int ledPin = 12;  // Pin you want to control (port 12)
const int spotlightPin = 11;
const int resetPin = 10;
int storedValue = 0;    // Variable to store the value
const unsigned long onDurationHours = 8;
unsigned long onDuration = 43200000; // 12 hours
unsigned long sleepLoopCount = 0;


void setup() {
  pinMode(ledPin, OUTPUT);  // Set port 12 as output
  pinMode(spotlightPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, HIGH);
  
  Serial.begin(9600);
  storedValue = EEPROM.read(0);

  if(onDurationHours > 0) {
    onDuration = onDurationHours * 3600000;
    if (Serial) Serial.println("On for Hours / Milliseconds");
    if (Serial) Serial.println(onDurationHours);
    if (Serial) Serial.println(onDuration);
  }

  if(storedValue == 1) {
    if (Serial) Serial.println("Going into ON modus");
    delay(500);
    setupWatchdog();
  } else {
    if (Serial) Serial.println("Going into OFF modus");
    delay(500);
  }
  
  setupLight();
}

void loop() {
  sleepLoopCount++;
  long calculatedMillis = sleepLoopCount * 8000;
  if (Serial) Serial.println("checking, calculated millis: ");
  if (Serial) Serial.println(calculatedMillis);
  delay(500);
  if(storedValue == 1 && calculatedMillis >= onDuration) {
    asm volatile ("jmp 0");
  } else if(storedValue == 1) {
    enterSleep(true);
  }
}

void setupLight() {
  if(storedValue == 0) {
    toggleOff();
  } else {
    toggleOn();
  }
}

void toggleOn() {
  digitalWrite(ledPin, HIGH);
  digitalWrite(spotlightPin, HIGH);
  if (EEPROM.read(0) != 0) {  // Minimize EEPROM writes
    EEPROM.write(0, 0);
  }
  enterSleep(true);
}

void toggleOff() {
  digitalWrite(ledPin, LOW);
  digitalWrite(spotlightPin, LOW);
  if (EEPROM.read(0) != 1) {  // Minimize EEPROM writes
    EEPROM.write(0, 1);
  }
  enterSleep(false);
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

void setupWatchdog() {
  MCUSR &= ~(1 << WDRF);  // Clear WDRF flag
  WDTCSR |= (1 << WDCE) | (1 << WDE);  // Enable WDT change
  WDTCSR = (1 << WDP3) | (1 << WDP0);  // Set WDT timeout to 8 seconds
  WDTCSR |= (1 << WDIE);  // Enable WDT interrupt
}

ISR(WDT_vect) {
}
