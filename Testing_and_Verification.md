# Testing and Verification Guide for RFID Attendance System

Follow these steps to test and verify that your RFID attendance system is working correctly.

## Hardware Testing

### 1. LCD Display Test

When you first power up the system, the LCD should:
1. Display "RFID Attendance" and "System Starting"
2. Show "Connecting WiFi" while connecting
3. Briefly display "WiFi Connected" with the IP address
4. Finally show "RFID Attendance" and "Scan Your Card"

If the LCD remains blank:
- Check the I2C address (default is 0x27)
- Verify the connections to D3 (SDA) and D4 (SCL)
- Try running an I2C scanner sketch to detect the correct address

### 2. RFID Reader Test

To test the RFID reader:
1. Open the Arduino IDE Serial Monitor (set to 115200 baud)
2. You should see initialization messages and version info for the RFID reader
3. Present an RFID card/tag to the reader
4. The Serial Monitor should display "Card ID: XXXXXXXX" with your card's ID
5. The LCD should also update to show the card was detected

If the RFID reader isn't detecting cards:
- Double-check all wiring connections following the diagram
- Make sure SDA (SS) is connected to D2 and RST to D1
- Verify that the card is compatible with the RC522 reader (13.56MHz)
- Try moving the card closer or at different angles

## WiFi and Google Sheets Integration Testing

### 1. WiFi Connection Test

1. Check that the system shows "WiFi Connected" during startup
2. The serial monitor should display "Connected to WiFi. IP: xxx.xxx.xxx.xxx"

If WiFi doesn't connect:
- Verify your SSID and password are correct in the code
- Check that your router is working and the signal is strong enough
- Try a static IP configuration if your network has issues with DHCP

### 2. Google Sheets Integration Test

1. Scan an RFID card that is registered in your Users sheet
2. The LCD should show "Sending data..."
3. After a few seconds, it should display "Attendance Recorded!"
4. Check your Google Sheet - there should be a new entry in the Attendance sheet
5. Scan the same card again within a short time
6. It should ignore the second scan (cooldown period)
7. Scan the same card much later in the same day
8. It should show "Already Recorded Today"

If Google Sheets integration isn't working:
- Check the Serial Monitor for HTTP response codes and messages
- Verify that your scriptUrl is correct in the Arduino code
- Make sure your Google Apps Script is deployed as a web app with anonymous access
- Check that the time on your ESP8266 is correct (NTP synchronization)

## Common Issues and Solutions

### System keeps restarting
- This is likely a power issue. Try using a better power supply or USB cable.
- Add a large capacitor (220µF or higher) between VIN and GND to stabilize power.

### HTTPS connection failing
- The SSL certificate validation might be failing. The code uses `setInsecure()` to bypass this.
- If you see SSL errors, verify you're using the latest ESP8266 board support.

### Inconsistent readings
- RFID cards need to be held steady near the reader.
- Metal objects nearby can interfere with the RFID field.
- Try to keep the reader away from power supplies, motors, or other electronic devices.

### Incorrect timestamps
- Check your timezone offset in the code (`gmtOffset_sec`).
- Verify that NTP synchronization is working by checking the timestamps in your Google Sheet.

## Adding Features

### Adding a Buzzer for Feedback
1. Connect a piezo buzzer to an available GPIO pin (e.g., D0) and GND
2. Add this code in your setup():
   ```
   #define BUZZER_PIN D0
   pinMode(BUZZER_PIN, OUTPUT);
   ```
3. Add functions for different beep patterns:
   ```
   void successBeep() {
     tone(BUZZER_PIN, 1000, 200);
     delay(200);
     tone(BUZZER_PIN, 1500, 200);
   }
   
   void errorBeep() {
     tone(BUZZER_PIN, 400, 400);
   }
   ```
4. Call these functions after successful or failed attendance logging

### Adding LEDs for Visual Feedback
1. Connect an RGB LED or separate LEDs to available GPIO pins
2. Update your code to indicate the status visually in addition to the LCD

## Regular Maintenance

1. **Backup Google Sheet**: Regularly make a copy of your Google Sheet to prevent data loss
2. **Clean Database**: Periodically archive old attendance records to keep the sheet performant
3. **Update Firmware**: Check for updates to the libraries used and update your system
4. **Check Physical Components**: Ensure all connections are secure and components are clean
5. **Battery Backup**: Consider adding a UPS or battery backup if power outages are common