/****************************************************************************************
 * File: thingspeak.cpp
 * Created on: 01.04.2021
 * Supported Hardware: Arduino MKR 1400
 * Firmware Version: 1.0
 ****************************************************************************************/

#include "thingspeak.h"
#include <HttpClient.h>

uint8_t THINGSPEAK::begin(const char *pin_number,
						  const char *gprs_apn,
						  const char *gprs_login,
						  const char *gprs_password)
{
	_pin = pin_number;
	_apn = gprs_apn;
	_username = gprs_login;
	_password = gprs_password;

	return TSPEAK_SUCCESS;
}

void THINGSPEAK::end(void)
{
	// empty
}

uint8_t THINGSPEAK::connect(void)
{
	bool connected = false;

	for (int i = 0; i < TSPEAK_CONNECT_NB_TRIAL; i++)
	{
		if ((gsmAccess.begin(_pin) == GSM_READY) &&
			(gprs.attachGPRS(_apn, _username, _password) == GPRS_READY))
		{
			// Successfully connected
			connected = true;
			break;
		}
		else
		{
			// Failed to connect, retry in 1s
			delay(1000);
		}
	}

	if (connected == true)
	{
		return TSPEAK_SUCCESS;
	}

	return TSPEAK_ERROR;
}

uint8_t THINGSPEAK::disconnect(void)
{
	if (gsmAccess.shutdown())
	{
		return TSPEAK_SUCCESS;
	}

	return TSPEAK_ERROR;
}

uint8_t THINGSPEAK::send_data(String data)
{
	bool data_posted = false;

	if (client.connect(TSPEAK_SERVER, TSPEAK_PORT))
	{
		// Post fata to the thingspeak server
		client.println("POST /update HTTP/1.1");
		client.print("Host: api.thingspeak.com");
		client.println("Connection: close");
		client.println("Content-Type: application/x-www-form-urlencoded");
		client.print("Content-Length: ");
		client.println(data.length());
		client.println();
		client.println(data);

		data_posted = true;
	}

	client.stop();

	if (data_posted == true)
	{
		return TSPEAK_SUCCESS;
	}

	return TSPEAK_ERROR;
}
