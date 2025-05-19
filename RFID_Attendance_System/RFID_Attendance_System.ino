/*
 * RFID Attendance System with Google Sheets Integration
 * 
 * Components:
 * - ESP8266 WiFi Module
 * - RFID RC522 Reader
 * - LCD 1602 I2C Display
 * 
 * This system reads RFID cards, displays information on the LCD,
 * and logs attendance data to Google Sheets via a web app.
 */

#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Network credentials
const char* WIFI_SSID = "AdminNet";
const char* WIFI_PASSWORD = "@ENSAM2010.";

// Google Script ID
const String GOOGLE_SCRIPT_ID = "AKfycbzQ92zb-Sgul1QEuNtGLhRwIBFK_l9oY2yg0w-7Kf9b2YWmHk7tpy0iG5mBxOMCCZbJ";

// RFID pins
#define RST_PIN D3
#define SS_PIN D4

// Initialize RFID reader
MFRC522 rfid(SS_PIN, RST_PIN);

// Initialize LCD (address, columns, rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// NTP Client for time synchronization
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Status LEDs (optional)
// #define LED_GREEN D0
// #define LED_RED D8

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD
  Wire.begin(D2, D1); // SDA, SCL
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Attendance");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");
  
  // Initialize SPI bus
  SPI.begin();
  
  // Initialize RFID reader
  rfid.PCD_Init();
  
  // Connect to WiFi
  connectToWiFi();
  
  // Initialize NTP client
  timeClient.begin();
  timeClient.setTimeOffset(3600); // Set your timezone offset in seconds
  
  // Initialize status LEDs (optional)
  // pinMode(LED_GREEN, OUTPUT);
  // pinMode(LED_RED, OUTPUT);
  // digitalWrite(LED_GREEN, LOW);
  // digitalWrite(LED_RED, LOW);
  
  // Display ready message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to Scan");
  lcd.setCursor(0, 1);
  lcd.print("RFID Card");
}

void loop() {
  // Keep NTP client updated
  timeClient.update();
  
  // Check if WiFi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
  
  // Check for new RFID cards
  if (!rfid.PICC_IsNewCardPresent())
    return;
  
  // Read the card
  if (!rfid.PICC_ReadCardSerial())
    return;
  
  // Get the card ID as a string
  String cardId = getCardId();
  Serial.println("Card ID: " + cardId);
  
  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card: " + cardId);
  lcd.setCursor(0, 1);
  lcd.print("Processing...");
  
  // Send data to Google Sheets
  String response = sendAttendanceData(cardId);
  
  // Parse the response
  String status = parseResponse(response, "status");
  String message = parseResponse(response, "message");
  String user = parseResponse(response, "user");
  
  // Display result on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  
  if (status == "success") {
    lcd.print("Welcome!");
    lcd.setCursor(0, 1);
    lcd.print(user);
    // digitalWrite(LED_GREEN, HIGH); // Optional
    // delay(1000);
    // digitalWrite(LED_GREEN, LOW);
  } 
  else if (status == "already") {
    lcd.print("Already logged!");
    lcd.setCursor(0, 1);
    lcd.print(user);
  } 
  else {
    lcd.print("Error!");
    lcd.setCursor(0, 1);
    lcd.print(message.substring(0, 16)); // Limit to 16 chars
    // digitalWrite(LED_RED, HIGH); // Optional
    // delay(1000);
    // digitalWrite(LED_RED, LOW);
  }
  
  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption
  rfid.PCD_StopCrypto1();
  
  delay(3000); // Display result for 3 seconds
  
  // Reset display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to Scan");
  lcd.setCursor(0, 1);
  lcd.print("RFID Card");
}

void connectToWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    lcd.setCursor(attempts % 16, 1);
    lcd.print(".");
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP().toString());
    delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!");
    lcd.setCursor(0, 1);
    lcd.print("Check Settings.");
    delay(2000);
  }
}

String getCardId() {
  String cardId = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    cardId.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
    cardId.concat(String(rfid.uid.uidByte[i], HEX));
  }
  cardId.toUpperCase();
  return cardId;
}

String sendAttendanceData(String cardId) {
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec";
  String timestamp = String(timeClient.getEpochTime());
  
  url += "?card_id=" + cardId + "&timestamp=" + timestamp;
  
  Serial.println("Sending to: " + url);
  
  WiFiClientSecure client;
  client.setInsecure(); // Skip certificate validation (not recommended for production)
  
  HTTPClient http;
  http.begin(client, url);
  
  int httpCode = http.GET();
  String response = "";
  
  if (httpCode > 0) {
    response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.println("HTTP request failed");
    response = "{\"status\":\"error\",\"message\":\"HTTP request failed\"}";
  }
  
  http.end();
  return response;
}

String parseResponse(String response, String key) {
  // Simple JSON parser
  int keyIndex = response.indexOf("\"" + key + "\"");
  if (keyIndex == -1) return "";
  
  int valueStartIndex = response.indexOf(":", keyIndex) + 1;
  int valueEndIndex;
  
  if (response.charAt(valueStartIndex) == '\"') {
    valueStartIndex++; // Skip the opening quote
    valueEndIndex = response.indexOf("\"", valueStartIndex);
  } else {
    valueEndIndex = response.indexOf(",", valueStartIndex);
    if (valueEndIndex == -1) {
      valueEndIndex = response.indexOf("}", valueStartIndex);
    }
  }
  
  if (valueEndIndex == -1) return "";
  
  return response.substring(valueStartIndex, valueEndIndex);
}
