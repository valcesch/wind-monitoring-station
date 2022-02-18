# Wind-monitoring-station

## Description
Autonomous wind monitoring station based on Arduino MKR GSM 1400. All data are pushed on [ThingSpeak](https://thingspeak.com/)

![installed](pictures/installed_view.png#center)

The following components are used: 

- Arduino MKR GSM 1400 (MKRGSM1400WANT)
- DFRobot Solar Lipo Charger (DFR0264)
- Solar panel 70x55mm (100mA, 0.5W)
- LiPo 3.7V 1000mAh (102050)
- Nano-Power System Timer With Watchdog Function (TPL5010)
- Taoglas 4G/LTE Flexible antenna (FXUB63.54.0150C)

![system_view](pictures/system_view_1.PNG#center)

Integration on website:

![system](pictures/website_view.PNG#center)

## Comments
The MKR1400 is a great platform, but not very well suited for low power IoT devices. After many researches, I could not bring the current below 6mA in deep-sleep mode. Note that running the same code on Seeeduino XIAO will result in a current consumption of only 6uA in deep-sleep... 

A few interesting discussions can be found here: 

- https://forum.qoitech.com/t/power-consumption-on-arduino-mkr-gsm-1400/271
- https://www.youtube.com/watch?v=YVho42UC43Q&t

Let me know if you can bring this board below a few hundreed uA !

The [Particle Boron](https://docs.particle.io/boron/) is a very good alternative that I tested in another project (it uses the same Ublox SARA-U201 chip than the MKR1400). The power consumption figures presented in the [datasheet](https://docs.particle.io/datasheets/boron/boron-datasheet/) are very well respected. Particle proposes as well a free data plan which is sufficient for the present project.

## Disclaimer

Copyright (C) valcesch

valcesch reserves all rights in this deliverable (documentation, software, etc., hereafter “Deliverable”).

valcesch grants you the right to use, copy, modify and distribute the Deliverable provided hereunder for any purpose without fee.

THIS DELIVERABLE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTY. IN PARTICULAR, NEITHER THE AUTHOR NOR valcesch MAKES ANY REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY OF THIS DELIVERABLE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.

In case you provide us a feedback or make a contribution in the form of a further development of the Deliverable (“Contribution”), valcesch will have the same rights as granted to you, namely to use, copy, modify and distribute the Contribution provided to us for any purpose without fee.