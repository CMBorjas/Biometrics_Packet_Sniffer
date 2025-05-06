#include "aes_wrapper.h"

static const uint8_t key[16] = {
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81
}; // AES-128 key (adjust for AES-256 if needed)

void setupCrypto() {
    Serial.println("[Crypto] Initializing encryption...");
    // AES setup or key load
}

String encryptData(String plainText) {
    return plainText;  // Stub
}

void encryptToHex(const char* plainText, char* outHex, size_t hexSize) {
  uint8_t buffer[16] = {0};
  strncpy((char*)buffer, plainText, 15);

  struct AES_ctx ctx;
  AES_init_ctx(&ctx, key);              // key should be 128 bits (16 bytes)
  AES_ECB_encrypt(&ctx, buffer);

  for (int i = 0; i < 16 && i * 2 < hexSize - 2; ++i) {
    sprintf(&outHex[i * 2], "%02x", buffer[i]);
  }
  outHex[32] = '\0'; // Null-terminate
}

