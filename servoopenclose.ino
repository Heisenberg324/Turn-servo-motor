#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Servo.h>

#define WIFI_SSID "POCO X3"
#define WIFI_PASSWORD "skadyoosh"
#define FIREBASE_HOST "rit24safaai-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "oTNDqOODORkq8tcz2T3OgtrQt66xqyZVKE4bB6iu"
#define TRANSPARENT_SENSOR_PIN A0   // Pin connected to transparent object sensor
#define IR_PIN D2

#define SERVO_PIN D1   // Pin connected to servo motor

FirebaseData firebaseData;
Servo servo;

unsigned long openTime = 0;
const unsigned long openDuration = 5000; // 5 seconds

void setup() {
  Serial.begin(9600);  // Select the same baud rate if you want to see the data on Serial Monitor
  Serial.println("Serial communication started\n\n");  
          
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Try to connect with WiFi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());  // Print local IP address

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  // Connect to Firebase
  Firebase.reconnectWiFi(true);

  servo.attach(SERVO_PIN); // Attach servo to pin
  servo.write(0); // Initially set servo to 0 degrees
  delay(1000);
}

void loop() { 
  int irValue= digitalRead(IR_PIN);
  int transparentSensorValue = analogRead(TRANSPARENT_SENSOR_PIN);  // Read the analog value from the transparent object sensor
  Serial.print("Transparent object sensor value: ");
  Serial.println(transparentSensorValue);  // Print the analog value to Serial Monitor

  if (irValue== LOW && transparentSensorValue < 9) {
    // If IR sensor is low and analog value is less than 9, read the value from the database and print it
    if (Firebase.getInt(firebaseData, "/data/num1")) {
      if (firebaseData.dataType() == "int") {
        int receivedValue = firebaseData.intData();
        Serial.print("Received value from database: ");
        Serial.println(receivedValue);
        // Move servo to 90 degrees
        servo.write(90);
        openTime = millis(); // Record the time when the lid opened
        delay(1000); // Delay to allow servo to reac  h its position
      } else {
        Serial.println("Received data type is not integer.");
      }
    } else {        
      Serial.println(firebaseData.errorReason());  // Print error message if reading failed
    }
  }
  
  // Check if lid should close after some time
  if (millis() - openTime >= openDuration) {
    servo.write(0); // Move servo to 0 degrees to close lid
  }

  delay(500);  // Delay to prevent rapid reading
}
