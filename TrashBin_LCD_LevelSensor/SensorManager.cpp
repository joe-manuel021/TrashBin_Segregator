#include "SensorManager.h"

// Pin definitions
static int trigPin1, echoPin1;
static int trigPin2, echoPin2;
static int trigPin3, echoPin3;

// Shared structure to store readings
static volatile UltrasonicReadings readings;

// Mutex for thread-safe access to the readings
SemaphoreHandle_t readingsMutex;

void initSensors(int t1, int e1, int t2, int e2, int t3, int e3) {
    trigPin1 = t1;
    echoPin1 = e1;
    trigPin2 = t2;
    echoPin2 = e2;
    trigPin3 = t3;
    echoPin3 = e3;

    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    pinMode(trigPin3, OUTPUT);
    pinMode(echoPin3, INPUT);

    readingsMutex = xSemaphoreCreateMutex(); // Create mutex for thread safety
}

float measureDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    return (duration * 0.0343) / 2; // Convert to cm
}

void sensorTask(void *parameter) {
    while (true) {
        // Temporary readings
        float dist1 = measureDistance(trigPin1, echoPin1);
 
        float dist2 = measureDistance(trigPin2, echoPin2);
     
        float dist3 = measureDistance(trigPin3, echoPin3);
    

        // Update shared readings with mutex
        if (xSemaphoreTake(readingsMutex, pdMS_TO_TICKS(100))) {
            readings.sensor1Distance = dist1;
            readings.sensor2Distance = dist2;
            readings.sensor3Distance = dist3;
            xSemaphoreGive(readingsMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Measure every 500ms
    }
}

UltrasonicReadings getUltrasonicReadings() {
    UltrasonicReadings copy;
    if (xSemaphoreTake(readingsMutex, pdMS_TO_TICKS(100))) {
        copy.sensor1Distance = readings.sensor1Distance;
        copy.sensor2Distance = readings.sensor2Distance;
        copy.sensor3Distance = readings.sensor3Distance;
        xSemaphoreGive(readingsMutex);
    }
    return copy;
}
