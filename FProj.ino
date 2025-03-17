#include <WiFiS3.h>


// Wi-Fi credentials
const char* ssid = "Sonar";          // Server AP SSID
const char* password = "password123"; // Server AP password
const char* serverIP = "192.168.4.1"; // Server's default IP in AP mode
const int serverPort = 80;
// Client setup
WiFiClient client;

#define SERIAL_PORT Serial1

// Includes the Servo library
#include <Servo.h>. 
String receivedMessage;
// Defines Trig and Echo pins of the Ultrasonic Sensor
const int trigPin = 11; //GREEN WIRE
const int echoPin = 10; //BLUE WIRE
// Variables for the duration and the distance
long duration;
int distance;

Servo myServoBottom; 
Servo myServoTop; 

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600);
  SERIAL_PORT.begin(9600);
  myServoBottom.attach(12); // Defines on which pin is the servo motor attached
  myServoTop.attach(8); // Defines on which pin is the servo motor attached

  while (!Serial);

  Serial.println("Connecting to Access Point...");
  WiFi.begin(ssid, password);

  // Wait for connection to AP
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Access Point");
  Serial.print("Client IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Attempting to connect to server...");
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Connected to server!");
    client.println("Hello from the client!");
  } else {
    Serial.println("Connection to server failed. Check server availability.");
  }

  Serial.println("Type messages in the serial monitor to send to the server.");

}//end of setup

void loop() {
// Read from the serial monitor and send to the server
  if (Serial.available()) {
    String inputMessage = Serial.readStringUntil('\n');
    inputMessage.trim(); // Remove any newline or whitespace characters
    if (client && client.connected()) {
      client.println(inputMessage);
      Serial.println("Sent to server: " + inputMessage);
    } else {
      Serial.println("Failed to send, not connected to server.");
    }
  }

  // Read messages from the server
  if (client && client.available()) {
    receivedMessage = client.readStringUntil('\n');
    Serial.println("Received from server: " + receivedMessage);
  }

  // Reconnect if the connection is lost
  if (!client.connected()) {
    Serial.println("Reconnecting to server...");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Reconnected to server!");
    } else {
      Serial.println("Failed to reconnect. Retrying...");
    }
  }
  if (receivedMessage.startsWith("1")) {

  for(int i=0;i<=180;i+=10){//bottom up to 180 
    myServoBottom.write(i);
    delay(60);
    distance = calculateDistance();
    if (distance < 10){
      Serial.println("Detected");
      SERIAL_PORT.write(i);
    }
  }
  for(int i=0;i<=180;i+=10){//top 180 up to 360
    myServoTop.write(i);
    delay(60);
    distance = calculateDistance();
    if (distance < 10){
      Serial.println("Detected");
      SERIAL_PORT.write(i + 180);
    }
  }
  for(int i=180;i>0;i-=10){//top back down to 180
    myServoTop.write(i);
    delay(60);
    distance = calculateDistance();
    if (distance < 10){
      Serial.println("Detected");
      SERIAL_PORT.write(i + 180);
    }
  }
  for(int i=180;i>0;i-=10){//bottom back down to 0
    myServoBottom.write(i);
    delay(60);
    distance = calculateDistance();
    if (distance < 10){
      Serial.println("Detected");
      SERIAL_PORT.write(i);
    }
  }
  }
}

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance(){
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance= duration*0.034/2;
  return distance;
}



