#include <Arduino.h>

// Declare setup functions for other modules
void setupBiometric();
void setupSniffer();
void setupUI();
void setupCrypto();
void setupInterrupts();

void setup() {
    Serial.begin(115200);
    setupBiometric();
    setupSniffer();
    setupUI();
    setupCrypto();
    setupInterrupts();
}

void loop() {
  // Main loop logic here (auth → sniff → display)
}
