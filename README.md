# Biometric-Bank-Locker-Security
## Overview
This project uses an **LPC2148 ARM7 microcontroller**, **R305 fingerprint module**, and **I2C EEPROM** to implement a **two-level authentication system** for bank lockers.  
A locker will only open if both the **password** and **fingerprint** match.

---

## Features
- Two-level security: Password + Fingerprint
- Passwords stored in I2C EEPROM
- Fingerprint enroll, delete, and search options
- Edit password and fingerprint via external interrupt
- DC motor control for locker open/close
- LCD display and 4x4 keypad for user interaction

---

## Hardware
- LPC2148 Development Board
- R305 Fingerprint Sensor
- AT24C256 EEPROM (I2C)
- 4x4 Matrix Keypad
- 16x2 LCD
- L293D Motor Driver + DC Motor
- Push button for interrupt menu

---

## 💻 Software Requirements
- **Keil uVision** (ARM Compiler)
- **Flash Magic** (for flashing code to LPC2148)
- Embedded C
- Basic I2C, UART, GPIO, and External Interrupt knowledge
  
---

## How It Works
1. **Unlock Mode**
   - User enters **User ID**
   - Enters **4-digit password** (stored in EEPROM)
   - Places finger on **R305 sensor**
   - If both password and fingerprint match → Locker opens

2. **Edit Mode** (Triggered via External Interrupt Button)
   - **Edit Password**:
     - Ask for User ID
     - Ask for current password
     - Set and confirm new password
     - Save to EEPROM
   - **Edit Fingerprint**:
     - Enroll new fingerprint
     - Delete specific fingerprint
     - Delete all fingerprints

3. **Motor Control**
   - Forward rotation → Locker open
   - Reverse rotation → Locker close
