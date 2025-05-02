from Crypto.Cipher import AES
from binascii import unhexlify

# 256-bit (32-byte) AES key — must match exactly with Arduino
KEY = bytes([
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
])

def decrypt_hex_log(hex_string):
    encrypted = unhexlify(hex_string.strip())
    cipher = AES.new(KEY, AES.MODE_ECB)
    decrypted = cipher.decrypt(encrypted)
    return decrypted.rstrip(b'\x00').decode(errors="ignore")

if __name__ == "__main__":
    print("🔐 Secure Log Decryption Tool")
    while True:
        try:
            hex_log = input("Paste encrypted hex log: ")
            print("📜 Decrypted:", decrypt_hex_log(hex_log))
        except Exception as e:
            print("❌ Error:", e)
