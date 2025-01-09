#include <ESP32Servo.h>
#include "SystemConfig.h"
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <HardwareSerial.h>
#include "GSMSIM800L.h"

HardwareSerial esp2Serial(1);
HardwareSerial gsmSerial(2);

GSMSIM800L sim800l(&gsmSerial);

String EmergencyContactNumber = "+639760158537";


// Servo objects
Servo segregatorServo;
Servo dropServo;

int segregatorServoPosition = 90;

// State machine states
enum State {
  IDLE,
  DETECT_OBJECT,
  CLASSIFY_METAL,
  CLASSIFY_PLASTIC,
  CLASSIFY_OTHER,
  DROP,
};

State currentState = IDLE;

void smoothServoMove(Servo &servo, int start, int target, int delayMs);

void setup() {

  Serial.begin(115200);
  esp2Serial.begin(115200, SERIAL_8N1, 18, 19);
  EEPROM.begin(EEPROM_SIZE);
  // Initialize GSM
  sim800l.init();
  sim800l.setDebugging(true);

  // Send startup SMS
  sim800l.SendSMS("Device has Started", EmergencyContactNumber);

  pinMode(PRESENCE_PIN, INPUT);
  pinMode(METAL_SENSE_PIN, INPUT);
  pinMode(IR_ACTIVATE_PIN, OUTPUT);

  digitalWrite(IR_ACTIVATE_PIN, HIGH);

  segregatorServo.attach(SEGREGATOR_SERVO);
  dropServo.attach(DROP_SERVO);

  // Initial positions for servos
  segregatorServo.write(segregatorServoPosition);  // Neutral position
  dropServo.write(70);                              // Closed position
  //delay(1000);
  //dropServo.detach();
}

void loop() {
  sim800l.monitorGSMSerial();
  static unsigned long lastStateChangeTime = 0;
  int plasticReading1 = 0;
  int plasticReading2 = 0;
  int plasticReading3 = 0;

  switch (currentState) {

    case IDLE:
      //Serial.println("State: IDLE");
      if (digitalRead(PRESENCE_PIN) == OBJECT_DETECTED) {
        currentState = DETECT_OBJECT;
      }
      break;



    case DETECT_OBJECT:
      Serial.println("State: DETECT_OBJECT");
      delay(700);
      if (digitalRead(METAL_SENSE_PIN) == METAL_DETECTED) {
        currentState = CLASSIFY_METAL;
      } else {
        currentState = CLASSIFY_PLASTIC;
      }
      break;



    case CLASSIFY_METAL:
      Serial.println("State: CLASSIFIED AS metal");
      esp2Serial.println("metal");
      smoothServoMove(segregatorServo, segregatorServoPosition, 90, 10);
      segregatorServoPosition = 90;
      delay(1000);
      currentState = DROP;
      break;



    case CLASSIFY_PLASTIC:
      digitalWrite(IR_ACTIVATE_PIN, LOW);
      delay(100);
      plasticReading1 = analogRead(PLASTIC_PIN_1);
      Serial.println("IR 1: " + String(plasticReading1));
      plasticReading2 = analogRead(PLASTIC_PIN_2);
      Serial.println("IR 2: " + String(plasticReading2));
      plasticReading3 = analogRead(PLASTIC_PIN_3);
      Serial.println("IR 3: " + String(plasticReading3));
      if (plasticReading3 > 512) { // Assuming a threshold for transparency
        Serial.println("State: CLASSIFIED AS plastic");
        esp2Serial.println("plastic");
        smoothServoMove(segregatorServo, segregatorServoPosition, 20, 10);
        segregatorServoPosition = 20;
        currentState = DROP;
        delay(1000);
      } else {
        currentState = CLASSIFY_OTHER;
      }

      digitalWrite(IR_ACTIVATE_PIN, HIGH);
      lastStateChangeTime = millis();
      break;

    case CLASSIFY_OTHER:
      Serial.println("State: CLASSIFIED AS other");
      esp2Serial.println("other");
      smoothServoMove(segregatorServo, segregatorServoPosition, 155, 10);
      segregatorServoPosition = 155;
      delay(1000);  // Allow servo to move
      currentState = DROP;
      break;



    case DROP:
      Serial.println("State: DROP");
      //dropServo.attach(DROP_SERVO);
      //delay(500);
      //smoothServoMove(dropServo, 70, 5, 0);  // Open drop servo
      dropServo.write(10);
      dropServo.write(10);
      delay(1000);
      dropServo.write(70);
      dropServo.write(70);                       // Wait for trash to drop
      //smoothServoMove(dropServo, 5, 70, 0);  // Close drop servo
      //delay(1000);
      //dropServo.detach();
      currentState = IDLE;
      lastStateChangeTime = millis();
      delay(2000);
      break;

    default:
      break;
  }
}

void smoothServoMove(Servo &servo, int start, int target, int delayMs) {
  if (start < target) {
    for (int pos = start; pos <= target; pos++) {
      servo.write(pos);
      delay(delayMs);  // Adjust this for smoother/slower movement
    }
  } else {
    for (int pos = start; pos >= target; pos--) {
      servo.write(pos);
      delay(delayMs);  // Adjust this for smoother/slower movement
    }
  }
}
