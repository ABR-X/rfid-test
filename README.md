# RFID Attendance System with Google Sheets Integration

This project implements an RFID-based attendance system using an ESP8266, RFID-RC522 reader, and LCD display. Attendance records are logged to Google Sheets.

## Components Required

- ESP8266 NodeMCU
- RFID RC522 Reader
- LCD 1602 with I2C Adapter
- Connecting Wires
- Micro USB Cable for power
- RFID Cards/Tags

## Wiring Diagram

Based on the architecture image, here's how to connect the components:

### ESP8266 to RFID RC522 Reader

| RFID RC522 Pin | ESP8266 Pin | Function            |
|----------------|-------------|--------------------|
| SDA            | D2 (GPIO4)  | SPI Slave Select   |
| SCK            | D5 (GPIO14) | SPI Clock          |
| MOSI           | D7 (GPIO13) | Master Out Slave In|
| MISO           | D6 (GPIO12) | Master In Slave Out|
| IRQ            | Not used    | -                  |
| GND            | GND         | Ground             |
| RST            | D1 (GPIO5)  | Reset              |
| 3.3V           | 3.3V        | Power              |

### ESP8266 to LCD 1602 I2C

| LCD I2C Pin | ESP8266 Pin | Function    |
|-------------|-------------|------------|
| SDA         | D3 (GPIO0)  | I2C Data   |
| SCL         | D4 (GPIO2)  | I2C Clock  |
| VCC         | 3.3V        | Power      |
| GND         | GND         | Ground     |

## Software Setup

### 1. Google Sheets Setup

1. Create a new Google Sheet
2. Go to Extensions > Apps Script
3. Replace the code with the content of `Google_Apps_Script.js`
4. Update the `SHEET_ID` variable with your Google Sheet ID (found in the URL)
5. Save the script
6. Deploy as a web app:
   - Click on Deploy > New deployment
   - Select type: Web app
   - Execute as: Me
   - Who has access: Anyone, even anonymous
   - Click Deploy
7. Copy the web app URL - this is your `scriptUrl`
8. Run the `setupSpreadsheet` function manually to set up your sheets
9. Optionally run `addSampleUser` to add a test user

### 2. Arduino IDE Setup

1. Install the Arduino IDE
2. Add ESP8266 board support:
   - Go to File > Preferences
   - Add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to Additional Boards Manager URLs
   - Go to Tools > Board > Boards Manager, search for ESP8266 and install
3. Install required libraries (Tools > Manage Libraries):
   - MFRC522 (by GithubCommunity)
   - LiquidCrystal I2C (by Frank de Brabander)
4. Open the `RFID_Attendance_System.ino` file
5. Update the following variables:
   - `ssid` - Your WiFi network name
   - `password` - Your WiFi password
   - `scriptUrl` - The Google Apps Script web app URL from step 1.7
   - `gmtOffset_sec` - Your timezone offset in seconds (e.g., +1 hour = 3600, -5 hours = -18000)
6. Select your board (Tools > Board > ESP8266 Boards > NodeMCU 1.0)
7. Select the correct port (Tools > Port)
8. Upload the sketch

## Usage

1. Power up the system
2. The LCD will show the initialization process, connecting to WiFi
3. Once ready, it will display "Scan Your Card"
4. Scan an RFID card/tag
5. The system will read the card ID and send it to Google Sheets
6. The LCD will display the result:
   - "Attendance Recorded!" - if successful
   - "Already Recorded Today" - if the same card was already logged today
   - Error message - if there was a problem

## Troubleshooting

- **LCD not working**: Check I2C address (default is 0x27), you might need to change it in the code
- **RFID reader not detecting cards**: Check the wiring, especially SDA (SS) connection
- **WiFi connection issues**: Verify SSID and password
- **Google Sheets not receiving data**: 
  - Verify the scriptUrl is correct
  - Check the script deployment settings
  - Ensure you've published the script as a web app with anonymous access

## Creating and Managing Users

Add users to the "Users" sheet in your Google Spreadsheet with the following columns:
- Card ID: The ID of the RFID card (will be displayed on the serial monitor when scanned)
- User ID: Employee or student ID
- Name: User's name
- Department: User's department (optional)
- Notes: Any additional information (optional)

## Exporting Reports

You can create reports directly in Google Sheets using formulas, pivot tables, or charts to analyze the attendance data. For example:
- Count days attended per person
- View who was present on a specific day
- Calculate attendance rates

## Future Improvements

- Add offline mode with local storage when WiFi is unavailable
- Implement user feedback using a buzzer or LED
- Add admin card functionality to manage users directly from the device
- Create a web interface for system administration