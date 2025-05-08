import serial
import time
import os
from collections import defaultdict, deque

# === Config ===
COM_PORT = 'COM5'
BAUD_RATE = 115200
DATA_DIR = 'data'
MAIN_LOG = 'session_logs.txt'
ALERT_LOG = 'suspicious_macs.txt'
ALERT_THRESHOLD = 5
ALERT_WINDOW_SECONDS = 10

# === Setup ===
os.makedirs(DATA_DIR, exist_ok=True)
log_path = os.path.join(DATA_DIR, MAIN_LOG)
alert_path = os.path.join(DATA_DIR, ALERT_LOG)

ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
main_log = open(log_path, 'a', encoding='utf-8')
alert_log = open(alert_path, 'a', encoding='utf-8')

print("📡 Serial logger with alert system running. Ctrl+C to stop.\n")

mac_activity = defaultdict(deque)

try:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if not line or 'SRC_MAC:' not in line:
            continue

        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        mac = line.split('SRC_MAC:')[1].split()[0].strip()
        now = time.time()

        # Track and clean old timestamps
        mac_activity[mac].append(now)
        while mac_activity[mac] and now - mac_activity[mac][0] > ALERT_WINDOW_SECONDS:
            mac_activity[mac].popleft()

        # Build log entry
        log_entry = f"[{timestamp}] {line}"

        # Check for suspicious activity
        if len(mac_activity[mac]) >= ALERT_THRESHOLD:
            alert = f"[{timestamp}] 🚨 Suspicious MAC detected: {mac} — seen {len(mac_activity[mac])} times in under {ALERT_WINDOW_SECONDS}s"
            log_entry += " [SUSPICIOUS]"
            print(alert)
            alert_log.write(alert + "\n")
            alert_log.flush()

        print(log_entry)
        main_log.write(log_entry + "\n")
        main_log.flush()

except KeyboardInterrupt:
    print("\n🛑 Logging stopped.")
finally:
    main_log.close()
    alert_log.close()
    ser.close()
