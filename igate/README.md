# Igate

## Source Code [igate](https://github.com/lora-aprs/LoRa_APRS_iGate) 

Complementary files

- AXP192 (power management)
- LilyGo_T3_V1.6 schematics

***
## TTGO BOARD

![TTGO](ttgo.jpg  "TTGO BOARD")

***

## TTGO TEST with esp32 and RA02 (SX1278)

Igate software recognize TTGO lora32 V1

change i2c lines in BoardFinder.cpp file to use ssd1306 display

```c++
BoardConfig TTGO_LORA32_V1        ("TTGO_LORA32_V1",         eTTGO_LORA32_V1,          21, 22, 0x3C,  0,  5, 19, 27, 18, 14, 26);
```

![TTGO](cablage_igate.png  "TTGO TEST")

```console
mode:DIO, clock div:2
load:0x3fff0018,len:4
load:0x3fff001c,len:1044
load:0x40078000,len:10124
load:0x40080400,len:5828
entry 0x400806a8
Set SPI pins!
Set LoRa pins!
frequency:
433775000
LoRa init done!
Tache RX en fonctionnement
(Press m key to enter config menu)
..help command for info 
>help
Configuation exemple
Set ssid : ssid mywifi
Set password : pass toto
Set new callsign : call f4goh-6
Set latitude in degrees minutes decimal: latitude 4753.41N
Set longitude in degrees minutes decimal: longitude 00016.61E
Set new comment : comment hello
Enable wifi for igate (aprs.fi): internet 1
Enable local wifi Access Point for AprsDroid: internet 0
When disable igate, local wifi AP are enable (ssid "APRS Droid" password "totototo")
Show configuration : shconfig
Reset default configuration : raz
Exit menu : exit
>shconfig
Ssid is : Livebox-xxxx
password is : zxxxxxxxxxxxxxxx
Call is : F4KMN-1
Frequency is : 433775000
latitude is : 4753.41N
longitude is : 00016.61E
Internet is : Enable
TCP Access point is : Disable
Comment is : LoRa iGATE : https://github.com/f4goh/lora-aprs-esp32
>exit
```




