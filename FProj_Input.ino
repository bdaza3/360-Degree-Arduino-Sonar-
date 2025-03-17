#include <WiFiS3.h>
#include <DIYables_Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// Wi-Fi credentials
const char* ssid = "Sonar";          // AP SSID
const char* password = "password123"; // AP password


// Keypad configuration
const int ROW_NUM = 4; // Four rows
const int COLUMN_NUM = 4; // Four columns


char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad


DIYables_Keypad keypad = DIYables_Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);


// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);


// Server setup
WiFiServer server(80);
WiFiClient client;


// Global variable to track LCD line
int lcdLine = 0;


void setup() {
  Serial.begin(9600);
  delay(1000);


  // Initialize I2C and LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Keypad LCD");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");


  Serial.println("Starting WiFi Server...");


  // Start Wi-Fi in AP mode
  int status = WiFi.beginAP(ssid, password);
  if (status == WL_AP_LISTENING) {
    Serial.println("Access point created successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
   
    // Display IP on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
  } else {
    Serial.println("Failed to create access point");
    lcd.clear();
    lcd.print("WiFi Error");
    while (true); // Halt on failure
  }


  server.begin();
  Serial.println("Server started!");
}


void loop() {
  // Check for new clients
  if (!client || !client.connected()) {
    client = server.available(); // Accept new client connection
    if (client) {
      Serial.println("Client connected!");
      client.println("Welcome! Keypad inputs and manual messages will appear here.");
     
      // Clear LCD and show connection status
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Client Connected");
    }
  }


  // Read keypad input
  char key = keypad.getKey();
  if (key) {
    String message = "";
    message += key;
    Serial.println("Broadcasting: " + message);


    // Display on LCD
    if (key == '1') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sonar System:");
      lcd.setCursor(0, 1);
      lcd.print("ON");
    } else if (key == '0') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sonar System:");
      lcd.setCursor(0, 1);
      lcd.print("OFF");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Key Pressed:");
      lcd.setCursor(0, 1);
      lcd.print(key);
    }
    // Send keypad input to client
    if (client && client.connected()) {
      client.println(message);
    }
  }


  // Read from the serial monitor and send to the client
  if (Serial.available()) {
    String inputMessage = Serial.readStringUntil('\n');
    inputMessage.trim(); // Remove any newline or whitespace characters
    if (client && client.connected()) {
      client.println("Server says: " + inputMessage);
    }
   
    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Serial Message:");
    lcd.setCursor(0, 1);
    lcd.print(inputMessage.substring(0, 16)); // Truncate to fit LCD
   
    Serial.println("Sent to client: " + inputMessage);
  }


  // Check if the client has disconnected
  if (client && !client.connected()) {
    Serial.println("Client disconnected.");
    client.stop();
   
    // Clear LCD and show disconnection
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Client Disc.");
  }


  // Read messages from the client
  if (client && client.available()) {                                  
    String receivedMessage = client.readString();
    Serial.println("Received from client: " + receivedMessage);
   
    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Client Message:");
    lcd.setCursor(0, 1);
    lcd.print(receivedMessage.substring(0, 16)); // Truncate to fit LCD
  }
}
