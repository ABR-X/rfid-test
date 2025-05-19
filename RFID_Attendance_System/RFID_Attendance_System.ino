/*
 * RFID Attendance System with Google Sheets Integration
 * Components:
 * - ESP8266 NodeMCU
 * - RFID RC522 Reader
 * - LCD 1602 with I2C
 */

#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Google Script ID
const char* scriptUrl = "https://script.google.com/macros/s/AKfycbzl3OCqMyP4fMvXzp0sTBdgGyfgzVpnUyD254RZ6DTP-GWAP3VyNAcxAhfQg5oXMMuZcA/exec";

// RFID Reader pins (as per the image)
#define RST_PIN D1  // GPIO5
#define SS_PIN  D2  // GPIO4

// Initialize RFID reader
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Initialize LCD (address 0x27, 16 cols, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// NTP Server to get time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;      // Change to your timezone offset in seconds
const int daylightOffset_sec = 3600; // Change for daylight saving

// Variables to track card state
String lastCardID = "";
unsigned long lastCardTime = 0;
const unsigned long cardCooldown = 3000; // 3 seconds cooldown

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD
  Wire.begin(D3, D4); // SDA, SCL
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Attendance");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");
  
  // Initialize WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print("Connecting WiFi ");
  }
  
  Serial.println();
  Serial.print("Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);
  
  // Initialize SPI bus
  SPI.begin();
  
  // Initialize RFID reader
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  
  // Configure time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Attendance");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card");
}

void loop() {
  // Check if new card is present
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Read the card
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Get current card ID
  String cardID = getCardID();
  
  // Check if this is the same card as before (within cooldown period)
  unsigned long currentTime = millis();
  if (cardID == lastCardID && (currentTime - lastCardTime < cardCooldown)) {
    Serial.println("Card cooldown active, ignoring duplicate read");
    return;
  }
  
  // Update last card info
  lastCardID = cardID;
  lastCardTime = currentTime;
  
  // Display card ID
  Serial.print("Card ID: ");
  Serial.println(cardID);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card detected:");
  lcd.setCursor(0, 1);
  lcd.print(cardID);
  
  // Send data to Google Sheets
  if (WiFi.status() == WL_CONNECTED) {
    sendAttendance(cardID);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Disconnected");
    lcd.setCursor(0, 1);
    lcd.print("Reconnecting...");
    
    WiFi.reconnect();
    delay(5000);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID Attendance");
    lcd.setCursor(0, 1);
    lcd.print("Scan Your Card");
  }
  
  // Wait a bit before reading again
  delay(1000);
  
  // Reset the LCD to ready state
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Attendance");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card");
  
  // Halt PICC and stop encryption
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// Function to get card ID as a hex string
String getCardID() {
  String cardID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardID += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    cardID += String(mfrc522.uid.uidByte[i], HEX);
  }
  cardID.toUpperCase();
  return cardID;
}

// Function to get current timestamp
String getTimeStamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "Time Error";
  }
  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}

// Function to send attendance data to Google Sheets
void sendAttendance(String cardID) {
  // Get current timestamp
  String timestamp = getTimeStamp();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sending data...");
  
  // Prepare URL with parameters
  String url = String(scriptUrl) + "?card_id=" + cardID + "&timestamp=" + timestamp;
  url.replace(" ", "%20"); // Replace spaces with URL encoding
  
  Serial.println("Sending HTTP request to: " + url);
  
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure(); // Skip certificate verification
  
  HTTPClient https;
  
  if (https.begin(*client, url)) {
    int httpCode = https.GET();
    
    if (httpCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpCode);
      String payload = https.getString();
      Serial.println("Response: " + payload);
      
      lcd.clear();
      lcd.setCursor(0, 0);
      
      if (payload.indexOf("success") >= 0) {
        lcd.print("Attendance");
        lcd.setCursor(0, 1);
        lcd.print("Recorded!");
      } else if (payload.indexOf("already") >= 0) {
        lcd.print("Already");
        lcd.setCursor(0, 1);
        lcd.print("Recorded Today");
      } else {
        lcd.print("Error Recording");
        lcd.setCursor(0, 1);
        lcd.print("Attendance");
      }
    } else {
      Serial.printf("HTTP Request failed: %s\n", https.errorToString(httpCode).c_str());
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("HTTP Error:");
      lcd.setCursor(0, 1);
      lcd.print(httpCode);
    }
    
    https.end();
  } else {
    Serial.println("HTTPS connection failed");
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connection");
    lcd.setCursor(0, 1);
    lcd.print("Failed");
  }
  
  delay(2000);
}