#define BLYNK_TEMPLATE_ID "TMPL3vbnH_wA2"
#define BLYNK_TEMPLATE_NAME "esp8266"
#define BLYNK_AUTH_TOKEN "U8CsaBB4s5laYTOVwPvgQNq1lCVSVLSW"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Define the turbidity sensor pin
const int turbiditySensorPin = A0;
const int phPin = A0;
const int relaypin = D0;
const int relaypin1 = D0;
const float m = -5.436;
#define DS18B20 5    // Connect to GPIO5 (D1 on NodeMCU) pin
OneWire ourWire(DS18B20);
DallasTemperature sensor(&ourWire);

char auth[] = BLYNK_AUTH_TOKEN;

const char* ssid = "hi";
const char* password = "";

BlynkTimer timer;

// Flag to control motor operation
bool motorRunning = false;
unsigned long motorStartTime = 0;

void startMotors() {
  // Turn on the motors
  digitalWrite(relaypin, HIGH);
  digitalWrite(relaypin1, HIGH);

  // Set the flag and record the start time
  motorRunning = true;
  motorStartTime = millis();
}

void stopMotors() {
  // Turn off the motors
  digitalWrite(relaypin, LOW);
  digitalWrite(relaypin1, LOW);

  // Reset the flag
  motorRunning = false;
}
int n=1;
void sendSensorData() {
  // Read the turbidity sensor value
  if(n%2==0)
  {
      int turbidityValue = analogRead(turbiditySensorPin);
      int turbidity = map(turbidityValue, 0, 750, 100, 0)-2;
      Serial.print("Turbidity: ");
      Serial.println(turbidity);
      Blynk.virtualWrite(V4, turbidity);
      if (turbidity > 8) {
    // If turbidity is above the threshold and motors are not running, start them
    if (!motorRunning) {
      Blynk.logEvent("turbidity_alert", "Turbidity is out of limits");
      startMotors();
    }

    // Check if the motors have been running for 10 seconds, then stop them
    if (millis() - motorStartTime >= 10000) {
      stopMotors();
    }
  } else {
    // If turbidity is within limits and motors were running, stop them
    if (motorRunning) {
      Blynk.logEvent("turbidity_alert", "Turbidity is within limits");
      stopMotors();
    }
  }

      n=n+1;
  }
  else{
  float phValue= (analogRead(phPin)+6.93)/100;
  Serial.print("pH: ");
  Serial.println(phValue);
  Blynk.virtualWrite(V2, phValue);
  if (phValue < 4) {
    // If turbidity is above the threshold and motors are not running, start them
    if (!motorRunning) {
      Blynk.logEvent("pH_alert", "pH is out of limits");
      startMotors();
    }

    // Check if the motors have been running for 10 seconds, then stop them
    if (millis() - motorStartTime >= 10000) {
      stopMotors();
    }
  } else {
    // If turbidity is within limits and motors were running, stop them
    if (motorRunning) {
      Blynk.logEvent("pH_alert", "pH is within limits");
      stopMotors();
    }
  }
  n=n+1;
  }
  
  // Print the turbidity value and pH value to the Serial Monitor
  
  

  // Send the turbidity value and pH value to the Blynk app
  
  
  

  
  // Read the DS18B20 temperature sensor value
  sensor.requestTemperatures();
  Serial.print("Celsius temperature: ");
  Serial.print(sensor.getTempCByIndex(0));
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensor.getTempFByIndex(0));

  int tempC = sensor.getTempCByIndex(0);
  int tempF = sensor.getTempFByIndex(0);

  // You can send any value at any time.
  // Please don't send more than 10 values per second.
  Blynk.virtualWrite(V0, tempC);
  Blynk.virtualWrite(V1, tempF);
  if (tempC > 31 || tempC < 25) {
    Blynk.logEvent("pH_alert", "Temperature is out of limits");
  }

  // Print a message when data is sent to Blynk
  Serial.println("Data pushed to Blynk");

  delay(500);
}

void setup() {
  Serial.begin(115200);
  pinMode(relaypin, OUTPUT);
  pinMode(relaypin1, OUTPUT);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  sensor.begin();

  Blynk.begin(auth, ssid, password);
  timer.setInterval(1000L, sendSensorData); // Adjust the interval based on your needs
}

void loop() {
  Blynk.run();
  timer.run();
}
