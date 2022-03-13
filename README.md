# Wind monitoring station

## Description
A fully autonomous wind monitoring station based on the Arduino MKR GSM 1400.

![installed](pictures/installed_view.png#center)

The following components are used: 

- Cup Anemometer (reed switch, trig one interruption per turn)
- Arduino MKR GSM 1400 (MKRGSM1400WANT)
- DFRobot Solar Lipo Charger (DFR0264)
- Solar panel 70x55mm (100mA, 0.5W)
- LiPo 3.7V 1000mAh (102050)
- Nano-Power System Timer With Watchdog Function (TPL5010)
- Taoglas 4G/LTE Flexible antenna (FXUB63.54.0150C)

![system_view](pictures/system_view_1.PNG#center)

The data collected by the station are pushed to channel in [ThingSpeak](https://thingspeak.com/) and displayed on a website:

![system](pictures/website_view.PNG#center)

## Comments
The MKR1400 is a great platform, but not very well suited for low power IoT devices. After many researches, I could not bring the current below 6mA in deep-sleep (wake-up with RTC).

A few interesting discussions can be found here: 

- https://forum.qoitech.com/t/power-consumption-on-arduino-mkr-gsm-1400/271
- https://www.youtube.com/watch?v=YVho42UC43Q&t

And the library used to put the SAMD21 in a low power state: https://github.com/ee-quipment/ZeroPowerManager 

Let me know if you can bring this board below a few hundred uA !

The [Particle Boron](https://docs.particle.io/boron/) is a very good alternative that I tested in another project (it uses the same Ublox SARA-U201 chip than the MKR1400). The power consumption figures presented in the [datasheet](https://docs.particle.io/datasheets/boron/boron-datasheet/) are very well respected. Particle proposes as well a free data plan which suits well the requirement of the project.

## Disclaimer

Copyright (C) valcesch

valcesch reserves all rights in this deliverable (documentation, software, etc., hereafter “Deliverable”).

valcesch grants you the right to use, copy, modify and distribute the Deliverable provided hereunder for any purpose without fee.

THIS DELIVERABLE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTY. IN PARTICULAR, NEITHER THE AUTHOR NOR valcesch MAKES ANY REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY OF THIS DELIVERABLE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.

In case you provide us a feedback or make a contribution in the form of a further development of the Deliverable (“Contribution”), valcesch will have the same rights as granted to you, namely to use, copy, modify and distribute the Contribution provided to us for any purpose without fee.