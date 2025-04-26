#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// Define SoftwareSerial pins
#define R307_RX 2  // Arduino RX (connects to R307 TX)
#define R307_TX 3  // Arduino TX (connects to R307 RX)

SoftwareSerial fingerSerial(R307_RX, R307_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

// Output pin to ESP32
const int esp32SignalPin = 7;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(100);

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("✅ Found fingerprint sensor!");
  } else {
    Serial.println("❌ Fingerprint sensor not found :(");
    while (1) { delay(1); }
  }

  pinMode(esp32SignalPin, OUTPUT);
  digitalWrite(esp32SignalPin, LOW);

  Serial.println(F("-------------------------------------"));
  Serial.println(F("Send 'e' to enroll a new fingerprint"));
  Serial.println(F("Otherwise place your finger to scan"));
  Serial.println(F("-------------------------------------"));
}

void loop() {
  if (Serial.available()) {
    char option = Serial.read();
    if (option == 'e') {
      enrollFingerprint();
    }
  }

  // Normal matching
  int id = getFingerprintID();
  if (id > 0) {
    Serial.print("🎉 Match found! ID #"); Serial.println(id);
    digitalWrite(esp32SignalPin, HIGH);
    delay(1000);
    digitalWrite(esp32SignalPin, LOW);
  } else {
    Serial.println("❌ No valid fingerprint found");
  }
  delay(1000);
}

// ==============================
// Basic fingerprint check
// ==============================
int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // Found a match!
  return finger.fingerID;
}

// ==============================
// Enrollment function
// ==============================
void enrollFingerprint() {
  int id;
  Serial.println(F("Enter ID # to enroll (1-127): "));

  // Wait until a full line of input is available
  String input = "";
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (input.length() > 0) break; // Only break if something was typed
      } else {
        input += c;
      }
    }
  }
  
  id = input.toInt();

  if (id < 1 || id > 127) {
    Serial.println(F("❌ Invalid ID number. Must be between 1 and 127."));
    return;
  }

  Serial.println(F("Place your finger on the sensor..."));
  while (finger.getImage() != FINGERPRINT_OK);

  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println(F("❌ Failed to capture first image"));
    return;
  }
  Serial.println(F("Image 1 captured. Remove finger."));

  delay(2000); // Small delay

  Serial.println(F("Place the same finger again..."));
  while (finger.getImage() != FINGERPRINT_OK);

  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println(F("❌ Failed to capture second image"));
    return;
  }

  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println(F("❌ Failed to create model"));
    return;
  }

  if (finger.storeModel(id) != FINGERPRINT_OK) {
    Serial.println(F("❌ Failed to store fingerprint"));
    return;
  }

  Serial.println(F("🎯 Fingerprint enrolled successfully!"));
}
