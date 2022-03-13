/****************************************************************************************
 * File: config.h
 * Created on: 01.04.2021
 * Supported Hardware: Arduino MKR 1400
 * Firmware Version: 1.0
 * Note: based on:
 ****************************************************************************************/

#ifndef _CONFIG_h
#define _CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define SYSTEM_WATCHDOG_RESET_PERIOD 250 //[ms]
#define SYSTEM_DATA_POST_PERIOD 600000   //[ms]

#define PIN_STATUS_LED LED_BUILTIN
#define PIN_WATCHDOG_DONE 3
#define PIN_D1_INTERF_BOARD 5

#endif