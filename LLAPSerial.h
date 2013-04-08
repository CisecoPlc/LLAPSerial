// LLAPSerial.h

#ifndef _LLAPSERIAL_h
#define _LLAPSERIAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class LLAPSerial
{
 private:
	char cMessage[13];
	void processMessage();
 public:
	void init();
	void init(char* cI);
	char deviceId[2];
	String sMessage;
	boolean bMsgReceived;
	void SerialEvent();
    void sendMessage(String sToSend);
	void sendInt(String sToSend, int value);
	void sendIntWithDP(String sToSend, int value, byte decimalPlaces);
    void setDeviceId(char* cId);
	byte sleepForaWhile (word msecs);
};

extern LLAPSerial LLAP;

#endif

