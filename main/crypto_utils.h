#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

extern "C" {
  #include "aes.h"
}

void encryptToHex(const char* input, char* output, size_t outputLen);
void setupCrypto();
String encryptData();

#endif
