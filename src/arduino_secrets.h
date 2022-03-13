/****************************************************************************************
 * File: arduino_secrets.h
 * Created on: 01.04.2021
 * Supported Hardware: Arduino MKR 1400
 * Firmware Version: 1.0
 * Note: based on:
 ****************************************************************************************/

#ifndef _ARDUINO_SECRETS_h
#define _ARDUINO_SECRETS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define GPRS_PIN_NUMBER ""
#define GPRS_APN ""
#define GPRS_LOGIN ""
#define GPRS_PASSWORD ""

#define THINGSPEAK_PRIVATE_KEY ""

#endif
