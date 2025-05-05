#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include "crypto_utils.h"  // Updated AES encryption module

#define R307_RX 2
#define R307_TX 3
#define ESP32_SIGNAL_PIN 7
#define ACCESS_ID 1
#define STOP_ID 99
#define MAX_FAILED_ATTEMPTS 5
#define LOCKOUT_DURATION 30000

SoftwareSerial fingerSerial(R307_RX, R307_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

int failedAttempts = 0;
unsigned long lockoutStart = 0;
unsigned long lastAccessTime = 0;

// Optional memory check
int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(100);

  finger.begin(57600);
  delay(500);

  if (finger.verifyPassword()) {
    Serial.println("Sensor OK");
  } else {
    Serial.println("Sensor FAIL");
    while (1) { delay(1); }
  }

  pinMode(ESP32_SIGNAL_PIN, OUTPUT);
  digitalWrite(ESP32_SIGNAL_PIN, LOW);

  Serial.println("Commands: e = enroll ID1, s = enroll ID99, d = delete ID1, x = clear DB");
  Serial.println("Place finger to scan.");

  lastAccessTime = millis();

  Serial.print("Free RAM: ");
  Serial.println(freeMemory());
}

void loop() {
  if (Serial.available()) {
    char option = Serial.read();
    if (option == 'e') {
      enrollFingerprint(ACCESS_ID);
      return;
    } else if (option == 's') {
      enrollFingerprint(STOP_ID);
      return;
    } else if (option == 'd') {
      deleteFingerprintByID(ACCESS_ID);
      return;
    } else if (option == 'x') {
      clearAllFingerprints();
      return;
    }
  }

  if (isLockedOut()) {
    Serial.println("LOCKED OUT");
    delay(1000);
    return;
  }

  scanFingerprint();
  delay(300);
}

void scanFingerprint() {
  Serial.println("Scanning...");

  const unsigned long scanTimeout = 15000; // 15 seconds max scan
  unsigned long scanStart = millis();

  while (millis() - scanStart < scanTimeout) {
    uint8_t p = finger.getImage();

    if (p == FINGERPRINT_NOFINGER) {
      delay(100);
      continue;
    }

    if (p != FINGERPRINT_OK) {
      Serial.print("getImage failed: ");
      Serial.println(p);
      return;
    }

    Serial.println("Finger detected");
    goto continue_scan;
  }

  Serial.println("❌ Fingerprint timeout (loop)");
  logUnauthorized("Timeout");
  return;

continue_scan:
  delay(1000);

  uint8_t p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    logUnauthorized("Bad image");
    return;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    logUnauthorized("No match");
    return;
  }

  int id = finger.fingerID;
  char logMsg[50];
  snprintf(logMsg, sizeof(logMsg), "ID #%d @ %lu", id, millis());

  char encrypted[33];
  encryptToHex(logMsg, encrypted, sizeof(encrypted));

  Serial.print("SECURE LOG: ");
  Serial.println(encrypted);

  digitalWrite(ESP32_SIGNAL_PIN, HIGH);
  delay(1000);
  digitalWrite(ESP32_SIGNAL_PIN, LOW);

  failedAttempts = 0;
  lastAccessTime = millis();

  if (id == ACCESS_ID) Serial.println("MATCH");
  else if (id == STOP_ID) Serial.println("STOP");
  else logUnauthorized("Unauthorized ID");
}

void logUnauthorized(const char* reason) {
  char logMsg[64];
  snprintf(logMsg, sizeof(logMsg), "%s @ %lu", reason, millis());

  char encrypted[33];
  encryptToHex(logMsg, encrypted, sizeof(encrypted));

  Serial.print("SECURE LOG: ");
  Serial.println(encrypted);

  failedAttempts++;
  if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
    lockoutStart = millis();
    Serial.println("LOCKOUT ACTIVATED");
  }
}

bool isLockedOut() {
  if (lockoutStart == 0) return false;

  if (millis() - lockoutStart >= LOCKOUT_DURATION) {
    lockoutStart = 0;
    failedAttempts = 0;
    Serial.println("LOCKOUT CLEARED");
    return false;
  }

  return true;
}

void enrollFingerprint(int id) {
  Serial.print("Enroll ID #");
  Serial.println(id);

  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("Enroll fail step 1");
    return;
  }

  Serial.println("Image 1 OK. Remove finger.");
  delay(2000);
  Serial.println("Place same finger again...");

  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("Enroll fail step 2");
    return;
  }

  if (finger.createModel() != FINGERPRINT_OK || finger.storeModel(id) != FINGERPRINT_OK) {
    Serial.println("Store fail");
    return;
  }

  Serial.println("Enroll OK");
}

void deleteFingerprintByID(int id) {
  if (finger.deleteModel(id) == FINGERPRINT_OK) {
    Serial.print("Deleted ID #");
    Serial.println(id);
  } else {
    Serial.println("Delete fail");
  }
}

void clearAllFingerprints() {
  if (finger.emptyDatabase() == FINGERPRINT_OK) {
    Serial.println("DB cleared");
  } else {
    Serial.println("Clear DB fail");
  }
}
