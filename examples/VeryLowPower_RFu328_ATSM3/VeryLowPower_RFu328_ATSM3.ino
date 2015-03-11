///////////////////////////////////////////////////////////////////////////////
//
// LLAP very low power example - RFu-328 only
//
///////////////////////////////////////////////////////////////////////////////
// 
// Target:       Ciseco RFu-328
// version:      0.1
// date:         25 August 2013
//
// copyright/left, limitations to use etc. statement to go here
//
//////////////////////////////////////////////////////////////////////////////
//
// Reads the voltage at A0 e.g. for moisture sensor
// Connections:
// Probes connected between GND and Analog 0 (A0)
// 10kOhm resistor connected between pin 9 and Analog 0 (A0)
//
/////////////////////////////////////////////////////////////////////////////
//
// RFu-328 specific AT commands (requires firmware RFu-328 V0.84 or better)
//    The following RFu specific AT commands are supported in V0.84 or later
//		Added a new sleep mode (ATSM3) Wake after timed interval
//			The duration is specified by a new command ATSD, units are mS 
//			and the range is 32bit (max approx. 49 days)
//				e.g. ATSD5265C00 is one day (hex 5265C00 is decimal 86400000)
//				ATSD36EE80 is one hour
//				ATSD493E0 is five minutes
//		This sleep mode can be used to wake the 328 after a timed interval 
//			by connecting RFu-328 pin 7 to pin 19, D3(INT1) or 20, D2(INT0).
//		Using this technique means that a RFu-328 can sleep for a timed 
//			period consuming about 0.6uA, instead of using the AVR328 watchdog (for timing) 
//			which uses around 7uA.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <LLAPSerial.h>


void setup()
{
  Serial.begin(115200);         // Start the serial port
  
  LLAP.init("LP");                  // Initialise the LLAPSerial library and the device identity

  pinMode(8, OUTPUT);           // pin 8 controls the radio
  digitalWrite(8, HIGH);        // select the radio

  pinMode(4, OUTPUT);           // pin 4 controls the radio sleep
  digitalWrite(4, LOW);        // wake the radio

  delay(400);						// let everything start up

  // set up sleep mode 3 (low = awake)
  uint8_t val;
  while ((val = setupSRF()) != 5)
  {
	  LLAP.sendInt("ERR",val); // Diagnostic
	  delay(5000);	// try again in 5 seconds
  }

  pinMode(9,OUTPUT);
  digitalWrite(9,LOW);	// No voltage to the sensor

  LLAP.sendMessage(F("STARTED"));  // send the usual "started message
}

void loop()
{
	pinMode(4, INPUT);        // sleep the radio

	LLAP.sleep(3, RISING, false);		// sleep until woken on pin 3, no pullup (low power)

	pinMode(4, OUTPUT);        // wake the radio

	digitalWrite(9,HIGH);	// provide voltage for the sensor
	int moist = analogRead(0);  // read sensor and convert to temperature
	digitalWrite(9,LOW);	// remove voltage from the sensor
	LLAP.sendInt("MST",moist);
 }


/////////////////////////////////////////////////////////
// SRF AT command handling
/////////////////////////////////////////////////////////

uint8_t setupSRF()	// set Sleep mode 2
{
  if (!enterCommandMode())	// if failed once then try again
	{
		if (!enterCommandMode()) return 1;
	}
  //if (!sendCommand("ATSD49E30")) return 2;	// 5 minutes
  //if (!sendCommand("ATSD4E20")) return 2;	// 20 seconds
 // if (!sendCommand("ATSD1388")) return 2;	// 5 seconds
  if (!sendCommand("ATSD3E8")) return 2;	// 1 second
  if (!sendCommand("ATSM3")) return 3;
  if (!sendCommand("ATDN")) return 3;
  return 5;
}

uint8_t enterCommandMode()
{
  delay(1200);
  Serial.print("+++");
  delay(500);
  while (Serial.available()) Serial.read();  // flush serial in
  delay(500);
  return checkOK(500);
}

uint8_t sendCommand(char* lpszCommand)
{
  Serial.print(lpszCommand);
  Serial.write('\r');
  return checkOK(100);
}

uint8_t checkOK(int timeout)
{
  uint32_t time = millis();
  while (millis() - time < timeout)
  {
    if (Serial.available() >= 3)
    {
      if (Serial.read() != 'O') continue;
      if (Serial.read() != 'K') continue;
      if (Serial.read() != '\r') continue;
      return 1;
    }
  }
  return 0;
}
