#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <MFRC522.h>

const int numSensors = 6;
const int numLed = 5;
const int ledPin[numLed] = {26, 25, 14, 12, 13};
const int numIrSensor = 5;
const int IrsensorPin[numIrSensor] = {33, 32, 35, 34, 27};
int sensorStatus[numSensors];
WiFiMulti WiFiMulti;

int Status1;
String Status2;
String Status3;
String Status4;
String Status5;
/********************************************************************************/
// Things to change
const char *ssid = "jet";
const char *password = "0959202399";
String GOOGLE_SCRIPT_ID = "AKfycbzg1S3gOvRd1SiEyUkf-s4I-B1g_E1RzkYNkJZyFHNCQ6ZG0LeJkB4nAiXbH7yR4o_EEQ";
int Buzzer = 15;
const int sendInterval = 1000;//5000
/********************************************************************************/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
String payload;
int payloadNumber = 0; // Variable to store the integer part of the payload
String payloadString = ""; // Variable to store the string part of the payload

int number;
String text;

WiFiClientSecure client;

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5  // ESP32 pin GIOP5
#define RST_PIN 0 // ESP32 pin GIOP27
String uidString = "";
MFRC522 rfid(SS_PIN, RST_PIN);

bool cardReadInProgress = false;
unsigned long lastCardRead = 0;
const unsigned long debounceDelay = 1000; // 3 seconds debounce delay

void setup() {
  Serial.begin(115200);
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Started");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  SPI.begin(); // init SPI bus
  rfid.PCD_Init();

  Serial.println("Ready to go");
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, HIGH);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (!cardReadInProgress && rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) { 
    cardReadInProgress = true;
    lastCardRead = currentMillis;
    
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.print("ประเภทของ RFID/NFC: ");
    Serial.println(rfid.PICC_GetTypeName(piccType));

    uidString = "";
    for (int i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) {
        uidString += " 0";
      } else {
        uidString += " ";
      }
      uidString += String(rfid.uid.uidByte[i], HEX);
    }

    Serial.print("UID: ");
    Serial.println(uidString);
    Serial.println();

    rfid.PICC_HaltA(); // halt PICC
    rfid.PCD_StopCrypto1(); // stop encryption on PCD

    // Perform necessary actions with the UID
    if (uidString == " 76 b1 b0 24") {
      Serial.println("Steve Job");
      digitalWrite(Buzzer, LOW);
      delay(1000);
      digitalWrite(Buzzer, HIGH);
    }
    if (uidString == " f5 47 a0 ac") {
      Serial.println("พยัคฆราชซ่อนเล็บ3");
    }
    if (uidString == " 0e 05 13 71") {
      Serial.println("พยัคฆราชซ่อนเล็บ1");
      digitalWrite(Buzzer, LOW);
      delay(1000);
      digitalWrite(Buzzer, HIGH);
    }
    if (uidString == " 5e 7b 15 71") {
      Serial.println("พยัคฆราชซ่อนเล็บ1");
      digitalWrite(Buzzer, LOW);
      delay(1000);
      digitalWrite(Buzzer, HIGH);
    }

    uidString = ""; // Reset UID string after processing

  } else if (cardReadInProgress && (currentMillis - lastCardRead > debounceDelay)) {
    cardReadInProgress = false;
  }

  for (int i = 0; i < numIrSensor; i++) {
    pinMode(IrsensorPin[i], INPUT);
    sensorStatus[i] = !digitalRead(IrsensorPin[i]);
    Serial.println(sensorStatus[i]);
    pinMode(ledPin[i], OUTPUT);

    if (sensorStatus[i] == 0) { 
      if(i == 0){
        Status1 = 1;
      }
      if(i == 1){
        Status1 = 2;
      }
      if(i == 2){
        Status1 = 3;
      }
      if(i == 3){
        Status1 = 4;
      }
      if(i == 4){
        Status1 = 5;
      }

      if(Status1 == number){
        digitalWrite(Buzzer, HIGH);
        Serial.println("is equal");
      }
      else{
        if(text == "รับ"){
          if(Status1 != number){
            digitalWrite(Buzzer, LOW);
            delay(1000);
            digitalWrite(Buzzer, HIGH);
          }

        }
        else{
          digitalWrite(Buzzer, LOW);
          delay(1000);
          digitalWrite(Buzzer, HIGH);
        }

      }

  
    }
  }

  delay(50);
  HTTPClient http;
  String url = "https://script.google.com/macros/s/AKfycby7PmyzJkj3eBHDUgAmVtuP1VnsBU8xVVDlEdihX8lzLkVVjfaMV48cfSxiBwagSUnH/exec?";

  for (int i = 0; i < numSensors; i++) {
    url += "value" + String(i) + "=" + String(sensorStatus[i]) + "&";
  }

  Serial.println("Making a request");
  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();

  if (httpCode > 0) {
    payload = http.getString();
    Serial.println(httpCode);
    Serial.println("Raw payload: " + payload); // Print the raw payload for debugging

    int spaceIndex = payload.indexOf(' ');
    Serial.println("Space index: " + String(spaceIndex)); // Debugging space index
    if (spaceIndex > 0) {
      payloadNumber = payload.substring(0, spaceIndex).toInt();
      payloadString = payload.substring(spaceIndex + 1);
    } else {
      payloadNumber = payload.toInt();
      payloadString = "";
    }
    Serial.print("Payload Number: ");
    Serial.println(payloadNumber);
    Serial.print("Payload String: ");
    Serial.println(payloadString); // Check if this prints anything
    
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
  delay(500);
  spreadsheet_comm();
  delay(sendInterval);
  condition();
}

void spreadsheet_comm() {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?read";
  Serial.print("Making a request");
  http.begin(url.c_str()); // Specify the URL and certificate
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();

  if (httpCode > 0) { // Check for the returning code
    payload = http.getString();
    Serial.println(httpCode);
    Serial.println("Raw payload: " + payload); // Print the raw payload for debugging
    
    // Separate the payload into integer and string parts
    int openParenIndex = payload.indexOf('(');
    String numberString = payload.substring(0, openParenIndex);
    number = numberString.toInt();
  
    int closeParenIndex = payload.indexOf(')');
    text = payload.substring(openParenIndex + 1, closeParenIndex);
// Check if this prints anything
    Serial.print("Number: ");
    Serial.println(number);
    Serial.print("Text: ");
    Serial.println(text);
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();
}

void condition() {
  if(text == "จอง"){
    if (number == 5) {
    digitalWrite(ledPin[4], HIGH);

      if(sensorStatus[4] == 0){
        Buzzer1();
      }
    } 
    else if (number == 4) {
      digitalWrite(ledPin[3], HIGH);
      if(sensorStatus[3] == 0){
        Buzzer1();
      }
      else{
        digitalWrite(Buzzer, HIGH);
      }
    }
    else if (number == 3) {
      digitalWrite(ledPin[2], HIGH);
      if(sensorStatus[2] == 0){
        Buzzer1();
      }
      else{
        digitalWrite(Buzzer, HIGH);
      }
    }
    else if (number == 2) {
      digitalWrite(ledPin[1], HIGH);
      if(sensorStatus[1] == 0){
        Buzzer1();
      }
      else{
        digitalWrite(Buzzer, HIGH);
      }
    }
    else if (number == 1) {
      digitalWrite(ledPin[0], HIGH);
      if(sensorStatus[0] == 0){
        Buzzer1();
      }
      else{
        digitalWrite(Buzzer, HIGH);
      }
    }

  }

  else{  
    if (number == 5) {
    digitalWrite(ledPin[4], LOW);

    } 
    else if (number == 4) {
      digitalWrite(ledPin[3], LOW);

    }
    else if (number == 3) {
      digitalWrite(ledPin[2], LOW);

    }
    else if (number == 2) {
      digitalWrite(ledPin[1], LOW);

    }
    else if (number == 1) {
      digitalWrite(ledPin[0], LOW);
    }

  }
  digitalWrite(Buzzer, HIGH);
}

void Buzzer1(){
  
  digitalWrite(Buzzer, LOW);
  delay(1000);
  digitalWrite(Buzzer, HIGH);
  //delay(100);
  //digitalWrite(Buzzer, LOW);
}
