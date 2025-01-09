// Function to write a phone number to EEPROM at a given index
void writePhoneNumber(int index, String phoneNumber) {
  if (index >= 0 && index < MAX_NUMBERS) {
    int address = START_ADDRESS + index * NUMBER_LENGTH;
    for (int i = 0; i < NUMBER_LENGTH; i++) {
      char c = (i < phoneNumber.length()) ? phoneNumber[i] : '\0';
      EEPROM.write(address + i, c);  // Write each character to EEPROM
    }
    EEPROM.commit();
  } else {
    Serial.println("Index out of range.");
  }
}

// Function to read a phone number from EEPROM at a given index
String readPhoneNumber(int index) {
  if (index >= 0 && index < MAX_NUMBERS) {
    String phoneNumber = "";
    int address = START_ADDRESS + index * NUMBER_LENGTH;
    for (int i = 0; i < NUMBER_LENGTH; i++) {
      char c = EEPROM.read(address + i);
      if (c == '\0') break;
      phoneNumber += c;
    }
    return phoneNumber;
  }
  return "Index out of range";
}

// Helper function to check if a phone number is valid (no "?" or unexpected characters)
bool isValidPhoneNumber(const String &phoneNumber) {
  if (phoneNumber.length() == 0) return false;  // Empty number is invalid

  for (int i = 0; i < phoneNumber.length(); i++) {
    char c = phoneNumber[i];
    if (!isdigit(c) && c != '+') {  // Allow only digits and "+"
      return false;  // Invalid character found
    }
  }
  return true;
}

// Function to send a text message to all saved contacts in EEPROM
void sendTextToAllContacts(String message) {
  for (int i = 0; i < MAX_NUMBERS; i++) {
    String phoneNumber = readPhoneNumber(i);  // Read each phone number from EEPROM
    if (isValidPhoneNumber(phoneNumber)) {  // Check if there's a valid phone number
      Serial.print("Sending message to ");
      Serial.println(phoneNumber);
      sim800l.SendSMS(message, phoneNumber);
    } else {
      Serial.print("No valid contact at index ");
      Serial.println(i);
    }
  }
  Serial.println("Messages sent to all valid saved contacts.");
}
