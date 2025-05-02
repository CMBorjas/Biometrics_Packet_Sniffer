#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include "crypto_utils.h"  // AES encryption module

#define R307_RX 2
#define R307_TX 3
#define ESP32_SIGNAL_PIN 7
#define ACCESS_ID 1
#define STOP_ID 99
#define MAX_FAILED_ATTEMPTS 3
#define LOCKOUT_DURATION 30000

SoftwareSerial fingerSerial(R307_RX, R307_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

bool scanningEnabled = true;
int failedAttempts = 0;
unsigned long lockoutStart = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(100);

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("UART: ✅ Found fingerprint sensor!");
  } else {
    Serial.println("UART: ❌ Fingerprint sensor not found");
    while (1) { delay(1); }
  }

  pinMode(ESP32_SIGNAL_PIN, OUTPUT);
  digitalWrite(ESP32_SIGNAL_PIN, LOW);

  Serial.println("UART: =====================================");
  Serial.println("UART: Send 'e' to enroll ACCESS fingerprint (ID 1)");
  Serial.println("UART: Send 's' to enroll STOP fingerprint (ID 99)");
  Serial.println("UART: Send 'd' to delete ACCESS fingerprint");
  Serial.println("UART: Send 'x' to clear ALL fingerprints");
  Serial.println("UART: Place a finger to scan");
  Serial.println("UART: =====================================");
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
    Serial.println("LOCKED OUT. Wait...");
    delay(1000);
    return;
  }

  scanFingerprint();
  delay(300);
}

void scanFingerprint() {
  if (finger.getImage() != FINGERPRINT_OK) return;
  if (finger.image2Tz() != FINGERPRINT_OK) {
    logUnauthorized("Bad image conversion");
    return;
  }
  if (finger.fingerFastSearch() != FINGERPRINT_OK) {
    logUnauthorized("No fingerprint match");
    return;
  }

  int id = finger.fingerID;

  if (id == ACCESS_ID && scanningEnabled) {
    Serial.println("MATCH");
    digitalWrite(ESP32_SIGNAL_PIN, HIGH);
    delay(1000);
    digitalWrite(ESP32_SIGNAL_PIN, LOW);
    scanningEnabled = false;
    failedAttempts = 0;
  } 
  else if (id == STOP_ID && !scanningEnabled) {
    Serial.println("STOP");
    digitalWrite(ESP32_SIGNAL_PIN, HIGH);
    delay(1000);
    digitalWrite(ESP32_SIGNAL_PIN, LOW);
    scanningEnabled = true;
    failedAttempts = 0;
  } 
  else {
    logUnauthorized("Fingerprint not authorized");
  }
}

void logUnauthorized(const char* reason) {
  String event = String(reason) + " @ " + millis();
  String hexEncrypted = encryptToHex(event);
  Serial.print("SECURE LOG: ");
  Serial.println(hexEncrypted);

  failedAttempts++;
  if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
    lockoutStart = millis();
    Serial.println("❌ LOCKOUT ACTIVATED.");
  }
}

bool isLockedOut() {
  if (lockoutStart == 0) return false;

  if (millis() - lockoutStart >= LOCKOUT_DURATION) {
    lockoutStart = 0;
    failedAttempts = 0;
    Serial.println("🔓 LOCKOUT CLEARED.");
    return false;
  }

  return true;
}

void enrollFingerprint(int id) {
  Serial.print("UART: Enrolling fingerprint ID #");
  Serial.println(id);
  Serial.println("UART: Place finger...");

  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("UART: ❌ Failed first image");
    return;
  }

  Serial.println("UART: ✅ Image 1 captured. Remove finger.");
  delay(2000);
  Serial.println("UART: Place same finger again...");

  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("UART: ❌ Failed second image");
    return;
  }

  if (finger.createModel() != FINGERPRINT_OK || finger.storeModel(id) != FINGERPRINT_OK) {
    Serial.println("UART: ❌ Failed to store fingerprint");
    return;
  }

  Serial.println("UART: ✅ Fingerprint enrolled!");
}

void deleteFingerprintByID(int id) {
  int status = finger.deleteModel(id);
  if (status == FINGERPRINT_OK) {
    Serial.print("UART: 🧹 Deleted fingerprint ID #");
    Serial.println(id);
  } else {
    Serial.println("UART: ❌ Failed to delete fingerprint.");
  }
}

void clearAllFingerprints() {
  int status = finger.emptyDatabase();
  if (status == FINGERPRINT_OK) {
    Serial.println("UART: 🧹 All fingerprints deleted.");
  } else {
    Serial.println("UART: ❌ Failed to clear fingerprint database.");
  }
}
