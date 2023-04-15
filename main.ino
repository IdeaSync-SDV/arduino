#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define CONNECTION_TIMEOUT 10

const char* ssid = "ssid_here";
const char* password = "password_here";

// Initialiser la bibliothèque LCD
const int rs = 16, en = 4, d4 = 17, d5 = 5, d6 = 18, d7 = 19;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int buttonPin = 15;

const int redPin = 25;
const int greenPin = 26;
const int bluePin = 27;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);

  // Leds
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Initialiser l'écran LCD
  lcd.begin(16, 2);  //16x2 pixels

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau WiFi...");
  }
  Serial.println("Connecté au réseau WiFi.");

  update_lcd();
}

void loop() {
  byte buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    update_todo();
  }

  update_lcd();

  delay(90);
}

void update_lcd() {
  HTTPClient http;
  http.begin("http://172.20.10.2:4000/todos/oldest");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    if (doc["result"] == "null") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Rien a faire !");
      
      change_to("green");
    } else {
      const char* title = doc["result"]["title"];
      const char* content = doc["result"]["content"];

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(title);
      lcd.setCursor(0, 1);
      lcd.print(content);

      change_to("red");
    }
  }
}

void update_todo() {
  HTTPClient http;
  http.begin("http://172.20.10.2:4000/todos/oldest");
  int httpCode = http.PATCH("");
}

void change_to(char* color) {
  if (color == "red") {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
  } else if (color == "green") {
    digitalWrite(redPin, LOW);
    digitalWrite(bluePin, LOW);
    digitalWrite(greenPin, HIGH);
  }
}