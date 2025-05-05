#include "aes.h"
#include "crypto_utils.h"
#include <string.h>
#include <stdio.h>

static uint8_t key[32] = {
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
  0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

void encryptToHex(const char* input, char* output, size_t outputLen) {
  if (!input || !output || outputLen < 33) return;  // 32 hex chars + null

  struct AES_ctx ctx;
  uint8_t buffer[16] = {0};  // Use 16 bytes for AES-128 or first block of AES-256

  // Copy input string to buffer (truncate if longer)
  strncpy((char*)buffer, input, sizeof(buffer) - 1);

  AES_init_ctx(&ctx, key);
  AES_ECB_encrypt(&ctx, buffer);  // Encrypt in-place

  // Convert encrypted bytes to hex
  for (int i = 0; i < 16; ++i) {
    sprintf(&output[i * 2], "%02x", buffer[i]);
  }

  output[32] = '\0';  // Null-terminate
}
