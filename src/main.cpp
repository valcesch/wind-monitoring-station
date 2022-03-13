/****************************************************************************************
 * File: main.cpp
 * Created on: 01.04.2021
 * Supported Hardware: Arduino MKR 1400
 * Firmware Version: 1.0
 * Note: based on:
 *  https://github.com/sparkfun/Weather_Shield/
 *  https://github.com/ee-quipment/ZeroPowerManager
 ****************************************************************************************/

#include "thingspeak.h"
#include "arduino_secrets.h"
#include "config.h"
#include "ArduinoLowPower.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTHEX(x) Serial.print(x, HEX)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTHEX(x)
#define DEBUG_PRINTLN(x)
#endif

THINGSPEAK cloud;

void serviceWatchdog();
void printWeather();
void calcWeather();
float get_wind_speed();
void setup_io_lp(void);
float get_battery_voltage(void);
void sendthingSpeakGSM(float field1, float field2, float field3, float field4, float field5);
uint32_t zpmRTCGetClock(void);
void zpmRTCSetClock(const uint32_t count);
double zpmRTCGetEpoch(void);
void zpmRTCInit(void);
void zpmRTCInterruptDisable(void);
void zpmRTCInterruptAt(const uint32_t count);
void zpmSleep(void);
void wspeedIRQ();

long lastSecond;  // The millis counter to see when a second rolls by
long last15min;   // The millis counter to see when a second rolls by
byte seconds;     // When it hits 60, increase the current minute
byte seconds_2m;  // Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
byte minutes;     // Keeps track of where we are in various arrays of data
byte minutes_10m; // Keeps track of where we are in wind gust/dir over last 10 minutes array of data

long lastWindCheck = 0;
volatile long lastWindIRQ = 0;
volatile byte windClicks = 0;

byte windspdavg[120]; // 120 bytes to keep track of 2 minute average

float windgust_10m[10]; // 10 floats to keep track of 10 minute max

// These are all the weather values that wunderground expects:
float windspeedmph = 0;     // [mph instantaneous wind speed]
float windgustmph = 0;      // [mph current wind gust, using software specific time period]
float windspdmph_avg2m = 0; // [mph 2 minute average wind speed mph]
float windgustmph_10m = 0;  // [mph past 10 minutes wind gust mph ]
float battery_level = 0;

void setup()
{
  zpmRTCInit();

  // Initialize I/O pins.
  pinMode(PIN_WATCHDOG_DONE, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_D1_INTERF_BOARD, INPUT_PULLDOWN);

  // Initiailize variables
  seconds = 0;
  lastSecond = zpmRTCGetEpoch();
  last15min = zpmRTCGetEpoch();

  // Attach interrupts
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(PIN_D1_INTERF_BOARD), wspeedIRQ, RISING);

  // Initialize connection to Thinkspeak
  cloud.begin(GPRS_PIN_NUMBER, GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD);
}

void loop()
{
  // Service watchdog
  serviceWatchdog();

  // Keep track of which minute it is
  if (zpmRTCGetEpoch() - lastSecond >= 1000)
  {
    lastSecond = zpmRTCGetEpoch();

    // Take a speed and direction reading every second for 2 minute average
    if (++seconds_2m > 119)
      seconds_2m = 0;

    // Calc the wind speed and direction every second for 120 second to get 2 minute average
    float currentSpeed = get_wind_speed();
    windspeedmph = currentSpeed; // update global variable for windspeed when using the printWeather() function
    // float currentSpeed = random(5); //For testing
    windspdavg[seconds_2m] = (int)currentSpeed;
    // if(seconds_2m % 10 == 0) displayArrays(); //For testing

    // Check to see if this is a gust for the minute
    if (currentSpeed > windgust_10m[minutes_10m])
    {
      windgust_10m[minutes_10m] = currentSpeed;
    }

    // Check to see if this is a gust for the day
    if (currentSpeed > windgustmph)
    {
      windgustmph = currentSpeed;
    }

    if (++seconds > 59)
    {
      seconds = 0;

      if (++minutes > 59)
        minutes = 0;
      if (++minutes_10m > 9)
        minutes_10m = 0;

      windgust_10m[minutes_10m] = 0; // Zero out this minute's gust
    }

    calcWeather(); // Go calc all the various sensors
  }

  if (zpmRTCGetEpoch() - last15min >= SYSTEM_DATA_POST_PERIOD)
  {

    digitalWrite(PIN_STATUS_LED, HIGH);

    last15min = zpmRTCGetEpoch();

    // Get battery level
    battery_level = get_battery_voltage();

    detachInterrupt(digitalPinToInterrupt(PIN_D1_INTERF_BOARD));

    // Connect to the Thingspeak server
    if (cloud.connect() == TSPEAK_SUCCESS)
    {
      sendthingSpeakGSM(windspeedmph, windgustmph, windspdmph_avg2m, windgustmph_10m, battery_level);
      cloud.disconnect();
    }

    LowPower.attachInterruptWakeup(digitalPinToInterrupt(PIN_D1_INTERF_BOARD), wspeedIRQ, RISING);

    // reset wind speed measurement (reset counters in read function)
    get_wind_speed();

    digitalWrite(PIN_STATUS_LED, LOW);
  }

  // Shut-down
  setup_io_lp();
  USBDevice.detach();
  uint32_t now = zpmRTCGetClock();
  zpmRTCInterruptAt(now + SYSTEM_WATCHDOG_RESET_PERIOD);
  zpmSleep();
  // delay(SYSTEM_WATCHDOG_RESET_PERIOD);
}

void serviceWatchdog()
{
  digitalWrite(PIN_WATCHDOG_DONE, HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_WATCHDOG_DONE, LOW);
}

void setup_io_lp(void)
{

  // PA12,13,14,15
  // To level shifter U2
  pinMode(26, OUTPUT);
  digitalWrite(26, 0); // PA12
  pinMode(27, INPUT);  // PA13
  pinMode(28, OUTPUT);
  digitalWrite(28, 0); // PA14
  pinMode(29, INPUT);
  digitalWrite(29, 0); // PA15

  // PA07,10,11,28
  //  To level shifter U7
  pinMode(21, OUTPUT);
  digitalWrite(21, 0); // PA07
  pinMode(35, OUTPUT);
  digitalWrite(35, 0); // PA28

  // PA18
  //  To DCDC
  pinMode(24, INPUT); // PA18

  // PB08 RST
  pinMode(30, INPUT); // PB08
}

/// sparkfun()///
void sendthingSpeakGSM(float field1, float field2, float field3, float field4, float field5)
{
  // Build the postData string.
  String postData = "api_key=" + String(THINGSPEAK_PRIVATE_KEY);
  postData += "&field1=" + String(field1);
  postData += "&field2=" + String(field2);
  postData += "&field3=" + String(field3);
  postData += "&field4=" + String(field4);
  postData += "&field5=" + String(field5);

  // Post data
  cloud.send_data(postData);
}

float get_wind_speed()
{
  float deltaTime = zpmRTCGetEpoch() - lastWindCheck; // 750ms

  deltaTime /= 1000.0; // Covert to seconds

  float windSpeed = (float)windClicks / deltaTime; // 3 / 0.750s = 4

  windClicks = 0; // Reset and start watching for new wind
  lastWindCheck = zpmRTCGetEpoch();

  windSpeed *= 1.492; // 4 * 1.492 = 5.968MPH

  return (windSpeed);
}

void calcWeather()
{
  // Calc windspeed
  // windspeedmph = get_wind_speed(); //This is calculated in the main loop on line 185

  // Calc windgustmph
  // Calc windgustdir
  // These are calculated in the main loop

  // Calc windspdmph_avg2m
  float temp = 0;
  for (int i = 0; i < 120; i++)
    temp += windspdavg[i];
  temp /= 120.0;
  windspdmph_avg2m = temp;

  // Calc windgustmph_10m
  // Calc windgustdir_10m
  // Find the largest windgust in the last 10 minutes
  windgustmph_10m = 0;
  // Step through the 10 minutes
  for (int i = 0; i < 10; i++)
  {
    if (windgust_10m[i] > windgustmph_10m)
    {
      windgustmph_10m = windgust_10m[i];
    }
  }
}

void printWeather()
{
  DEBUG_PRINT("$,windspeedmph=");
  DEBUG_PRINT(windspeedmph);
  DEBUG_PRINT(",windgustmph=");
  DEBUG_PRINT(windgustmph);
  DEBUG_PRINT(",windspdmph_avg2m=");
  DEBUG_PRINT(windspdmph_avg2m);
  DEBUG_PRINT(",windgustmph_10m=");
  DEBUG_PRINT(windgustmph_10m);
  DEBUG_PRINT(",");
  DEBUG_PRINTLN("#");
}

float get_battery_voltage(void)
{
  analogReadResolution(12); // To use full potential of ADC on MKR boards
  return analogRead(ADC_BATTERY) * 3.3f / 4095.0f / 1.2f * (1.2f + 0.33f);
}

uint32_t zpmRTCGetClock(void)
{
  return RTC->MODE0.COUNT.reg;
}

void zpmRTCSetClock(const uint32_t count)
{
  RTC->MODE0.COUNT.reg = count;
}

double zpmRTCGetEpoch(void)
{
  return ((double)(RTC->MODE0.COUNT.reg) / 1024.0) * 1000;
}

void zpmRTCInit(void)
{

  // keep the XOSC32K running in standy
  SYSCTRL->XOSC32K.reg |= SYSCTRL_XOSC32K_RUNSTDBY;

  // attach GCLK_RTC to generic clock generator 1
  GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK1 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));

  // ensure module is reset
  RTC->MODE0.CTRL.bit.SWRST = 1;
  while (RTC->MODE0.CTRL.bit.SWRST == 1)
    ;

  // reset configuration is mode=0, no clear on match
  RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_PRESCALER_DIV32 | RTC_MODE0_CTRL_ENABLE;

  NVIC_EnableIRQ(RTC_IRQn);
  NVIC_SetPriority(RTC_IRQn, 0x00);

  zpmRTCSetClock(0); // reset to zero in case of warm start
}

void zpmRTCInterruptDisable(void)
{
  RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_MASK;
}

void zpmRTCInterruptAt(const uint32_t count)
{
  zpmRTCInterruptDisable();

  // clear any pending interrupts, set compare register and enable interrupt
  RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_MASK;
  RTC->MODE0.COMP[0].reg = count;
  RTC->MODE0.INTENSET.bit.CMP0 = 1;
}

void zpmSleep(void)
{
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __DSB();
  __WFI();
}

void wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
  if (zpmRTCGetEpoch() - lastWindIRQ > 20) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
  {
    lastWindIRQ = zpmRTCGetEpoch(); // Grab the current time
    windClicks++;                   // There is 1.492MPH for each click per second.
  }
}