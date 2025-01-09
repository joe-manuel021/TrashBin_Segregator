#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>

// Define a structure to hold sensor readings
typedef struct {
    float sensor1Distance;
    float sensor2Distance;
    float sensor3Distance;
} UltrasonicReadings;

// Initialize the sensors
void initSensors(int trig1, int echo1, int trig2, int echo2, int trig3, int echo3);

// Task function to read ultrasonic sensors
void sensorTask(void *parameter);

// Function to get the latest sensor readings
UltrasonicReadings getUltrasonicReadings();

#endif // SENSOR_MANAGER_H
