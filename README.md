# RFID Attendance System with Google Sheets Integration

This project implements an RFID-based attendance system that logs data to Google Sheets. When an RFID card is scanned, the system checks the user information in the Google Sheet and logs their attendance.

## Components

- ESP8266 WiFi Module (NodeMCU)
- MFRC522 RFID Reader Module
- LCD 1602 I2C Display
- Connecting Wires
- RFID Cards/Tags

## Setup Instructions

### 1. Google Sheets Setup

1. Create a new Google Sheet at [sheets.google.com](https://sheets.google.com)
2. Note the Sheet ID from the URL (the long string between `/d/` and `/edit`)
3. Go to Extensions > Apps Script
4. Copy and paste the content of the `Google_Apps_Script.js` file
5. Replace `YOUR_SHEET_ID_HERE` with your actual Sheet ID
6. Save the script and name it "RFID Attendance System"
7. Run the `setupSpreadsheet()` function to create the necessary sheets and formatting
8. Deploy the script as a web app:
   - Click Deploy > New deployment
   - Select type: Web app
   - Set "Who has access" to "Anyone, even anonymous"
   - Click "Deploy"
   - Copy the Web App URL - you'll need the script ID later

### 2. Hardware Wiring

Connect the components as follows:

#### ESP8266 (NodeMCU) to MFRC522 RFID Reader:
- RST (Reset) → D3
- SDA (SS) → D4
- MOSI → D7
- MISO → D6
- SCK → D5
- GND → GND
- 3.3V → 3.3V

#### ESP8266 to LCD I2C Display:
- SDA → D2
- SCL → D1
- GND → GND
- VCC → 3.3V

#### Wiring Diagram:
```
+------------+      +------------+      +------------+
|            |      |            |      |            |
| ESP8266    |      | RFID-RC522 |      | LCD 1602   |
| (NodeMCU)  |      |            |      | with I2C   |
|            |      |            |      |            |
| D1 --------+------+------------+------+-> SCL      |
| D2 --------+------+------------+------+-> SDA      |
| D3 --------+------+-> RST      |      |            |
| D4 --------+------+-> SDA(SS)  |      |            |
| D5 --------+------+-> SCK      |      |            |
| D6 --------+------+-> MISO     |      |            |
| D7 --------+------+-> MOSI     |      |            |
| 3.3V ------+------+-> 3.3V     +------+-> VCC      |
| GND -------+------+-> GND      +------+-> GND      |
+------------+      +------------+      +------------+
```

### 3. Arduino Setup

1. Install the Arduino IDE and set it up for ESP8266 development
2. Install the following libraries through Library Manager:
   - MFRC522 (by GithubCommunity)
   - LiquidCrystal_I2C (by Frank de Brabander)
   - ESP8266WiFi (should be included with ESP8266 board manager)
   - ESP8266HTTPClient
   - NTPClient (by Fabrice Weinberg)

3. Open the `RFID_Attendance_System.ino` file
4. Update the following configuration:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   const String GOOGLE_SCRIPT_ID = "YOUR_GOOGLE_SCRIPT_ID";
   ```
   - Replace `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD` with your WiFi credentials
   - Replace `YOUR_GOOGLE_SCRIPT_ID` with the script ID from your Google Apps Script URL
     (The script ID is the string between `/macros/s/` and `/exec` in the web app URL)

5. Adjust the NTP time offset according to your timezone:
   ```cpp
   timeClient.setTimeOffset(0); // Set your timezone offset in seconds
   ```
   For example, for UTC+1, use 3600; for UTC-5, use -18000

6. Upload the code to your ESP8266

### 4. User Registration

1. Open your Google Sheet
2. Go to the "Users" sheet
3. Add RFID card IDs and user information:
   - Column A: Card ID (e.g., "ABCD1234")
   - Column B: User ID (e.g., "EMP001")
   - Column C: Name (e.g., "John Doe")
   - Column D: Department (e.g., "Engineering")
   - Column E: Notes (optional)

4. You can run the `addSampleUser()` function in the Apps Script to add a sample user

## Usage

1. Power up the system
2. The LCD should display "Ready to Scan RFID Card"
3. Scan an RFID card/tag
4. The system will:
   - Read the card ID
   - Send the data to Google Sheets
   - Display the result on the LCD:
     - Success: "Welcome! [User Name]"
     - Already logged: "Already logged! [User Name]"
     - Error: Error message

5. View attendance records in the "Attendance" sheet of your Google Sheet

## Troubleshooting

- **LCD not displaying**: Check I2C address - may need to change from 0x27 to 0x3F depending on your LCD model
- **RFID not reading**: Verify wiring connections, especially SDA and SCK
- **Connection failure**: Ensure WiFi credentials are correct and signal is strong
- **Google Sheets errors**: Verify script ID and ensure the web app is deployed correctly
- **Time issues**: Adjust NTP server and time offset according to your location

## Extending the Project

- Add LED indicators for successful/failed scans
- Implement a buzzer for audio feedback
- Add buttons for manual user interface options
- Create a custom case using 3D printing
- Add offline mode with local storage when WiFi is unavailable
- Implement user registration directly from the device

## License

This project is open source and available under the MIT License.

---

Created with ❤️ by [Your Name]
