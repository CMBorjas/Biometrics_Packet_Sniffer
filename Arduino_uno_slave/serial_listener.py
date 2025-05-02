import serial
import os
from datetime import datetime

# === CONFIGURATION ===
SERIAL_PORT = 'COM3'       # Replace with your Arduino Uno's port
BAUD_RATE = 9600
LOG_FILE = "encrypted_logs.txt"

# === Initialize Serial and File ===
print(f"📡 Connecting to {SERIAL_PORT} @ {BAUD_RATE} baud...")
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
except Exception as e:
    print(f"❌ Failed to connect: {e}")
    exit()

# === Open/Create Log File in the Current Directory ===
log_path = os.path.join(os.getcwd(), LOG_FILE)
print(f"📝 Logging to {log_path}")

with open(log_path, "a") as log_file:
    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()
            if line.startswith("SECURE LOG:"):
                hex_data = line.replace("SECURE LOG:", "").strip()
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                log_entry = f"{timestamp} | {hex_data}\n"
                log_file.write(log_entry)
                log_file.flush()
                print(f"🔐 Encrypted log saved: {log_entry.strip()}")
        except KeyboardInterrupt:
            print("\n👋 Stopped by user.")
            break
        except Exception as e:
            print(f"❌ Error: {e}")
