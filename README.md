# Packet Sniffer with Biometrics for Secure Network Monitoring

## Overview
This project integrates real-time network traffic analysis with biometric authentication to enhance network security. It ensures secure access to packet sniffing functionalities and protects sensitive network data.

## Team
**Team Name:** Byte Builders  
**Members:**  
- Thong Dang  
- Oluwatosin Omiteru  
- Christian Mandujano Brojas  

## Features

### Packet Sniffing
- Real-time network packet capture and analysis.
- Extraction of key details: source IP, destination IP, protocol type, and payload data.
- Traffic filtering based on predefined rules (e.g., HTTP, FTP, DNS).
- Secure logging and storage of network traffic for future analysis.

### Biometric Authentication
- Fingerprint or facial recognition for user authentication.
- Secure storage of biometric data.
- Restricted access to functionalities without successful authentication.

### Secure Data Storage
- Encryption of captured packet data before storage.
- Maintenance of secure logs for historical network activity.
- Retrieval of logged data for analysis.

### User Interface
- Web-based dashboard or LCD display for real-time monitoring.
- Accessible only after biometric authentication.
- Displays network statistics, security alerts, and logged traffic.

### Real-Time Alerts
- Detection of suspicious activities (e.g., abnormal data transfer rates).
- Alerts and logs for unauthorized access attempts.

## System Requirements

### Hardware
- **Packet Sniffer Device:** Raspberry Pi or microcontroller.
- **Biometric Sensor:** R307 Fingerprint Sensor or Raspberry Pi Camera.
- **Display Interface:** LCD screen or web-based dashboard.
- **Storage:** Local or cloud storage with encryption.

### Software
- **Packet Sniffer Libraries:** Wireshark, Scapy, or custom scripts.
- **Biometric Software:** OpenCV or Fingerprint Processing SDK.
- **Encryption Libraries:** AES-256 or RSA.
- **UI Framework:** HTML, CSS, JavaScript.

## Non-Functional Requirements
- **Performance:** Real-time processing with minimal latency; biometric verification within 2 seconds.
- **Security:** AES-256 encryption for data; logging of unauthorized access attempts.
- **Reliability:** Continuous monitoring with 95% biometric accuracy.
- **Scalability:** Support for multiple protocols and scalable data storage.
- **Usability:** Intuitive UI for administrators; accessible alerts and logs.

## Expected Outcomes
- A secure packet sniffer with biometric login.
- Encrypted traffic logging and secure data storage.
- Real-time monitoring and alerting for suspicious activities.

## Conclusion
The Packet Sniffer with Biometrics for Secure Network Monitoring provides a robust solution for network security by combining real-time traffic analysis with biometric authentication, ensuring sensitive data remains protected and unauthorized access is prevented.

## Folder Structure
/ Biometrics_Packet_Sniffer
├── .git/                         # Git repository (hidden folder)
├── Arduino_uno_slave/             # (Folder for Arduino Uno code)
├── data/                          # (Probably for saved packets, etc.)
├── Documentation/                 # (Project docs)
├── main/                          # (ESP32 Main Code Here)
│   ├── biometric.ino              # Fingerprint/UART handling
│   ├── crypto_utils.ino           # AES-256 encryption utilities
│   ├── isr_handlers.ino           # ISR (interrupt service routines)
│   ├── lcd_ui.ino                 # LCD display code
│   ├── main.ino                   # Entry point (setup + loop)
│   ├── packet_sniffer.ino         # WiFi packet capture/sniffing
├── test/                          # (Test code folder, maybe mockups)
├── Byte_builder_progress_report.docx   # Progress report
├── README.md                      # Project overview
├── Requirements Specification.pdf # Final project requirements