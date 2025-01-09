#include <Arduino.h>
#include "SensorManager.h"
#include "SystemConfig.h"
#include <HardwareSerial.h>


HardwareSerial esp1Serial(1);
HardwareSerial lcdSerial(2);


// Task Handle
TaskHandle_t sensorTaskHandle = NULL;

// Function Prototypes
void sendLCDCommand(const uint8_t *command, size_t length);
void processAndSendLCDCommands(const UltrasonicReadings &readings);
void processSerialInput();


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  lcdSerial.begin(115200, SERIAL_8N1, 16, 17);
  esp1Serial.begin(115200, SERIAL_8N1, 18, 19);

  // Initialize Sensors
  initSensors(trigPin1, echoPin1, trigPin2, echoPin2, trigPin3, echoPin3);

  // Create Sensor Task
  xTaskCreate(
    sensorTask,        // Task function
    "Sensor Task",     // Task name
    4096,              // Stack size
    NULL,              // Task parameters
    1,                 // Priority
    &sensorTaskHandle  // Task handle
  );
}

void loop() {
  // Fetch the latest readings
  UltrasonicReadings currentReadings = getUltrasonicReadings();

  // Print readings to Serial Monitor
  Serial.print("Sensor 1: ");
  Serial.print(currentReadings.sensor1Distance);
  Serial.print(" cm, Sensor 2: ");
  Serial.print(currentReadings.sensor2Distance);
  Serial.print(" cm, Sensor 3: ");
  Serial.print(currentReadings.sensor3Distance);
  Serial.println(" cm");

  // Process readings and send commands to the LCD
  processAndSendLCDCommands(currentReadings);

  // Process Serial Input
  processSerialInput();

  vTaskDelay(pdMS_TO_TICKS(500));  // 1-second delay
}

// Function to send commands to the LCD
void sendLCDCommand(const uint8_t *command, size_t length) {
  lcdSerial.write(command, length);
  lcdSerial.flush();  // Ensure the command is sent immediately
}

// Function to process ultrasonic readings and send the appropriate LCD commands
void processAndSendLCDCommands(const UltrasonicReadings &readings) {
  // Process Sensor 1 (Plastic)
  if (readings.sensor1Distance <= LEVEL_FULL || readings.sensor1Distance > LEVEL_EMPTY) {
    sendLCDCommand(plasticRed, sizeof(plasticRed));
  } else if (readings.sensor1Distance <= LEVEL_MID) {
    sendLCDCommand(plasticYellow, sizeof(plasticYellow));
  } else if (readings.sensor1Distance > LEVEL_MID) {
    sendLCDCommand(plasticGreen, sizeof(plasticGreen));
  } 

  // Process Sensor 2 (Metal)
  if (readings.sensor2Distance <= LEVEL_FULL || readings.sensor2Distance > LEVEL_EMPTY) {
    sendLCDCommand(metalRed, sizeof(metalRed));
  } else if (readings.sensor2Distance <= LEVEL_MID) {
    sendLCDCommand(metalYellow, sizeof(metalYellow));
  } else if (readings.sensor2Distance > LEVEL_MID) {
    sendLCDCommand(metalGreen, sizeof(metalGreen));
  }

  // Process Sensor 3 (Other)
  if (readings.sensor3Distance <= LEVEL_FULL || readings.sensor3Distance > LEVEL_EMPTY) {
    sendLCDCommand(otherRed, sizeof(otherRed));
  } else if (readings.sensor3Distance <= LEVEL_MID) {
    sendLCDCommand(otherYellow, sizeof(otherYellow));
  } else if (readings.sensor3Distance > LEVEL_MID) {
    sendLCDCommand(otherGreen, sizeof(otherGreen));
  }

}

// Function to process Serial Input for "plastic", "metal", or "other"
void processSerialInput() {
  static String inputBuffer = "";  // Buffer to store serial input
  while (esp1Serial.available()) {
    char incomingChar = esp1Serial.read();
    if (incomingChar == '\n' || incomingChar == '\r') {
      // Process the command when newline or carriage return is detected
      inputBuffer.trim();
      Serial.println(inputBuffer);

      if (inputBuffer.indexOf("plastic") > -1) {
        sendLCDCommand(dialogPlastic, sizeof(dialogPlastic));
        delay(3000);  // Wait for 3 seconds
        sendLCDCommand(dialogNone, sizeof(dialogNone));
      } else if (inputBuffer.indexOf("metal") > -1) {
        sendLCDCommand(dialogMetal, sizeof(dialogMetal));
        delay(3000);  // Wait for 3 seconds
        sendLCDCommand(dialogNone, sizeof(dialogNone));
      } else if (inputBuffer.indexOf("other") > -1) {
        sendLCDCommand(dialogOther, sizeof(dialogOther));
        delay(3000);  // Wait for 3 seconds
        sendLCDCommand(dialogNone, sizeof(dialogNone));
      }

      inputBuffer = "";  // Clear the buffer
    } else {
      inputBuffer += incomingChar;  // Append character to buffer
    }
  }
}

