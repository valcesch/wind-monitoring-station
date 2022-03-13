/****************************************************************************************
 * File: thingspeak.h
 * Created on: 01.04.2021
 * Supported Hardware: Arduino MKR 1400
 * Firmware Version: 1.0
 ****************************************************************************************/

#ifndef _THINGSPEAK_h
#define _THINGSPEAK_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <MKRGSM.h>

#define TSPEAK_CONNECT_NB_TRIAL 3

#define TSPEAK_PDP_PIN_NUMBER_MAX_LENGTH 64
#define TSPEAK_PDP_APN_MAX_LENGTH 64
#define TSPEAK_PDP_USER_NAME_MAX_LENGTH 64
#define TSPEAK_PDP_USER_PASSWD_MAX_LENGTH 64

#define TSPEAK_SUCCESS 0x00
#define TSPEAK_ERROR 0x01

#define TSPEAK_SERVER "api.thingspeak.com"
#define TSPEAK_PORT 80

class THINGSPEAK
{
private:
	GSMClient client;
	GPRS gprs;
	GSM gsmAccess;

	const char *_pin;
	const char *_apn;
	const char *_username;
	const char *_password;

public:
	uint8_t begin(const char *pin_number,
				  const char *gprs_apn,
				  const char *gprs_login,
				  const char *gprs_password);
	void end(void);

	uint8_t connect(void);
	uint8_t disconnect(void);

	uint8_t send_data(String data);
};

#endif
