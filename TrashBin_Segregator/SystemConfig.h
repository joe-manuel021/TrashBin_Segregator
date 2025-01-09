#define PRESENCE_PIN    33  // IR Pin to check presence of an object
#define METAL_SENSE_PIN 26  // Inductive Proximity Sensor
#define PLASTIC_PIN_1     34  // IR Pin to read transparency, Analog Pin
#define PLASTIC_PIN_2     39  // IR Pin to read transparency, Analog Pin
#define PLASTIC_PIN_3     36  // IR Pin to read transparency, Analog Pin

#define IR_ACTIVATE_PIN 25

#define SEGREGATOR_SERVO 14
#define DROP_SERVO       27


#define METAL_DETECTED   HIGH
#define OBJECT_DETECTED  LOW

const int EEPROM_SIZE = 512;       // Define EEPROM size (ESP32 has up to 512 bytes by default)
const int MAX_NUMBERS = 10;        // Max number of cellphone numbers to store
const int NUMBER_LENGTH = 14;      // Max length of each phone number (e.g., "+1234567890123")
const int START_ADDRESS = 0;       // Starting address in EEPROM
