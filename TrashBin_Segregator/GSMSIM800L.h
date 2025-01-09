// LED.h
#ifndef GSMSIM800L_h
#define GSMSIM800L_h

#include <HardwareSerial.h>
#include <Arduino.h>

class GSMSIM800L{
  
  private:
    HardwareSerial *gsmSerial;
    String ATResponse = "";
    bool debug_mode = true;
    String SMSBuffer = "";
    

  public:
    GSMSIM800L(HardwareSerial *serial);
    void init();
    void setDebugging(bool mode);
    String monitorGSMSerial();
    void SendSMS(String msg, String num);
    String ATCommand(String cmd);

    bool receivedSMS = false;
    String senderNumber = "";

    String getReceivedSMS();
    
};

#endif
