// Include necessary libraries
#include <LiquidCrystal_I2C.h> // For LCD display
#include <Wire.h>              // For I2C communication
#include <WiFiClient.h>        // WiFi library
#include <BlynkSimpleEsp32.h>  // Blynk library for ESP32

// Define pins
#define SENSOR_PIN 33 // Soil moisture sensor pin
#define RELAY_PIN 4   // Relay control pin for motor

// Initialize LCD with I2C address and dimensions
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Blynk authentication and WiFi credentials
char auth[] = "BLYNK_AUTH_TOKEN"; // Auth token from Blynk
char ssid[] = "katinat";          // WiFi SSID
char pass[] = "12345678";         // WiFi password

// Initialize Blynk timer
BlynkTimer timer;

void setup()
{
  // Start serial communication for debugging
  Serial.begin(115200);

  // Initialize Blynk with cloud server and authentication token
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  // Initialize LCD and set up relay pin
  lcd.init();
  lcd.backlight(); // Enable LCD backlight for visibility
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Default state: motor off

  // Display loading message on LCD
  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int i = 0; i <= 15; i++)
  {
    lcd.setCursor(i, 1);
    lcd.print(".");
    delay(200); // Simulate loading with delay
  }
  lcd.clear(); // Clear LCD for next messages
}

// Function to read and send soil moisture data
void soilMoisture()
{
  int rawValue = analogRead(SENSOR_PIN);         // Read raw sensor value
  int moisture = map(rawValue, 0, 4095, 0, 100); // Convert to percentage
  moisture = (moisture - 100) * -1;              // Invert to get moisture level

  // Send moisture level to Blynk virtual pin V0
  Blynk.virtualWrite(V0, moisture);

  // Print moisture level to serial monitor for debugging
  Serial.println(moisture);

  // Display moisture level on LCD
  lcd.setCursor(0, 0);
  lcd.print("Moisture :");
  lcd.print(moisture);
  lcd.print(" "); // Clear old values
}

// Blynk button handler for relay control (V1 button)
BLYNK_WRITE(V1)
{
  bool relayState = param.asInt(); // Get button state from Blynk
  if (relayState == 1)
  {
    digitalWrite(RELAY_PIN, LOW); // Turn on motor
    lcd.setCursor(0, 1);
    lcd.print("Motor is ON ");
  }
  else
  {
    digitalWrite(RELAY_PIN, HIGH); // Turn off motor
    lcd.setCursor(0, 1);
    lcd.print("Motor is OFF");
  }
}

void loop()
{
  // Check soil moisture and update Blynk and LCD
  soilMoisture();

  // Run Blynk tasks
  Blynk.run();

  delay(200); // Control sensor read frequency to reduce noise
}
